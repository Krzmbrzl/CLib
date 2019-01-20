#include "DllEntry.hpp"

#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include <thread>
#include <future>
#include <exception>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

#ifdef WINDOWS
    #include <shlwapi.h> // requires Shlwapi.lib / Shlwapi.dll (version 4.71 or later)
    #include <WinBase.h> // requires Kernel32.lib - on windows 8+: #include <Processenv.h>
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include "ControlCharacter.hpp"
#include "ArmaRequest.hpp"
#include "ArmaExtension.hpp"
#include "Utils.hpp"

/**
 * Executes the given ArmaRequest by scheduling it to be run in a separate thread
 * @param the Clib::ArmaRequest to execute
 * 
 * @return Either the result of the request (if id==-1) or the CLib::ControlCharacter::ACK
 */
const char* executeRequest(CLib::ArmaRequest request);
/**
 * Checks if the given path is absolute
 * @param path The std::string "path" to check
 */
bool isAbsolutePath(const std::string& path);
/**
 * A utility function that detects all extensions available for the running Arma process.
 * It will populete the availableExtensions member
 */
void detectExtensions();

// variable definitions
static std::string inputBuffer;
static std::string outputBuffer;
static std::map<std::string, std::string> availableExtensions;
static std::map<int, std::future<const char *>> tasks;

static std::string version = std::string("CLib v") + std::string(CLIB_VERSION);

void RVExtensionVersion(char *output, int outputSize)
{
	std::strncpy(output, version.c_str(), outputSize - 1);
}

void RVExtension(char *armaOutput, int outputSize, const char *in)
{
    static bool initialized = false;

    if(!initialized) {
        detectExtensions();
        initialized = true;
    }

    int maxOutputSize = outputSize - 1;
    
	std::string input = in;
    std::string output;

    if(input.empty()) {
        return;
    } 
    else if (input == "debugger")
    {
        // TODO
        std::strncpy(armaOutput, "Debugger not yet implemented in Cpp-version!", maxOutputSize);
        return;
    } 
    else if (input == "version")
    {
        output += version;
    } else {
        char first = input.at(0);

        if(first == CLib::ControlCharacter::ACK) {
            output += outputBuffer;
        }
        else if (first == CLib::ControlCharacter::ENQ)
        {
            if(tasks.size() > 0) {
                try
                {
                    std::vector<int> completedTaskIDs;
                
                    for(auto& current : tasks)
                    {
                        std::future<const char *>* taskEntry = &current.second;
                        int taskID = current.first;

                        // check if future is valid
                        if(!taskEntry->valid()) {
                            throw std::future_error(std::future_errc::no_state);
                        }

                        // get task status
                        std::future_status taskStatus = taskEntry->wait_for(std::chrono::seconds(0));

                        if(taskStatus == std::future_status::ready || taskStatus == std::future_status::deferred) {
                            // task has finished or will only do something when being called
                            int initialOutputLength = output.size();
                            try {
                                output += CLib::ControlCharacter::SOH;
                                output += taskID;
                                output += CLib::ControlCharacter::STX;
                                output += taskEntry->get();
                            } catch (...) {
                                // Make sure output is well formatted
                                if(output.size() > initialOutputLength) {
                                    // content has been added to it -> remove it again
                                    output = output.substr(0,initialOutputLength);
                                }
                                
                                output += CLib::ControlCharacter::SOH;
                                output += taskID;
                                output += CLib::ControlCharacter::STX;
                                output += CLib::ControlCharacter::EOT; // end transmission as rethrow will surpass respective block

                                throw; // rethrow exception
                            }

                            // TODO: Debugger.Log("Task result: " + taskEntry.Key);

                            // store ID of completed task
                            completedTaskIDs.push_back(taskID);
                        }
                    }

                    // remove processed tasks
                    for (const int& currentID : completedTaskIDs) {
                        tasks.erase(currentID);
                    }

                    // end transmission
                    output += CLib::ControlCharacter::EOT;
                } catch(const std::exception& e)
                {
                    output += std::string("Error during task processing: ") + e.what();
                } catch (...) {
                    output += "Unknown error during task processing!";
                }
            }
        } else {
            try {
            inputBuffer += input;

            if(inputBuffer.at(inputBuffer.size() -1) == CLib::ControlCharacter::ETX) {
                output += executeRequest(CLib::ArmaRequest::parse(inputBuffer));
            } else {
                output += CLib::ControlCharacter::ACK;
            }
            } catch (const std::exception& e){
                std::string msg = std::string("Failed at processing request: ") + e.what();

                output += msg;
            } catch(...) {
                output += "Failed at processing request!";
            }
        }
    }
    
    // ensure that the output has the proper size
    if(output.size() > maxOutputSize) {
        // split the output and move the remains into outputBuffer
        int outputSplitPosition = maxOutputSize;

        // don't split unicode multi-byte sequence
        while(outputSplitPosition >= 1 && (output.at(outputSplitPosition - 1) & 0xC0) == 0x80){
            outputSplitPosition--;
        }

        // store remaining output in the buffer for later retrieval
        outputBuffer = output.substr(outputSplitPosition);

        // cut the output accordingly
        output = output.substr(0, outputSplitPosition);
    }

    // write output back to Arma
    std::strncpy(armaOutput, output.c_str(), maxOutputSize);
}

/**
 * Checks if the given path is absolute
 * @param path The std::string "path" to check
 */
bool isAbsolutePath(const std::string& path) {
    #ifdef WINDOWS
        return PathIsRelative(path.c_str());
    #else
        return (path.size() == 0 ? false : path.at(0) == '/');
    #endif
}

/**
 * Executes the given ArmaRequest by scheduling it to be run in a separate thread
 * @param the Clib::ArmaRequest to execute
 * 
 * @return Either the result of the request (if id==-1) or the CLib::ControlCharacter::ACK
 * 
 * @throw std::invalid_argument when the extension specified in the request can't be found or loaded
 * or if the given function does not exist within that extension
 */
const char* executeRequest(CLib::ArmaRequest request) {
    // clear inputBuffer -> was all parsed into the given request
    inputBuffer = "";

    // check if the given extension has been located before
    if(availableExtensions.find(request.getExtensionName()) != availableExtensions.end()) {
        throw std::invalid_argument("Unknown extension '" + request.getExtensionName() + "'!");
    }

    CLib::ArmaExtension extension = CLib::ArmaExtension(availableExtensions[request.getExtensionName()]);

    CLib::ArmaExtensionFunction func = extension.getFunction(request.getActionName());

    if(!func) {
        throw std::invalid_argument("Can't locate function '" + request.getActionName() + "' in '" 
            + availableExtensions[request.getExtensionName()] + "'!");
    }

    if (request.getTaskId() == -1) {
        // Execute request immediately
        std::string msg;
        msg += CLib::ControlCharacter::STX;
        msg += func(request.getData().c_str());
        msg += CLib::ControlCharacter::EOT;

        return msg.c_str();
    }

    // schedule request to be completed in different thread and store its future in task-list
    tasks[request.getTaskId()] = std::async(std::launch::async, func, request.getData().c_str());

    return std::string(1, CLib::ControlCharacter::ACK).c_str();
}

int RVExtensionArgs(char *output, int outputSize, const char *function, const char **argv, int argc)
{
	std::stringstream sstream;
	for (int i = 0; i < argc; i++)
	{
		sstream << argv[i];
	}
	std::strncpy(output, sstream.str().c_str(), outputSize - 1);
	return 0;
}

#ifdef WINDOWS
    #define FILE_SEP '\\'
#else
    #define FILE_SEP '/'
#endif

void detectExtensions() {
    // get command line arguments
    #ifdef WINDOWS
        std::string commandLineArgs = ::GetCommandLine();
    #else
        long pid = ::getpid();
        std::ifstream inFile;
        inFile.open(std::string("/proc/") + std::to_string(pid) + "/cmdline");

        std::stringstream sstr;
        sstr << inFile.rdbuf();
        std::string commandLineArgs = sstr.str();
    #endif

    char buffer[1024];
    char *answer = getcwd(buffer, sizeof(buffer));
    std::string cwd;
    if (answer)
    {
      cwd = answer;
    } else {
        throw std::invalid_argument("Unable to get the cwd of this process!");
    }

    if(cwd.empty()) {
        throw std::invalid_argument("Retrieved cwd is empty!");
    }

    if(cwd[cwd.length() - 1] != FILE_SEP) {
        cwd += FILE_SEP;
    }

    // TODO: Debugger.Log($"Current directory is: {Environment.CurrentDirectory}");
    // TODO: Debugger.Log("Extensions Found:");

    // find mod-start-parameter
    std::regex modParamEx("-(server)?mod=");
    std::sregex_iterator mods_begin = std::sregex_iterator(commandLineArgs.begin(), commandLineArgs.end(), modParamEx);
    std::sregex_iterator mods_end = std::sregex_iterator();
 
    for (std::sregex_iterator i = mods_begin; i != mods_end; ++i) {
        std::smatch match = *i;              
        int currentIndex = match.position();                                  
        
        // skip until = is found
        while(commandLineArgs[currentIndex] != '=') {
            currentIndex++;
        }
        // skip the = itself as well
        currentIndex++;

        // check if there are multiple mods wrapped in quotes
        int endIndex;
        char nextChar = commandLineArgs[currentIndex];
        if(nextChar == '"' || nextChar == '\'') {
            currentIndex++;
            endIndex = commandLineArgs.find(nextChar, currentIndex);
        } else {
            endIndex = commandLineArgs.find(' ', currentIndex);
            if(endIndex == std::string::npos) {
                // wasn't found
                endIndex = commandLineArgs.length();
            }
        }

        std::string modList = commandLineArgs.substr(currentIndex, endIndex);

        for(std::string currentModPath : CLib::Utils::split(modList, ';')) {
            if(!isAbsolutePath(currentModPath)) {
                // make absolute
                currentModPath = cwd + currentModPath;
            }

            if(currentModPath[currentModPath.length() - 1] != FILE_SEP) {
                currentModPath += FILE_SEP;
            }

            for(const std::string currentFile : CLib::Utils::listFiles(currentModPath)) {
                #ifdef WINDOWS
                    #ifdef CLIB64
                        std::string fileExtension = "_x64.dll";
                    #else
                        std::string fileExtension = ".dll";
                    #endif
                #else
                    std::string fileExtension = ".so";
                #endif

                if (currentFile.length() > fileExtension.length() 
                    && currentFile.substr(currentFile.length() - fileExtension.length(), currentFile.length()) == fileExtension) {
                    // this appears to be a proper library
                    try {
                        std::string absFilePath = currentModPath + currentFile;

                        CLib::ArmaExtension extension(absFilePath);

                        #ifdef WINDOWS
                            #ifdef CLIB64
                                std::string entryPoint = "RVExtension";
                            #else
                                std::string entryPoint = "_RVExtension@12";
                            #endif
                        #else
                            std::string entryPoint = "RVExtension";
                        #endif

                        if(extension.containsFunction(entryPoint)) {
                            // This appears to be a proper extension => add to list
                            availableExtensions[currentFile.substr(0, currentFile.length() - fileExtension.length())] = absFilePath;
                        }
                    } catch (const std::invalid_argument& e) {
                        // ignore and don't add to list
                    }
                }
            }
        }
    }  
}
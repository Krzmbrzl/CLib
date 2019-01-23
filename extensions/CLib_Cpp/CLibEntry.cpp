#include "DllEntry.hpp"

#include <string>
#include <cstring>
#include <map>
#include <thread>
#include <future>
#include <exception>
#include <algorithm>

#ifdef WINDOWS
    #include <shlwapi.h> // requires Shlwapi.lib / Shlwapi.dll (version 4.71 or later)
    #include <WinBase.h> // requires Kernel32.lib - on windows 8+: #include <Processenv.h>
#else
#endif

#include "ControlCharacter.hpp"
#include "ArmaRequest.hpp"
#include "ArmaExtension.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

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

void RVExtensionVersion(char *output, int outputSize)
{
	std::strncpy(output, CLIB_VERSION, outputSize - 1);
}

void RVExtension(char *armaOutput, int outputSize, const char *in)
{
    static bool initialized = false;

    if(!initialized) {
        try {
            CLib::Logger::getInstance().info("Starting Extension detection");

            detectExtensions();
            
            CLib::Logger::getInstance().info("Finished Extension detection");
        } catch (const std::exception& e) {
            std::string msg("Error during extension detection: ");
            msg += e.what();

            std::strncpy(armaOutput, msg.c_str(), outputSize - 1);
            return;
        }
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
        output += "CLib v";
        output += CLIB_VERSION;
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

void detectExtensions() {
    // get command line arguments
    std::vector<std::string> commandLineArgs = CLib::Utils::getCommandLineSegments();

    std::string mainDir = CLib::Utils::getExecutableDirectory();

    if(mainDir.empty()) {
        throw std::invalid_argument("Retrieved mainDir is empty!");
    }

    CLib::Logger::getInstance().info(std::string("Current directory is: ") + mainDir);

    if(mainDir[mainDir.length() - 1] != FILE_SEP) {
        mainDir += FILE_SEP;
    }


    // iterate over commandLineArgs
    for(std::string currentParam : commandLineArgs) {
        if(currentParam.length() <=2 || currentParam[0] != '-') {
            continue;
        }
        int equalPos = currentParam.find('=');
        if(equalPos == std::string::npos) {
            continue;
        }
        std::string paramName = currentParam.substr(1,equalPos - 1);
        
        // compare paramName case-insensitive
        std::transform(paramName.begin(), paramName.end(), paramName.begin(), ::tolower);

        if(paramName == "mod" || paramName == "servermod") {
            currentParam = currentParam.substr(paramName.length() + 2);
        } else {
            continue;
        }

        // Process mod-specification

        if(currentParam.empty()) {
            // something's worong
            continue;
        }

        // strip potential quotes
        if (currentParam[0] == '"' || currentParam[0] == '\'') {
            currentParam = currentParam.substr(1,currentParam.length() - 2);
        }

        // currentParam contains the mod-list only
        std::vector<std::string> modList = CLib::Utils::split(currentParam, ';');

        for(std::string currentModPath : modList) {
            if(!isAbsolutePath(currentModPath)) {
                currentModPath = mainDir + currentModPath;
            }

            // make sur it has a trailing file separator so it will be recognized as a directory
            if(currentModPath[currentModPath.length() - 1] != FILE_SEP) {
                currentModPath += FILE_SEP;
            }

            CLib::Logger::getInstance().info(std::string("Loaded mod: ") + currentModPath);

            try {
                for(const std::string currentFile : CLib::Utils::listFiles(currentModPath)) {
                    #ifdef WINDOWS
                        #ifdef CLIB64
                            #define LIB_EXTENSION "_x64.dll"
                            #define LIB_EXTENSION_LENGTH 8
                        #else
                            #define LIB_EXTENSION ".dll"
                            #define LIB_EXTENSION_LENGTH 4
                        #endif
                    #else
                        #define LIB_EXTENSION ".so"
                        #define LIB_EXTENSION_LENGTH 3
                    #endif

                    // check if file has proper extension
                    if (currentFile.length() > LIB_EXTENSION_LENGTH 
                        && currentFile.substr(currentFile.length() - LIB_EXTENSION_LENGTH) == LIB_EXTENSION) {
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
                                std::string extName = currentFile.substr(0, currentFile.length() - LIB_EXTENSION_LENGTH);

                                CLib::Logger::getInstance().info(std::string(std::string("Adding extension: ") + absFilePath + " as " + extName));
                                
                                availableExtensions[extName] = absFilePath;
                            }
                        } catch (const std::invalid_argument& e) {
                            // ignore and don't add to list
                        }
                    }
                }
            } catch (const std::invalid_argument e) {
                std::string msg = e.what();

                if(msg.substr(0,18) == "Unable to open dir") {
                    // ignore - was thrown because given path couldn't be resovled
                } else {
                    // was another cause => rethrow
                    throw;
                }
            }
        }
    }
}
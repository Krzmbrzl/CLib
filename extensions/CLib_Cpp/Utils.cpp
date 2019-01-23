#include "Utils.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <fstream>

#ifdef WINDOWS
    #include <windows.h>
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <sys/types.h>
    #include <dirent.h>
    #include <unistd.h>
#endif

namespace CLib {
    namespace Utils {
        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter A vector of chars containing the characters to split at
         * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const std::vector<char> delimiter, int count, std::vector<std::string>& output ) {
            if ( count == 0 ) {
                return;
            }
            if ( count ==1 ) {
                output.push_back ( inputString );
                return;
            }

            std::string currentSubString = "";

            int i=0;
            for ( ; i<inputString.length(); i++ ) {
                bool matched = false;
                char current = inputString.at ( i );

                for ( int k=0; k < delimiter.size(); k++ ) {
                    if ( current == delimiter[k] ) {
                        matched = true;
                        break;
                    }
                }

                if ( !matched ) {
                    currentSubString += current;
                } else {
                    output.push_back ( currentSubString );

                    currentSubString = "";

                    count--;

                    if ( count == 1 ) {
                        output.push_back ( inputString.substr ( i + 1) );
                        return;
                    }
                }
            }

            output.push_back ( currentSubString );
        }

        /**
         * Splits a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter A vector of chars containing the characters to split at
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const std::vector<char> delimiter, std::vector<std::string>& output ) {
            split ( inputString, delimiter, -1, output );
        }

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const char delimiter, int count, std::vector<std::string>& output ) {
            std::vector<char> delimiterVector;
            delimiterVector.push_back ( delimiter );

            split ( inputString, delimiterVector, count, output );
        }

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const char delimiter, std::vector<std::string>& output ) {
            split ( inputString, delimiter, -1, output );
        }

        /**
        * Splits  a string at the given delimiters
        * @param inputString The string to split
        * @param delimiter A vector of chars containing the characters to split at
        * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
        *
        * @return A vector containing the respective substrings
        */
        std::vector<std::string> split ( const std::string inputString, const std::vector<char> delimiter, int count ) {
            std::vector<std::string> elements;
            split ( inputString, delimiter, count, elements );

            return elements;
        }

        /**
        * Splits a string at the given delimiters
        * @param inputString The string to split
        * @param delimiter A vector of chars containing the characters to split at
        *
        * @return A vector containing the respective substrings
        */
        std::vector<std::string> split ( const std::string inputString, const std::vector<char> delimiter ) {
            return split ( inputString, delimiter, -1 );
        }

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
         *
         * @return A vector containing the respective substrings
         */
        std::vector<std::string> split ( const std::string inputString, const char delimiter, int count ) {
            std::vector<char> delimiterVector;
            delimiterVector.push_back ( delimiter );

            return split ( inputString, delimiterVector, count );
        }

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         *
         * @return A vector containing the respective substrings
         */
        std::vector<std::string> split ( const std::string inputString, const char delimiter ) {
            return split ( inputString, delimiter, -1 );
        }

        /**
         * Trims the right side of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& rtrim ( std::string& s, const char* t ) {
            s.erase ( s.find_last_not_of ( t ) + 1 );
            return s;
        }

        /**
         * Trims the left side of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& ltrim ( std::string& s, const char* t ) {
            s.erase ( 0, s.find_first_not_of ( t ) );
            return s;
        }

        /**
         * Trims the both sides of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& trim ( std::string& s, const char* t ) {
            return ltrim ( rtrim ( s, t ), t );
        }

        /**
         * Lists all files contained in the given directory
         * @param path The absolute file path to the directory that should be inspected
         * @return A std::vector<std::string> containing all file names
         * 
         * @throws std::invalid_argument if the given path does not point to a directory
         */
         std::vector<std::string> listFiles(const std::string path) {
             std::vector<std::string> fileNames;

             #ifdef WINDOWS
                if(path[path.length() - -1] != '\\') {
                    path += '\\';
                }

                path += '*';

                WIN32_FIND_DATA data;
                HANDLE hFind = FindFirstFile(path.c_str(), &data);      // DIRECTORY

                if ( hFind != INVALID_HANDLE_VALUE ) {
                    do {
                        fileNames.push_back(std::string(data.cFileName));
                    } while (FindNextFile(hFind, &data));
                    FindClose(hFind);
                }
             #else
                DIR *dp;
                struct dirent *dirp;
                if((dp  = opendir(path.c_str())) == NULL) {
                    throw std::invalid_argument(std::string("Unable to open dir '") + path + "'!");
                }

                while ((dirp = readdir(dp)) != NULL) {
                    fileNames.push_back(std::string(dirp->d_name));
                }
                closedir(dp);
             #endif

             return fileNames;
         }

        /**
         * Gets the current working directory of this process
         * @param bufSize The maximum size of the CWD-buffer
         * 
         * @return The respective CWD or an empty string if something went wrong
         */
        std::string getCWD(int bufSize) {
            char buffer[bufSize];
            char *answer = getcwd(buffer, bufSize);
            std::string cwd;
            if (answer) {
                cwd = answer;
            } else {
                // CWD could't be determined -> return empty string
            }

            return cwd;
        }

        /**
         * Gets the directory in which the executable of this process is located
         * @param bufSize (otpional) The size of the path-buffer to use
         * 
         * @return The path to the respective directory or an empty string if something went wrong
         */
        std::string getExecutableDirectory(int bufSize) {
            std::string path;
            #ifdef WINDOWS
                #error getExecutableDirectory not yet implemented for Windows!
            #else
                char buffer[bufSize];

                int size = readlink("/proc/self/exe", buffer, bufSize);

                if(size <= 0) {
                    return std::string();
                }
                
                for(int i=0; i<size; i++) {
                    path += buffer[i];
                }

                // remove executable name but leave trailing FILE_SEP
                int index = path.find_last_of(FILE_SEP);
                if(index > 0) {
                    path = path.substr(0,index + 1);
                }

                return path;
            #endif
        }

        /**
         * Gets all segments of the command line that has been used to call this process
         * 
         * @return A std::vector<std::string> containing the respective segments or an empty one of something went wrong
         */
        std::vector<std::string> getCommandLineSegments() {
            #ifdef WINDOWS
                #error getCommandLineSegments not yet implemented for windows!
                std::string commandLine = ::GetCommandLine();

                // TODO: split to array
            #else
                long pid = ::getpid();

                std::ifstream inFile;
                try {
                    inFile.open(std::string("/proc/") + std::to_string(pid) + "/cmdline");

                    if(inFile.fail()) {
                        throw std::exception();
                    }
                } catch (const std::exception& e) {
                    // failed => return empty list
                    return std::vector<std::string>();
                }

                std::stringstream sstr;
                sstr << inFile.rdbuf();

                std::string commandLine = sstr.str();

                std::vector<std::string> commandLineArgs = CLib::Utils::split(commandLine, '\0');

                return commandLineArgs;
            #endif
        }
    }
}

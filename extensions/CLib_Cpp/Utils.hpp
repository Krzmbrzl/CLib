#ifndef CLIB_UTILS
#define CLIB_UTILS

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#ifdef WINDOWS
    #define FILE_SEP '\\'
#else
    #define FILE_SEP '/'
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
        void split ( const std::string inputString, const std::vector<char> delimiter, int count, std::vector<std::string>& output );

        /**
         * Splits a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter A vector of chars containing the characters to split at
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const std::vector<char> delimiter, std::vector<std::string>& output );

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const char delimiter, int count, std::vector<std::string>& output );

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param output The vector to add the produced substrings to
         */
        void split ( const std::string inputString, const char delimiter, std::vector<std::string>& output );

        /**
        * Splits  a string at the given delimiters
        * @param inputString The string to split
        * @param delimiter A vector of chars containing the characters to split at
        * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
        *
        * @return A vector containing the respective substrings
        */
        std::vector<std::string> split ( const std::string inputString, const std::vector<char> delimiter, int count );

        /**
        * Splits a string at the given delimiters
        * @param inputString The string to split
        * @param delimiter A vector of chars containing the characters to split at
        *
        * @return A vector containing the respective substrings
        */
        std::vector<std::string> split ( const std::string inputString, const std::vector<char> delimiter );

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         * @param count The maxmimum number of produced substrings during splitting or a negative value for splitting as often as needed
         *
         * @return A vector containing the respective substrings
         */
        std::vector<std::string> split ( const std::string inputString, const char delimiter, int count );

        /**
         * Splits  a string at the given delimiters
         * @param inputString The string to split
         * @param delimiter The character to split at
         *
         * @return A vector containing the respective substrings
         */
        std::vector<std::string> split ( const std::string inputString, const char delimiter );


        static const char* ws = " \t\n\r\f\v";

        /**
         * Trims the right side of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& rtrim ( std::string& s, const char* t = ws );

        /**
         * Trims the left side of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& ltrim ( std::string& s, const char* t = ws );

        /**
         * Trims the both sides of the given string
         * @param s The string to trim
         * @param ws The characters to trim. If omitted the default whitespace characters will be used
         *
         * @return The trimmed string
         */
        std::string& trim ( std::string& s, const char* t = ws );

        /**
         * Lists all files contained in the given directory
         * @param path The absolute file path to the directory that should be inspected
         * @return A std::vector<std::string> containing all file names
         * 
         * @throws std::invalid_argument if the given path does not point to a directory
         */
        std::vector<std::string> listFiles(const std::string path);

        /**
         * Gets the current working directory of this process
         * @param bufSize The maximum size of the CWD-buffer
         * 
         * @return The respective CWD or an empty string if something went wrong
         */
        std::string getCWD(int bufSize=1024);

        /**
         * Gets the directory in which the executable of this process is located
         * @param bufSize (otpional) The size of the path-buffer to use
         * 
         * @return The path to the respective directory or an empty string if something went wrong
         */
        std::string getExecutableDirectory(int bufSize=1024);

        /**
         * Gets all segments of the command line that has been used to call this process
         * 
         * @return A std::vector<std::string> containing the respective segments or an empty one of something went wrong
         */
        std::vector<std::string> getCommandLineSegments();
    }
}

#endif

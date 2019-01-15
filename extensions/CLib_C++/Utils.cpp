#include "Utils.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

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
                        output.push_back ( inputString.substr ( i + 1, inputString.length() ) );
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
    }
}

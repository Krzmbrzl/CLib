#ifndef CLIB_LOGGER
#define CLIB_LOGGER

#include <string>
#include <fstream>

namespace CLib {
    namespace LogLevel {
        enum ELogLevel {
            ERROR,
            WARNING,
            INFO,
            DEBUG
        };
    }

    class Logger {
        private:
            Logger();
            ~Logger();

            /**
             * The outputstream to the log
             */
            std::ofstream logStream;

            /**
             * The current log-level
             */
            CLib::LogLevel::ELogLevel logLevel = CLib::LogLevel::INFO;
        public:
            /**
             * Gets the instance of this logger
             */
            static CLib::Logger& getInstance() {
                static CLib::Logger loggerInstance;

                return loggerInstance;
            }

            Logger(Logger const&)     = delete;
            void operator=(Logger const&)  = delete;

            /**
             * Logs the message of the given type
             * @param msg The message to log
             * @param level The Clib::Logger::LogLevel corresponding to the message
             */
            void log(const std::string& msg, CLib::LogLevel::ELogLevel level);

            /**
             * Logs the given error message
             * @param msg The message to log
             */
            void error(const std::string& msg) {
                log(msg, CLib::LogLevel::ERROR);
            }

            /**
             * Logs the given warning message
             * @param msg The message to log
             */
            void warning(const std::string& msg) {
                log(msg, CLib::LogLevel::WARNING);
            }

            /**
             * Logs the given info message
             * @param msg The message to log
             */
            void info(const std::string& msg) {
                log(msg, CLib::LogLevel::INFO);
            }

            /**
             * Logs the given debug message
             * @param msg The message to log
             */
            void debugLog(const std::string& msg) {
                log(msg, CLib::LogLevel::DEBUG);
            }

            /**
             * Sets the current log level. All messages with a higher log level won't actually be logged
             * @param level the CLib::LogLevel::ELogLevel to use as the new level
             */
            void setLogLevel(CLib::LogLevel::ELogLevel level) {
                logLevel = level;
            }
    };
}

#endif
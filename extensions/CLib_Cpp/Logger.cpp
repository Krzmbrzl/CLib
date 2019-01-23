#include "Logger.hpp"

#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

#include "Utils.hpp"

CLib::Logger::Logger() {
    std::string dir = "";//CLib::Utils::getExecutableDirectory();
    std::string name = "clib.log";

    if(!dir.empty()) {
        name = dir + name;
    }

    logStream = std::ofstream(name, std::ios_base::app);

    logStream << "--------------------------------- CLib (" << CLIB_VERSION << ") Logger loaded ---------------------------------" << std::endl;

    logStream.flush();
}

CLib::Logger::~Logger() {
    logStream << "--------------------------------- CLib Logger closed ---------------------------------" << std::endl << std::endl;
    logStream.close();
}

void CLib::Logger::log(const std::string& msg, CLib::LogLevel::ELogLevel level) {
    if(level > logLevel) {
        // filter out unwanted log messages
        return;
    }

    // add timestamp
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string strTime(buffer);

    logStream << strTime << " ";

    switch(level) {
        case CLib::LogLevel::ERROR:
            logStream << "[CLib ERROR]: ";
            break;
        case CLib::LogLevel::WARNING:
            logStream << "[CLib WARNING]: ";
            break;
        case CLib::LogLevel::INFO:
            logStream << "[CLib INFO]: ";
            break;
        case CLib::LogLevel::DEBUG:
            logStream << "[CLib DEBUG INFO]: ";
            break;
    }

    logStream << msg << std::endl;
}
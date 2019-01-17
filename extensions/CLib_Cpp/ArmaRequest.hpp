#ifndef CLIB_ARMA_REQUEST
#define CLIB_ARMA_REQUEST

#include <string>
#include <stdexcept>
#include <vector>

#include "ControlCharacter.hpp"
#include "Utils.hpp"

namespace CLib
{

class ArmaRequest
{
  private:
    int TaskId;
    std::string ExtensionName;
    std::string ActionName;
    std::string Data;

  public:
    /**
     * Gets the task ID corresponding to this request
     */
    int getTaskId();

    /**
     * Gets the name of the extension that should be used for handling this request
     */
    std::string getExtensionName();

    /**
     * Gets the name of the action 8function) inside the extension that should be used for this request
     */
    std::string getActionName();

    /**
     * Gets the data meant to be passed to the extension*s action as a parameter
     */
    std::string getData();


    /**
     * Parses the given String into an ArmaRequest
     * @param input The std::string that should be parsed
     * @return The created ArmaRequest
     * 
     * @throws std::invalid_argument in case something's not right with the given string
     */
    static ArmaRequest parse(std::string input);
};
} // namespace CLib

#endif
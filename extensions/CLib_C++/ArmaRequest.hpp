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
    int getTaskId();

    std::string getExtensionName();

    std::string getActionName();

    std::string getData();

    static ArmaRequest parse(std::string input);
};
} // namespace CLib

#endif
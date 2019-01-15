#include <iostream>
#include "Utils.hpp"
#include "ArmaRequest.hpp"
#include "ControlCharacter.hpp"

int main(int argc, char **argv)
{
    std::string test = " I am a completely new test String ";
    std::vector<std::string> elements = CLib::Utils::split(CLib::Utils::trim(test), ' ');

    for (int i = 0; i < elements.size(); i++)
    {
        std::cout << "|" << elements.at(i) << "|" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;
    std::string requestString;
    requestString += CLib::ControlCharacter::SOH;
    requestString += "2";
    requestString += CLib::ControlCharacter::US;
    requestString += "MyTextExtension";
    requestString += CLib::ControlCharacter::US;
    requestString += "MySampleAction";
    requestString += CLib::ControlCharacter::STX;
    requestString += "Some text here";
    requestString += CLib::ControlCharacter::ETX;

    CLib::ArmaRequest request = CLib::ArmaRequest::parse(requestString);

    std::cout << "----------------------------------------------" << std::endl;

    return 0;
}

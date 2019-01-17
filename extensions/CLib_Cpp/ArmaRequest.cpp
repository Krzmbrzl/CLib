#include "ArmaRequest.hpp"

#include <string>
#include <stdexcept>
#include <vector>

#include "ControlCharacter.hpp"
#include "Utils.hpp"

namespace CLib {

	int ArmaRequest::getTaskId() {
		return TaskId;
	}

	std::string ArmaRequest::getExtensionName() {
		return ExtensionName;
	}
	
	std::string ArmaRequest::getActionName() {
		return ActionName;
	}
	
	std::string ArmaRequest::getData() {
		return Data;
	}
	


	ArmaRequest ArmaRequest::parse(std::string input)
	{
		if(input.empty()) {
			throw std::invalid_argument("Can't create ArmaRequest from empty input!");
		}
		int headerStart = input.find(ControlCharacter::SOH);
		int textStart = input.find(ControlCharacter::STX);
		int textEnd = input.find(ControlCharacter::ETX);

		if(textStart >= 0 && textEnd < 0) {
			throw std::invalid_argument("Missing ETX-symbol!");
		}
		if(textStart >= 0 && textEnd < textStart) {
			throw std::invalid_argument("ETX-symbol before STX!");
		}

		std::string header = input.substr(headerStart < 0 ? 0 : headerStart + 1, (textStart < 0 ? input.length() : textStart) - 1);
		std::vector<std::string> headerValues = CLib::Utils::split(header, ControlCharacter::US, 3);

		if(headerValues.size() < 3) {
			throw std::invalid_argument("Expected 3 header elements but got " + headerValues.size());
		}

		ArmaRequest request = ArmaRequest();
		int taskId;
		try {
			taskId = std::stoi(headerValues[0]);
		} catch (...) {
			throw std::invalid_argument("Invalid task id: " + headerValues[0]);
		}
		
		request.TaskId = taskId;
		request.ExtensionName = CLib::Utils::trim(headerValues[1]);
		request.ActionName = CLib::Utils::trim(headerValues[2]);
		request.Data = textStart < 0 ? "" : input.substr(textStart + 1, textEnd - textStart - 1);

		return request;
	}
} // namespace CLib
#include "ArmaExtension.hpp"

#include <string>
#include <stdexcept>

namespace CLib {
    libraryHandle loadExtension(const char* extensionPath) {
# ifdef _WIN32
        return ::LoadLibraryA(extensionPath);
# else //_WIN32
        return ::dlopen(extensionPath, RTLD_LAZY);
# endif //_WIN32
    }

    int unloadExtension(libraryHandle hMyLib) {
# ifdef _WIN32
        return ::FreeLibrary(hMyLib);
# else //_WIN32
        return ::dlclose(hMyLib);
# endif //_WIN32
    }

    void* getFunctionPointer(libraryHandle hMyLib, const char* szMyProc) {
# ifdef _WIN32
        return ::GetProcAddress(hMyLib, szMyProc);
# else //_WIN32
        return ::dlsym(hMyLib, szMyProc);
# endif //_WIN32
    }


    CLib::ArmaExtension::ArmaExtension(std::string path) {
        extension = loadExtension(path.c_str());

        if(!extension){
            throw std::invalid_argument("Unable to load extension " + path);
        }

        #ifdef _WIN32
            char sep = '\\';
        #else
            char sep = '/';
        #endif

        CLib::ArmaExtension::path = path;
        int index = path.find_last_of(sep);

        CLib::ArmaExtension::name = (index < 0 ? path : path.substr(index + 1, path.length()));
    }

    CLib::ArmaExtension::~ArmaExtension() {
        unloadExtension(extension);
    }

    std::string CLib::ArmaExtension::call(std::string functionName, std::string parameter) {
        void* ptr = getFunctionPointer(extension, functionName.c_str());

        if (!ptr) {
            throw std::invalid_argument("Unable to find a function with the name '" + functionName + "' in " + CLib::ArmaExtension::path + "!");
        }

        typedef const char* (*ArmaExtensionFunction)(const char*);

        ArmaExtensionFunction func = (ArmaExtensionFunction) ptr;

        std::string returnValue = func(parameter.c_str());

        return returnValue;
    }

    bool CLib::ArmaExtension::containsFunction(std::string functionName) {
        return getFunctionPointer(extension, functionName.c_str());
    }

    std::string CLib::ArmaExtension::getName() {
        return CLib::ArmaExtension::name;
    }

    std::string CLib::ArmaExtension::getPath() {
        return CLib::ArmaExtension::path;
    }

    bool CLib::ArmaExtension::isValid() {
        return extension;
    }
} // namespace CLib
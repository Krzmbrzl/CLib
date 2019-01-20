#include "ArmaExtension.hpp"

#include <string>
#include <stdexcept>

namespace CLib {
    libraryHandle loadExtension(const char* extensionPath) {
# ifdef WINDOWS
        return ::LoadLibraryA(extensionPath);
# else //WINDOWS
        return ::dlopen(extensionPath, RTLD_LAZY);
# endif //WINDOWS
    }

    int unloadExtension(libraryHandle hMyLib) {
# ifdef WINDOWS
        return ::FreeLibrary(hMyLib);
# else //WINDOWS
        return ::dlclose(hMyLib);
# endif //WINDOWS
    }

    void* getFunctionPointer(libraryHandle hMyLib, const char* szMyProc) {
# ifdef WINDOWS
        return ::GetProcAddress(hMyLib, szMyProc);
# else //WINDOWS
        return ::dlsym(hMyLib, szMyProc);
# endif //WINDOWS
    }

    char * getError() {
        #ifdef WINDOWS
            return GetLastError();
        #else
            return dlerror();
        #endif
    }


    CLib::ArmaExtension::ArmaExtension(std::string path) {
        extension = loadExtension(path.c_str());

        if(!extension){
            throw std::invalid_argument("Unable to load extension " + path + "\n" + "Reason: " + getError());
        }

        #ifdef WINDOWS
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

    CLib::ArmaExtensionFunction CLib::ArmaExtension::getFunction(std::string name) {
        return (ArmaExtensionFunction) getFunctionPointer(extension, name.c_str());
    }
} // namespace CLib
#ifndef CLIB_ARMA_EXTENSION
#define CLIB_ARMA_EXTENSION

#include <string>

#ifdef _WIN32
    #include <windows.h>
    typedef HANDLE libraryHandle;
#else
    #include <dlfcn.h>
    typedef void* libraryHandle;
#endif

namespace CLib{

    class ArmaExtension {
        private:
        libraryHandle extension;
        std::string name;
        std::string path;

        public:
        /**
         * Creates a new instance of this class on the library at the given path
         * @param A string containing the path to the extension on the hard drive
         * 
         * @throws std::invalid_argument When the given path doesn't point to a library
         */
        ArmaExtension(std::string path);

        /**
         * Destroys this instance and frees the respective library
         */
        ~ArmaExtension();

        /**
         * Calls the function with the given name and the given parameter. Note that the given function is
         * assumed to return a const char* and take another const char* as the only parameter!
         * @param functionName The name of the function inside this extension to call
         * @param parameter The parameter that should be passed to the called function
         * @return The return value of the called function
         * 
         * @throws std::invalid_argument when the given function can't be found in this extension
         */
        std::string call(std::string functionName, std::string parameter = "");

        /**
         * Checks whether a function with the given name exists inside this extension
         * @param functionName The name of the function to check for
         */
        bool containsFunction(std::string functionName);

        /**
         * Gets the name of this extension
         */
        std::string getName();

        /**
         * Gets the absolute path of this extension
         */
        std::string getPath();

        /**
         * Checks if this extension is valid
         */
        bool isValid();
    };
} // namespace CLib

#endif
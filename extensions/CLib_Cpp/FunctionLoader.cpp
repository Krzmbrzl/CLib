#ifdef _WIN32
#include <windows.h>
typedef HANDLE library_handle;
#else
#include <dlfcn.h>
typedef void *library_handle;
#endif

#include <string>
#include <vector>
#include <stdexcept>

namespace CLib
{
namespace FunctionLoader
{
    library_handle loadLibrary(const char *szMyLib)
    {
#ifdef _WIN32
        return ::LoadLibraryA(szMyLib);
#else  //_WIN32
        return ::dlopen(szMyLib, RTLD_LAZY);
#endif //_WIN32
    }

    int unloadLibrary(library_handle hMyLib)
    {
#ifdef _WIN32
        return ::FreeLibrary(hMyLib);
#else  //_WIN32
        return ::dlclose(hMyLib);
#endif //_WIN32
    }

    void *MyLoadProc(library_handle hMyLib, const char *szMyProc)
    {
#ifdef _WIN32
        return ::GetProcAddress(hMyLib, szMyProc);
#else  //_WIN32
        return ::dlsym(hMyLib, szMyProc);
#endif //_WIN32
    }

    class Lib
    {
        private:
        library_handle handle;

        public:
        Lib(std::string path) {
            handle = CLib::FunctionLoader::loadLibrary(path.c_str());

            if (!handle) {
                throw std::invalid_argument("Unable to load the library " + path);
            }
        }

        ~Lib() {
            if (!handle) {
                CLib::FunctionLoader::unloadLibrary(handle);
            }
        }

        library_handle getHandle() {
            return handle;
        }
    };

    /**
     * Tries to read the export table of the library at the specified path
     * @param libPath The std::string contiainging the path to the library
     * @return A vector containing all functions exported by the given dll
     * @throw std::incalud_argument if the given dll is not accessible
     */
    static std::vector<std::string> ExportTable(std::string libPath)
    {
        CLib::FunctionLoader::Lib library = CLib::FunctionLoader::Lib(libPath);
    }

/*    static T LoadFunction<T>(string dllPath, string functionName)
    {
        var hModule = LoadLibraryEx(dllPath, IntPtr.Zero, 0);
        if (hModule == IntPtr.Zero)
            throw new ArgumentException($ "Dll not found: {dllPath} - Error code: {GetLastError()}");

        var functionAddress = GetProcAddress(hModule, functionName);
        if (functionAddress == IntPtr.Zero)
            throw new ArgumentException($ "Function not found: {functionName} - Error code: {GetLastError()}");

        return Marshal.GetDelegateForFunctionPointer<T>(functionAddress);
    }*/
};
} // namespace CLib
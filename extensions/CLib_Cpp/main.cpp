#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dlfcn.h>
typedef void* libraryHandle;

int main(int argc, char **argv)
{

    struct passwd *pw = getpwuid(getuid());

    std::string homedir = pw->pw_dir;
    std::string path = homedir + "/Documents/Git/CLib/extensions/CLib_Cpp/build/clib.so";

    libraryHandle handle = ::dlopen(path.c_str(), RTLD_LAZY);

    // std::cout << ::dlerror() << std::endl;

    void* funcPtr = ::dlsym(handle, "RVExtension");
    
    if(funcPtr == 0) {
        std::cout << "Unable to open function" << std::endl;
    } else {
        typedef void(*Function)(char *, int, const char *);

        Function func = (Function) funcPtr;

        char response[120];

        func(response, 120, "Hello");
    }

    if(handle != 0) {
        ::dlclose(handle);
    }

    return 0;
}

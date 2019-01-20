#include <iostream>
#include <string>
#include <dlfcn.h>
typedef void* libraryHandle;

int main(int argc, char **argv)
{
    libraryHandle handle = ::dlopen("/home/robert/Documents/Git/CLib/extensions/CLib_Cpp/build/libclib.so", RTLD_LAZY);

    std::cout << ::dlerror() << std::endl;

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

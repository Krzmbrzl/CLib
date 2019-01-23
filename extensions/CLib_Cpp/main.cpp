#include <iostream>
#include <string>
#include <unistd.h>
#include <dlfcn.h>
typedef void* libraryHandle;

int main(int argc, char **argv)
{
    char buffer[1024];
    // char *answer = getcwd(buffer, sizeof(buffer));
    int size = readlink("/proc/self/exe", buffer, 1024);

    if(size < 5) {
        return 1;
    }

    // remove "main"
    size -= 4;
    char answerBuffer[size + 1];
    for(int i=0; i<size; i++) {
        answerBuffer[i] = buffer[i];
    }
    answerBuffer[size] = '\0';
    char * answer = &(answerBuffer[0]);
    std::string cwd;
    if (answer)
    {
      cwd = answer;
    } else {
        throw std::invalid_argument("Unable to get the cwd of this process!");
    }

    if (cwd[cwd.length() - 1] != '/') {
        cwd += '/';
    }

    std::cout << "Executable dir: " << cwd << std::endl;

    std::string path = cwd + "clib.so";

    std::cout << "CLib SO at: " << path << std::endl;

    
    libraryHandle handle = ::dlopen(path.c_str(), RTLD_LAZY);

    if (handle != 0) {
        void* funcPtr = ::dlsym(handle, "RVExtension");
        
        if(funcPtr == 0) {
            std::cout << "Unable to open function" << std::endl;
        } else {
            typedef void(*Function)(char *, int, const char *);

            Function func = (Function) funcPtr;

            int length = 120;

            char response[length];

            func(response, length, "debugger");

            std::cout << "Arma response:" << std::endl;

            for(int i=0; i<length; i++) {
                if(response[i] == 0) {
                    break;
                } else {
                    std::cout << response[i];
                }
            }

            std::cout << std::endl << "End of Arma response" << std::endl;
        }

        ::dlclose(handle);
    } else {
        std::cout << "Unable to open lib: " << ::dlerror() << std::endl;
    }

    return 0;
}

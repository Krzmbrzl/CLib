#ifndef CLIB_DllEntry
#define CLIB_DllEntry

#ifdef WINDOWS
    #define ARMA_API __declspec (dllexport)
#else
    #define ARMA_API
#endif

/**
 * Entry point for getting the version of this entry on extension load
 */
extern "C"
ARMA_API void RVExtensionVersion(char *output, int outputSize);

/**
 * Entry point for STRING callExtension STRING
 */
extern "C"
ARMA_API void RVExtension(char *output, int outputSize, const char *function);

/**
 * Entry point for STRING callExtension ARRAY
 */
extern "C"
ARMA_API int RVExtensionArgs(char *output, int outputSize, const char *function, const char **argv, int argc);


#endif
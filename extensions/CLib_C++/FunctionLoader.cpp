#include <vector>
#include <string>
#include <stdexcept>

namespace CLib
{
class FunctionLoader
{
    static std::vector<std::string> ExportTable(std::string dllPath)
    {
        auto hModule = LoadLibraryEx(dllPath, IntPtr.Zero, LoadLibraryFlags.DONT_RESOLVE_DLL_REFERENCES);
        if (hModule == IntPtr.Zero)
            throw new Win32Exception(GetLastError());

        try
        {
            auto imageDosHeader = Marshal.PtrToStructure<IMAGE_DOS_HEADER>(hModule);
            if (!imageDosHeader.IsValid)
                throw std::invalid_argument("IMAGE_DOS_HEADER is invalid: " + dllPath);

#if WIN64
            auto imageNtHeaders = Marshal.PtrToStructure<IMAGE_NT_HEADERS64>(hModule + imageDosHeader.e_lfanew);
#else
            auto imageNtHeaders = Marshal.PtrToStructure<IMAGE_NT_HEADERS32>(hModule + imageDosHeader.e_lfanew);
#endif
            if (!imageNtHeaders.IsValid)
                throw std::invalid_argument("IMAGE_NT_HEADERS is invalid: " + dllPath);

            IMAGE_DATA_DIRECTORY exportTabledataDirectory = imageNtHeaders.OptionalHeader.ExportTable;
            if (exportTabledataDirectory.Size == 0)
                return new List<string>();

            auto exportTable =
                Marshal.PtrToStructure<IMAGE_EXPORT_DIRECTORY>(hModule + exportTabledataDirectory.VirtualAddress);

            auto names = new List<string>();
            for (auto i = 0; i < exportTable.NumberOfNames; i++)
            {
                auto name = Marshal.PtrToStringAnsi(hModule + Marshal.ReadInt32(hModule + exportTable.AddressOfNames + (i * 4)));
                if (name)
                    continue;

                names.Add(name);
            }

            return names;
        } catch (...) {
            // TODO: handle exceptions
        };

        FreeLibrary(hModule);
    }

    static T LoadFunction<T>(string dllPath, string functionName)
    {
        var hModule = LoadLibraryEx(dllPath, IntPtr.Zero, 0);
        if (hModule == IntPtr.Zero)
            throw new ArgumentException($ "Dll not found: {dllPath} - Error code: {GetLastError()}");

        var functionAddress = GetProcAddress(hModule, functionName);
        if (functionAddress == IntPtr.Zero)
            throw new ArgumentException($ "Function not found: {functionName} - Error code: {GetLastError()}");

        return Marshal.GetDelegateForFunctionPointer<T>(functionAddress);
    }
};
} // namespace CLib
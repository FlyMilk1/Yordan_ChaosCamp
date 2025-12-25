#pragma once
#include <string>
#include <windows.h>
#include <dxcapi.h>
#include <comdef.h>
#include <wrl/client.h>
#include <iostream>


#define MAKE_SMART_COM_POINTER(_a) _COM_SMARTPTR_TYPEDEF(_a, __uuidof(_a))

MAKE_SMART_COM_POINTER(IDxcUtils);
MAKE_SMART_COM_POINTER(IDxcCompiler3);
MAKE_SMART_COM_POINTER(IDxcBlobEncoding);
MAKE_SMART_COM_POINTER(IDxcOperationResult);
MAKE_SMART_COM_POINTER(IDxcBlob);
MAKE_SMART_COM_POINTER(IDxcIncludeHandler);
MAKE_SMART_COM_POINTER(IDxcResult);
MAKE_SMART_COM_POINTER(IDxcBlobUtf8);

class ShaderCompiler
{
public:
    /// <summary>
    /// Compiles a DXC shader at runtime from file using DXC DLL.
    /// </summary>
    static IDxcBlobPtr compileShaders(
        const std::wstring& fileName,
        const std::wstring& entryPoint,
        const std::wstring& target
    );
};

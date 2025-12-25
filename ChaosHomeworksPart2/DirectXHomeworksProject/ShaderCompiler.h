#pragma once
#include <string>
#include <windows.h>
#include <dxc/dxcapi.h>
#include <comdef.h>
#include <wrl/client.h>
#include <iostream>
#include <dxc/dxcapi.use.h>


#define MAKE_SMART_COM_POINTER(_a) _COM_SMARTPTR_TYPEDEF(_a, __uuidof(_a))

MAKE_SMART_COM_POINTER(IDxcLibrary);
MAKE_SMART_COM_POINTER(IDxcCompiler);
MAKE_SMART_COM_POINTER(IDxcBlobEncoding);
MAKE_SMART_COM_POINTER(IDxcOperationResult);
MAKE_SMART_COM_POINTER(IDxcBlob);

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

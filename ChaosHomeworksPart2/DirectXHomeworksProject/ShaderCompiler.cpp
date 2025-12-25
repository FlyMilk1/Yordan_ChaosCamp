#pragma comment(lib, "dxcompiler.lib")
#include "ShaderCompiler.h"

#include <assert.h>
#include <iostream>
IDxcBlobPtr ShaderCompiler::compileShaders(const std::wstring& fileName, const std::wstring& entryPoint, const std::wstring& target)
{
    dxc::DXCLibraryDllLoader support;
    if (FAILED(support.Initialize())) {
        std::cerr << "Failed to initialize DXC support\n";
        return nullptr;
    }

    IDxcLibraryPtr library;
    IDxcCompilerPtr compiler;
    IDxcBlobEncodingPtr sourceBlob;

    HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    assert(SUCCEEDED(hr));
    hr = library->CreateBlobFromFile(fileName.c_str(), nullptr, &sourceBlob);
    assert(SUCCEEDED(hr));

    LPCWSTR args[] = {
        fileName.c_str(),
        L"-E", entryPoint.c_str(),
        L"-T", target.c_str(),
        L"-Zi", L"-Qembed_debug", L"-Od", L"-Zpr"
    };

    IDxcOperationResultPtr result;
    hr = compiler->Compile(
        sourceBlob,
        fileName.c_str(),
        entryPoint.c_str(),
        target.c_str(),
        args,
        _countof(args),
        nullptr,
        0,
        nullptr,
        &result
    );
    assert(SUCCEEDED(hr));
    IDxcBlobPtr shaderBlob;
    hr = result->GetResult(&shaderBlob);
    assert(SUCCEEDED(hr));
    return shaderBlob;
}

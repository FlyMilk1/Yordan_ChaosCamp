#pragma comment(lib, "dxcompiler.lib")

#include "ShaderCompiler.h"
#include <assert.h>
#include <iostream>
#include <dxcapi.h>

IDxcBlobPtr ShaderCompiler::compileShaders(
    const std::wstring& fileName,
    const std::wstring& entryPoint,
    const std::wstring& target)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE);

    assert(LoadLibraryW(L"dxcompiler.dll"));
    assert(LoadLibraryW(L"dxil.dll"));

    IDxcUtilsPtr utils;
    IDxcCompiler3Ptr compiler;

    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
    assert(SUCCEEDED(hr));

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    assert(SUCCEEDED(hr));

    IDxcBlobEncodingPtr sourceBlob;
    hr = utils->LoadFile(fileName.c_str(), nullptr, &sourceBlob);
    assert(SUCCEEDED(hr));

    DxcBuffer sourceBuffer{};
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();
    sourceBuffer.Encoding = DXC_CP_UTF8;

    LPCWSTR args[] =
    {
        fileName.c_str(),
        L"-E", entryPoint.c_str(),
        L"-T", target.c_str(),
        L"-Zi",
        L"-Qembed_debug",
        L"-Od",
        L"-Zpr"
    };

    IDxcIncludeHandlerPtr includeHandler;
    hr = utils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

    IDxcResultPtr result;
    hr = compiler->Compile(
        &sourceBuffer,
        args,
        _countof(args),
        includeHandler,
        IID_PPV_ARGS(&result)
    );
    assert(SUCCEEDED(hr));

    IDxcBlobUtf8Ptr errors;
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
    if (errors && errors->GetStringLength() > 0)
    {
        std::cerr << errors->GetStringPointer() << std::endl;
        assert(false);
    }

    IDxcBlobPtr shaderBlob;
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));

    return shaderBlob;
}

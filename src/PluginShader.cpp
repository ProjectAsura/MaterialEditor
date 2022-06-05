//-----------------------------------------------------------------------------
// File : PluginShader.cpp
// Desc : Plugin Shader.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <PluginMgr.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <asdxRenderState.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxDeviceContext.h>
#include <ctime>


namespace {

//-----------------------------------------------------------------------------
//      時刻を文字列として取得します.
//-----------------------------------------------------------------------------
std::string GetTimeString()
{
    auto t = time(nullptr);
    tm local = {};
    localtime_s(&local, &t);

    char buf[128] = {};
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &local);

    return std::string(buf);
}

//-----------------------------------------------------------------------------
//      エラーシェーダを出力します.
//-----------------------------------------------------------------------------
void OutputErrorShader(const char* sourceCode, size_t size)
{
    std::string path = "ErrorShader_";
    path += GetTimeString();
    path += ".hlsl";

    FILE* file;
    auto err = fopen_s(&file, path.c_str(), "w");
    if (err != 0)
    { return; }

    fwrite(sourceCode, size, 1, file);
    fclose(file);
}


} // namespace


///////////////////////////////////////////////////////////////////////////////
// PluginShader class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
PluginShader::PluginShader()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
PluginShader::~PluginShader()
{ Term(); }

//-----------------------------------------------------------------------------
//      コンパイルします.
//-----------------------------------------------------------------------------
bool PluginShader::Compile(const char* sourceCode, size_t size, const char* entryPoint)
{
    if (sourceCode == nullptr || size == 0 || entryPoint == nullptr)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

#if defined(DEBUG) || defined(_DEBUG)
    UINT compileFlag = D3DCOMPILE_DEBUG;
#else
    UINT compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    asdx::RefPtr<ID3DBlob> pBlob;
    asdx::RefPtr<ID3DBlob> pErrorBlob;

    // ピクセルシェーダをロード.
    auto hr = D3DCompile(
        sourceCode,
        size,
        nullptr,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        "ps_5_0",
        compileFlag,
        0,
        pBlob.GetAddress(),
        pErrorBlob.GetAddress());

    if (FAILED(hr))
    {
        ELOGA("Error : D3DCompileFromFile() Failed. errcode = 0x%x, msg = %s",
            hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
    #if defined(DEBUG) || defined(_DEBUG)
        OutputErrorShader(sourceCode, size);
    #endif
        return false;
    }

    asdx::RefPtr<ID3D11ShaderReflection> pReflection;

    // シェーダリフレクション生成.
    hr = D3DReflect(
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        IID_PPV_ARGS(pReflection.GetAddress()));
    if (FAILED(hr))
    {
        ELOGA("Error : D3DReflect() Failed. errcode = 0x%x", hr);
        return false;
    }

    // 辞書を作成.
    {
        D3D11_SHADER_DESC shaderDesc = {};
        hr = pReflection->GetDesc(&shaderDesc);
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11ShaderReflection::GetDesc() Failed. errcode = 0x%x", hr);
            return false;
        }

        for(auto i=0u; i<shaderDesc.ConstantBuffers; ++i)
        {
            auto reflectionCB = pReflection->GetConstantBufferByIndex(i);
            if (pReflection == nullptr)
            { continue; }

            D3D11_SHADER_BUFFER_DESC bufferDesc = {};
            hr = reflectionCB->GetDesc(&bufferDesc);
            if (FAILED(hr))
            { continue; }

            BufferInfo info;
            info.BufferSize = bufferDesc.Size;

            for(auto j=0u; j<bufferDesc.Variables; ++j)
            {
                auto reflectionVariable = reflectionCB->GetVariableByIndex(j);
                if (reflectionVariable == nullptr)
                { continue; }

                D3D11_SHADER_VARIABLE_DESC variableDesc = {};
                hr = reflectionVariable->GetDesc(&variableDesc);
                if (FAILED(hr))
                { continue; }

                MemberInfo member;
                member.Offset = variableDesc.StartOffset;
                member.Size   = variableDesc.Size;
                info.MemberTable[variableDesc.Name] = member;
            }

            m_BufferInfo[bufferDesc.Name] = info;
        }

        for(auto i=0u; i<shaderDesc.BoundResources; ++i)
        {
            D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
            hr = pReflection->GetResourceBindingDesc(i, &bindDesc);
            if (FAILED(hr))
            { continue; }

            switch(bindDesc.Type)
            {
            case D3D_SIT_CBUFFER:
            case D3D_SIT_TBUFFER:
                m_TableCBV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;

            case D3D_SIT_TEXTURE:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                m_TableSRV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;

            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                m_TableUAV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;
            }
        }
    }

    auto pDevice = asdx::DeviceContext::Instance().GetDevice();
    hr = pDevice->CreatePixelShader(
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        nullptr,
        m_PS.GetAddress());
    if (FAILED(hr))
    {
        ELOGA("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
        return false;
    }

    m_EntryPoint = entryPoint;

    return true;
}

//-----------------------------------------------------------------------------
//      解放処理を行います.
//-----------------------------------------------------------------------------
void PluginShader::Term()
{
    m_BufferInfo    .clear();
    m_TableCBV      .clear();
    m_TableSRV      .clear();
    m_TableUAV      .clear();
    m_PS            .Reset();
}

//-----------------------------------------------------------------------------
//      シェーダを設定します.
//-----------------------------------------------------------------------------
void PluginShader::Bind(ID3D11DeviceContext* pContext) const
{
    pContext->PSSetShader(m_PS.GetPtr(), nullptr, 0);

    // サンプラー設定.
    ID3D11SamplerState* pSmps[] = {
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointClamp),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointWrap),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointMirror),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearWrap),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearMirror),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::AnisotropicClamp),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::AnisotropicWrap),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::AnisotropicMirror)
    };
    pContext->PSSetSamplers(0, _countof(pSmps), pSmps);
}

//-----------------------------------------------------------------------------
//      シェーダの設定を解除します.
//-----------------------------------------------------------------------------
void PluginShader::Unbind(ID3D11DeviceContext* pContext) const
{
    ID3D11SamplerState* pSmps[] = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };

    ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];
    for(auto i=0; i<D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT; ++i)
    { pSRVs[i] = nullptr; }

    pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT, pSRVs);
    pContext->PSSetSamplers(0, _countof(pSmps), pSmps);
    pContext->PSSetShader(nullptr, nullptr, 0);
}

//-----------------------------------------------------------------------------
//      外部定数バッファを設定します.
//-----------------------------------------------------------------------------
void PluginShader::SetCBV
(
    ID3D11DeviceContext*    pContext,
    const char*             name,
    ID3D11Buffer*           pCB
) const
{
    if (m_TableCBV.find(name) == m_TableCBV.end())
    { return; }

    auto slot = m_TableCBV.at(name);
    pContext->PSSetConstantBuffers(slot, 1, &pCB);
}

//-----------------------------------------------------------------------------
//      外部テクスチャを設定します.
//-----------------------------------------------------------------------------
void PluginShader::SetSRV
(
    ID3D11DeviceContext*        pContext,
    const char*                 name,
    ID3D11ShaderResourceView*   pSRV
) const
{
    if (m_TableSRV.find(name) == m_TableSRV.end())
    { return; }

    auto slot = m_TableSRV.at(name);
    pContext->PSSetShaderResources(slot, 1, &pSRV);
}


const std::map<std::string, uint8_t>& PluginShader::GetTableCBV() const
{ return m_TableCBV; }

const std::map<std::string, uint8_t>& PluginShader::GetTableSRV() const
{ return m_TableSRV; }

const std::map<std::string, uint8_t>& PluginShader::GetTableUAV() const
{ return m_TableUAV; }

const std::map<std::string, PluginShader::BufferInfo>& PluginShader::GetBufferInfo() const
{ return m_BufferInfo; }

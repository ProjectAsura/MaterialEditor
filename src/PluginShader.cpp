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
//      ロードします.
//-----------------------------------------------------------------------------
bool PluginShader::Load(const char* path)
{
    if (path == nullptr)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    auto temp = asdx::ToStringW(path);

    std::wstring wpath;
    if (!asdx::SearchFilePathW(temp.c_str(), wpath))
    {
        ELOGA("Error : File Not Found. path = %s", path);
        return false;
    }

    UINT compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;

    asdx::RefPtr<ID3DBlob> pBlob;
    asdx::RefPtr<ID3DBlob> pErrorBlob;

    // ピクセルシェーダをロード.
    auto hr = D3DCompileFromFile(
        wpath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlag,
        0,
        pBlob.GetAddress(),
        pErrorBlob.GetAddress());

    if (FAILED(hr))
    {
        ELOGA("Error : D3DCompileFromFile() Failed. errcode = 0x%x, msg = %s",
            hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
        return false;
    }

    asdx::RefPtr<ID3D11ShaderReflection> pReflection;

    // シェーダリフレクション生成.
    hr = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(pReflection.GetAddress()));
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

            m_BufferTable[bufferDesc.Name] = info;
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
    hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_PS.GetAddress());
    if (FAILED(hr))
    {
        ELOGA("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
        return false;
    }

    if (m_BufferTable.find("CbUser") != m_BufferTable.end())
    {
        auto info = m_BufferTable["CbUser"];

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth  = info.BufferSize;
        desc.Usage      = D3D11_USAGE_DYNAMIC;
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = pDevice->CreateBuffer(&desc, nullptr, m_EditableCB.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreateBuffer() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      解放処理を行います.
//-----------------------------------------------------------------------------
void PluginShader::Term()
{
    m_BufferTable   .clear();
    m_TableCBV      .clear();
    m_TableSRV      .clear();
    m_TableUAV      .clear();

    m_PS        .Reset();
    m_EditableCB.Reset();
}

//-----------------------------------------------------------------------------
//      シェーダを設定します.
//-----------------------------------------------------------------------------
void PluginShader::Bind(ID3D11DeviceContext* pContext) const
{
    pContext->PSSetShader(m_PS.GetPtr(), nullptr, 0);

    // サンプラー設定.
    ID3D11SamplerState* pSmps[] = {
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointWrap),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointClamp),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::PointMirror),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearWrap),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp),
        asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearMirror),
    };
    pContext->PSSetSamplers(0, _countof(pSmps), pSmps);
}

//-----------------------------------------------------------------------------
//      定数バッファを更新します.
//-----------------------------------------------------------------------------
void PluginShader::Update
(
    ID3D11DeviceContext*    pContext,
    const MaterialInstance* instance
)
{
    if (m_BufferTable.find("CbUser") == m_BufferTable.end())
    { return; }

    auto bufferSlot = m_TableCBV["CbUser"];

    D3D11_MAPPED_SUBRESOURCE res = {};
    auto pCB = m_EditableCB.GetPtr();
    auto hr = pContext->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    if (FAILED(hr))
    { return; }

    auto ptr = reinterpret_cast<uint8_t*>(res.pData);

    for(size_t i=0; i<instance->m_Bool.size(); ++i)
    {
        const auto& info = instance->m_Bool[i];
        auto val = info.Param.GetValue() ? 1 : 0;
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Int.size(); ++i)
    {
        const auto& info = instance->m_Int[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float.size(); ++i)
    {
        const auto& info = instance->m_Float[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float2.size(); ++i)
    {
        const auto& info = instance->m_Float2[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float3.size(); ++i)
    {
        const auto& info = instance->m_Float3[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float4.size(); ++i)
    {
        const auto& info = instance->m_Float4[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Color3.size(); ++i)
    {
        const auto& info = instance->m_Color3[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Color4.size(); ++i)
    {
        const auto& info = instance->m_Color4[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Bit32.size(); ++i)
    {
        const auto& info = instance->m_Bit32[i];
        auto val = info.Param.GetValue();
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    pContext->Unmap(m_EditableCB.GetPtr(), 0);

    pContext->PSSetConstantBuffers(bufferSlot, 1, &pCB);

    for(size_t i=0; i<instance->m_Texture2D.size(); ++i)
    {
        const auto& info = instance->m_Texture2D[i];
        ID3D11ShaderResourceView* pSRV = info.Param.GetSRV();
        if (pSRV == nullptr)
        { pSRV = PluginMgr::Instance().GetDefaultSRV(info.Default); }

        pContext->PSSetShaderResources(info.Register, 1, &pSRV);
    }
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
        nullptr
    };
    pContext->PSSetSamplers(0, _countof(pSmps), pSmps);
    pContext->PSSetShader(nullptr, nullptr, 0);
}

//-----------------------------------------------------------------------------
//      外部定数バッファを設定します.
//-----------------------------------------------------------------------------
void PluginShader::SetCBV
(
    ID3D11DeviceContext*    pContext,
    const std::string&      name,
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
    const std::string&          name,
    ID3D11ShaderResourceView*   pSRV
) const
{
    if (m_TableSRV.find(name) == m_TableSRV.end())
    { return; }

    auto slot = m_TableSRV.at(name);
    pContext->PSSetShaderResources(slot, 1, &pSRV);
}

//-----------------------------------------------------------------------------
//      メンバー情報を検索します.
//-----------------------------------------------------------------------------
bool PluginShader::FindMemberInfo
(
    const std::string&  bufferName,
    const std::string&  memberName,
    MemberInfo&         result
) const
{
    if (m_BufferTable.find(bufferName) == m_BufferTable.end())
    { return false; }

    auto info = m_BufferTable.at(bufferName);
    if (info.MemberTable.find(memberName) == info.MemberTable.end())
    { return false; }

    result = info.MemberTable[memberName];
    return true;
}

//-----------------------------------------------------------------------------
//      テクスチャスロットを検索します.
//-----------------------------------------------------------------------------
bool PluginShader::FindTextureSlot(const std::string& textureName, uint8_t& slot) const
{
    if (m_TableSRV.find(textureName) == m_TableSRV.end())
    { return false; }

    slot = m_TableSRV.at(textureName);
    return true;
}

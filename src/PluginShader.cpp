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


namespace {

//-----------------------------------------------------------------------------
//      変換を考慮して値を取得します.
//-----------------------------------------------------------------------------
float GetValue(const UiFloat& info)
{
    switch (info.Converter)
    {
    case CONVERTER_NONE:
    default:
        return info.Param.GetValue();

    case CONVERTER_RADIAN:
        return asdx::ToRadian(info.Param.GetValue());

    case CONVERTER_DEGREE:
        return asdx::ToDegree(info.Param.GetValue());
    }
}

//-----------------------------------------------------------------------------
//      変換を考慮して値を取得します.
//-----------------------------------------------------------------------------
asdx::Vector2 GetValue(const UiFloat2& info)
{
    switch (info.Converter)
    {
    case CONVERTER_NONE:
    default:
        return info.Param.GetValue();

    case CONVERTER_RADIAN:
        return asdx::Vector2(
            asdx::ToRadian(info.Param.GetValue().x),
            asdx::ToRadian(info.Param.GetValue().y));

    case CONVERTER_DEGREE:
        return asdx::Vector2(
            asdx::ToDegree(info.Param.GetValue().x),
            asdx::ToDegree(info.Param.GetValue().y));
    }
}

//-----------------------------------------------------------------------------
//      変換を考慮して値を取得します.
//-----------------------------------------------------------------------------
asdx::Vector3 GetValue(const UiFloat3& info)
{
    switch (info.Converter)
    {
    case CONVERTER_NONE:
    default:
        return info.Param.GetValue();

    case CONVERTER_RADIAN:
        return asdx::Vector3(
            asdx::ToRadian(info.Param.GetValue().x),
            asdx::ToRadian(info.Param.GetValue().y),
            asdx::ToRadian(info.Param.GetValue().z));

    case CONVERTER_DEGREE:
        return asdx::Vector3(
            asdx::ToDegree(info.Param.GetValue().x),
            asdx::ToDegree(info.Param.GetValue().y),
            asdx::ToDegree(info.Param.GetValue().z));
    }
}

//-----------------------------------------------------------------------------
//      変換を考慮して値を取得します.
//-----------------------------------------------------------------------------
asdx::Vector4 GetValue(const UiFloat4& info)
{
    switch (info.Converter)
    {
    case CONVERTER_NONE:
    default:
        return info.Param.GetValue();

    case CONVERTER_RADIAN:
        return asdx::Vector4(
            asdx::ToRadian(info.Param.GetValue().x),
            asdx::ToRadian(info.Param.GetValue().y),
            asdx::ToRadian(info.Param.GetValue().z),
            asdx::ToRadian(info.Param.GetValue().w));

    case CONVERTER_DEGREE:
        return asdx::Vector4(
            asdx::ToDegree(info.Param.GetValue().x),
            asdx::ToDegree(info.Param.GetValue().y),
            asdx::ToDegree(info.Param.GetValue().z),
            asdx::ToDegree(info.Param.GetValue().w));
    }
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
//      ロードします.
//-----------------------------------------------------------------------------
bool PluginShader::Load(const char* path, const char* entryPoint)
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

#if defined(DEBUG) || defined(_DEBUG)
    UINT compileFlag = D3DCOMPILE_DEBUG;
#else
    UINT compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif


    asdx::RefPtr<ID3DBlob> pBlob;
    asdx::RefPtr<ID3DBlob> pErrorBlob;

    // ピクセルシェーダをロード.
    auto hr = D3DCompileFromFile(
        wpath.c_str(),
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

    m_Path       = path;
    m_EntryPoint = entryPoint;

    return true;
}

//-----------------------------------------------------------------------------
//      リロードします.
//-----------------------------------------------------------------------------
bool PluginShader::Reload()
{
    if (m_Path.empty())
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    auto temp = asdx::ToStringW(m_Path.c_str());

    std::wstring wpath;
    if (!asdx::SearchFilePathW(temp.c_str(), wpath))
    {
        ELOGA("Error : File Not Found. path = %s", m_Path.c_str());
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
    auto hr = D3DCompileFromFile(
        wpath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        m_EntryPoint.c_str(),
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
    std::map<std::string, BufferInfo> bufferTable;
    std::map<std::string, uint8_t> tableCBV;
    std::map<std::string, uint8_t> tableSRV;
    std::map<std::string, uint8_t> tableUAV;
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

            bufferTable[bufferDesc.Name] = info;
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
                tableCBV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;

            case D3D_SIT_TEXTURE:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                tableSRV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;

            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                tableUAV[bindDesc.Name] = uint8_t(bindDesc.BindPoint);
                break;
            }
        }
    }

    auto pDevice = asdx::DeviceContext::Instance().GetDevice();
    asdx::RefPtr<ID3D11PixelShader> pPS;
    hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPS.GetAddress());
    if (FAILED(hr))
    {
        ELOGA("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
        return false;
    }

    asdx::RefPtr<ID3D11Buffer> pCB;
    if (bufferTable.find("CbUser") != bufferTable.end())
    {
        auto info = bufferTable["CbUser"];

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth  = info.BufferSize;
        desc.Usage      = D3D11_USAGE_DYNAMIC;
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = pDevice->CreateBuffer(&desc, nullptr, pCB.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreateBuffer() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    auto pOldPS = m_PS.Detach();
    auto pOldCB = m_EditableCB.Detach();
    PluginMgr::Instance().DisposeShader(pOldPS);
    PluginMgr::Instance().DisposeBuffer(pOldCB);

    // 新しいものに差し替え.
    m_PS            = pPS;
    m_EditableCB    = pCB;
    m_BufferTable   = bufferTable;
    m_TableCBV      = tableCBV;
    m_TableSRV      = tableSRV;
    m_TableUAV      = tableUAV;

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

    m_PS.Reset();
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
        auto val = GetValue(info);
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float2.size(); ++i)
    {
        const auto& info = instance->m_Float2[i];
        auto val = GetValue(info);
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float3.size(); ++i)
    {
        const auto& info = instance->m_Float3[i];
        auto val = GetValue(info);
        memcpy(ptr + info.Offset, &val, sizeof(val));
    }

    for(size_t i=0; i<instance->m_Float4.size(); ++i)
    {
        const auto& info = instance->m_Float4[i];
        auto val = GetValue(info);
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

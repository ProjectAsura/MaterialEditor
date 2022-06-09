//-----------------------------------------------------------------------------
// File : PluginMaterial.cpp
// Desc : Plugin Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <PluginMgr.h>
#include <asdxLogger.h>
#include <asdxMisc.h>
#include <asdxRenderState.h>
#include <asdxDeviceContext.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// DefaultTexturePair structure
///////////////////////////////////////////////////////////////////////////////
struct DefaultTexturePair
{
    const char*             Tag;
    DEFAULT_TEXTURE_TYPE    Type;
};

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------

// デフォルトテクスチャテーブル.
static const DefaultTexturePair kDefaultTexutreTable[] = {
    { "checkerboard", DEFAULT_TEXTURE_CHECKER_BOARD },
    { "white"       , DEFAULT_TEXTURE_WHITE },
    { "black"       , DEFAULT_TEXTURE_BLACK },
    { "normal"      , DEFAULT_TEXTURE_NORMAL },
    { "mro"         , DEFAULT_TEXTURE_MRO },
    { "orm"         , DEFAULT_TEXTURE_ORM },
    { "velocity"    , DEFAULT_TEXTURE_VELCOITY },
    { "red"         , DEFAULT_TEXTURE_RED },
    { "green"       , DEFAULT_TEXTURE_GREEN },
    { "blue"        , DEFAULT_TEXTURE_BLUE },
    { "gray"        , DEFAULT_TEXTURE_GRAY },
};

} // namespace


//-----------------------------------------------------------------------------
//      デフォルトテクスチャタイプを解析します.
//-----------------------------------------------------------------------------
DEFAULT_TEXTURE_TYPE ParseDefaultTextureType(const char* text)
{
    auto count = _countof(kDefaultTexutreTable);
    for(size_t i=0; i<count; ++i)
    {
        if (_stricmp(kDefaultTexutreTable[i].Tag, text) == 0)
        { return kDefaultTexutreTable[i].Type; }
    }

    return DEFAULT_TEXTURE_CHECKER_BOARD;
}


///////////////////////////////////////////////////////////////////////////////
// PluginMaterial class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
PluginMaterial::PluginMaterial()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
PluginMaterial::~PluginMaterial()
{ Term(); }

//-----------------------------------------------------------------------------
//      マテリアル名を取得します.
//-----------------------------------------------------------------------------
const std::string& PluginMaterial::GetName() const
{ return m_Name; }

//-----------------------------------------------------------------------------
//      ロードします.
//-----------------------------------------------------------------------------
bool PluginMaterial::Load(const char* path)
{
    Term();

    m_Name = asdx::RemoveDirectoryPathA(path);
    m_Name = asdx::GetPathWithoutExtA(m_Name.c_str());

    asura::FxParser parser;
    if (!parser.Parse(path))
    {
        ELOGA("Error : PluginMaterial::Load() Failed. path = %s", path);
        return false;
    }

#if 0
    // デバッグファイル出力.
    {
        FILE* pFile;
        auto err = fopen_s(&pFile, "test.fx", "wb");
        if (err != 0)
        { return false; }

        fprintf_s(pFile, "%s", parser.GetSourceCode());
        fclose(pFile);
    }
#endif

    if (!m_LightingShader.Compile(
        parser.GetSourceCode(), parser.GetSourceCodeSize(), "LightingPS"))
    {
        ELOG("Error : LightingPS Func Compile Failed.");
        return false;
    }

    if (!m_ShadowingShader.Compile(
        parser.GetSourceCode(), parser.GetSourceCodeSize(), "ShadowingPS"))
    {
        ELOG("Error : ShadowingPS Func Compile Failed.");
        return false;
    }

    m_Properties = parser.GetProperties();
    {
        auto pDevice = asdx::DeviceContext::Instance().GetDevice();

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth      = asdx::RoundUp(m_Properties.BufferSize, 16); // 16 byte アライメント.
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_EditableCB.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreateBuffer() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    m_ShaderPath = asdx::ToFullPath(path);

    return true;
}

//-----------------------------------------------------------------------------
//      シェーダをリロードします.
//-----------------------------------------------------------------------------
bool PluginMaterial::ReloadShader()
{
    auto path = m_ShaderPath;
    return Load(path.c_str());
}

//-----------------------------------------------------------------------------
//      解放処理を行います.
//-----------------------------------------------------------------------------
void PluginMaterial::Term()
{
    m_LightingShader .Term();
    m_ShadowingShader.Term();
    m_EditableCB     .Reset();
    m_Name           .clear();
    m_ShaderPath     .clear();
}

//-----------------------------------------------------------------------------
//      シェーダを設定します.
//-----------------------------------------------------------------------------
void PluginMaterial::Bind(ID3D11DeviceContext* pContext, bool lightingPass)
{
    if (pContext == nullptr)
    { return; }

    if (lightingPass)
    { m_LightingShader.Bind(pContext); }
    else
    { m_ShadowingShader.Bind(pContext); }
}

//-----------------------------------------------------------------------------
//      シェーダの設定を解除します.
//-----------------------------------------------------------------------------
void PluginMaterial::Unbind(ID3D11DeviceContext* pContext, bool lightingPass)
{
    if (pContext == nullptr)
    { return; }

    if (lightingPass)
    { m_LightingShader.Unbind(pContext); }
    else
    { m_ShadowingShader.Unbind(pContext); }
}

//-----------------------------------------------------------------------------
//      ライティングシェーダを取得します.
//-----------------------------------------------------------------------------
const PluginShader* PluginMaterial::GetLightingShader() const
{ return &m_LightingShader; }

//-----------------------------------------------------------------------------
//      シャドウイングシェーダを取得します.
//-----------------------------------------------------------------------------
const PluginShader* PluginMaterial::GetShadowingShader() const
{ return &m_ShadowingShader; }

//-----------------------------------------------------------------------------
//      プロパティを取得します.
//-----------------------------------------------------------------------------
const asura::Properties& PluginMaterial::GetProperties() const
{ return m_Properties; }

//-----------------------------------------------------------------------------
//      メモリマッピングを行います.
//-----------------------------------------------------------------------------
void* PluginMaterial::Map(ID3D11DeviceContext* pContext)
{
    D3D11_MAPPED_SUBRESOURCE subResource = {};
    auto hr = pContext->Map(
        m_EditableCB.GetPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    if (FAILED(hr))
    { return nullptr; }

    return subResource.pData;
}

//-----------------------------------------------------------------------------
//      メモリマッピングを解除します.
//-----------------------------------------------------------------------------
void PluginMaterial::Unmap(ID3D11DeviceContext* pContext)
{ pContext->Unmap(m_EditableCB.GetPtr(), 0); }

//-----------------------------------------------------------------------------
//      定数バッファのレジスタテーブルを取得します.
//-----------------------------------------------------------------------------
bool PluginMaterial::FindSlotCBV(const std::string& name, uint32_t& slot, bool lightingPass) const
{
    if (lightingPass)
    {
        if (m_LightingShader.GetTableCBV().find(name) != m_LightingShader.GetTableCBV().end())
        {
            slot = m_LightingShader.GetTableSRV().at(name);
            return true;
        }
    }
    else
    {
        if (m_ShadowingShader.GetTableSRV().find(name) != m_ShadowingShader.GetTableCBV().end())
        {
            slot = m_ShadowingShader.GetTableSRV().at(name);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//      シェーダリソースビューのレジスタテーブルを取得します.
//-----------------------------------------------------------------------------
bool PluginMaterial::FindSlotSRV(const std::string& name, uint32_t& slot, bool lightingPass) const
{
    if (lightingPass)
    {
        if (m_LightingShader.GetTableSRV().find(name) != m_LightingShader.GetTableSRV().end())
        {
            slot = m_LightingShader.GetTableSRV().at(name);
            return true;
        }
    }
    else
    {
        if (m_ShadowingShader.GetTableSRV().find(name) != m_ShadowingShader.GetTableSRV().end())
        {
            slot = m_ShadowingShader.GetTableSRV().at(name);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//      アンオーダードアクセスビューのレジスタテーブルを取得します.
//-----------------------------------------------------------------------------
bool PluginMaterial::FindSlotUAV(const std::string& name, uint32_t& slot, bool lightingPass) const
{
    if (lightingPass)
    {
        if (m_LightingShader.GetTableUAV().find(name) != m_LightingShader.GetTableUAV().end())
        {
            slot = m_LightingShader.GetTableSRV().at(name);
            return true;
        }
    }
    else
    {
        if (m_ShadowingShader.GetTableUAV().find(name) != m_ShadowingShader.GetTableUAV().end())
        {
            slot = m_ShadowingShader.GetTableSRV().at(name);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//      シェーダパスを取得します.
//-----------------------------------------------------------------------------
const std::string& PluginMaterial::GetShaderPath() const
{ return m_ShaderPath; }

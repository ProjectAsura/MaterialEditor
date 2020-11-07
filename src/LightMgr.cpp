//-----------------------------------------------------------------------------
// File : LightMgr.cpp
// Desc : Light Manager.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <LightMgr.h>
#include <asdxMisc.h>
#include <asdxLogger.h>


///////////////////////////////////////////////////////////////////////////////
// EditorLight class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorLight::EditorLight()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorLight::~EditorLight()
{ Reset(); }

//-----------------------------------------------------------------------------
//      テクスチャを破棄します.
//-----------------------------------------------------------------------------
void EditorLight::Reset()
{
    m_Original_SRV  .Reset();
    m_DiffuseLD_SRV .Reset();
    m_DiffuseLD_UAV .Reset();
    m_SpecularLD_SRV.Reset();
    m_SpecularLD_UAV.Reset();
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditorLight::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("Light");


    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditorLight::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("Light");
    if (e == nullptr)
    { return; }



}

//-----------------------------------------------------------------------------
//      背景用キューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetOriginal() const
{ return m_Original_SRV.GetPtr(); }

//-----------------------------------------------------------------------------
//      ディフューズLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetDiffuseLD() const
{ return m_DiffuseLD_SRV.GetPtr(); }

//-----------------------------------------------------------------------------
//      スペキュラーLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetSpecularLD() const
{ return m_SpecularLD_SRV.GetPtr(); }


///////////////////////////////////////////////////////////////////////////////
// LightMgr class
///////////////////////////////////////////////////////////////////////////////
LightMgr LightMgr::s_Instance = {};

//-----------------------------------------------------------------------------
//      シングルトンインスタンスを取得します.
//-----------------------------------------------------------------------------
LightMgr& LightMgr::Instance()
{ return s_Instance; }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool LightMgr::Init(const char* path)
{
    std::string filePath;
    if (!asdx::SearchFilePathA(path, filePath))
    {
        ELOGA("Error : File Not Found. path = %s", path);
        return false;
    }

    tinyxml2::XMLDocument doc;
    auto ret = doc.LoadFile(filePath.c_str());
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : File Load Failed. path = %s", filePath.c_str());
        return false;
    }

    auto root = doc.FirstChildElement("LightPreset");
    if (root == nullptr)
    {
        ELOGA("Error : Root Node Not Found.");
        return false;
    }

    // ライトをデシリアライズ.



    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Term()
{
    for(size_t i=0; i<m_Light.size(); ++i)
    { m_Light[i].Reset(); }

    m_SkyBox    .Term();
    m_LightCB   .Term();

    m_PrefilterDiffuseCS.Reset();
    m_PrefilterSpcularCS.Reset();
    m_EnvBRDF.Reset();

    m_CurrentIndex = 0;
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Update(ID3D11DeviceContext* pContext)
{
    for(size_t i=0; i<m_Light.size(); ++i)
    {

    }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Draw(ID3D11DeviceContext* pContext)
{
}

//-----------------------------------------------------------------------------
//      環境BRDFを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* LightMgr::GetEnvBRDF() const
{ return m_EnvBRDF.GetPtr(); }

//-----------------------------------------------------------------------------
//      背景用キューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* LightMgr::GetOriginal() const
{
    if (m_CurrentIndex >= m_Light.size())
    { return nullptr; }

    return m_Light[m_CurrentIndex].GetOriginal();
}

//-----------------------------------------------------------------------------
//      DiffuseLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* LightMgr::GetDiffuseLD() const
{
    if (m_CurrentIndex >= m_Light.size())
    { return nullptr; }

    return m_Light[m_CurrentIndex].GetDiffuseLD();
}

//-----------------------------------------------------------------------------
//      SpecularLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* LightMgr::GetSpecularLD() const
{
    if (m_CurrentIndex >= m_Light.size())
    { return nullptr; }

    return m_Light[m_CurrentIndex].GetSpecularLD();
}

//-----------------------------------------------------------------------------
//      定数バッファを取得します.
//-----------------------------------------------------------------------------
ID3D11Buffer* LightMgr::GetBuffer() const
{ return m_LightCB.GetBuffer(); }

//-----------------------------------------------------------------------------
//      DiffuseLDキューブマップを生成します.
//-----------------------------------------------------------------------------
void LightMgr::ConvolutionDiffuse(ID3D11DeviceContext* pContext, EditorLight& light)
{

}

//-----------------------------------------------------------------------------
//      SpecularLDキューブマップを生成します.
//-----------------------------------------------------------------------------
void LightMgr::ConvolutionSpecular(ID3D11DeviceContext* pContext, EditorLight& light)
{
}

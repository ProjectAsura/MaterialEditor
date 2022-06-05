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
#include <asdxDeviceContext.h>
#include <asdxRenderState.h>
#include <asdxLocalization.h>
#include <imgui.h>
#include "..\external\directxtex_desktop_2019.2022.5.10.1\include\DirectXTex.h"


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const asdx::Localization kTagHideTexture(u8"背景画像無し", u8"Hide Texture");
static const asdx::Localization kTagLightParameter(u8"ライトパラメータ", u8"Light Parameter");
static const asdx::Localization kTagIntensity(u8"強度", u8"Intensity");
static const asdx::Localization kTagAngleV(u8"垂直角", u8"Angle V");
static const asdx::Localization kTagAngleH(u8"水平角", u8"Angle H");

} // namespace


///////////////////////////////////////////////////////////////////////////////
// EditorLight class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorLight::EditorLight()
: SunLightAngle     (90.0f, 0.0f) // -Z軸方向.
, SunLightIntensity (1.0f)
, IBLIntensity      (1.0f)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorLight::~EditorLight()
{ Reset(); }

//-----------------------------------------------------------------------------
//      ファイルをロードします.
//-----------------------------------------------------------------------------
bool EditorLight::Load(const char* path, const std::string& dir)
{
    tinyxml2::XMLDocument doc;
    auto ret = doc.LoadFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : File Load Failed. path = %s", path);
        return false;
    }

    auto root = doc.FirstChildElement("EditorLight");
    if (root == nullptr)
    {
        ELOGA("Error : Root Not Found.");
        return false;
    }

    Tag = root->Attribute("name");

    {
        auto e = root->FirstChildElement("SunLight");
        if (e != nullptr)
        {
            SunLightAngle.x     = e->FloatAttribute("x");
            SunLightAngle.y     = e->FloatAttribute("y");
            SunLightIntensity   = e->FloatAttribute("intensity");
        }
    }

    {
        auto e = root->FirstChildElement("Background");
        if (e != nullptr)
        {
            BackgroundPath = e->Attribute("path");
        }
    }

    {
        auto e = root->FirstChildElement("IBL");
        if (e != nullptr)
        {
            DiffuseIBLPath  = e->Attribute("diffuse_path");
            SpecularIBLPath = e->Attribute("specular_path");
            IBLIntensity    = e->FloatAttribute("intensity");
        }
    }

    if (!LoadTexture(dir))
    { return false; }

    return true;
}

//-----------------------------------------------------------------------------
//      ファイルをセーブします.
//-----------------------------------------------------------------------------
bool EditorLight::Save(const char* path)
{
    tinyxml2::XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());

    auto root = doc.NewElement("EditorLight");
    root->SetAttribute("name", Tag.c_str());

    {
        auto e = doc.NewElement("SunLight");
        e->SetAttribute("x", SunLightAngle.x);
        e->SetAttribute("y", SunLightAngle.y);
        e->SetAttribute("intensity", SunLightIntensity);
        root->InsertEndChild(e);
    }

    {
        auto e = doc.NewElement("Background");
        e->SetAttribute("path", BackgroundPath.c_str());
        root->InsertEndChild(e);
    }

    {
        auto e = doc.NewElement("SphereIBL");
        e->SetAttribute("diffuse_path", DiffuseIBLPath.c_str());
        e->SetAttribute("specular_path", SpecularIBLPath.c_str());
        e->SetAttribute("intensity", IBLIntensity);
        root->InsertEndChild(e);
    }

    auto ret = doc.SaveFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : File Save Failed. path = %s", path);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      テクスチャをロードします.
//-----------------------------------------------------------------------------
bool EditorLight::LoadTexture(const std::string& dir)
{
    std::string diffusePath;
    {
        auto temp = dir + "/" + DiffuseIBLPath;
        if (!asdx::SearchFilePathA(temp.c_str(), diffusePath))
        {
            ELOGA("Error : File Not Found. path = %s", DiffuseIBLPath.c_str());
            return false;
        }
    }

    std::string specularPath;
    {
        auto temp = dir + "/" + SpecularIBLPath;
        if (!asdx::SearchFilePathA(temp.c_str(), specularPath))
        {
            ELOGA("Error : File Not Found. path = %s", SpecularIBLPath.c_str());
            return false;
        }
    }

    std::string backgroundPath;
    if (BackgroundPath != "" && !BackgroundPath.empty())
    {
        auto temp = dir + "/" + BackgroundPath;
        if (!asdx::SearchFilePathA(temp.c_str(), backgroundPath))
        {
            ELOGA("Error : File Not Found. path = %s", BackgroundPath.c_str());
            return false;
        }
    }

    auto pDevice = asdx::DeviceContext::Instance().GetDevice();

    // 背景キューブマップ読み込み.
    if (!backgroundPath.empty())
    {
        auto path = asdx::ToStringW(backgroundPath);

        DirectX::ScratchImage scrathImage;
        auto hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scrathImage);
        if (SUCCEEDED(hr))
        {
            if (m_Background.GetPtr() != nullptr)
            { m_Background.Reset(); }

            hr = DirectX::CreateShaderResourceViewEx(
                pDevice,
                scrathImage.GetImages(),
                scrathImage.GetImageCount(),
                scrathImage.GetMetadata(),
                D3D11_USAGE_DEFAULT,
                D3D11_BIND_SHADER_RESOURCE,
                0,
                DirectX::TEX_MISC_TEXTURECUBE,
                false,
                m_Background.GetAddress());
            if (FAILED(hr))
            {
                ELOGA("Error : DirectX::CreateShaderResourceViewEx() Failed. errcode = 0x%x", hr);
            }
        }
    }

    // DiffuseLDキューブマップ読み込み.
    {
        auto path = asdx::ToStringW(diffusePath);

        DirectX::ScratchImage scrathImage;
        auto hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scrathImage);
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::LoadFromDDSFile() Failed. path = %s", diffusePath.c_str());
            return false;
        }

        if (m_DiffuseLD.GetPtr() != nullptr)
        { m_DiffuseLD.Reset(); }

        hr = DirectX::CreateShaderResourceViewEx(
            pDevice,
            scrathImage.GetImages(),
            scrathImage.GetImageCount(),
            scrathImage.GetMetadata(),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            DirectX::TEX_MISC_TEXTURECUBE,
            false,
            m_DiffuseLD.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::CreateShaderResourceViewEx() Faile. errcode = 0x%x", hr);
            return false;
        }
    }

    // SpecularLDキューブマップ読み込み.
    {
        auto path = asdx::ToStringW(specularPath);

        DirectX::ScratchImage scrathImage;
        auto hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scrathImage);
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::LoadFromDDSFile() Failed. path = %s", specularPath.c_str());
            return false;
        }

        if (m_SpecularLD.GetPtr() != nullptr)
        { m_SpecularLD.Reset(); }

        hr = DirectX::CreateShaderResourceViewEx(
            pDevice,
            scrathImage.GetImages(),
            scrathImage.GetImageCount(),
            scrathImage.GetMetadata(),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            DirectX::TEX_MISC_TEXTURECUBE,
            false,
            m_SpecularLD.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::CreateShaderResourceViewEx() Faile. errcode = 0x%x", hr);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      テクスチャを破棄します.
//-----------------------------------------------------------------------------
void EditorLight::Reset()
{
    m_Background.Reset();
    m_DiffuseLD .Reset();
    m_SpecularLD.Reset();
}

//-----------------------------------------------------------------------------
//      背景用キューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetBackground() const
{ return m_Background.GetPtr(); }

//-----------------------------------------------------------------------------
//      ディフューズLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetDiffuseLD() const
{ return m_DiffuseLD.GetPtr(); }

//-----------------------------------------------------------------------------
//      スペキュラーLDキューブマップを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditorLight::GetSpecularLD() const
{ return m_SpecularLD.GetPtr(); }


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
bool LightMgr::Init()
{
    auto pDevice = asdx::DeviceContext::Instance().GetDevice();

    // EnvBRDFをロード.
    {
        std::string path;
        if (!asdx::SearchFilePathA("../res/lights/env_brdf.dds", path))
        {
            ELOGA("Error : File Not Found.");
            return false;
        }

        auto pathW = asdx::ToStringW(path);

        DirectX::ScratchImage scrathImage;
        auto hr = DirectX::LoadFromDDSFile(pathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scrathImage);
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::LoadFromDDSFile() Failed. path = %s", path.c_str());
            return false;
        }

        if (m_EnvBRDF.GetPtr() != nullptr)
        { m_EnvBRDF.Reset(); }

        hr = DirectX::CreateShaderResourceViewEx(
            pDevice,
            scrathImage.GetImages(),
            scrathImage.GetImageCount(),
            scrathImage.GetMetadata(),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            0,
            false,
            m_EnvBRDF.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : DirectX::CreateShaderResourceViewEx() Faile. errcode = 0x%x", hr);
            return false;
        }
    }

    // スカイボックス初期化.
    if (!m_SkyBox.Init(pDevice))
    {
        ELOG("Error : SkyBox::Init() Failed.");
        return false;
    }

    // lightsフォルダ下のライト設定ファイル(xml)をロードしていく.
    {
        std::string dir;
        if (!asdx::SearchFilePathA("../res/lights", dir))
        {
            ELOGA("Error : Folder Not Found.");
            return false;
        }

        std::list<std::string> lightPresets;
        if (!asdx::SearchFilesA(dir.c_str(), ".xml", lightPresets))
        {
            ELOGA("Error : File Not Found.");
            return false;
        }

        m_Light.resize(lightPresets.size());
        auto idx = 0;

        for(auto& itr : lightPresets)
        {
            if (!m_Light[idx].Load(itr.c_str(), dir))
            { 
                ELOGA("Error : Light Load Failed. path = %s", itr.c_str());
                return false;
            }

            idx++;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Term()
{
    for(size_t i=0; i<m_Light.size(); ++i)
    { m_Light[i].Reset(); }

    m_SkyBox .Term();
    m_EnvBRDF.Reset();

    m_CurrentIndex = 0;
}

//-----------------------------------------------------------------------------
//      編集処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Edit()
{
    static const ImVec2 kCubmapSize = ImVec2(64.0f * 4, 64. * 2.0f);
    static const ImVec4 kRed = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    for(size_t i=0; i<m_Light.size(); ++i)
    {
        auto& light = m_Light[i];

        ImGui::PushID(int(i));

        auto pSRV = light.GetBackground();

        if (pSRV != nullptr)
        {
            if (ImGui::ImageButton(pSRV, kCubmapSize))
            { m_CurrentIndex = i; }
        }
        else
        {
            if (ImGui::Button(kTagHideTexture.c_str()))
            { m_CurrentIndex = i; }
        }
        ImGui::SameLine();
        if (ImGui::BeginChild(kTagLightParameter.c_str()))
        {
            ImGui::Text(light.Tag.c_str());
            ImGui::DragFloat(kTagIntensity.c_str(), &light.IBLIntensity, 0.1f);
            ImGui::DragFloat(kTagAngleV.c_str(), &light.SunLightAngle.x, 0.1f, -360.0f, 360.0f);
            ImGui::DragFloat(kTagAngleH.c_str(), &light.SunLightAngle.y, 0.1f, -360.0f, 360.0f);
            ImGui::EndChild();
        }

        ImGui::PopID();
    }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void LightMgr::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Vector3&    cameraPos,
    const asdx::Matrix&     view,
    const asdx::Matrix&     proj,
    float                   farClip
)
{
    if (m_CurrentIndex >= m_Light.size())
    { return; }

    auto pSRV = m_Light[m_CurrentIndex].GetBackground();
    if (pSRV == nullptr)
    { return; }

    float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32_t sampleMask = 0xffff;
    auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp);
    auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::None);
    auto pBS  = asdx::RenderState::GetInstance().GetBS (asdx::BlendType::Opaque);
    pContext->OMSetDepthStencilState(pDSS, 0);
    pContext->OMSetBlendState(pBS, blendFactor, sampleMask);
    m_SkyBox.Draw(pContext, pSRV, pSmp, farClip * 0.5f, cameraPos, view, proj);
}

//-----------------------------------------------------------------------------
//      環境BRDFを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* LightMgr::GetEnvBRDF() const
{ return m_EnvBRDF.GetPtr(); }

//-----------------------------------------------------------------------------
//      DiffuseLDキューブマップを取得します.
//-----------------------------------------------------------------------------
const EditorLight* LightMgr::GetLight() const
{
    if (m_CurrentIndex >= m_Light.size())
    { return nullptr; }

    return &m_Light[m_CurrentIndex];
}

//-----------------------------------------------------------------------------
//      マウス操作を行います.
//-----------------------------------------------------------------------------
void LightMgr::OnMouse(float x, float y, float gain, bool down)
{
    if (down)
    {
        if (!m_Drag)
        {
            m_CurrCursor.x = x;
            m_CurrCursor.y = y;
            m_PrevCursor.x = x;
            m_PrevCursor.y = y;
            m_Drag = true;
        }
        else
        {
            m_PrevCursor.x = m_CurrCursor.x;
            m_PrevCursor.y = m_CurrCursor.y;

            m_CurrCursor.x = x;
            m_CurrCursor.y = y;

            auto diffV = -(m_CurrCursor.x - m_PrevCursor.x) * gain;
            auto diffH =  (m_CurrCursor.y - m_PrevCursor.y) * gain;

            auto& light = m_Light[m_CurrentIndex];
            light.SunLightAngle.x += diffH;
            light.SunLightAngle.y += diffV;

            asdx::Clamp(light.SunLightAngle.x, -360.0f, 360.0f);
            asdx::Clamp(light.SunLightAngle.y, -360.0f, 360.0f);
        }
    }
    else
    {
        if (m_Drag)
        {
            m_PrevCursor.x = m_CurrCursor.x;
            m_PrevCursor.y = m_CurrCursor.y;

            m_CurrCursor.x = x;
            m_CurrCursor.y = y;

            auto diffH = -(m_CurrCursor.x - m_PrevCursor.x) * gain;
            auto diffV =  (m_CurrCursor.y - m_PrevCursor.y) * gain;

            auto& light = m_Light[m_CurrentIndex];
            light.SunLightAngle.x += diffH;
            light.SunLightAngle.y += diffV;

            asdx::Clamp(light.SunLightAngle.x, -360.0f, 360.0f);
            asdx::Clamp(light.SunLightAngle.y, -360.0f, 360.0f);
        }

        m_Drag = false;
        m_PrevCursor.x = 0;
        m_PrevCursor.y = 0;
        m_CurrCursor.x = 0;
        m_CurrCursor.y = 0;
    }
}
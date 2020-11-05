//-----------------------------------------------------------------------------
// File : PluginMgr.cpp
// Desc : Plugin Manager.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <PluginMgr.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxDeviceContext.h>
#include <imgui.h>
#include <DirectXTex.h>


//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const std::string kInvalidMaterialName = "";


///////////////////////////////////////////////////////////////////////////////
// PluginMgr class
///////////////////////////////////////////////////////////////////////////////
PluginMgr PluginMgr::s_Instance = {};

//-----------------------------------------------------------------------------
//      シングルトンインスタンスを取得します.
//-----------------------------------------------------------------------------
PluginMgr& PluginMgr::Instance()
{ return s_Instance; }

//-----------------------------------------------------------------------------
//      ロードします.
//-----------------------------------------------------------------------------
bool PluginMgr::Load()
{
    // pluginフォルダ下にあるファイルを全部開く.
    std::string path;
    if (!asdx::SearchFilePathA("res/plugins/material", path))
    {
        ELOG("Error : Folder Not Found.");
        return false;
    }

    std::list<std::string> files;
    if (!asdx::SearchFilesA(path.c_str(), ".mat", files))
    {
        ELOG("Error : File Not Found");
        return false;
    }

    for(auto& itr : files)
    {
        auto mat = new PluginMaterial();
        if (!mat->Load(itr.c_str()))
        {
            delete mat;
            mat = nullptr;
            continue;
        }

        m_Materials[mat->GetName()] = mat;
    }

    auto pDevice  = asdx::DeviceContext::Instance().GetDevice();
    auto pContext = asdx::DeviceContext::Instance().GetContext();

    // チェッカーボード.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                // 黒
                if ((i < 16 && j < 16) || (i >= 16 && j >= 16))
                {
                    ptr[idx + 0] = 46;
                    ptr[idx + 1] = 46;
                    ptr[idx + 2] = 46;
                    ptr[idx + 3] = 255;
                }
                // 白.
                else
                {
                    ptr[idx + 0] = 230;
                    ptr[idx + 1] = 230;
                    ptr[idx + 2] = 230;
                    ptr[idx + 3] = 255;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_CHECKER_BOARD].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // 速度テクスチャ生成.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                {
                    ptr[idx + 0] = 128;
                    ptr[idx + 1] = 128;
                    ptr[idx + 2] = 0;
                    ptr[idx + 3] = 0;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_VELCOITY].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // オクルージョン/ラフネス/メタルネス生成.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                {
                    ptr[idx + 0] = 255;     // Occlusion
                    ptr[idx + 1] = 255;     // Roughness
                    ptr[idx + 2] = 0;       // Metallness
                    ptr[idx + 3] = 0;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_ORM].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // 法線生成.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                {
                    ptr[idx + 0] = 128;
                    ptr[idx + 1] = 128;
                    ptr[idx + 2] = 255;
                    ptr[idx + 3] = 0;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_NORMAL].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // ホワイトマップ生成.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                {
                    ptr[idx + 0] = 255;
                    ptr[idx + 1] = 255;
                    ptr[idx + 2] = 255;
                    ptr[idx + 3] = 255;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_WHITE].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // ブラックマップ生成.
    {
        DirectX::ScratchImage image;
        auto hr = image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::ScratchImage::Initialize2D() Failed.");
            return false;
        }

        auto ptr = image.GetPixels();
        auto idx = 0;
        for(auto i=0; i<32; ++i)
        {
            for(auto j=0; j<32; ++j)
            {
                {
                    ptr[idx + 0] = 0;
                    ptr[idx + 1] = 0;
                    ptr[idx + 2] = 0;
                    ptr[idx + 3] = 0;
                }
                idx += 4;
            }
        }

        hr = DirectX::CreateShaderResourceView(
            pDevice,
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            m_DefaultSRV[DEFAULT_TEXTURE_BLACK].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }
    
    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void PluginMgr::Term()
{
    for(auto& itr : m_Materials)
    {
        auto mat = itr.second;
        itr.second = nullptr;

        mat->Term();
        delete mat;
        mat = nullptr;
    }
    m_Materials.clear();

    for(auto i=0; i<DEFAULT_TEXTURE_COUNT; ++i)
    { m_DefaultSRV[i].Reset(); }
}

//-----------------------------------------------------------------------------
//      マテリアルを検索します.
//-----------------------------------------------------------------------------
bool PluginMgr::FindMaterial(const std::string& name, PluginMaterial** result)
{
    if (m_Materials.find(name) == m_Materials.end())
    { return false; }

    *result = m_Materials[name];
    return true;
}

//-----------------------------------------------------------------------------
//      マテリアルインスタンスを生成します.
//-----------------------------------------------------------------------------
MaterialInstance* PluginMgr::CreateInstance(const std::string& name)
{
    PluginMaterial* mat;
    if (!FindMaterial(name, &mat))
    { return nullptr; }

    auto instance = new MaterialInstance();
    instance->m_MaterialName = name;

    auto shader = mat->GetLightingShader();

    // パラメータ生成.
    instance->m_Bool.resize(mat->m_Bool.size());
    for (size_t i = 0; i < instance->m_Bool.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Bool[i].Target, info);
        instance->m_Bool[i].Tag = mat->m_Bool[i].DisplayTag;
        instance->m_Bool[i].Param.SetValue(mat->m_Bool[i].Default);
        instance->m_Bool[i].Offset = info.Offset;
        instance->m_Bool[i].Converter = CONVERTER_NONE;
    }

    instance->m_Int.resize(mat->m_Int.size());
    for (size_t i = 0; i < instance->m_Int.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Int[i].Target, info);
        instance->m_Int[i].Tag = mat->m_Int[i].DisplayTag;
        instance->m_Int[i].Param.SetValue(mat->m_Int[i].Default);
        instance->m_Int[i].Offset = info.Offset;
        instance->m_Int[i].Converter = CONVERTER_NONE;
    }

    instance->m_Float.resize(mat->m_Float.size());
    for (size_t i = 0; i < instance->m_Float.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Float[i].Target, info);
        instance->m_Float[i].Tag = mat->m_Float[i].DisplayTag;
        instance->m_Float[i].Param.SetValue(mat->m_Float[i].Default);
        instance->m_Float[i].Offset = info.Offset;
        instance->m_Float[i].Converter = mat->m_Float[i].Converter;
    }

    instance->m_Float2.resize(mat->m_Float2.size());
    for (size_t i = 0; i < instance->m_Float2.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Float2[i].Target, info);
        instance->m_Float2[i].Tag = mat->m_Float2[i].DisplayTag;
        instance->m_Float2[i].Param.SetValue(mat->m_Float2[i].Default);
        instance->m_Float2[i].Offset = info.Offset;
        instance->m_Float2[i].Converter = mat->m_Float2[i].Converter;
    }

    instance->m_Float3.resize(mat->m_Float3.size());
    for (size_t i = 0; i < instance->m_Float3.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Float3[i].Target, info);
        instance->m_Float3[i].Tag = mat->m_Float3[i].DisplayTag;
        instance->m_Float3[i].Param.SetValue(mat->m_Float3[i].Default);
        instance->m_Float3[i].Offset = info.Offset;
        instance->m_Float3[i].Converter = mat->m_Float3[i].Converter;
    }

    instance->m_Float4.resize(mat->m_Float4.size());
    for (size_t i = 0; i < instance->m_Float4.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Float4[i].Target, info);
        instance->m_Float4[i].Tag = mat->m_Float4[i].DisplayTag;
        instance->m_Float4[i].Param.SetValue(mat->m_Float4[i].Default);
        instance->m_Float4[i].Offset = info.Offset;
        instance->m_Float4[i].Converter = mat->m_Float4[i].Converter;
    }

    instance->m_Color3.resize(mat->m_Color3.size());
    for (size_t i = 0; i < instance->m_Color3.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Color3[i].Target, info);
        instance->m_Color3[i].Tag = mat->m_Color3[i].DisplayTag;
        instance->m_Color3[i].Param.SetValue(mat->m_Color3[i].Default);
        instance->m_Color3[i].Offset = info.Offset;
        instance->m_Color3[i].Converter = CONVERTER_NONE;
    }

    instance->m_Color4.resize(mat->m_Color4.size());
    for (size_t i = 0; i < instance->m_Color4.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Color4[i].Target, info);
        instance->m_Color4[i].Tag = mat->m_Color4[i].DisplayTag;
        instance->m_Color4[i].Param.SetValue(mat->m_Color4[i].Default);
        instance->m_Color4[i].Offset = info.Offset;
        instance->m_Color4[i].Converter = CONVERTER_NONE;
    }

    instance->m_Bit32.resize(mat->m_Bit32.size());
    for (size_t i = 0; i < instance->m_Bit32.size(); ++i)
    {
        PluginShader::MemberInfo info = {};
        shader->FindMemberInfo("CbUser", mat->m_Bit32[i].Target, info);
        instance->m_Bit32[i].Tag = mat->m_Bit32[i].DisplayTag;
        instance->m_Bit32[i].Param.SetValue(mat->m_Bit32[i].Default);
        instance->m_Bit32[i].Offset = info.Offset;
        instance->m_Bit32[i].Converter = CONVERTER_NONE;
    }

    instance->m_Texture2D.resize(mat->m_Texture2D.size());
    for (size_t i = 0; i < instance->m_Texture2D.size(); ++i)
    {
        uint8_t slot = 0;
        shader->FindTextureSlot(mat->m_Texture2D[i].Target, slot);
        instance->m_Texture2D[i].Tag      = mat->m_Texture2D[i].DisplayTag;
        instance->m_Texture2D[i].Default  = mat->m_Texture2D[i].Default;
        instance->m_Texture2D[i].Register = slot;
    }

    return instance;
}

//-----------------------------------------------------------------------------
//      マテリアルインスタンスを破棄します.
//-----------------------------------------------------------------------------
void PluginMgr::DeleteInstance(MaterialInstance*& instance)
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      コンボボックスを描画します.
//-----------------------------------------------------------------------------
const std::string& PluginMgr::DrawCombo(const std::string& selectedItem)
{
    PluginMaterial* selectedMat = nullptr;
    if (ImGui::BeginCombo(u8"マテリアルタイプ", selectedItem.c_str()))
    {
        for (auto& itr : m_Materials)
        {
            auto selected = (itr.first == selectedItem);
            if (ImGui::Selectable(itr.first.c_str(), selected))
                selectedMat = itr.second;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (selectedMat == nullptr)
    { return selectedItem; }
    
    return selectedMat->GetName();
}

//-----------------------------------------------------------------------------
//      デフォルトテクスチャを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* PluginMgr::GetDefaultSRV(DEFAULT_TEXTURE_TYPE type) const
{ return m_DefaultSRV[type].GetPtr(); }

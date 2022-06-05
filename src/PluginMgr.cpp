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
#include <asdxLocalization.h>
#include <imgui.h>
#include "..\external\directxtex_desktop_2019.2022.5.10.1\include\DirectXTex.h"


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const asdx::Localization kTagMaterialType(u8"マテリアルタイプ", u8"Material Type");
static const asdx::Localization kTagFilterType(u8"フィルタタイプ", u8"Filter Type");
static const asdx::Localization kTagNoFilter(u8"フィルタなし", u8"No Filter");
static const asdx::Localization kTagNoExporter(u8"エクスポーターがありません", u8"No Exporter");
static const asdx::Localization kTagExporter(u8"エクスポーター", u8"Exporter");

} // namspace


//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const std::string kInvalidMaterialName = "";


//-----------------------------------------------------------------------------
//      エクスポーターを呼び出し，エクスポート処理を実行します.
//-----------------------------------------------------------------------------
bool CallExporter
(
    const char*             dllName,
    const ExportContext*    context
)
{
    auto handle = LoadLibraryExA(dllName, nullptr, 0);
    if (handle == nullptr)
    {
        ELOGA("Error : LoadLibraryExA() Failed. dllname = %s", dllName);
        return false;
    }

    auto func = reinterpret_cast<ExportFunc>(GetProcAddress(handle, "ExportMaterial"));
    if (func == nullptr)
    {
        ELOGA("Error : Not Found Function in DLL. dllname = %s", dllName);
        FreeLibrary(handle);
        return false;
    }

    auto ret = func(context);
    FreeLibrary(handle);

    return ret;
}


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
    // plugins/shaderフォルダ下にあるAFXファイルを全部開く.
    {
        std::string path;
        if (!asdx::SearchFilePathA("res/plugins/shader", path))
        {
            ELOG("Error : Folder Not Found.");
            return false;
        }

        std::list<std::string> files;
        if (!asdx::SearchFilesA(path.c_str(), ".afx", files))
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

            m_MasterMaterials[mat->GetName()] = mat;
        }
    }

    ReloadExporter();

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

    // メタルネス/ラフネス/オクルージョン生成.
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
                    ptr[idx + 0] = 0;       // Metallness
                    ptr[idx + 1] = 255;     // Roughness
                    ptr[idx + 2] = 255;     // Occlusion
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
            m_DefaultSRV[DEFAULT_TEXTURE_MRO].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // オクルージョン／ラフネス／メタルネス生成.
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
                    ptr[idx + 2] = 0;       // Metalness
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

    // レッドマップ生成.
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
                    ptr[idx + 1] = 0;
                    ptr[idx + 2] = 0;
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
            m_DefaultSRV[DEFAULT_TEXTURE_RED].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // グリーンマップ生成.
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
                    ptr[idx + 1] = 255;
                    ptr[idx + 2] = 0;
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
            m_DefaultSRV[DEFAULT_TEXTURE_GREEN].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // ブルーマップ生成.
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
            m_DefaultSRV[DEFAULT_TEXTURE_BLUE].GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : DirectX::CreatShaderResourceView() Failed.");
            return false;
        }
    }

    // グレーマップ生成.
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
                    ptr[idx + 2] = 128;
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
            m_DefaultSRV[DEFAULT_TEXTURE_GRAY].GetAddress());
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
    for(auto& itr : m_MasterMaterials)
    {
        auto mat = itr.second;
        itr.second = nullptr;

        mat->Term();
        delete mat;
        mat = nullptr;
    }
    m_MasterMaterials.clear();

    for(auto i=0; i<DEFAULT_TEXTURE_COUNT; ++i)
    { m_DefaultSRV[i].Reset(); }
}

//-----------------------------------------------------------------------------
//      シェーダをリロードします.
//-----------------------------------------------------------------------------
void PluginMgr::ReloadShader()
{
    auto failed  = 0;
    auto success = 0;
    for (auto& itr : m_MasterMaterials)
    { 
        auto ret = itr.second->ReloadShader();
        if (ret)
        { success++; }
        else
        { failed++; }
    }

    ILOGA("---- Reload Shader: %d success,  %d failed. ----", success, failed);
}

//-----------------------------------------------------------------------------
//      エクスポーターをリロードします.
//-----------------------------------------------------------------------------
void PluginMgr::ReloadExporter()
{
    // plugins/exporterフォルダ下にあるDLLファイル名を取得.
    std::string path;
    if (asdx::SearchFilePathA("res/plugins/exporter", path))
    {
        std::list<std::string> files;
        if (asdx::SearchFilesA(path.c_str(), ".dll", files))
        {
            m_Exporters.clear();
            m_Exporters.resize(files.size());

            auto idx = 0;
            for(auto& itr : files)
            {
                m_Exporters[idx] = itr;
                idx++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//      マスターマテリアルを検索します.
//-----------------------------------------------------------------------------
bool PluginMgr::FindMasterMaterial(const std::string& name, PluginMaterial** result)
{
    if (m_MasterMaterials.find(name) == m_MasterMaterials.end())
    { return false; }

    *result = m_MasterMaterials[name];
    return true;
}

//-----------------------------------------------------------------------------
//      コンボボックスを描画します.
//-----------------------------------------------------------------------------
const std::string& PluginMgr::DrawTypeCombo(const std::string& selectedItem)
{
    PluginMaterial* selectedMat = nullptr;
    if (ImGui::BeginCombo(kTagMaterialType.c_str(), selectedItem.c_str()))
    {
        for (auto& itr : m_MasterMaterials)
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
//      コンボボックスを描画します.
//-----------------------------------------------------------------------------
const std::string& PluginMgr::DrawFilterCombo(const std::string& selectedItem)
{
    bool nofilter = false;
    PluginMaterial* selectedMat = nullptr;
    if (ImGui::BeginCombo(kTagFilterType.c_str(), selectedItem.c_str()))
    {
        auto selected = (kTagNoFilter.c_str() == selectedItem);
        if (ImGui::Selectable(kTagNoFilter.c_str(), selected))
            nofilter = true;
        if (selected)
            ImGui::SetItemDefaultFocus();

        for (auto& itr : m_MasterMaterials)
        {
            selected = (itr.first == selectedItem);
            if (ImGui::Selectable(itr.first.c_str(), selected))
                selectedMat = itr.second;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (selectedMat == nullptr)
    { return (nofilter) ? kInvalidMaterialName : selectedItem; }

    return selectedMat->GetName();
}

//-----------------------------------------------------------------------------
//      エクスポーターコンボボックスを描画します.
//-----------------------------------------------------------------------------
const std::string& PluginMgr::DrawExporterCombo(const std::string& selectedItem)
{
    if (m_Exporters.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), kTagNoExporter.c_str());
        return selectedItem;
    }

    auto idx = -1;
    if (ImGui::BeginCombo(kTagExporter.c_str(), selectedItem.c_str()))
    {
        for(size_t i=0; i<m_Exporters.size(); ++i)
        {
            auto selected = (m_Exporters[i] == selectedItem);
            if (ImGui::Selectable(m_Exporters[i].c_str(), selected))
                idx = int(i);
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (idx == -1)
    { return selectedItem; }

    return m_Exporters[idx];
}

//-----------------------------------------------------------------------------
//      デフォルトテクスチャを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* PluginMgr::GetDefaultSRV(DEFAULT_TEXTURE_TYPE type) const
{ return m_DefaultSRV[type].GetPtr(); }

//-----------------------------------------------------------------------------
//      シェーダを破棄します.
//-----------------------------------------------------------------------------
void PluginMgr::DisposeShader(ID3D11PixelShader*& pItem)
{
    m_ShaderDisposer.Push(pItem);
}

//-----------------------------------------------------------------------------
//      バッファを破棄します.
//-----------------------------------------------------------------------------
void PluginMgr::DisposeBuffer(ID3D11Buffer*& pItem)
{
    m_BufferDisposer.Push(pItem);
}

//-----------------------------------------------------------------------------
//      同期タイミングを設定します.
//-----------------------------------------------------------------------------
void PluginMgr::Sync()
{
    m_ShaderDisposer.FrameSync();
    m_BufferDisposer.FrameSync();
}

//-----------------------------------------------------------------------------
//      最初のマスターマテリアル名を取得します.
//-----------------------------------------------------------------------------
const std::string& PluginMgr::GetFirstMasterMaterialName() const
{
    auto itr = m_MasterMaterials.begin();
    return itr->first;
}

//-----------------------------------------------------------------------------
//      マスターマテリアルが含まれるかどうかチェックします.
//-----------------------------------------------------------------------------
bool PluginMgr::ContainMasterMaterial(const std::string& name) const
{ return m_MasterMaterials.find(name) != m_MasterMaterials.end(); }

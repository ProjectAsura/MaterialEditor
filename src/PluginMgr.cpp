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
#include <imgui.h>


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
    if (!asdx::SearchFilePathA("plugin/material", path))
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

    // パラメータ初期化.
    mat->Reset(instance);

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
    ImGui::BeginCombo(u8"マテリアル", selectedItem.c_str());

    for(auto& itr : m_Materials)
    {
        auto selected = (itr.first == selectedItem);
        if (ImGui::Selectable(itr.first.c_str(), selected))
            selectedMat = itr.second;
        if (selected)
            ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();

    if (selectedMat == nullptr)
    { return kInvalidMaterialName; }
    
    return selectedMat->GetName();
}
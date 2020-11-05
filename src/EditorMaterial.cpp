//-----------------------------------------------------------------------------
// File : EditorMaterial.cpp
// Desc : Editor Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <EditorMaterial.h>
#include <imgui.h>
#include <asdxLogger.h>


///////////////////////////////////////////////////////////////////////////////
// EditorMaterial class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorMaterial::EditorMaterial()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorMaterial::~EditorMaterial()
{
    for(auto& itr : m_Instances)
    {
        auto instance = itr.second;
        itr.second = nullptr;
        PluginMgr::Instance().DeleteInstance(instance);
    }

    m_Instances.clear();
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditorMaterial::Serialize(tinyxml2::XMLDocument* doc)
{
    auto element = doc->NewElement(m_Name.c_str());
    element->SetAttribute("selected_material",  m_SelectedMaterial.c_str());
    element->SetAttribute("instance_count",     m_Instances.size());

    for(auto& itr : m_Instances)
    {
        if (itr.second == nullptr)
        { continue; }

        element->InsertEndChild(itr.second->Serialize(doc));
    }

    return element;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditorMaterial::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement(m_Name.c_str());
    if (e == nullptr)
    { return; }

    m_SelectedMaterial  = e->Attribute("selected_material");
    auto count = e->Unsigned64Attribute("instance_count");

    auto child = e->FirstChildElement();
    for(uint64_t i=0; i<count; ++i)
    {
        if (child == nullptr)
        { return; }
    
        auto tag = child->GetText();
        PluginMaterial* mat;
        if (!PluginMgr::Instance().FindMaterial(tag, &mat))
        {
            child = child->NextSiblingElement();
            continue;
        }

        auto instance = PluginMgr::Instance().CreateInstance(tag);
        instance->Deserialize(child);

        m_Instances[tag] = instance;
        child = child->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------
//      マテリアルシェーダを適用します.
//-----------------------------------------------------------------------------
const PluginShader* EditorMaterial::Bind(ID3D11DeviceContext* pContext, bool lightingPass)
{
    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
    { return nullptr; }

    auto instance = m_Instances[m_SelectedMaterial];

    material->Bind(pContext, instance, lightingPass);
    return (lightingPass) ? material->GetLightingShader() : material->GetShadowingShader();
}

//-----------------------------------------------------------------------------
//      マテリアルシェーダの適用を解除します.
//-----------------------------------------------------------------------------
void EditorMaterial::Unbind(ID3D11DeviceContext* pContext, const PluginShader* shader)
{
    if (shader != nullptr)
    { shader->Unbind(pContext); }
    else
    {
        PluginMaterial* material;
        if (!PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
        { return; }

        material->Unbind(pContext);
    }
}

//-----------------------------------------------------------------------------
//      マテリアルを編集します.
//-----------------------------------------------------------------------------
void EditorMaterial::Edit()
{
    ImGui::PushID(m_Name.c_str());

    if (ImGui::CollapsingHeader(m_Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        // マテリアル選択コンボボックス.
        m_SelectedMaterial = PluginMgr::Instance().DrawCombo(m_SelectedMaterial);

        PluginMaterial* material;
        if (PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
        {
            // 存在チェック.
            if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
            {
                m_Instances[m_SelectedMaterial] = 
                    PluginMgr::Instance().CreateInstance(m_SelectedMaterial);
            }

            // マテリアル編集.
            auto instance = m_Instances[m_SelectedMaterial];
            material->Edit(instance);
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"無効なマテリアルです.");
        }
    }

    ImGui::PopID();
}

//-----------------------------------------------------------------------------
//      マテリアル名を設定します.
//-----------------------------------------------------------------------------
void EditorMaterial::SetName(const std::string& name)
{ m_Name = name; }

//-----------------------------------------------------------------------------
//      マテリアル名を取得します.
//-----------------------------------------------------------------------------
const std::string& EditorMaterial::GetName() const
{ return m_Name; }

//-----------------------------------------------------------------------------
//      シャドウをキャストするかどうか?
//-----------------------------------------------------------------------------
bool EditorMaterial::CastShadow() const
{
    if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
    { return true; }

    auto instance = m_Instances.at(m_SelectedMaterial);
    return instance->CastShadow();
}

//-----------------------------------------------------------------------------
//      ブレンドステートを取得します.
//-----------------------------------------------------------------------------
int EditorMaterial::GetBlendState() const
{
    if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
    { return 0; }

    auto instance = m_Instances.at(m_SelectedMaterial);
    return instance->GetBlendState();
}


///////////////////////////////////////////////////////////////////////////////
// EditorMaterials class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorMaterials::EditorMaterials()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorMaterials::~EditorMaterials()
{
    m_Materials.clear();
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditorMaterials::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("EditorMaterials");

    for(size_t i=0; i<m_Materials.size(); ++i)
    { e->InsertEndChild(m_Materials[i].Serialize(doc)); }

    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditorMaterials::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("EditorMaterials");
    if (e == nullptr)
    { return; }

    for(uint64_t i=0; i<m_Materials.size(); ++i)
    { m_Materials[i].Deserialize(e); }
}

//-----------------------------------------------------------------------------
//      リサイズします.
//-----------------------------------------------------------------------------
void EditorMaterials::Resize(uint32_t count)
{
    m_Materials.resize(count);
    m_Materials.shrink_to_fit();
}

//-----------------------------------------------------------------------------
//      マテリアル数を取得します.
//-----------------------------------------------------------------------------
uint32_t EditorMaterials::GetCount() const
{ return uint32_t(m_Materials.size()); }

//-----------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------
EditorMaterial& EditorMaterials::GetMaterial(uint32_t index)
{
    assert(size_t(index) < m_Materials.size());
    return m_Materials[index];
}

//-----------------------------------------------------------------------------
//      マテリアルを検索します.
//-----------------------------------------------------------------------------
bool EditorMaterials::FindMaterial(const std::string& name, EditorMaterial& material)
{
    for(size_t i=0; i<m_Materials.size(); ++i)
    {
        if (m_Materials[i].GetName() == name)
        {
            material = m_Materials[i];
            return true;
        }
    }

    return false;
}

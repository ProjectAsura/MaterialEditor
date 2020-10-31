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
    auto element = doc->NewElement("material");
    element->SetAttribute("name",               m_Name.c_str());
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
    auto e = element->FirstChildElement("material");
    if (e == nullptr)
    { return; }

    m_Name              = e->Attribute("name");
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
PluginMaterial* EditorMaterial::Bind(ID3D11DeviceContext* pContext)
{
    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
    { return nullptr; }

    auto instance = m_Instances[m_SelectedMaterial];

    material->Bind(pContext, instance);
    return material;
}

//-----------------------------------------------------------------------------
//      マテリアルシェーダの適用を解除します.
//-----------------------------------------------------------------------------
void EditorMaterial::Unbind(ID3D11DeviceContext* pContext, PluginMaterial* material)
{ material->Unbind(pContext); }

//-----------------------------------------------------------------------------
//      マテリアルを編集します.
//-----------------------------------------------------------------------------
void EditorMaterial::Edit()
{
    ImGui::PushID(m_Name.c_str());

    // マテリアル選択コンボボックス.
    m_SelectedMaterial = PluginMgr::Instance().DrawCombo(m_SelectedMaterial);

    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
    {
        ImGui::PopID();
        return;
    }

    // マテリアル編集.
    auto instance = m_Instances[m_SelectedMaterial];
    material->Edit(instance);

    ImGui::PopID();
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
    e->SetAttribute("count", m_Materials.size());

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

    auto count = e->Unsigned64Attribute("count");
    m_Materials.resize(count);

    for(uint64_t i=0; i<count; ++i)
    { m_Materials[i].Deserialize(e); }
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


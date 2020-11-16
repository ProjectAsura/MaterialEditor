//-----------------------------------------------------------------------------
// File : EditorMaterial.cpp
// Desc : Editor Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <exception>
#include <EditorMaterial.h>
#include <imgui.h>
#include <asdxLogger.h>


//-----------------------------------------------------------------------------
//      エクスポートコンテキストを破棄します.
//-----------------------------------------------------------------------------
void DisposeExportContext(MaterialEditor::ExportContext*& context)
{
    if (context == nullptr)
    { return; }

    if (context->Materials == nullptr)
    { return; }

    for(auto i=0u; i<context->MaterialCount; ++i)
    {
        if (context->Materials[i].BoolParams != nullptr)
        {
            delete[] context->Materials[i].BoolParams;
            context->Materials[i].BoolParams = nullptr;
        }

        if (context->Materials[i].IntParams != nullptr)
        {
            delete[] context->Materials[i].IntParams;
            context->Materials[i].IntParams = nullptr;
        }

        if (context->Materials[i].FloatParams != nullptr)
        {
            delete[] context->Materials[i].FloatParams;
            context->Materials[i].FloatParams = nullptr;
        }

        if (context->Materials[i].Float2Params != nullptr)
        {
            delete[] context->Materials[i].Float2Params;
            context->Materials[i].Float2Params = nullptr;
        }

        if (context->Materials[i].Float3Params != nullptr)
        {
            delete[] context->Materials[i].Float3Params;
            context->Materials[i].Float3Params = nullptr;
        }

        if (context->Materials[i].FloatParams != nullptr)
        {
            delete[] context->Materials[i].FloatParams;
            context->Materials[i].FloatParams = nullptr;
        }

        if (context->Materials[i].Float2Params != nullptr)
        {
            delete[] context->Materials[i].Float2Params;
            context->Materials[i].Float2Params = nullptr;
        }

        if (context->Materials[i].Float3Params != nullptr)
        {
            delete[] context->Materials[i].FloatParams;
            context->Materials[i].FloatParams = nullptr;
        }

        if (context->Materials[i].Float4Params != nullptr)
        {
            delete[] context->Materials[i].Float4Params;
            context->Materials[i].Float4Params = nullptr;
        }

        if (context->Materials[i].Color3Params != nullptr)
        {
            delete[] context->Materials[i].Color3Params;
            context->Materials[i].Color3Params = nullptr;
        }

        if (context->Materials[i].Color4Params != nullptr)
        {
            delete[] context->Materials[i].Color4Params;
            context->Materials[i].Color4Params = nullptr;
        }

        if (context->Materials[i].Bit32Params != nullptr)
        {
            delete[] context->Materials[i].Bit32Params;
            context->Materials[i].Bit32Params = nullptr;
        }

        if (context->Materials[i].Texture2DParams != nullptr)
        {
            delete[] context->Materials[i].Texture2DParams;
            context->Materials[i].Texture2DParams = nullptr;
        }
    }

    delete[] context->Materials;
    context->Materials = nullptr;

    delete context;
    context = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// EditorMaterial class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorMaterial::EditorMaterial()
: m_SelectedMaterial(u8"標準")
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

    if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
    { return nullptr; }

    auto instance = m_Instances[m_SelectedMaterial];
    assert(instance != nullptr);

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

        assert(material != nullptr);
        material->Unbind(pContext);
    }
}

//-----------------------------------------------------------------------------
//      マテリアルを編集します.
//-----------------------------------------------------------------------------
void EditorMaterial::Edit(const std::string& filterType)
{
    ImGui::PushID(m_Name.c_str());

    if (ImGui::CollapsingHeader(m_Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        // マテリアル選択コンボボックス.
        m_SelectedMaterial = PluginMgr::Instance().DrawTypeCombo(m_SelectedMaterial);

        if (filterType != "")
        {
            if (m_SelectedMaterial != filterType)
            {
                ImGui::PopID();
                return;
            }
        }

        PluginMaterial* material;
        if (PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
        {
            // 存在チェック.
            if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
            {
                auto instance = PluginMgr::Instance().CreateInstance(m_SelectedMaterial);
                assert(instance != nullptr);
                m_Instances[m_SelectedMaterial] = instance;
            }

            // マテリアル編集.
            auto instance = m_Instances[m_SelectedMaterial];
            assert(instance != nullptr);
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
    assert(instance != nullptr);
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
    assert(instance != nullptr);
    return instance->GetBlendState();
}

//-----------------------------------------------------------------------------
//      エクスポートデータを生成します.
//-----------------------------------------------------------------------------
void EditorMaterial::CreateExportData(MaterialEditor::MaterialData* dst)
{
    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMaterial(m_SelectedMaterial, &material))
    { return; }

    if (m_Instances.find(m_SelectedMaterial) == m_Instances.end())
    { return; }

    auto src = m_Instances.at(m_SelectedMaterial);
    assert(src != nullptr);

    dst->Name            = material->m_Name.c_str();
    dst->ShaderPath      = material->m_ShaderPath.c_str();
    dst->CastShadow      = src->m_ShadowCast.GetValue();
    dst->ReceiveShadow   = src->m_ShadowReceive.GetValue();
    dst->BlendState      = src->m_BlendState.GetValue();
    dst->RasterizerState = src->m_RasterizerState.GetValue();
    dst->DepthState      = src->m_DepthState.GetValue();

    dst->BoolCount  = uint32_t(src->m_Bool.size());
    dst->BoolParams = new MaterialEditor::BoolParam[dst->BoolCount];
    for(auto i=0u; i<dst->BoolCount; ++i)
    {
        dst->BoolParams[i].Target    = material->m_Bool[i].Target.c_str();
        dst->BoolParams[i].Name      = src->m_Bool[i].Tag.c_str();
        dst->BoolParams[i].Value     = src->m_Bool[i].Param.GetValue();
    }

    dst->IntCount  = uint32_t(src->m_Int.size());
    dst->IntParams = new MaterialEditor::IntParam[dst->IntCount];
    for(auto i=0u; i<dst->IntCount; ++i)
    {
        dst->IntParams[i].Target     = material->m_Int[i].Target.c_str();
        dst->IntParams[i].Name       = src->m_Int[i].Tag.c_str();
        dst->IntParams[i].Value      = src->m_Int[i].Param.GetValue();
    }

    dst->FloatCount  = uint32_t(src->m_Float.size());
    dst->FloatParams = new MaterialEditor::FloatParam[dst->FloatCount];
    for(auto i=0u; i<dst->FloatCount; ++i)
    {
        dst->FloatParams[i].Target   = material->m_Float[i].Target.c_str();
        dst->FloatParams[i].Name     = src->m_Float[i].Tag.c_str();
        dst->FloatParams[i].Value    = src->m_Float[i].Param.GetValue();
    }

    dst->Float2Count  = uint32_t(src->m_Float2.size());
    dst->Float2Params = new MaterialEditor::Float2Param[dst->Float2Count];
    for(auto i=0u; i<dst->Float2Count; ++i)
    {
        auto& val = src->m_Float2[i].Param.GetValue();

        dst->Float2Params[i].Target  = material->m_Float2[i].Target.c_str();
        dst->Float2Params[i].Name    = src->m_Float2[i].Tag.c_str();
        dst->Float2Params[i].X       = val.x;
        dst->Float2Params[i].Y       = val.y;
    }

    dst->Float3Count  = uint32_t(src->m_Float3.size());
    dst->Float3Params = new MaterialEditor::Float3Param[dst->Float3Count];
    for(auto i=0u; i<dst->Float3Count; ++i)
    {
        auto& val = src->m_Float3[i].Param.GetValue();

        dst->Float3Params[i].Target  = material->m_Float3[i].Target.c_str();
        dst->Float3Params[i].Name    = src->m_Float3[i].Tag.c_str();
        dst->Float3Params[i].X       = val.x;
        dst->Float3Params[i].Y       = val.y;
        dst->Float3Params[i].Z       = val.z;
    }

    dst->Float4Count  = uint32_t(src->m_Float4.size());
    dst->Float4Params = new MaterialEditor::Float4Param[dst->Float4Count];
    for(auto i=0u; i<dst->Float4Count; ++i)
    {
        auto& val = src->m_Float4[i].Param.GetValue();

        dst->Float4Params[i].Target  = material->m_Float4[i].Target.c_str();
        dst->Float4Params[i].Name    = src->m_Float4[i].Tag.c_str();
        dst->Float4Params[i].X       = val.x;
        dst->Float4Params[i].Y       = val.y;
        dst->Float4Params[i].Z       = val.z;
        dst->Float4Params[i].W       = val.w;
    }

    dst->Color3Count  = uint32_t(src->m_Color3.size());
    dst->Color3Params = new MaterialEditor::Color3Param[dst->Color3Count];
    for(auto i=0u; i<dst->Color3Count; ++i)
    {
        auto& val = src->m_Color3[i].Param.GetValue();

        dst->Color3Params[i].Target  = material->m_Color3[i].Target.c_str();
        dst->Color3Params[i].Name    = src->m_Color3[i].Tag.c_str();
        dst->Color3Params[i].R       = val.x;
        dst->Color3Params[i].G       = val.y;
        dst->Color3Params[i].B       = val.z;
    }

    dst->Color4Count  = uint32_t(src->m_Color4.size());
    dst->Color4Params = new MaterialEditor::Color4Param[dst->Color4Count];
    for(auto i=0u; i<dst->Color4Count; ++i)
    {
        auto& val = src->m_Color4[i].Param.GetValue();

        dst->Color4Params[i].Target  = material->m_Color4[i].Target.c_str();
        dst->Color4Params[i].Name    = src->m_Color4[i].Tag.c_str();
        dst->Color4Params[i].R       = val.x;
        dst->Color4Params[i].G       = val.y;
        dst->Color4Params[i].B       = val.z;
        dst->Color4Params[i].A       = val.w;
    }

    dst->Bit32Count  = uint32_t(src->m_Bit32.size());
    dst->Bit32Params = new MaterialEditor::Bit32Param[dst->Bit32Count];
    for(auto i=0u; i<dst->Bit32Count; ++i)
    {
        dst->Bit32Params[i].Target   = material->m_Bit32[i].Target.c_str();
        dst->Bit32Params[i].Name     = src->m_Bit32[i].Tag.c_str();
        dst->Bit32Params[i].Value    = src->m_Bit32[i].Param.GetValue();
    }

    dst->Texture2DCount  = uint32_t(src->m_Texture2D.size());
    dst->Texture2DParams = new MaterialEditor::TextureParam[dst->Texture2DCount];
    for(auto i=0u; i<dst->Texture2DCount; ++i)
    {
        dst->Texture2DParams[i].Target   = material->m_Texture2D[i].Target.c_str();
        dst->Texture2DParams[i].Name     = src->m_Texture2D[i].Tag.c_str();
        dst->Texture2DParams[i].Path     = src->m_Texture2D[i].Param.GetPath().c_str();
    }
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

//-----------------------------------------------------------------------------
//      エクスポートコンテキストを生成します.
//-----------------------------------------------------------------------------
MaterialEditor::ExportContext* EditorMaterials::CreateExportContext()
{
    MaterialEditor::ExportContext* context = nullptr;

    try {
        context = new MaterialEditor::ExportContext();

        context->OutputPath    = nullptr; // 作った側で入れる想定.
        context->MaterialCount = uint32_t(m_Materials.size());

        if (m_Materials.empty())
        { context->Materials = nullptr; }
        else
        {
            context->Materials = new MaterialEditor::MaterialData[m_Materials.size()];
                
            for(size_t i=0; i<m_Materials.size(); ++i)
            { m_Materials[i].CreateExportData(&context->Materials[i]); }
        }
    }
    catch(std::exception e)
    {
        ELOGA("Error : exception occurred. msg = %s", e.what());
        DisposeExportContext(context);
        return nullptr;
    }

    return context;
};
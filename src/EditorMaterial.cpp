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
#include <asdxRenderState.h>
#include <asdxLocalization.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const uint32_t kSizeTable[] = {
    4,  // PROPERTY_TYPE_BOOL
    4,  // PROPERTY_TYPE_INT
    4,  // PROPERTY_TYEP_FLOAT
    8,  // PROPERTY_TYPE_FLOAT2
    12, // PROPERTY_TYPE_FLOAT3
    16, // PROPERTY_TYPE_FLOAT4
    12, // PROPERTY_TYPE_COLOR3
    16, // PROPERTY_TYPE_COLOR4
    0,  // PROPERTY_TYPE_TEXTURE1D
    0,  // PROPERTY_TYPE_TEXTURE1D_ARRAY
    0,  // PROPERTY_TYPE_TEXTURE2D
    0,  // PROPERTY_TYPE_TEXTURE2D_ARRAY
    0,  // PROPERTY_TYPE_TEXTURE3D
    0,  // PROPERTY_TYPE_TEXTURECUBE
    0,  // PROPERTY_TYPE_TEXTURECUBE_ARRAY
};

static const asdx::Localization kBlendState[] = {
    asdx::Localization(u8"不透明", u8"Opaque"),
    asdx::Localization(u8"アルファブレンド", u8"Alpha Blend"),
    asdx::Localization(u8"加算", u8"Additive"),
    asdx::Localization(u8"減算", u8"Subtract"),
    asdx::Localization(u8"事前乗算済みアルファブレンド", u8"PreMultyplied Alpha"),
    asdx::Localization(u8"乗算", u8"Multiply"),
    asdx::Localization(u8"スクリーン", u8"Screen"),
};

static const asdx::Localization kRasterizerState[] = {
    asdx::Localization(u8"両面表示", u8"Both"),
    asdx::Localization(u8"背面表示", u8"Back Face"),
    asdx::Localization(u8"前面表示", u8"Front Face"),
};

static const asdx::Localization kDepthState[] = {
    asdx::Localization(u8"深度テスト無効・深度書き込み無効", u8"Depth Test Disable / Write Disable"),
    asdx::Localization(u8"深度テスト有効・深度書き込み有効", u8"Depth Test Enable  / Write Enable"),
    asdx::Localization(u8"深度テスト有効・深度書き込み無効", u8"Depth Test Enable  / Write Disable"),
    asdx::Localization(u8"深度テスト無効・深度書き込み有効", u8"Depth Test Disable / Write Enable"),
};

static const asdx::Localization kTagShadowReceive(u8"シャドウレシーブ", u8"Shadow Receive");
static const asdx::Localization kTagShadowCast(u8"シャドウキャスト", u8"Shadow Cast");
static const asdx::Localization kTagDisplaceFace(u8"表示面", u8"Display Face");
static const asdx::Localization kTagBlendSettings(u8"ブレンド設定", u8"Blend State");
static const asdx::Localization kTagDepthSettings(u8"深度設定", u8"Depth State");
static const asdx::Localization kTagInvalidMaterial(u8"無効なマテリアルです", u8"Invalid Material");

} // namespace 


///////////////////////////////////////////////////////////////////////////////
// EditMaterialParameter structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      GUIを描画します.
//-----------------------------------------------------------------------------
void EditMaterialParameter::Draw()
{
    switch(Type)
    {
    case asura::PROPERTY_TYPE_BOOL:
        Param.pBool->DrawCheckbox(Label.c_str());
        break;

    case asura::PROPERTY_TYPE_INT:
        Param.pInt->DrawSlider(Label.c_str(), int(Slider.Step), int(Slider.Min), int(Slider.Max));
        break;

    case asura::PROPERTY_TYPE_FLOAT:
        Param.pFloat->DrawSlider(Label.c_str(), Slider.Step, Slider.Min, Slider.Max);
        break;

    case asura::PROPERTY_TYPE_FLOAT2:
        Param.pFloat2->DrawSlider(Label.c_str(), Slider.Step, Slider.Min, Slider.Max);
        break;

    case asura::PROPERTY_TYPE_FLOAT3:
        Param.pFloat3->DrawSlider(Label.c_str(), Slider.Step, Slider.Min, Slider.Max);
        break;

    case asura::PROPERTY_TYPE_FLOAT4:
        Param.pFloat4->DrawSlider(Label.c_str(), Slider.Step, Slider.Min, Slider.Max);
        break;

    case asura::PROPERTY_TYPE_COLOR3:
        Param.pColor3->DrawPicker(Label.c_str());
        break;

    case asura::PROPERTY_TYPE_COLOR4:
        Param.pColor4->DrawPicker(Label.c_str());
        break;

    default:
        break;
    }
};

//-----------------------------------------------------------------------------
//      指定ポインタにコピーします.
//-----------------------------------------------------------------------------
void EditMaterialParameter::CopyTo(uint8_t* head) const
{
    switch(Type)
    {
    case asura::PROPERTY_TYPE_BOOL:
        {
            int v = Param.pBool->GetValue() ? 1 : 0;
            memcpy(head + Offset, &v, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_INT:
        {
            auto v = Param.pInt->GetValue();
            memcpy(head + Offset, &v, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT:
        {
            auto v = Param.pFloat->GetValue();
            memcpy(head + Offset, &v, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT2:
        {
            auto v = Param.pFloat2->GetValue();
            memcpy(head + Offset, &v.x, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT3:
        {
            auto v = Param.pFloat3->GetValue();
            memcpy(head + Offset, &v.x, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT4:
        {
            auto v = Param.pFloat4->GetValue();
            memcpy(head + Offset, &v.x, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_COLOR3:
        {
            auto v = Param.pColor3->GetValue();
            memcpy(head + Offset, &v.x, sizeof(v));
        }
        break;

    case asura::PROPERTY_TYPE_COLOR4:
        {
            auto v = Param.pColor4->GetValue();
            memcpy(head + Offset, &v.x, sizeof(v));
        }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------
//      破棄処理を行います.
//-----------------------------------------------------------------------------
void EditMaterialParameter::Dispose()
{
    switch(Type)
    {
    case asura::PROPERTY_TYPE_BOOL:
        {
            if (Param.pBool != nullptr)
            {
                delete Param.pBool;
                Param.pBool = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_INT:
        {
            if (Param.pInt != nullptr)
            {
                delete Param.pInt;
                Param.pInt = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT:
        {
            if (Param.pFloat != nullptr)
            {
                delete Param.pFloat;
                Param.pFloat = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT2:
        {
            if (Param.pFloat2 != nullptr)
            {
                delete Param.pFloat2;
                Param.pFloat2 = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT3:
        {
            if (Param.pFloat3 != nullptr)
            {
                delete Param.pFloat3;
                Param.pFloat3 = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_FLOAT4:
        {
            if (Param.pFloat4 != nullptr)
            {
                delete Param.pFloat4;
                Param.pFloat4 = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_COLOR3:
        {
            if (Param.pColor3 != nullptr)
            {
                delete Param.pColor3;
                Param.pColor3 = nullptr;
            }
        }
        break;

    case asura::PROPERTY_TYPE_COLOR4:
        {
            if (Param.pColor4 != nullptr)
            {
                delete Param.pColor4;
                Param.pColor4 = nullptr;
            }
        }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditMaterialParameter::Deserialize(tinyxml2::XMLElement* element)
{
    switch(Type)
    {
    case asura::PROPERTY_TYPE_BOOL:
        asdx::Deserialize(element, Label.c_str(), *Param.pBool);
        break;

    case asura::PROPERTY_TYPE_INT:
        asdx::Deserialize(element, Label.c_str(), *Param.pInt);
        break;

    case asura::PROPERTY_TYPE_FLOAT:
        asdx::Deserialize(element, Label.c_str(), *Param.pFloat);
        break;

    case asura::PROPERTY_TYPE_FLOAT2:
        asdx::Deserialize(element, Label.c_str(), *Param.pFloat2);
        break;

    case asura::PROPERTY_TYPE_FLOAT3:
        asdx::Deserialize(element, Label.c_str(), *Param.pFloat3);
        break;

    case asura::PROPERTY_TYPE_FLOAT4:
        asdx::Deserialize(element, Label.c_str(), *Param.pFloat4);
        break;

    case asura::PROPERTY_TYPE_COLOR3:
        asdx::Deserialize(element, Label.c_str(), *Param.pColor3);
        break;

    case asura::PROPERTY_TYPE_COLOR4:
        asdx::Deserialize(element, Label.c_str(), *Param.pColor4);
        break;
    }
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditMaterialParameter::Serialize(tinyxml2::XMLDocument* doc)
{
    switch(Type)
    {
    case asura::PROPERTY_TYPE_BOOL:
        return asdx::Serialize(doc, Label.c_str(), *Param.pBool);

    case asura::PROPERTY_TYPE_INT:
        return asdx::Serialize(doc, Label.c_str(), *Param.pInt);

    case asura::PROPERTY_TYPE_FLOAT:
        return asdx::Serialize(doc, Label.c_str(), *Param.pFloat);

    case asura::PROPERTY_TYPE_FLOAT2:
        return asdx::Serialize(doc, Label.c_str(), *Param.pFloat2);

    case asura::PROPERTY_TYPE_FLOAT3:
        return asdx::Serialize(doc, Label.c_str(), *Param.pFloat3);

    case asura::PROPERTY_TYPE_FLOAT4:
        return asdx::Serialize(doc, Label.c_str(), *Param.pFloat4);

    case asura::PROPERTY_TYPE_COLOR3:
        return asdx::Serialize(doc, Label.c_str(), *Param.pColor3);

    case asura::PROPERTY_TYPE_COLOR4:
        return asdx::Serialize(doc, Label.c_str(), *Param.pColor4);
    }

    return nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// EditMaterialTexture structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void EditMaterialTexture::Draw()
{ Texture.DrawControl(Label.c_str(), "", 100, 100, true); }

//-----------------------------------------------------------------------------
//      シェーダリソースビューを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* EditMaterialTexture::GetSRV() const
{
    auto pSRV = Texture.GetSRV();
    if (pSRV != nullptr)
    { return pSRV; }

    return PluginMgr::Instance().GetDefaultSRV(DefaultValue);
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditMaterialTexture::Deserialize(tinyxml2::XMLElement* element)
{ asdx::Deserialize(element, Label.c_str(), Texture); }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditMaterialTexture::Serialize(tinyxml2::XMLDocument* doc)
{ return asdx::Serialize(doc, Label.c_str(), Texture); }


///////////////////////////////////////////////////////////////////////////////
// EditMaterialView class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      生成処理を行います.
//-----------------------------------------------------------------------------
EditMaterialView* EditMaterialView::Create(const asura::Properties& props)
{
    auto instance = new EditMaterialView();

    for(auto& itr: props.Values)
    {
        EditMaterialParameter param = {};
        param.Type          = itr.Type;
        param.Label         = itr.DisplayTag;
        param.Offset        = itr.Offset;
        param.Slider.Step   = itr.Step;
        param.Slider.Min    = itr.Min;
        param.Slider.Max    = itr.Max;

        switch(itr.Type)
        {
        case asura::PROPERTY_TYPE_BOOL:
            param.Param.pBool = new asdx::EditBool(std::stoi(itr.DefaultValue0) == 1);
            break;

        case asura::PROPERTY_TYPE_INT:
            param.Param.pInt = new asdx::EditInt(std::stoi(itr.DefaultValue0));
            break;

        case asura::PROPERTY_TYPE_FLOAT:
            param.Param.pFloat = new asdx::EditFloat(std::stof(itr.DefaultValue0));
            break;

        case asura::PROPERTY_TYPE_FLOAT2:
            param.Param.pFloat2 = new asdx::EditFloat2(
                std::stof(itr.DefaultValue0),
                std::stof(itr.DefaultValue1));
            break;

        case asura::PROPERTY_TYPE_FLOAT3:
            param.Param.pFloat3 = new asdx::EditFloat3(
                std::stof(itr.DefaultValue0),
                std::stof(itr.DefaultValue1),
                std::stof(itr.DefaultValue2));
            break;

        case asura::PROPERTY_TYPE_FLOAT4:
            param.Param.pFloat4 = new asdx::EditFloat4(
                std::stof(itr.DefaultValue0),
                std::stof(itr.DefaultValue1),
                std::stof(itr.DefaultValue2),
                std::stof(itr.DefaultValue3));
            break;

        case asura::PROPERTY_TYPE_COLOR3:
            param.Param.pColor3 = new asdx::EditColor3(
                std::stof(itr.DefaultValue0),
                std::stof(itr.DefaultValue1),
                std::stof(itr.DefaultValue2));
            break;

        case asura::PROPERTY_TYPE_COLOR4:
            param.Param.pColor4 = new asdx::EditColor4(
                std::stof(itr.DefaultValue0),
                std::stof(itr.DefaultValue1),
                std::stof(itr.DefaultValue2),
                std::stof(itr.DefaultValue3));
            break;
        }

        instance->m_Params.push_back(param);
    }

    for(auto& itr : props.Textures)
    {
        EditMaterialTexture param = {};
        param.Name          = itr.Name;
        param.Label         = itr.DisplayTag;
        param.DefaultValue  = ParseDefaultTextureType(itr.DefaultValue.c_str());

        instance->m_Textures.push_back(param);
    }

    return instance;
}

//-----------------------------------------------------------------------------
//      再生成処理を行います.
//-----------------------------------------------------------------------------
EditMaterialView* EditMaterialView::Recreate(const asura::Properties& props, EditMaterialView* pView)
{
    //　生成処理.
    auto instance = Create(props);
    if (pView == nullptr)
    { return instance; }

    // パラメータを再設定.
    for(auto& src : pView->m_Params)
    {
        for(auto& dst : instance->m_Params)
        {
            if (dst.Label == src.Label && dst.Type == src.Type)
            {
                switch(dst.Type)
                {
                case asura::PROPERTY_TYPE_BOOL:
                    dst.Param.pBool->SetValue(src.Param.pBool->GetValue());
                    break;

                case asura::PROPERTY_TYPE_INT:
                    dst.Param.pInt->SetValue(src.Param.pInt->GetValue());
                    break;

                case asura::PROPERTY_TYPE_FLOAT:
                    dst.Param.pFloat->SetValue(src.Param.pFloat->GetValue());
                    break;

                case asura::PROPERTY_TYPE_FLOAT2:
                    dst.Param.pFloat2->SetValue(src.Param.pFloat2->GetValue());
                    break;

                case asura::PROPERTY_TYPE_FLOAT3:
                    dst.Param.pFloat3->SetValue(src.Param.pFloat3->GetValue());
                    break;

                case asura::PROPERTY_TYPE_FLOAT4:
                    dst.Param.pFloat4->SetValue(src.Param.pFloat4->GetValue());
                    break;

                case asura::PROPERTY_TYPE_COLOR3:
                    dst.Param.pColor3->SetValue(src.Param.pColor3->GetValue());
                    break;

                case asura::PROPERTY_TYPE_COLOR4:
                    dst.Param.pColor4->SetValue(src.Param.pColor4->GetValue());
                    break;
                }
            }
        }
    }

    // テクスチャを再設定.
    for(auto& src : pView->m_Textures)
    {
        for(auto& dst : instance->m_Textures)
        {
            if (dst.Label == src.Label)
            { dst.Texture.SetPath(src.Texture.GetPath(), true); }
        }
    }

    return instance;
}

//-----------------------------------------------------------------------------
//      破棄処理を行います.
//-----------------------------------------------------------------------------
void EditMaterialView::Dispose(EditMaterialView*& pView)
{
    if (pView == nullptr)
    { return; }

    for(auto& itr : pView->m_Params)
    { itr.Dispose(); }

    pView->m_Params  .clear();
    pView->m_Textures.clear();

    delete pView;
    pView = nullptr;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditMaterialView::Serialize(const char* tag, tinyxml2::XMLDocument* doc)
{
    auto element = doc->NewElement(tag);

    for(auto& itr : m_Params)
    { element->InsertEndChild(itr.Serialize(doc)); }

    return element;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void EditMaterialView::Deserialize(const char* tag, tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    for(auto& itr : m_Params)
    { itr.Deserialize(e); }
}

//-----------------------------------------------------------------------------
//      描画処理します.
//-----------------------------------------------------------------------------
void EditMaterialView::Draw()
{
    m_ShadowCast.DrawCheckbox(kTagShadowCast.c_str());
    m_ShadowReceive.DrawCheckbox(kTagShadowReceive.c_str());
    m_BlendState.DrawCombo(
            kTagBlendSettings.c_str(),
            _countof(kBlendState),
            kBlendState);

    m_RasterizerState.DrawCombo(
            kTagDisplaceFace.c_str(),
            _countof(kRasterizerState),
            kRasterizerState);

    m_DepthState.DrawCombo(
            kTagDepthSettings.c_str(),
            _countof(kDepthState),
            kDepthState);

    for(auto& itr : m_Params)
    { itr.Draw(); }

    for(auto& itr : m_Textures)
    { itr.Draw(); }
}

//-----------------------------------------------------------------------------
//      定数バッファを更新します.
//-----------------------------------------------------------------------------
void EditMaterialView::UpdateBuffer(uint8_t* head)
{
    for(auto& itr : m_Params)
    { itr.CopyTo(head); }
}

//-----------------------------------------------------------------------------
//      テクスチャを設定します.
//-----------------------------------------------------------------------------
void EditMaterialView::SetTextures(ID3D11DeviceContext* pContext, const PluginShader* pShader)
{
    if (pContext == nullptr || pShader == nullptr)
    { return; }

    for(auto& itr : m_Textures)
    { pShader->SetSRV(pContext, itr.Name.c_str(), itr.GetSRV()); }
}

//-----------------------------------------------------------------------------
//      シャドウをキャストするかどうか?
//-----------------------------------------------------------------------------
bool EditMaterialView::CastShadow() const
{ return m_ShadowCast.GetValue(); }

//-----------------------------------------------------------------------------
//      シャドウをレシーブするかどうか?
//-----------------------------------------------------------------------------
bool EditMaterialView::ReceiveShadow() const
{ return m_ShadowReceive.GetValue(); }

//-----------------------------------------------------------------------------
//      ブレンドステートを取得します.
//-----------------------------------------------------------------------------
int EditMaterialView::GetBlendState() const
{ return m_BlendState.GetValue(); }

//-----------------------------------------------------------------------------
//      ラスタライザーステートを取得します.
//-----------------------------------------------------------------------------
int EditMaterialView::GetRasterizerState() const
{ return m_RasterizerState.GetValue(); }

//-----------------------------------------------------------------------------
//      深度ステートを取得します.
//-----------------------------------------------------------------------------
int EditMaterialView::GetDepthState() const
{ return m_DepthState.GetValue(); }


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
    for(auto& itr : m_EditViews)
    {
        auto instance = itr.second;
        itr.second = nullptr;
        EditMaterialView::Dispose(instance);
    }

    m_EditViews.clear();
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* EditorMaterial::Serialize(tinyxml2::XMLDocument* doc)
{
    auto element = doc->NewElement(m_Name.c_str());
    element->SetAttribute("selected_material",  m_SelectedMaterial.c_str());
    element->SetAttribute("instance_count",     m_EditViews.size());

    for(auto& itr : m_EditViews)
    {
        if (itr.second == nullptr)
        { continue; }

        element->InsertEndChild(itr.second->Serialize(itr.first.c_str(), doc));
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
        if (!PluginMgr::Instance().FindMasterMaterial(tag, &mat))
        {
            child = child->NextSiblingElement();
            continue;
        }

        auto instance = EditMaterialView::Create(mat->GetProperties());
        instance->Deserialize(tag, child);

        m_EditViews[tag] = instance;
        child = child->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------
//      マテリアルシェーダを適用します.
//-----------------------------------------------------------------------------
const PluginShader* EditorMaterial::Bind(ID3D11DeviceContext* pContext, bool lightingPass)
{
    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMasterMaterial(m_SelectedMaterial, &material))
    { return nullptr; }

    if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
    { return nullptr; }

    auto instance = m_EditViews[m_SelectedMaterial];
    assert(instance != nullptr);

    // シェーダ設定.
    material->Bind(pContext, lightingPass);

    // 定数バッファ更新.
    auto head = reinterpret_cast<uint8_t*>(material->Map(pContext));
    instance->UpdateBuffer(head);
    material->Unmap(pContext);

    // シェーダを取得.
    auto shader = (lightingPass) ? material->GetLightingShader() : material->GetShadowingShader();

    // テクスチャ設定.
    instance->SetTextures(pContext, shader);

    // レンダリングステート設定.
    auto blendType      = asdx::BlendType(instance->GetBlendState());
    auto depthType      = asdx::DepthType(instance->GetDepthState());
    auto rasterizerType = asdx::RasterizerType(instance->GetRasterizerState());
 
    auto pBS  = asdx::RenderState::GetInstance().GetBS(blendType);
    auto pDSS = asdx::RenderState::GetInstance().GetDSS(depthType);
    auto pRS  = asdx::RenderState::GetInstance().GetRS(rasterizerType);

    float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32_t sampleMask = 0xffff;
    pContext->OMSetBlendState(pBS, blendFactor, sampleMask);
    pContext->OMSetDepthStencilState(pDSS, 0);
    pContext->RSSetState(pRS);

    return shader;
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
        if (!PluginMgr::Instance().FindMasterMaterial(m_SelectedMaterial, &material))
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

    if (m_SelectedMaterial.empty() || !PluginMgr::Instance().ContainMasterMaterial(m_SelectedMaterial))
    { m_SelectedMaterial = PluginMgr::Instance().GetFirstMasterMaterialName(); }

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
        if (PluginMgr::Instance().FindMasterMaterial(m_SelectedMaterial, &material))
        {
            // 存在チェック.
            if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
            {
                auto instance = EditMaterialView::Create(material->GetProperties());
                assert(instance != nullptr);
                m_EditViews[m_SelectedMaterial] = instance;
            }

            // マテリアル編集.
            auto instance = m_EditViews.at(m_SelectedMaterial);
            assert(instance != nullptr);
            instance->Draw();
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), kTagInvalidMaterial.c_str());
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
    if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
    { return true; }

    auto instance = m_EditViews.at(m_SelectedMaterial);
    assert(instance != nullptr);
    return instance->CastShadow();
}

//-----------------------------------------------------------------------------
//      ブレンドステートを取得します.
//-----------------------------------------------------------------------------
int EditorMaterial::GetBlendState() const
{
    if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
    { return 0; }

    auto instance = m_EditViews.at(m_SelectedMaterial);
    assert(instance != nullptr);
    return instance->GetBlendState();
}
//
////-----------------------------------------------------------------------------
////      エクスポートデータを生成します.
////-----------------------------------------------------------------------------
//void EditorMaterial::CreateExportData(ExportMaterial* dst)
//{
//    PluginMaterial* material;
//    if (!PluginMgr::Instance().FindMasterMaterial(m_SelectedMaterial, &material))
//    { return; }
//
//    if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
//    { return; }
//
//    auto src = m_EditViews.at(m_SelectedMaterial);
//    assert(src != nullptr);
//
//    //dst->Name            = material->m_Name.c_str();
//    //dst->ShaderPath      = material->m_ShaderPath.c_str();
//    //dst->CastShadow      = src->m_ShadowCast.GetValue();
//    //dst->ReceiveShadow   = src->m_ShadowReceive.GetValue();
//    //dst->BlendState      = src->m_BlendState.GetValue();
//    //dst->RasterizerState = src->m_RasterizerState.GetValue();
//    //dst->DepthState      = src->m_DepthState.GetValue();
//
//    //dst->BoolCount  = uint32_t(src->m_Bool.size());
//    //dst->BoolParams = new BoolParam[dst->BoolCount];
//    //for(auto i=0u; i<dst->BoolCount; ++i)
//    //{
//    //    dst->BoolParams[i].Target    = material->m_Bool[i].Target.c_str();
//    //    dst->BoolParams[i].Name      = src->m_Bool[i].Tag.c_str();
//    //    dst->BoolParams[i].Value     = src->m_Bool[i].Param.GetValue();
//    //}
//
//    //dst->IntCount  = uint32_t(src->m_Int.size());
//    //dst->IntParams = new IntParam[dst->IntCount];
//    //for(auto i=0u; i<dst->IntCount; ++i)
//    //{
//    //    dst->IntParams[i].Target     = material->m_Int[i].Target.c_str();
//    //    dst->IntParams[i].Name       = src->m_Int[i].Tag.c_str();
//    //    dst->IntParams[i].Value      = src->m_Int[i].Param.GetValue();
//    //}
//
//    //dst->FloatCount  = uint32_t(src->m_Float.size());
//    //dst->FloatParams = new FloatParam[dst->FloatCount];
//    //for(auto i=0u; i<dst->FloatCount; ++i)
//    //{
//    //    dst->FloatParams[i].Target   = material->m_Float[i].Target.c_str();
//    //    dst->FloatParams[i].Name     = src->m_Float[i].Tag.c_str();
//    //    dst->FloatParams[i].Value    = src->m_Float[i].Param.GetValue();
//    //}
//
//    //dst->Float2Count  = uint32_t(src->m_Float2.size());
//    //dst->Float2Params = new Float2Param[dst->Float2Count];
//    //for(auto i=0u; i<dst->Float2Count; ++i)
//    //{
//    //    auto& val = src->m_Float2[i].Param.GetValue();
//
//    //    dst->Float2Params[i].Target  = material->m_Float2[i].Target.c_str();
//    //    dst->Float2Params[i].Name    = src->m_Float2[i].Tag.c_str();
//    //    dst->Float2Params[i].X       = val.x;
//    //    dst->Float2Params[i].Y       = val.y;
//    //}
//
//    //dst->Float3Count  = uint32_t(src->m_Float3.size());
//    //dst->Float3Params = new Float3Param[dst->Float3Count];
//    //for(auto i=0u; i<dst->Float3Count; ++i)
//    //{
//    //    auto& val = src->m_Float3[i].Param.GetValue();
//
//    //    dst->Float3Params[i].Target  = material->m_Float3[i].Target.c_str();
//    //    dst->Float3Params[i].Name    = src->m_Float3[i].Tag.c_str();
//    //    dst->Float3Params[i].X       = val.x;
//    //    dst->Float3Params[i].Y       = val.y;
//    //    dst->Float3Params[i].Z       = val.z;
//    //}
//
//    //dst->Float4Count  = uint32_t(src->m_Float4.size());
//    //dst->Float4Params = new Float4Param[dst->Float4Count];
//    //for(auto i=0u; i<dst->Float4Count; ++i)
//    //{
//    //    auto& val = src->m_Float4[i].Param.GetValue();
//
//    //    dst->Float4Params[i].Target  = material->m_Float4[i].Target.c_str();
//    //    dst->Float4Params[i].Name    = src->m_Float4[i].Tag.c_str();
//    //    dst->Float4Params[i].X       = val.x;
//    //    dst->Float4Params[i].Y       = val.y;
//    //    dst->Float4Params[i].Z       = val.z;
//    //    dst->Float4Params[i].W       = val.w;
//    //}
//
//    //dst->Color3Count  = uint32_t(src->m_Color3.size());
//    //dst->Color3Params = new Color3Param[dst->Color3Count];
//    //for(auto i=0u; i<dst->Color3Count; ++i)
//    //{
//    //    auto& val = src->m_Color3[i].Param.GetValue();
//
//    //    dst->Color3Params[i].Target  = material->m_Color3[i].Target.c_str();
//    //    dst->Color3Params[i].Name    = src->m_Color3[i].Tag.c_str();
//    //    dst->Color3Params[i].R       = val.x;
//    //    dst->Color3Params[i].G       = val.y;
//    //    dst->Color3Params[i].B       = val.z;
//    //}
//
//    //dst->Color4Count  = uint32_t(src->m_Color4.size());
//    //dst->Color4Params = new Color4Param[dst->Color4Count];
//    //for(auto i=0u; i<dst->Color4Count; ++i)
//    //{
//    //    auto& val = src->m_Color4[i].Param.GetValue();
//
//    //    dst->Color4Params[i].Target  = material->m_Color4[i].Target.c_str();
//    //    dst->Color4Params[i].Name    = src->m_Color4[i].Tag.c_str();
//    //    dst->Color4Params[i].R       = val.x;
//    //    dst->Color4Params[i].G       = val.y;
//    //    dst->Color4Params[i].B       = val.z;
//    //    dst->Color4Params[i].A       = val.w;
//    //}
//
//    //dst->Bit32Count  = uint32_t(src->m_Bit32.size());
//    //dst->Bit32Params = new Bit32Param[dst->Bit32Count];
//    //for(auto i=0u; i<dst->Bit32Count; ++i)
//    //{
//    //    dst->Bit32Params[i].Target   = material->m_Bit32[i].Target.c_str();
//    //    dst->Bit32Params[i].Name     = src->m_Bit32[i].Tag.c_str();
//    //    dst->Bit32Params[i].Value    = src->m_Bit32[i].Param.GetValue();
//    //}
//
//    //dst->Texture2DCount  = uint32_t(src->m_Texture2D.size());
//    //dst->Texture2DParams = new TextureParam[dst->Texture2DCount];
//    //for(auto i=0u; i<dst->Texture2DCount; ++i)
//    //{
//    //    dst->Texture2DParams[i].Target   = material->m_Texture2D[i].Target.c_str();
//    //    dst->Texture2DParams[i].Name     = src->m_Texture2D[i].Tag.c_str();
//    //    dst->Texture2DParams[i].Path     = src->m_Texture2D[i].Param.GetPath().c_str();
//    //}
//}


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

////-----------------------------------------------------------------------------
////      エクスポートコンテキストを生成します.
////-----------------------------------------------------------------------------
//ExportContext* EditorMaterials::CreateExportContext()
//{
//    ExportContext* context = nullptr;
//
//    try {
//        context = new ExportContext();
//
//        context->OutputPath    = nullptr; // 作った側で入れる想定.
//        context->MaterialCount = uint32_t(m_Materials.size());
//
//        if (m_Materials.empty())
//        { context->Materials = nullptr; }
//        else
//        {
//            context->Materials = new ExportMaterial[m_Materials.size()];
//                
//            for(size_t i=0; i<m_Materials.size(); ++i)
//            { m_Materials[i].CreateExportData(&context->Materials[i]); }
//        }
//    }
//    catch(std::exception e)
//    {
//        ELOGA("Error : exception occurred. msg = %s", e.what());
//        DisposeExportContext(context);
//        return nullptr;
//    }
//
//    return context;
//};
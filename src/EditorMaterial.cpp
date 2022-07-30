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
    asdx::Localization(u8"デフォルト"   , u8"Default"),
    asdx::Localization(u8"読み込みのみ", u8"Read Only"),
    asdx::Localization(u8"書き込みのみ", u8"Write Only"),
    asdx::Localization(u8"無効"      , u8"None"),
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
{ Texture.DrawControl(Label.c_str(), "", 100, 100); }

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

    for(auto& itr : pView->m_Textures)
    { itr.Texture.Term(); }

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

//-----------------------------------------------------------------------------
//      エクスポートデータを設定します.
//-----------------------------------------------------------------------------
void EditMaterialView::SetExportData(ExportMaterial* dst, const PluginMaterial* mat)
{
    dst->CastShadow         = m_ShadowCast.GetValue();
    dst->ReceiveShadow      = m_ShadowReceive.GetValue();
    dst->BlendState         = m_BlendState.GetValue();
    dst->RasterizerState    = m_RasterizerState.GetValue();
    dst->DepthState         = m_DepthState.GetValue();

    dst->ParamCount = uint32_t(m_Params.size());
    dst->pParams = new ExportParameter[dst->ParamCount];

    auto& props = mat->GetProperties();
    assert(props.Values.size() == m_Params.size());

    for(size_t i=0; i<m_Params.size(); ++i)
    {
        auto& srcParam = m_Params[i];
        auto  dstParam = &dst->pParams[i];

        switch(m_Params[i].Type)
        {
        case asura::PROPERTY_TYPE_BOOL:
            {
                dstParam->Type       = EXPORT_PARAMETER_TYPE_BOOL;
                dstParam->Value.Bool = srcParam.Param.pBool->GetValue();
                dstParam->Name       = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_INT:
            {
                dstParam->Type       = EXPORT_PARAMETER_TYPE_INT;
                dstParam->Value.Int  = srcParam.Param.pInt->GetValue();
                dstParam->Name       = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_FLOAT:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_FLOAT;
                dstParam->Value.Float    = srcParam.Param.pFloat->GetValue();
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_FLOAT2:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_FLOAT2;
                dstParam->Value.Float2.x = srcParam.Param.pFloat2->GetValue().x;
                dstParam->Value.Float2.y = srcParam.Param.pFloat2->GetValue().y;
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_FLOAT3:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_FLOAT3;
                dstParam->Value.Float3.x = srcParam.Param.pFloat3->GetValue().x;
                dstParam->Value.Float3.y = srcParam.Param.pFloat3->GetValue().y;
                dstParam->Value.Float3.z = srcParam.Param.pFloat3->GetValue().z;
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_FLOAT4:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_FLOAT4;
                dstParam->Value.Float4.x = srcParam.Param.pFloat4->GetValue().x;
                dstParam->Value.Float4.y = srcParam.Param.pFloat4->GetValue().y;
                dstParam->Value.Float4.z = srcParam.Param.pFloat4->GetValue().z;
                dstParam->Value.Float4.w = srcParam.Param.pFloat4->GetValue().w;
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_COLOR3:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_COLOR3;
                dstParam->Value.Color3.x = srcParam.Param.pColor3->GetValue().x;
                dstParam->Value.Color3.y = srcParam.Param.pColor3->GetValue().y;
                dstParam->Value.Color3.z = srcParam.Param.pColor3->GetValue().z;
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;

        case asura::PROPERTY_TYPE_COLOR4:
            {
                dstParam->Type           = EXPORT_PARAMETER_TYPE_COLOR4;
                dstParam->Value.Color4.x = srcParam.Param.pColor4->GetValue().x;
                dstParam->Value.Color4.y = srcParam.Param.pColor4->GetValue().y;
                dstParam->Value.Color4.z = srcParam.Param.pColor4->GetValue().z;
                dstParam->Value.Color4.w = srcParam.Param.pColor4->GetValue().w;
                dstParam->Name           = props.Values[i].Name.c_str();
            }
            break;
        }
    }

    dst->Texture2DCount = uint32_t(m_Textures.size());
    dst->Texture2Ds     = new ExportTexture[dst->Texture2DCount];

    assert(m_Textures.size() == props.Textures.size());

    for(size_t i=0; i<m_Textures.size(); ++i)
    {
        auto dstTex = &dst->Texture2Ds[i];
        dstTex->Name = props.Textures[i].Name.c_str();
        dstTex->Path = m_Textures[i].Texture.GetPath().c_str();
    }
}


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

    // 定数バッファ設定.
    shader->SetCBV(pContext, "CbProperties", material->m_EditableCB.GetPtr());

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

//-----------------------------------------------------------------------------
//      エクスポートデータを生成します.
//-----------------------------------------------------------------------------
void EditorMaterial::SetExportData(ExportMaterial* dst)
{
    PluginMaterial* material;
    if (!PluginMgr::Instance().FindMasterMaterial(m_SelectedMaterial, &material))
    { return; }

    if (m_EditViews.find(m_SelectedMaterial) == m_EditViews.end())
    { return; }

    auto view = m_EditViews.at(m_SelectedMaterial);
    assert(view != nullptr);

    dst->Name       = m_Name.c_str();
    dst->ShaderPath = material->GetShaderPath().c_str();
    view->SetExportData(dst, material);
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


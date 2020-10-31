//-----------------------------------------------------------------------------
// File : PluginMaterial.cpp
// Desc : Plugin Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <PluginMgr.h>
#include <asdxLogger.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
const char* kBlendState[] = {
    u8"不透明",
    u8"アルファブレンド",
    u8"加算",
    u8"減算",
    u8"事前乗算済みアルファブレンド",
    u8"乗算",
    u8"スクリーン"
};

const char* kRasterizerState[] = {
    u8"両面表示",
    u8"背面表示",
    u8"前面表示"
};

const char* kDepthState[] = {
    u8"深度テスト無効・深度書き込み無効",
    u8"深度テスト有効・深度書き込み有効",
    u8"深度テスト有効・深度書き込み無効",
    u8"深度テスト無効・深度書き込み有効"
};

}// namespace

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefShadowCast& param)
{
    auto element = doc->NewElement("shadow_cast");
    element->SetAttribute("editable", param.Editable);
    element->SetAttribute("default",  param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefShadowReceive& param)
{
    auto element = doc->NewElement("shadow_receive");
    element->SetAttribute("editable", param.Editable);
    element->SetAttribute("default",  param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefBlendState& param)
{
    auto element = doc->NewElement("blend_state");
    element->SetAttribute("editable", param.Editable);
    element->SetAttribute("default",  param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefRasterizerState& param)
{
    auto element = doc->NewElement("rasterizer_state");
    element->SetAttribute("editable", param.Editable);
    element->SetAttribute("default",  param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefDepthState& param)
{
    auto element = doc->NewElement("depth_state");
    element->SetAttribute("editable", param.Editable);
    element->SetAttribute("default",  param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefBool& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefInt& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    element->SetAttribute("step",    param.Step);
    element->SetAttribute("mini",    param.Mini);
    element->SetAttribute("maxi",    param.Maxi);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    element->SetAttribute("step",    param.Step);
    element->SetAttribute("mini",    param.Mini);
    element->SetAttribute("maxi",    param.Maxi);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat2& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat3& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("default_z",  param.Default.z);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat4& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("default_z",  param.Default.z);
    element->SetAttribute("default_w",  param.Default.w);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefColor3& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_r",  param.Default.x);
    element->SetAttribute("default_g",  param.Default.y);
    element->SetAttribute("default_b",  param.Default.z);
    element->SetAttribute("wheel",      param.Wheel);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefColor4& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_r",  param.Default.x);
    element->SetAttribute("default_g",  param.Default.y);
    element->SetAttribute("default_b",  param.Default.z);
    element->SetAttribute("default_a",  param.Default.w);
    element->SetAttribute("wheel",      param.Wheel);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefBit32& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefTexture2D& param)
{
    auto element = doc->NewElement(param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", int(param.Default));
    return element;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefShadowCast& param)
{
    auto e = element->FirstChildElement("shadow_cast");
    if (e == nullptr)
    { return; }

    param.Editable = e->BoolAttribute("editable");
    param.Default  = e->BoolAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefShadowReceive& param)
{
    auto e = element->FirstChildElement("shadow_receive");
    if (e == nullptr)
    { return; }

    param.Editable = e->BoolAttribute("editable");
    param.Default  = e->BoolAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefBlendState& param)
{
    auto e = element->FirstChildElement("blend_state");
    if (e == nullptr)
    { return; }

    param.Editable = e->BoolAttribute("editable");
    param.Default  = e->IntAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefRasterizerState& param)
{
    auto e = element->FirstChildElement("rasterizer_state");
    if (e == nullptr)
    { return; }

    param.Editable = e->BoolAttribute("editable");
    param.Default  = e->IntAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefDepthState& param)
{
    auto e = element->FirstChildElement("depth_state");
    if (e == nullptr)
    { return; }

    param.Editable = e->BoolAttribute("editable");
    param.Default  = e->IntAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefBool& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target  = e->Attribute("target");
    param.Default = e->BoolAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefInt& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target    = e->Attribute("target");
    param.Default   = e->IntAttribute("default");
    param.Step      = e->IntAttribute("step");
    param.Mini      = e->IntAttribute("mini");
    param.Maxi      = e->IntAttribute("maxi");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefFloat& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target    = e->Attribute("target");
    param.Default   = e->FloatAttribute("default");
    param.Step      = e->FloatAttribute("step");
    param.Mini      = e->FloatAttribute("mini");
    param.Maxi      = e->FloatAttribute("maxi");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefFloat2& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target        = e->Attribute("target");
    param.Default.x     = e->FloatAttribute("default_x");
    param.Default.y     = e->FloatAttribute("default_y");
    param.Step          = e->FloatAttribute("step");
    param.Mini          = e->FloatAttribute("mini");
    param.Maxi          = e->FloatAttribute("maxi");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefFloat3& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target        = e->Attribute("target");
    param.Default.x     = e->FloatAttribute("default_x");
    param.Default.y     = e->FloatAttribute("default_y");
    param.Default.z     = e->FloatAttribute("default_z");
    param.Step          = e->FloatAttribute("step");
    param.Mini          = e->FloatAttribute("mini");
    param.Maxi          = e->FloatAttribute("maxi");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefFloat4& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target        = e->Attribute("target");
    param.Default.x     = e->FloatAttribute("default_x");
    param.Default.y     = e->FloatAttribute("default_y");
    param.Default.z     = e->FloatAttribute("default_z");
    param.Default.w     = e->FloatAttribute("default_w");
    param.Step          = e->FloatAttribute("step");
    param.Mini          = e->FloatAttribute("mini");
    param.Maxi          = e->FloatAttribute("maxi");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefColor3& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target        = e->Attribute("target");
    param.Default.x     = e->FloatAttribute("default_r");
    param.Default.y     = e->FloatAttribute("default_g");
    param.Default.z     = e->FloatAttribute("default_b");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefColor4& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target        = e->Attribute("target");
    param.Default.x     = e->FloatAttribute("default_r");
    param.Default.y     = e->FloatAttribute("default_g");
    param.Default.z     = e->FloatAttribute("default_b");
    param.Default.w     = e->FloatAttribute("default_a");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefBit32& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target    = e->Attribute("target");
    param.Default   = e->UnsignedAttribute("default");
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, UiDefTexture2D& param)
{
    auto e = element->FirstChildElement(param.DisplayTag.c_str());
    if (e == nullptr)
    { return; }

    param.Target    = e->Attribute("target");
    param.Default   = DEFAULT_TEXTURE_TYPE(e->IntAttribute("default"));
}


///////////////////////////////////////////////////////////////////////////////
// Materialinstance class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
MaterialInstance::MaterialInstance()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
MaterialInstance::~MaterialInstance()
{
    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { m_Texture2D[i].Param.Term(); }
    m_Texture2D.clear();
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* MaterialInstance::Serialize(tinyxml2::XMLDocument* doc)
{
    auto element = doc->NewElement(m_MaterialName.c_str());
    element->SetAttribute("shadow_cast", m_ShadowCast.GetValue());
    element->SetAttribute("shadow_receive", m_ShadowReceive.GetValue());
    element->SetAttribute("blend_state", m_BlendState.GetValue());
    element->SetAttribute("rasterizer_state", m_RasterizerState.GetValue());
    element->SetAttribute("depth_state", m_DepthStencilState.GetValue());
    element->SetAttribute("count_bool", m_Bool.size());
    element->SetAttribute("count_int", m_Int.size());
    element->SetAttribute("count_float", m_Float.size());
    element->SetAttribute("count_float2", m_Float2.size());
    element->SetAttribute("count_float3", m_Float3.size());
    element->SetAttribute("count_float4", m_Float4.size());
    element->SetAttribute("count_color3", m_Color3.size());
    element->SetAttribute("count_color4", m_Color4.size());
    element->SetAttribute("count_bit32", m_Bit32.size());
    element->SetAttribute("count_texture2d", m_Texture2D.size());

    for(size_t i=0; i<m_Bool.size(); ++i)
    { element->InsertEndChild(m_Bool[i].Serialize(doc)); }

    for(size_t i=0; i<m_Int.size(); ++i)
    { element->InsertEndChild(m_Int[i].Serialize(doc)); }

    for(size_t i=0; i<m_Float.size(); ++i)
    { element->InsertEndChild(m_Float[i].Serialize(doc)); }

    for(size_t i=0; i<m_Float2.size(); ++i)
    { element->InsertEndChild(m_Float2[i].Serialize(doc)); }

    for(size_t i=0; i<m_Float3.size(); ++i)
    { element->InsertEndChild(m_Float3[i].Serialize(doc)); }

    for(size_t i=0; i<m_Float4.size(); ++i)
    { element->InsertEndChild(m_Float4[i].Serialize(doc)); }

    for(size_t i=0; i<m_Color3.size(); ++i)
    { element->InsertEndChild(m_Color3[i].Serialize(doc)); }

    for(size_t i=0; i<m_Color4.size(); ++i)
    { element->InsertEndChild(m_Color4[i].Serialize(doc)); }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    { element->InsertEndChild(m_Bit32[i].Serialize(doc)); }

    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { element->InsertEndChild(m_Texture2D[i].Serialize(doc)); }

    return element;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void MaterialInstance::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement(m_MaterialName.c_str());
    if (e == nullptr)
    { return; }

    m_ShadowCast        .SetValue(e->BoolAttribute("shadow_cast"));
    m_ShadowReceive     .SetValue(e->BoolAttribute("shadow_receive"));
    m_BlendState        .SetValue(e->IntAttribute("blend_state"));
    m_RasterizerState   .SetValue(e->IntAttribute("rasterizer_state"));
    m_DepthStencilState .SetValue(e->IntAttribute("depth_state"));

    auto count_bool         = e->Unsigned64Attribute("count_bool");
    auto count_int          = e->Unsigned64Attribute("count_int");
    auto count_float        = e->Unsigned64Attribute("count_float");
    auto count_float2       = e->Unsigned64Attribute("count_float2");
    auto count_float3       = e->Unsigned64Attribute("count_float3");
    auto count_float4       = e->Unsigned64Attribute("count_float4");
    auto count_color3       = e->Unsigned64Attribute("count_color3");
    auto count_color4       = e->Unsigned64Attribute("count_color4");
    auto count_bit32        = e->Unsigned64Attribute("count_bit32");
    auto count_texture2d    = e->Unsigned64Attribute("count_texture2d");

    m_Bool      .resize(count_bool);
    m_Int       .resize(count_int);
    m_Float     .resize(count_float);
    m_Float2    .resize(count_float2);
    m_Float3    .resize(count_float3);
    m_Float4    .resize(count_float4);
    m_Color3    .resize(count_color3);
    m_Color4    .resize(count_color4);
    m_Bit32     .resize(count_bit32);
    m_Texture2D .resize(count_texture2d);

    for(size_t i=0; i<m_Bool.size(); ++i)
    { m_Bool[i].Deserialize(e); }

    for(size_t i=0; i<m_Int.size(); ++i)
    { m_Int[i].Deserialize(e); }

    for(size_t i=0; i<m_Float.size(); ++i)
    { m_Float[i].Deserialize(e); }

    for(size_t i=0; i<m_Float2.size(); ++i)
    { m_Float2[i].Deserialize(e); }

    for(size_t i=0; i<m_Float3.size(); ++i)
    { m_Float3[i].Deserialize(e); }

    for(size_t i=0; i<m_Float4.size(); ++i)
    { m_Float4[i].Deserialize(e); }

    for(size_t i=0; i<m_Color3.size(); ++i)
    { m_Color3[i].Deserialize(e); }

    for(size_t i=0; i<m_Color4.size(); ++i)
    { m_Color4[i].Deserialize(e); }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    { m_Bit32[i].Deserialize(e); }

    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { m_Texture2D[i].Deserialize(e); }
}


///////////////////////////////////////////////////////////////////////////////
// PluginMaterial class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
PluginMaterial::PluginMaterial()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
PluginMaterial::~PluginMaterial()
{ Term(); }

//-----------------------------------------------------------------------------
//      マテリアル名を取得します.
//-----------------------------------------------------------------------------
const std::string& PluginMaterial::GetName() const
{ return m_Name; }

//-----------------------------------------------------------------------------
//      ロードします.
//-----------------------------------------------------------------------------
bool PluginMaterial::Load(const char* path)
{
    tinyxml2::XMLDocument doc;
    auto ret = doc.LoadFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOG("Error : PluginMaterial Load Failed. path = %s", path);
        return false;
    }

    // デシリアライズ.
    Deserialize(&doc);

    // シェーダをロード.
    if (!m_Shader.Load(m_ShaderPath.c_str()))
    {
        ELOG("Error : PluginShader::Load() Failed. path = %s", m_ShaderPath.c_str());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      セーブします.
//-----------------------------------------------------------------------------
bool PluginMaterial::Save(const char* path)
{
    tinyxml2::XMLDocument doc;
    auto decl = doc.NewDeclaration();
    doc.InsertEndChild(decl);
    doc.InsertEndChild(Serialize(&doc));

    auto ret = doc.SaveFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    { return false; }

    return true;
}

//-----------------------------------------------------------------------------
//      解放処理を行います.
//-----------------------------------------------------------------------------
void PluginMaterial::Term()
{
    m_Shader.Term();

    m_Name.clear();

    m_Bool.clear();
    m_Bool.shrink_to_fit();

    m_Int.clear();
    m_Int.shrink_to_fit();

    m_Float.clear();
    m_Float.shrink_to_fit();

    m_Float2.clear();
    m_Float2.shrink_to_fit();

    m_Float3.clear();
    m_Float3.shrink_to_fit();

    m_Float4.clear();
    m_Float4.shrink_to_fit();

    m_Color3.clear();
    m_Color3.shrink_to_fit();

    m_Color4.clear();
    m_Color4.shrink_to_fit();

    m_Bit32.clear();
    m_Bit32.shrink_to_fit();

    m_Texture2D.clear();
    m_Texture2D.shrink_to_fit();
}

//-----------------------------------------------------------------------------
//      シェーダを設定します.
//-----------------------------------------------------------------------------
void PluginMaterial::Bind(ID3D11DeviceContext* pContext, const MaterialInstance* instance)
{
    if (pContext == nullptr || instance == nullptr)
    { return; }

    m_Shader.Bind(pContext);
    m_Shader.Update(pContext, instance);
}

//-----------------------------------------------------------------------------
//      シェーダの設定を解除します.
//-----------------------------------------------------------------------------
void PluginMaterial::Unbind(ID3D11DeviceContext* pContext)
{
    if (pContext == nullptr)
    { return; }

    m_Shader.Unbind(pContext);
}

//-----------------------------------------------------------------------------
//      マテリアルインスタンスを編集します.
//-----------------------------------------------------------------------------
void PluginMaterial::Edit(MaterialInstance* instance)
{
    if (instance == nullptr)
    { return; }

    if (m_ShadowCast.Editable)
    { instance->m_ShadowCast.DrawCheckbox(u8"シャドウキャスト"); }

    if (m_ShadowReceive.Editable)
    { instance->m_ShadowReceive.DrawCheckbox(u8"シャドウレシーブ"); }

    if (m_BlendState.Editable)
    {
        instance->m_BlendState.DrawCombo(
            u8"ブレンドステート",
            _countof(kBlendState),
            kBlendState);
    }

    if (m_RasterizerState.Editable)
    {
        instance->m_RasterizerState.DrawCombo(
            u8"ラスタライザーステート",
            _countof(kRasterizerState),
            kRasterizerState);
    }

    if (m_DepthState.Editable)
    {
        instance->m_DepthStencilState.DrawCombo(
            u8"深度ステート",
            _countof(kDepthState),
            kDepthState);
    }

    for(size_t i=0; i<instance->m_Bool.size(); ++i)
    { instance->m_Bool[i].Param.DrawCheckbox(m_Bool[i].DisplayTag.c_str()); }

    for(size_t i=0; i<instance->m_Int.size(); ++i)
    {
        instance->m_Int[i].Param.DrawSlider(
            m_Int[i].DisplayTag.c_str(),
            m_Int[i].Step,
            m_Int[i].Mini,
            m_Int[i].Maxi);
    }

    for(size_t i=0; i<instance->m_Float.size(); ++i)
    {
        instance->m_Float[i].Param.DrawSlider(
            m_Float[i].DisplayTag.c_str(),
            m_Float[i].Step,
            m_Float[i].Mini,
            m_Float[i].Maxi);
    }

    for(size_t i=0; i<instance->m_Float2.size(); ++i)
    {
        instance->m_Float2[i].Param.DrawSlider(
            m_Float2[i].DisplayTag.c_str(),
            m_Float2[i].Step,
            m_Float2[i].Mini,
            m_Float2[i].Maxi);
    }

    for(size_t i=0; i<instance->m_Float3.size(); ++i)
    {
        instance->m_Float3[i].Param.DrawSlider(
            m_Float3[i].DisplayTag.c_str(),
            m_Float3[i].Step,
            m_Float3[i].Mini,
            m_Float3[i].Maxi);
    }

    for(size_t i=0; i<instance->m_Float4.size(); ++i)
    {
        instance->m_Float4[i].Param.DrawSlider(
            m_Float4[i].DisplayTag.c_str(),
            m_Float4[i].Step,
            m_Float4[i].Mini,
            m_Float4[i].Maxi);
    }

    for(size_t i=0; i<instance->m_Color3.size(); ++i)
    {
        if (m_Color3[i].Wheel)
        { instance->m_Color3[i].Param.DrawWheel(m_Color3[i].DisplayTag.c_str()); }
        else
        { instance->m_Color3[i].Param.DrawPicker(m_Color3[i].DisplayTag.c_str()); }
    }

    for(size_t i=0; i<instance->m_Color4.size(); ++i)
    {
        if (m_Color4[i].Wheel)
        { instance->m_Color4[i].Param.DrawWheel(m_Color4[i].DisplayTag.c_str()); }
        else
        { instance->m_Color4[i].Param.DrawPicker(m_Color4[i].DisplayTag.c_str()); }
    }

    for(size_t i=0; i<instance->m_Bit32.size(); ++i)
    { instance->m_Bit32[i].Param.DrawCheckBox(m_Bit32[i].DisplayTag.c_str()); }

    for(size_t i=0; i<instance->m_Texture2D.size(); ++i)
    {
        instance->m_Texture2D[i].Param.DrawControl(
            m_Texture2D[i].DisplayTag.c_str(), "", 64, 64);
    }
}

//-----------------------------------------------------------------------------
//      シェーダを取得します.
//-----------------------------------------------------------------------------
const PluginShader* PluginMaterial::GetShader() const
{ return &m_Shader; }

//-----------------------------------------------------------------------------
//      デフォルト値を設定します.
//-----------------------------------------------------------------------------
void PluginMaterial::Reset(MaterialInstance* instance)
{
    for(size_t i=0; i<m_Bool.size(); ++i)
    {
        if (m_Bool[i].DisplayTag != instance->m_Bool[i].Tag)
        { continue; }

        instance->m_Bool[i].Param.SetValue(m_Bool[i].Default);
    }

    for(size_t i=0; i<m_Int.size(); ++i)
    {
        if (m_Int[i].DisplayTag != instance->m_Int[i].Tag)
        { continue; }

        instance->m_Int[i].Param.SetValue(m_Int[i].Default);
    }

    for(size_t i=0; i<m_Float.size(); ++i)
    {
        if (m_Float[i].DisplayTag != instance->m_Float[i].Tag)
        { continue; }

        instance->m_Float[i].Param.SetValue(m_Float[i].Default);
    }

    for(size_t i=0; i<m_Float2.size(); ++i)
    {
        if (m_Float2[i].DisplayTag != instance->m_Float2[i].Tag)
        { continue; }

        instance->m_Float2[i].Param.SetValue(m_Float2[i].Default);
    }

    for(size_t i=0; i<m_Float3.size(); ++i)
    {
        if (m_Float3[i].DisplayTag != instance->m_Float3[i].Tag)
        { continue; }

        instance->m_Float3[i].Param.SetValue(m_Float3[i].Default);
    }

    for(size_t i=0; i<m_Float4.size(); ++i)
    {
        if (m_Float4[i].DisplayTag != instance->m_Float4[i].Tag)
        { continue; }

        instance->m_Float4[i].Param.SetValue(m_Float4[i].Default);
    }

    for(size_t i=0; i<m_Color3.size(); ++i)
    {
        if (m_Color3[i].DisplayTag != instance->m_Color3[i].Tag)
        { continue; }

        instance->m_Color3[i].Param.SetValue(m_Color3[i].Default);
    }

    for(size_t i=0; i<m_Color4.size(); ++i)
    {
        if (m_Color4[i].DisplayTag != instance->m_Color4[i].Tag)
        { continue; }

        instance->m_Color4[i].Param.SetValue(m_Color4[i].Default);
    }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    {
        if (m_Bit32[i].DisplayTag != instance->m_Bit32[i].Tag)
        { continue; }

        instance->m_Bit32[i].Param.SetValue(m_Bit32[i].Default);
    }
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* PluginMaterial::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("PluginMaterial");

    e->SetAttribute("name",             m_Name      .c_str());
    e->SetAttribute("shader_path",      m_ShaderPath.c_str());
    e->SetAttribute("count_bool",       m_Bool      .size());
    e->SetAttribute("count_int",        m_Int       .size());
    e->SetAttribute("count_float",      m_Float     .size());
    e->SetAttribute("count_float2",     m_Float2    .size());
    e->SetAttribute("count_float3",     m_Float3    .size());
    e->SetAttribute("count_float4",     m_Float3    .size());
    e->SetAttribute("count_color3",     m_Color3    .size());
    e->SetAttribute("count_color4",     m_Color4    .size());
    e->SetAttribute("count_bit32",      m_Bit32     .size());
    e->SetAttribute("count_texture2d",  m_Texture2D .size());

    for(size_t i=0; i<m_Bool.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Bool[i])); }

    for(size_t i=0; i<m_Int.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Int[i])); }

    for(size_t i=0; i<m_Float.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Float[i])); }

    for(size_t i=0; i<m_Float2.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Float2[i])); }

    for(size_t i=0; i<m_Float3.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Float3[i])); }

    for(size_t i=0; i<m_Float4.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Float4[i])); }

    for(size_t i=0; i<m_Color3.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Color3[i])); }

    for(size_t i=0; i<m_Color4.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Color4[i])); }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Bit32[i])); }

    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { e->InsertEndChild(::Serialize(doc, m_Texture2D[i])); }

    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void PluginMaterial::Deserialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->FirstChildElement("PluginMaterial");
    if (e == nullptr)
    { return; }

    m_Name          = e->Attribute("name");
    m_ShaderPath    = e->Attribute("shader_path");

    auto count_bool         = e->Unsigned64Attribute("count_bool");
    auto count_int          = e->Unsigned64Attribute("count_int");
    auto count_float        = e->Unsigned64Attribute("count_float");
    auto count_float2       = e->Unsigned64Attribute("count_float2");
    auto count_float3       = e->Unsigned64Attribute("count_float3");
    auto count_float4       = e->Unsigned64Attribute("count_float4");
    auto count_color3       = e->Unsigned64Attribute("count_color3");
    auto count_color4       = e->Unsigned64Attribute("count_color4");
    auto count_bit32        = e->Unsigned64Attribute("count_bit32");
    auto count_texture2d    = e->Unsigned64Attribute("count_texture2d");

    ::Deserialize(e, m_ShadowCast);
    ::Deserialize(e, m_ShadowReceive);
    ::Deserialize(e, m_BlendState);
    ::Deserialize(e, m_RasterizerState);
    ::Deserialize(e, m_DepthState);

    m_Bool      .resize(count_bool);
    m_Int       .resize(count_int);
    m_Float     .resize(count_float);
    m_Float2    .resize(count_float2);
    m_Float3    .resize(count_float3);
    m_Float4    .resize(count_float4);
    m_Color3    .resize(count_color3);
    m_Color4    .resize(count_color4);
    m_Bit32     .resize(count_bit32);
    m_Texture2D .resize(count_texture2d);

    for(size_t i=0; i<m_Bool.size(); ++i)
    { ::Deserialize(e, m_Bool[i]); }

    for(size_t i=0; i<m_Int.size(); ++i)
    { ::Deserialize(e, m_Int[i]); }

    for(size_t i=0; i<m_Float.size(); ++i)
    { ::Deserialize(e, m_Float[i]); }

    for(size_t i=0; i<m_Float2.size(); ++i)
    { ::Deserialize(e, m_Float2[i]); }

    for(size_t i=0; i<m_Float3.size(); ++i)
    { ::Deserialize(e, m_Float3[i]); }

    for(size_t i=0; i<m_Float4.size(); ++i)
    { ::Deserialize(e, m_Float4[i]); }

    for(size_t i=0; i<m_Color3.size(); ++i)
    { ::Deserialize(e, m_Color3[i]); }

    for(size_t i=0; i<m_Color4.size(); ++i)
    { ::Deserialize(e, m_Color4[i]); }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    { ::Deserialize(e, m_Bit32[i]); }

    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { ::Deserialize(e, m_Texture2D[i]); }

}


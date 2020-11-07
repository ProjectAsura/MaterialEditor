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
#include <asdxMisc.h>
#include <asdxRenderState.h>


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
    auto element = doc->NewElement("bool");
    element->SetAttribute("label", param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefInt& param)
{
    auto element = doc->NewElement("int");
    element->SetAttribute("label",   param.DisplayTag.c_str());
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
    auto element = doc->NewElement("float");
    element->SetAttribute("label",   param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    element->SetAttribute("step",    param.Step);
    element->SetAttribute("mini",    param.Mini);
    element->SetAttribute("maxi",    param.Maxi);
    element->SetAttribute("converter", int(param.Converter));
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat2& param)
{
    auto element = doc->NewElement("float2");
    element->SetAttribute("label",      param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    element->SetAttribute("converter", int(param.Converter));
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat3& param)
{
    auto element = doc->NewElement("float3");
    element->SetAttribute("label",      param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("default_z",  param.Default.z);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    element->SetAttribute("converter", int(param.Converter));
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat4& param)
{
    auto element = doc->NewElement("float4");
    element->SetAttribute("label",      param.DisplayTag.c_str());
    element->SetAttribute("target",     param.Target.c_str());
    element->SetAttribute("default_x",  param.Default.x);
    element->SetAttribute("default_y",  param.Default.y);
    element->SetAttribute("default_z",  param.Default.z);
    element->SetAttribute("default_w",  param.Default.w);
    element->SetAttribute("step",       param.Step);
    element->SetAttribute("mini",       param.Mini);
    element->SetAttribute("maxi",       param.Maxi);
    element->SetAttribute("converter", int(param.Converter));
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefColor3& param)
{
    auto element = doc->NewElement("color3");
    element->SetAttribute("label",      param.DisplayTag.c_str());
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
    auto element = doc->NewElement("color4");
    element->SetAttribute("label",      param.DisplayTag.c_str());
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
    auto element = doc->NewElement("bit32");
    element->SetAttribute("label",   param.DisplayTag.c_str());
    element->SetAttribute("target",  param.Target.c_str());
    element->SetAttribute("default", param.Default);
    return element;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefTexture2D& param)
{
    auto element = doc->NewElement("texture2D");
    element->SetAttribute("label",   param.DisplayTag.c_str());
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
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefBool>& param)
{
    auto e = element->FirstChildElement("bool");
    if (e == nullptr)
    { return; }

    while (e != nullptr)
    {
        UiDefBool p;
        p.DisplayTag = e->Attribute("label");
        p.Target     = e->Attribute("target");
        p.Default    = e->BoolAttribute("default");
        param.push_back(p);

        e = e->NextSiblingElement("bool");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefInt>& param)
{
    auto e = element->FirstChildElement("int");
    if (e == nullptr)
    { return; }

    while (e != nullptr)
    {
        UiDefInt p;
        p.DisplayTag = e->Attribute("label");
        p.Target     = e->Attribute("target");
        p.Default    = e->IntAttribute("default");
        p.Step       = e->IntAttribute("step");
        p.Mini       = e->IntAttribute("mini");
        p.Maxi       = e->IntAttribute("maxi");
        param.push_back(p);

        e = e->NextSiblingElement("int");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat>& param)
{
    auto e = element->FirstChildElement("float");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefFloat p;
        p.DisplayTag = e->Attribute("label");
        p.Target     = e->Attribute("target");
        p.Default    = e->FloatAttribute("default");
        p.Step       = e->FloatAttribute("step");
        p.Mini       = e->FloatAttribute("mini");
        p.Maxi       = e->FloatAttribute("maxi");
        p.Converter  = CONVERTER_TYPE(e->IntAttribute("converter"));
        param.push_back(p);

        e = e->NextSiblingElement("float");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat2>& param)
{
    auto e = element->FirstChildElement("float2");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefFloat2 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default.x     = e->FloatAttribute("default_x");
        p.Default.y     = e->FloatAttribute("default_y");
        p.Step          = e->FloatAttribute("step");
        p.Mini          = e->FloatAttribute("mini");
        p.Maxi          = e->FloatAttribute("maxi");
        p.Converter     = CONVERTER_TYPE(e->IntAttribute("converter"));
        param.push_back(p);

        e = e->NextSiblingElement("float2");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat3>& param)
{
    auto e = element->FirstChildElement("float3");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefFloat3 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default.x     = e->FloatAttribute("default_x");
        p.Default.y     = e->FloatAttribute("default_y");
        p.Default.z     = e->FloatAttribute("default_z");
        p.Step          = e->FloatAttribute("step");
        p.Mini          = e->FloatAttribute("mini");
        p.Maxi          = e->FloatAttribute("maxi");
        p.Converter     = CONVERTER_TYPE(e->IntAttribute("converter"));
        param.push_back(p);

        e = e->NextSiblingElement("float3");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat4>& param)
{
    auto e = element->FirstChildElement("float4");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefFloat4 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default.x     = e->FloatAttribute("default_x");
        p.Default.y     = e->FloatAttribute("default_y");
        p.Default.z     = e->FloatAttribute("default_z");
        p.Default.w     = e->FloatAttribute("default_w");
        p.Step          = e->FloatAttribute("step");
        p.Mini          = e->FloatAttribute("mini");
        p.Maxi          = e->FloatAttribute("maxi");
        p.Converter     = CONVERTER_TYPE(e->IntAttribute("converter"));
        param.push_back(p);

        e = e->NextSiblingElement("float4");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefColor3>& param)
{
    auto e = element->FirstChildElement("color3");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefColor3 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default.x     = e->FloatAttribute("default_r");
        p.Default.y     = e->FloatAttribute("default_g");
        p.Default.z     = e->FloatAttribute("default_b");
        param.push_back(p);

        e = e->NextSiblingElement("Color3");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefColor4>& param)
{
    auto e = element->FirstChildElement("color4");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefColor4 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default.x     = e->FloatAttribute("default_r");
        p.Default.y     = e->FloatAttribute("default_g");
        p.Default.z     = e->FloatAttribute("default_b");
        p.Default.w     = e->FloatAttribute("default_a");
        param.push_back(p);

        e = e->NextSiblingElement("Color4");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefBit32>& param)
{
    auto e = element->FirstChildElement("bit32");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefBit32 p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default       = e->UnsignedAttribute("default");
        param.push_back(p);

        e = e->NextSiblingElement("Bit32");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefTexture2D>& param)
{
    auto e = element->FirstChildElement("texture2D");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiDefTexture2D p;
        p.DisplayTag    = e->Attribute("label");
        p.Target        = e->Attribute("target");
        p.Default       = DEFAULT_TEXTURE_TYPE(e->IntAttribute("default"));
        param.push_back(p);

        e = e->NextSiblingElement("Texture2D");
    }
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiBool& value)
{
    auto e = doc->NewElement("bool");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("value", value.Param.GetValue());
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiInt& value)
{
    auto e = doc->NewElement("int");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("value", value.Param.GetValue());
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat& value)
{
    auto e = doc->NewElement("float");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("value", value.Param.GetValue());
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat2& value)
{
    auto e = doc->NewElement("float2");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("x", value.Param.GetValue().x);
    e->SetAttribute("y", value.Param.GetValue().y);
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat3& value)
{
    auto e = doc->NewElement("float3");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("x", value.Param.GetValue().x);
    e->SetAttribute("y", value.Param.GetValue().y);
    e->SetAttribute("z", value.Param.GetValue().z);
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat4& value)
{
    auto e = doc->NewElement("float4");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("x", value.Param.GetValue().x);
    e->SetAttribute("y", value.Param.GetValue().y);
    e->SetAttribute("z", value.Param.GetValue().z);
    e->SetAttribute("w", value.Param.GetValue().w);
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiColor3& value)
{
    auto e = doc->NewElement("color3");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("r", value.Param.GetValue().x);
    e->SetAttribute("g", value.Param.GetValue().y);
    e->SetAttribute("b", value.Param.GetValue().z);
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiColor4& value)
{
    auto e = doc->NewElement("color4");
    e->SetAttribute("tag", value.Tag.c_str());
    e->SetAttribute("r", value.Param.GetValue().x);
    e->SetAttribute("g", value.Param.GetValue().y);
    e->SetAttribute("b", value.Param.GetValue().z);
    e->SetAttribute("a", value.Param.GetValue().w);
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiBit32& value)
{
    auto e = doc->NewElement("bit32");
    e->SetAttribute("tag",   value.Tag.c_str());
    e->SetAttribute("value", value.Param.GetValue());
    return e;
}

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiTexture2D& value)
{
    auto e = doc->NewElement("texture2D");
    e->SetAttribute("tag",  value.Tag.c_str());
    e->SetAttribute("path", value.Param.GetPath().c_str());
    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiBool>& value)
{
    auto e = element->FirstChildElement("bool");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiBool p = {};
        p.Tag = e->Attribute("tag");
        p.Param.SetValue(e->BoolAttribute("value"));
        value.push_back(p);

        e = e->NextSiblingElement("bool");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiInt>& value)
{
    auto e = element->FirstChildElement("int");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiInt p = {};
        p.Tag = e->Attribute("tag");
        p.Param.SetValue(e->IntAttribute("value"));
        value.push_back(p);

        e = e->NextSiblingElement("int");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat>& value)
{
    auto e = element->FirstChildElement("float");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiFloat p = {};
        p.Tag = e->Attribute("tag");
        p.Param.SetValue(e->FloatAttribute("value"));

        e = e->NextSiblingElement("float");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat2>& value)
{
    auto e = element->FirstChildElement("float2");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        asdx::Vector2 v;
        UiFloat2 p = {};
        p.Tag = e->Attribute("tag");
        v.x = e->FloatAttribute("x");
        v.y = e->FloatAttribute("y");
        p.Param.SetValue(v);

        e = e->NextSiblingElement("float2");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat3>& value)
{
    auto e = element->FirstChildElement("float3");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        asdx::Vector3 v;
        UiFloat3 p = {};
        p.Tag = e->Attribute("tag");
        v.x = e->FloatAttribute("x");
        v.y = e->FloatAttribute("y");
        v.z = e->FloatAttribute("z");
        p.Param.SetValue(v);

        e = e->NextSiblingElement("float3");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat4>& value)
{
    auto e = element->FirstChildElement("float4");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        asdx::Vector4 v;
        UiFloat4 p = {};
        p.Tag = e->Attribute("tag");
        v.x = e->FloatAttribute("x");
        v.y = e->FloatAttribute("y");
        v.z = e->FloatAttribute("z");
        v.w = e->FloatAttribute("w");
        p.Param.SetValue(v);

        e = e->NextSiblingElement("float4");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiColor3>& value)
{
    auto e = element->FirstChildElement("color3");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        asdx::Vector3 v;
        UiColor3 p = {};
        p.Tag = e->Attribute("tag");
        v.x = e->FloatAttribute("r");
        v.y = e->FloatAttribute("g");
        v.z = e->FloatAttribute("b");
        p.Param.SetValue(v);

        e = e->NextSiblingElement("color3");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiColor4>& value)
{
    auto e = element->FirstChildElement("color4");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        asdx::Vector4 v;
        UiFloat4 p = {};
        p.Tag = e->Attribute("tag");
        v.x = e->FloatAttribute("r");
        v.y = e->FloatAttribute("g");
        v.z = e->FloatAttribute("b");
        v.w = e->FloatAttribute("a");
        p.Param.SetValue(v);

        e = e->NextSiblingElement("color4");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiBit32>& value)
{
    auto e = element->FirstChildElement("bit32");
    if (e == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiBit32 p = {};
        p.Tag = e->Attribute("tag");
        p.Param.SetValue(e->UnsignedAttribute("value"));

        e = e->NextSiblingElement("bit32");
    }
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiTexture2D>& value)
{
    auto e = element->FirstChildElement("texture2D");
    if (e  == nullptr)
    { return; }

    while(e != nullptr)
    {
        UiTexture2D p = {};
        p.Tag = e->Attribute("tag");
        p.Param.SetPath(e->Attribute("path"));

        e = e->NextSiblingElement("texture2D");
    }
}

///////////////////////////////////////////////////////////////////////////////
// Materialinstance class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
MaterialInstance::MaterialInstance()
: m_ShadowCast      (false)
, m_ShadowReceive   (false)
, m_BlendState      (0)
, m_RasterizerState (0)
, m_DepthState      (1)
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
    element->SetAttribute("shadow_cast",        m_ShadowCast        .GetValue());
    element->SetAttribute("shadow_receive",     m_ShadowReceive     .GetValue());
    element->SetAttribute("blend_state",        m_BlendState        .GetValue());
    element->SetAttribute("rasterizer_state",   m_RasterizerState   .GetValue());
    element->SetAttribute("depth_state",        m_DepthState        .GetValue());

    for(size_t i=0; i<m_Bool.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Bool[i])); }

    for(size_t i=0; i<m_Int.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Int[i])); }

    for(size_t i=0; i<m_Float.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Float[i])); }

    for(size_t i=0; i<m_Float2.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Float2[i])); }

    for(size_t i=0; i<m_Float3.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Float3[i])); }

    for(size_t i=0; i<m_Float4.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Float4[i])); }

    for(size_t i=0; i<m_Color3.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Color3[i])); }

    for(size_t i=0; i<m_Color4.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Color4[i])); }

    for(size_t i=0; i<m_Bit32.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Bit32[i])); }

    for(size_t i=0; i<m_Texture2D.size(); ++i)
    { element->InsertEndChild(::Serialize(doc, m_Texture2D[i])); }

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
    m_DepthState        .SetValue(e->IntAttribute("depth_state"));

    ::Deserialize(e, m_Bool);
    ::Deserialize(e, m_Int);
    ::Deserialize(e, m_Float);
    ::Deserialize(e, m_Float2);
    ::Deserialize(e, m_Float3);
    ::Deserialize(e, m_Float4);
    ::Deserialize(e, m_Color3);
    ::Deserialize(e, m_Color4);
    ::Deserialize(e, m_Bit32);
    ::Deserialize(e, m_Texture2D);
}

//-----------------------------------------------------------------------------
//      シャドウをキャストするかどうか?
//-----------------------------------------------------------------------------
bool MaterialInstance::CastShadow() const
{ return m_ShadowCast.GetValue(); }

//-----------------------------------------------------------------------------
//      シャドウをレシーブするかどうか?
//-----------------------------------------------------------------------------
bool MaterialInstance::ReceiveShadow() const
{ return m_ShadowReceive.GetValue(); }

//-----------------------------------------------------------------------------
//      ブレンドステートを取得します.
//-----------------------------------------------------------------------------
int MaterialInstance::GetBlendState() const
{ return m_BlendState.GetValue(); }

//-----------------------------------------------------------------------------
//      深度ステートを取得します.
//-----------------------------------------------------------------------------
int MaterialInstance::GetDepthState() const
{ return m_DepthState.GetValue(); }

//-----------------------------------------------------------------------------
//      ラスタライザーステートを取得します.
//-----------------------------------------------------------------------------
int MaterialInstance::GetRasterizerState() const
{ return m_RasterizerState.GetValue(); }

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
        ELOGA("Error : PluginMaterial Load Failed. path = %s", path);
        return false;
    }

    // デシリアライズ.
    Deserialize(&doc);

    auto shaderPath = asdx::ToFullPath(path);
    shaderPath = asdx::GetDirectoryPathA(shaderPath.c_str());
    shaderPath = asdx::ToFullPath((shaderPath + m_ShaderPath).c_str());

    // シェーダをロード.
    if (!m_LightingShader.Load(shaderPath.c_str(), "LightingPS"))
    {
        ELOGA("Error : PluginShader::Load() Failed. path = %s", shaderPath.c_str());
        return false;
    }

    if (!m_ShadowingShader.Load(shaderPath.c_str(), "ShadowingPS"))
    {
        ELOGA("Error : PluginShader::Load() Failed. path = %s", shaderPath.c_str());
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
    m_LightingShader.Term();
    m_ShadowingShader.Term();

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
void PluginMaterial::Bind(ID3D11DeviceContext* pContext, const MaterialInstance* instance, bool lightingPass)
{
    if (pContext == nullptr || instance == nullptr)
    { return; }

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

    if (lightingPass)
    {
        m_LightingShader.Bind(pContext);
        m_LightingShader.Update(pContext, instance);
    }
    else
    {
        m_ShadowingShader.Bind(pContext);
        m_ShadowingShader.Update(pContext, instance);
    }
}

//-----------------------------------------------------------------------------
//      シェーダの設定を解除します.
//-----------------------------------------------------------------------------
void PluginMaterial::Unbind(ID3D11DeviceContext* pContext, bool lightingPass)
{
    if (pContext == nullptr)
    { return; }

    if (lightingPass)
    {
        m_LightingShader.Unbind(pContext);
    }
    else
    {
        m_ShadowingShader.Unbind(pContext);
    }
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
            u8"ブレンド設定",
            _countof(kBlendState),
            kBlendState);
    }

    if (m_RasterizerState.Editable)
    {
        instance->m_RasterizerState.DrawCombo(
            u8"表示面",
            _countof(kRasterizerState),
            kRasterizerState);
    }

    if (m_DepthState.Editable)
    {
        instance->m_DepthStencilState.DrawCombo(
            u8"深度設定",
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
//      ライティングシェーダを取得します.
//-----------------------------------------------------------------------------
const PluginShader* PluginMaterial::GetLightingShader() const
{ return &m_LightingShader; }

//-----------------------------------------------------------------------------
//      シャドウイングシェーダを取得します.
//-----------------------------------------------------------------------------
const PluginShader* PluginMaterial::GetShadowingShader() const
{ return &m_ShadowingShader; }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* PluginMaterial::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("PluginMaterial");

    e->SetAttribute("name",             m_Name      .c_str());
    e->SetAttribute("shader_path",      m_ShaderPath.c_str());

    e->InsertEndChild(::Serialize(doc, m_ShadowCast));
    e->InsertEndChild(::Serialize(doc, m_ShadowReceive));
    e->InsertEndChild(::Serialize(doc, m_BlendState));
    e->InsertEndChild(::Serialize(doc, m_RasterizerState));
    e->InsertEndChild(::Serialize(doc, m_DepthState));

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

    ::Deserialize(e, m_ShadowCast);
    ::Deserialize(e, m_ShadowReceive);
    ::Deserialize(e, m_BlendState);
    ::Deserialize(e, m_RasterizerState);
    ::Deserialize(e, m_DepthState);
    ::Deserialize(e, m_Bool);
    ::Deserialize(e, m_Int);
    ::Deserialize(e, m_Float);
    ::Deserialize(e, m_Float2);
    ::Deserialize(e, m_Float3);
    ::Deserialize(e, m_Float4);
    ::Deserialize(e, m_Color3);
    ::Deserialize(e, m_Color4);
    ::Deserialize(e, m_Bit32);
    ::Deserialize(e, m_Texture2D);
}

//-----------------------------------------------------------------------------
//      テンプレートを生成します.
//-----------------------------------------------------------------------------
PluginMaterial PluginMaterial::CreateTemplate()
{
    PluginMaterial temp;
    temp.m_Name                     = u8"TemplateMaterial";
    temp.m_ShaderPath               = "../shader/StandardPS.hlsl";
    temp.m_ShadowCast.Default       = true;
    temp.m_ShadowCast.Editable      = true;
    temp.m_ShadowReceive.Default    = true;
    temp.m_ShadowReceive.Editable   = true;
    temp.m_BlendState.Default       = 0;
    temp.m_BlendState.Editable      = true;
    temp.m_RasterizerState.Default  = 0;
    temp.m_RasterizerState.Editable = true;
    temp.m_DepthState.Default       = 1;
    temp.m_DepthState.Editable      = true;

    UiDefBool boolParam = {};
    boolParam.DisplayTag    = u8"ブール値";
    boolParam.Target        = "BoolParam";
    boolParam.Default       = false;
    temp.m_Bool.push_back(boolParam);

    UiDefInt intParam = {};
    intParam.DisplayTag = u8"整数値";
    intParam.Target     = "IntParam";
    intParam.Default    = 0;
    temp.m_Int.push_back(intParam);

    UiDefFloat floatParam = {};
    floatParam.DisplayTag   = u8"浮動小数値";
    floatParam.Target       = "FloatParam";
    floatParam.Step         = 0.1f;
    floatParam.Default      = 0.0f;
    floatParam.Mini         = 0.0f;
    floatParam.Maxi         = 1.0f;
    floatParam.Converter    = CONVERTER_NONE;
    temp.m_Float.push_back(floatParam);

    UiDefFloat2 float2Param = {};
    float2Param.DisplayTag = u8"2次元ベクトル";
    float2Param.Target     = "Vec2Param";
    float2Param.Step       = 0.1f;
    float2Param.Default    = asdx::Vector2(0.0f, 0.0f);
    float2Param.Mini       = 0.0f;
    float2Param.Maxi       = 1.0f;
    float2Param.Converter  = CONVERTER_NONE;
    temp.m_Float2.push_back(float2Param);

    UiDefTexture2D texture2dParam = {};
    texture2dParam.DisplayTag   = u8"テクスチャ0";
    texture2dParam.Target       = "ColorMap";
    texture2dParam.Default      = DEFAULT_TEXTURE_BLACK;
    temp.m_Texture2D.push_back(texture2dParam);

    return temp;
}
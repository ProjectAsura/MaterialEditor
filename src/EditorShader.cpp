//-----------------------------------------------------------------------------
// File : EditorShader.cpp
// Desc : Shader For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <EditorShader.h>
#include <dxcapi.h>




///////////////////////////////////////////////////////////////////////////////
// EditorShader class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorShader::EditorShader()
: m_Hash(0)
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorShader::~EditorShader()
{

}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool EditorShader::Init(const char* path)
{
    // XMLファイル読み込み.


    // シェーダコンパイル.

    return true;
}

//-----------------------------------------------------------------------------
//      シェーダ設定を取得します.
//-----------------------------------------------------------------------------
const ShaderSetting& EditorShader::GetSetting() const
{ return m_Setting; }

//-----------------------------------------------------------------------------
//      シェーダバイナリを取得します.
//-----------------------------------------------------------------------------
D3D12_SHADER_BYTECODE EditorShader::GetShader() const
{
    D3D12_SHADER_BYTECODE result;
    result.BytecodeLength  = m_Blob->GetBufferSize();
    result.pShaderBytecode = m_Blob->GetBufferPointer();
    return result;
}

//-----------------------------------------------------------------------------
//      シェーダハッシュを取得します.
//-----------------------------------------------------------------------------
uint32_t EditorShader::GetHash() const
{ return m_Hash; }

#define TO_VAL(x) #x, value.x

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const bool& value)
{
    auto element = doc->NewElement(tag);
    element->SetAttribute("value", value);
    return element;
}

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const BoolSetting& value)
{
    auto element = doc->NewElement(tag);
    element->SetAttribute("enable", value.Enable);
    element->SetAttribute("label", value.Label.c_str());
    return element;
}

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const FloatSetting& value)
{
    auto element = doc->NewElement(tag);
    element->SetAttribute("enable", value.Enable);
    element->SetAttribute("label", value.Label.c_str());
    element->SetAttribute("step", value.Step);
    element->SetAttribute("min", value.Min);
    element->SetAttribute("max", value.Max);
    return element;
}

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const MapSetting& value)
{
    auto element = doc->NewElement(tag);
    element->SetAttribute("enable", value.Enable);
    element->SetAttribute("label", value.Label.c_str());
    return element;
}

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const ShaderSetting& value)
{
    auto element = doc->NewElement(tag);
    element->SetAttribute("display_tag", value.DisplayTag.c_str());
    element->SetAttribute("pixel_shader", value.PixelShader.c_str());
    element->SetAttribute("entry_point", value.EntryPoint.c_str());

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableBaseColorMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableBaseColorMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableBaseColorMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableBaseColorMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableORMMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableORMMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableORMMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableORMMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableNormalMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableNormalMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableNormalMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableNormalMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableOpacityMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableOpacityMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableOpacityMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableOpacityMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableMaskMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableMaskMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableMaskMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableMaskMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableEmissiveMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableEmissiveMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableEmissiveMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableEmissiveMap3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableCurvatureMap)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableDistortionMap)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableFlowMap)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScale0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScale1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScale2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScale3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVRotate0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVRotate1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVRotate2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVRotate3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScroll0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScroll1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScroll2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableUVScroll3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableAlphaThreshold0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableAlphaThreshold1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableAlphaThreshold2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableAlphaThreshold3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColor0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColor1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColor2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColor3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColorScale0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColorScale1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColorScale2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(EnableColorScale3)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomBool_7)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat_7)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat2_7)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat3_7)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomFloat4_7)));

    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap0)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap1)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap2)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap3)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap4)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap5)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap6)));
    element->InsertEndChild(Serialize(doc, TO_VAL(CustomMap7)));

    return element;
}

void Deserialize(tinyxml2::XMLElement* element, const char* tag, bool& value)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    value = e->BoolAttribute("value");
}

void Deserialize(tinyxml2::XMLElement* element, const char* tag, BoolSetting& value)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    value.Enable = e->BoolAttribute("enable");
    value.Label  = e->Attribute("label");
}

void Deserialize(tinyxml2::XMLElement* element, const char* tag, FloatSetting& value)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    value.Enable    = e->BoolAttribute("enable");
    value.Label     = e->Attribute("label");
    value.Step      = e->FloatAttribute("step");
    value.Min       = e->FloatAttribute("min");
    value.Max       = e->FloatAttribute("max");
}

void Deserialize(tinyxml2::XMLElement* element, const char* tag, MapSetting& value)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    value.Enable = e->BoolAttribute("enable");
    value.Label  = e->Attribute("label");
}

void Deserialize(tinyxml2::XMLElement* element, const char* tag, ShaderSetting& value)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    auto dt = e->Attribute("display_tag");
    auto ps = e->Attribute("pixel_shader");
    auto ep = e->Attribute("entry_point");

    value.DisplayTag  = dt;
    value.PixelShader = ps;
    value.EntryPoint  = ep;

    Deserialize(e, TO_VAL(EnableBaseColorMap0));
    Deserialize(e, TO_VAL(EnableBaseColorMap1));
    Deserialize(e, TO_VAL(EnableBaseColorMap2));
    Deserialize(e, TO_VAL(EnableBaseColorMap3));

    Deserialize(e, TO_VAL(EnableORMMap0));
    Deserialize(e, TO_VAL(EnableORMMap1));
    Deserialize(e, TO_VAL(EnableORMMap2));
    Deserialize(e, TO_VAL(EnableORMMap3));

    Deserialize(e, TO_VAL(EnableNormalMap0));
    Deserialize(e, TO_VAL(EnableNormalMap1));
    Deserialize(e, TO_VAL(EnableNormalMap2));
    Deserialize(e, TO_VAL(EnableNormalMap3));

    Deserialize(e, TO_VAL(EnableOpacityMap0));
    Deserialize(e, TO_VAL(EnableOpacityMap1));
    Deserialize(e, TO_VAL(EnableOpacityMap2));
    Deserialize(e, TO_VAL(EnableOpacityMap3));

    Deserialize(e, TO_VAL(EnableMaskMap0));
    Deserialize(e, TO_VAL(EnableMaskMap1));
    Deserialize(e, TO_VAL(EnableMaskMap2));
    Deserialize(e, TO_VAL(EnableMaskMap3));

    Deserialize(e, TO_VAL(EnableEmissiveMap0));
    Deserialize(e, TO_VAL(EnableEmissiveMap1));
    Deserialize(e, TO_VAL(EnableEmissiveMap2));
    Deserialize(e, TO_VAL(EnableEmissiveMap3));

    Deserialize(e, TO_VAL(EnableCurvatureMap));
    Deserialize(e, TO_VAL(EnableDistortionMap));
    Deserialize(e, TO_VAL(EnableFlowMap));

    Deserialize(e, TO_VAL(EnableUVScale0));
    Deserialize(e, TO_VAL(EnableUVScale1));
    Deserialize(e, TO_VAL(EnableUVScale2));
    Deserialize(e, TO_VAL(EnableUVScale3));

    Deserialize(e, TO_VAL(EnableUVRotate0));
    Deserialize(e, TO_VAL(EnableUVRotate1));
    Deserialize(e, TO_VAL(EnableUVRotate2));
    Deserialize(e, TO_VAL(EnableUVRotate3));

    Deserialize(e, TO_VAL(EnableUVScroll0));
    Deserialize(e, TO_VAL(EnableUVScroll1));
    Deserialize(e, TO_VAL(EnableUVScroll2));
    Deserialize(e, TO_VAL(EnableUVScroll3));

    Deserialize(e, TO_VAL(EnableAlphaThreshold0));
    Deserialize(e, TO_VAL(EnableAlphaThreshold1));
    Deserialize(e, TO_VAL(EnableAlphaThreshold2));
    Deserialize(e, TO_VAL(EnableAlphaThreshold3));

    Deserialize(e, TO_VAL(EnableColor0));
    Deserialize(e, TO_VAL(EnableColor1));
    Deserialize(e, TO_VAL(EnableColor2));
    Deserialize(e, TO_VAL(EnableColor3));

    Deserialize(e, TO_VAL(EnableColorScale0));
    Deserialize(e, TO_VAL(EnableColorScale1));
    Deserialize(e, TO_VAL(EnableColorScale2));
    Deserialize(e, TO_VAL(EnableColorScale3));

    Deserialize(e, TO_VAL(CustomBool_0));
    Deserialize(e, TO_VAL(CustomBool_1));
    Deserialize(e, TO_VAL(CustomBool_2));
    Deserialize(e, TO_VAL(CustomBool_3));
    Deserialize(e, TO_VAL(CustomBool_4));
    Deserialize(e, TO_VAL(CustomBool_5));
    Deserialize(e, TO_VAL(CustomBool_6));
    Deserialize(e, TO_VAL(CustomBool_7));

    Deserialize(e, TO_VAL(CustomFloat_0));
    Deserialize(e, TO_VAL(CustomFloat_1));
    Deserialize(e, TO_VAL(CustomFloat_2));
    Deserialize(e, TO_VAL(CustomFloat_3));
    Deserialize(e, TO_VAL(CustomFloat_4));
    Deserialize(e, TO_VAL(CustomFloat_5));
    Deserialize(e, TO_VAL(CustomFloat_6));
    Deserialize(e, TO_VAL(CustomFloat_7));

    Deserialize(e, TO_VAL(CustomFloat2_0));
    Deserialize(e, TO_VAL(CustomFloat2_1));
    Deserialize(e, TO_VAL(CustomFloat2_2));
    Deserialize(e, TO_VAL(CustomFloat2_3));
    Deserialize(e, TO_VAL(CustomFloat2_4));
    Deserialize(e, TO_VAL(CustomFloat2_5));
    Deserialize(e, TO_VAL(CustomFloat2_6));
    Deserialize(e, TO_VAL(CustomFloat2_7));

    Deserialize(e, TO_VAL(CustomFloat3_0));
    Deserialize(e, TO_VAL(CustomFloat3_1));
    Deserialize(e, TO_VAL(CustomFloat3_2));
    Deserialize(e, TO_VAL(CustomFloat3_3));
    Deserialize(e, TO_VAL(CustomFloat3_4));
    Deserialize(e, TO_VAL(CustomFloat3_5));
    Deserialize(e, TO_VAL(CustomFloat3_6));
    Deserialize(e, TO_VAL(CustomFloat3_7));

    Deserialize(e, TO_VAL(CustomFloat4_0));
    Deserialize(e, TO_VAL(CustomFloat4_1));
    Deserialize(e, TO_VAL(CustomFloat4_2));
    Deserialize(e, TO_VAL(CustomFloat4_3));
    Deserialize(e, TO_VAL(CustomFloat4_4));
    Deserialize(e, TO_VAL(CustomFloat4_5));
    Deserialize(e, TO_VAL(CustomFloat4_6));
    Deserialize(e, TO_VAL(CustomFloat4_7));

    Deserialize(e, TO_VAL(CustomMap0));
    Deserialize(e, TO_VAL(CustomMap1));
    Deserialize(e, TO_VAL(CustomMap2));
    Deserialize(e, TO_VAL(CustomMap3));
    Deserialize(e, TO_VAL(CustomMap4));
    Deserialize(e, TO_VAL(CustomMap5));
    Deserialize(e, TO_VAL(CustomMap6));
    Deserialize(e, TO_VAL(CustomMap7));
}

#undef TO_VAL
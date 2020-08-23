//-----------------------------------------------------------------------------
// File : EditorShader.h
// Desc : Shader For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <string>
#include <tinyxml2.h>
#include <d3d12.h>
#include <asdxRef.h>
#include <asdxShaderCompiler.h>


///////////////////////////////////////////////////////////////////////////////
// BoolSetting structure
///////////////////////////////////////////////////////////////////////////////
struct BoolSetting
{
    bool        Enable;
    std::string Label;
};

///////////////////////////////////////////////////////////////////////////////
// FloatSetting structure
///////////////////////////////////////////////////////////////////////////////
struct FloatSetting
{
    bool        Enable;
    std::string Label;
    float       Step;
    float       Min;
    float       Max;
};

///////////////////////////////////////////////////////////////////////////////
// MapSetting
///////////////////////////////////////////////////////////////////////////////
struct MapSetting
{
    bool        Enable;
    std::string Label;
};

///////////////////////////////////////////////////////////////////////////////
// ShaderSetting structure
///////////////////////////////////////////////////////////////////////////////
struct ShaderSetting
{
    std::string         DisplayTag;
    std::string         PixelShader;
    std::string         EntryPoint;

    bool EnableBaseColorMap0;
    bool EnableBaseColorMap1;
    bool EnableBaseColorMap2;
    bool EnableBaseColorMap3;

    bool EnableORMMap0;
    bool EnableORMMap1;
    bool EnableORMMap2;
    bool EnableORMMap3;

    bool EnableNormalMap0;
    bool EnableNormalMap1;
    bool EnableNormalMap2;
    bool EnableNormalMap3;

    bool EnableOpacityMap0;
    bool EnableOpacityMap1;
    bool EnableOpacityMap2;
    bool EnableOpacityMap3;

    bool EnableMaskMap0;
    bool EnableMaskMap1;
    bool EnableMaskMap2;
    bool EnableMaskMap3;

    bool EnableEmissiveMap0;
    bool EnableEmissiveMap1;
    bool EnableEmissiveMap2;
    bool EnableEmissiveMap3;

    bool EnableCurvatureMap;
    bool EnableDistortionMap;
    bool EnableFlowMap;

    bool EnableUVScale0;
    bool EnableUVScale1;
    bool EnableUVScale2;
    bool EnableUVScale3;

    bool EnableUVRotate0;
    bool EnableUVRotate1;
    bool EnableUVRotate2;
    bool EnableUVRotate3;

    bool EnableUVScroll0;
    bool EnableUVScroll1;
    bool EnableUVScroll2;
    bool EnableUVScroll3;

    bool EnableAlphaThreshold0;
    bool EnableAlphaThreshold1;
    bool EnableAlphaThreshold2;
    bool EnableAlphaThreshold3;

    bool EnableColor0;
    bool EnableColor1;
    bool EnableColor2;
    bool EnableColor3;

    bool EnableColorScale0;
    bool EnableColorScale1;
    bool EnableColorScale2;
    bool EnableColorScale3;

    BoolSetting    CustomBool_0;
    BoolSetting    CustomBool_1;
    BoolSetting    CustomBool_2;
    BoolSetting    CustomBool_3;
    BoolSetting    CustomBool_4;
    BoolSetting    CustomBool_5;
    BoolSetting    CustomBool_6;
    BoolSetting    CustomBool_7;

    FloatSetting    CustomFloat_0;
    FloatSetting    CustomFloat_1;
    FloatSetting    CustomFloat_2;
    FloatSetting    CustomFloat_3;
    FloatSetting    CustomFloat_4;
    FloatSetting    CustomFloat_5;
    FloatSetting    CustomFloat_6;
    FloatSetting    CustomFloat_7;

    FloatSetting    CustomFloat2_0;
    FloatSetting    CustomFloat2_1;
    FloatSetting    CustomFloat2_2;
    FloatSetting    CustomFloat2_3;
    FloatSetting    CustomFloat2_4;
    FloatSetting    CustomFloat2_5;
    FloatSetting    CustomFloat2_6;
    FloatSetting    CustomFloat2_7;

    FloatSetting    CustomFloat3_0;
    FloatSetting    CustomFloat3_1;
    FloatSetting    CustomFloat3_2;
    FloatSetting    CustomFloat3_3;
    FloatSetting    CustomFloat3_4;
    FloatSetting    CustomFloat3_5;
    FloatSetting    CustomFloat3_6;
    FloatSetting    CustomFloat3_7;

    FloatSetting    CustomFloat4_0;
    FloatSetting    CustomFloat4_1;
    FloatSetting    CustomFloat4_2;
    FloatSetting    CustomFloat4_3;
    FloatSetting    CustomFloat4_4;
    FloatSetting    CustomFloat4_5;
    FloatSetting    CustomFloat4_6;
    FloatSetting    CustomFloat4_7;

    MapSetting    CustomMap0;
    MapSetting    CustomMap1;
    MapSetting    CustomMap2;
    MapSetting    CustomMap3;
    MapSetting    CustomMap4;
    MapSetting    CustomMap5;
    MapSetting    CustomMap6;
    MapSetting    CustomMap7;
};


///////////////////////////////////////////////////////////////////////////////
// EditorShader class
///////////////////////////////////////////////////////////////////////////////
class EditorShader
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    EditorShader();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~EditorShader();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //!
    //! @param[in]      path        XMLファイルパスです.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool Init(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      設定を取得します.
    //!
    //! @return     設定を返却します.
    //-------------------------------------------------------------------------
    const ShaderSetting& GetSetting() const;

    //-------------------------------------------------------------------------
    //! @brief      シェーダバイナリを取得します.
    //!
    //! @return     シェーダバイナリを返却します.
    //-------------------------------------------------------------------------
    D3D12_SHADER_BYTECODE GetShader() const;

    //-------------------------------------------------------------------------
    //! @brief      シェーダハッシュを取得します.
    //!
    //! @return     シェーダハッシュを返却します.
    //-------------------------------------------------------------------------
    uint32_t GetHash() const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    uint32_t                    m_Hash;     //!< シェーダハッシュ.
    ShaderSetting               m_Setting;  //!< 設定.
    asdx::RefPtr<asdx::IBlob>   m_Blob;     //!< シェーダバイナリ.

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};


tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const BoolSetting&   value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const FloatSetting&  value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const MapSetting&    value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const char* tag, const ShaderSetting& value);

void Deserialize(tinyxml2::XMLElement* element, const char* tag, BoolSetting&   value);
void Deserialize(tinyxml2::XMLElement* element, const char* tag, FloatSetting&  value);
void Deserialize(tinyxml2::XMLElement* element, const char* tag, MapSetting&    value);
void Deserialize(tinyxml2::XMLElement* element, const char* tag, ShaderSetting& value);

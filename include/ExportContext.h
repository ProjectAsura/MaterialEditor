//-----------------------------------------------------------------------------
// File : MaterialExportContext.h
// Desc : Material Export Context.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>

struct ExportContext;
typedef bool (__stdcall *ExportFunc)(const ExportContext* context);


///////////////////////////////////////////////////////////////////////////////
// BoolParam structure
///////////////////////////////////////////////////////////////////////////////
struct BoolParam
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    bool        Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// IntParam structure
///////////////////////////////////////////////////////////////////////////////
struct IntParam
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    int         Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// FloatParam structure
///////////////////////////////////////////////////////////////////////////////
struct FloatParam
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// Float2Param structure
///////////////////////////////////////////////////////////////////////////////
struct Float2Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       X;          //!< X成分.
    float       Y;          //!< Y成分.
};

///////////////////////////////////////////////////////////////////////////////
// Float3Param structure
///////////////////////////////////////////////////////////////////////////////
struct Float3Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       X;          //!< X成分.
    float       Y;          //!< Y成分.
    float       Z;          //!< Z成分.
};

///////////////////////////////////////////////////////////////////////////////
// Float4Param structure
///////////////////////////////////////////////////////////////////////////////
struct Float4Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       X;          //!< X成分.
    float       Y;          //!< Y成分.
    float       Z;          //!< Z成分.
    float       W;          //!< W成分.
};

///////////////////////////////////////////////////////////////////////////////
// Color3Param structure
///////////////////////////////////////////////////////////////////////////////
struct Color3Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       R;          //!< R成分.
    float       G;          //!< G成分.
    float       B;          //!< B成分.
};

///////////////////////////////////////////////////////////////////////////////
// Color4Param structure
///////////////////////////////////////////////////////////////////////////////
struct Color4Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    float       R;          //!< R成分.
    float       G;          //!< G成分.
    float       B;          //!< B成分.
    float       A;          //!< A成分.
};

///////////////////////////////////////////////////////////////////////////////
// Bit32Param structure
///////////////////////////////////////////////////////////////////////////////
struct Bit32Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    uint32_t    Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// TextureParam structure
///////////////////////////////////////////////////////////////////////////////
struct TextureParam
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    const char* Path;       //!< テクスチャファイルパス.
};

///////////////////////////////////////////////////////////////////////////////
// ExportMaterial structure
///////////////////////////////////////////////////////////////////////////////
struct ExportMaterial
{
    const char*     Name;               //!< マテリアル名.
    const char*     ShaderPath;         //!< シェーダファイルパス.
    bool            CastShadow;         //!< シャドウキャスト.
    bool            ReceiveShadow;      //!< シャドウレシーブ.
    int             BlendState;         //!< ブレンドステート.
    int             RasterizerState;    //!< ラスタライザーステート.
    int             DepthState;         //!< 深度ステート.
    uint32_t        BoolCount;          //!< boolパラメータ数.
    BoolParam*      BoolParams;         //!< boolパラメータ.
    uint32_t        IntCount;           //!< intパラメータ数.
    IntParam*       IntParams;          //!< intパラメータ.
    uint32_t        FloatCount;         //!< floatパラメータ数.
    FloatParam*     FloatParams;        //!< floatパラメータ.
    uint32_t        Float2Count;        //!< float2パラメータ数.
    Float2Param*    Float2Params;       //!< float2パラメータ.
    uint32_t        Float3Count;        //!< float3パラメータ数.
    Float3Param*    Float3Params;       //!< float3パラメータ.
    uint32_t        Float4Count;        //!< float4パラメータ数.
    Float4Param*    Float4Params;       //!< float4パラメータ.
    uint32_t        Color3Count;        //!< color3パラメータ数.
    Color3Param*    Color3Params;       //!< color3パラメータ.
    uint32_t        Color4Count;        //!< color4パラメータ数.
    Color4Param*    Color4Params;       //!< color4パラメータ.
    uint32_t        Bit32Count;         //!< bit32パラメータ数.
    Bit32Param*     Bit32Params;        //!< bit32パラメータ.
    uint32_t        Texture2DCount;     //!< texture2Dパラメータ数.
    TextureParam*   Texture2DParams;    //!< texture2Dパラメータ.
};

///////////////////////////////////////////////////////////////////////////////
// ExportContext structure
///////////////////////////////////////////////////////////////////////////////
struct ExportContext
{
    char*               OutputPath;     //!< 出力ファイルパス.
    uint32_t            MaterialCount;  //!< マテリアル数.
    ExportMaterial*     Materials;      //!< マテリアル.
};


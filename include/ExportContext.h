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
// Vec2 structure
///////////////////////////////////////////////////////////////////////////////
struct Vec2
{
    float x;
    float y;
};

///////////////////////////////////////////////////////////////////////////////
// Vec3 structure
///////////////////////////////////////////////////////////////////////////////
struct Vec3
{
    float x;
    float y;
    float z;
};

///////////////////////////////////////////////////////////////////////////////
// Vec4 structure
///////////////////////////////////////////////////////////////////////////////
struct Vec4
{
    float x;
    float y;
    float z;
    float w;
};

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
    Vec2        Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// Float3Param structure
///////////////////////////////////////////////////////////////////////////////
struct Float3Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    Vec3        Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// Float4Param structure
///////////////////////////////////////////////////////////////////////////////
struct Float4Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    Vec4        Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// Color3Param structure
///////////////////////////////////////////////////////////////////////////////
struct Color3Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    Vec3        Value;      //!< 値.
};

///////////////////////////////////////////////////////////////////////////////
// Color4Param structure
///////////////////////////////////////////////////////////////////////////////
struct Color4Param
{
    const char* Name;       //!< パラメータ名.
    const char* Target;     //!< バインド先名.
    Vec4        Value;      //!< 値.
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

//-----------------------------------------------------------------------------
//      エクスポートコンテキストを破棄します.
//-----------------------------------------------------------------------------
inline void DisposeExportContext(ExportContext*& context)
{
    if (context == nullptr)
    { return; }

    if (context->Materials != nullptr)
    {
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
    }

    delete context;
    context = nullptr;
}

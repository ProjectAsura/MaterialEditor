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


namespace MaterialEditor {

enum PARAM_TYPE
{
    PARAM_TYPE_BOOL,
    PARAM_TYPE_INT,
    PARAM_TYPE_FLOAT,
    PARAM_TYPE_FLOAT2,
    PARAM_TYPE_FLOAT3,
    PARAM_TYPE_FLOAT4,
    PARAM_TYPE_COLOR3,
    PARAM_TYPE_COLOR4,
    PARAM_TYPE_BIT32,
};

struct BoolParam
{
    const char* Name;
    const char* Target;
    bool        Value;
};

struct IntParam
{
    const char* Name;
    const char* Target;
    int         Value;
};

struct FloatParam
{
    const char* Name;
    const char* Target;
    float       Value;
};

struct Float2Param
{
    const char* Name;
    const char* Target;
    float       X;
    float       Y;
};

struct Float3Param
{
    const char* Name;
    const char* Target;
    float       X;
    float       Y;
    float       Z;
};

struct Float4Param
{
    const char* Name;
    const char* Target;
    float       X;
    float       Y;
    float       Z;
    float       W;
};

struct Color3Param
{
    const char* Name;
    const char* Target;
    float       R;
    float       G;
    float       B;
};

struct Color4Param
{
    const char* Name;
    const char* Target;
    float       R;
    float       G;
    float       B;
    float       A;
};

struct Bit32Param
{
    const char* Name;
    const char* Target;
    uint32_t    Value;
};

struct TextureParam
{
    const char* Name;
    const char* Target;
    const char* Path;
};

struct MaterialData
{
    const char*     Name;
    const char*     ShaderPath;
    bool            CastShadow;
    bool            ReceiveShadow;
    int             BlendState;
    int             RasterizerState;
    int             DepthState;
    uint32_t        BoolCount;
    BoolParam*      BoolParams;
    uint32_t        IntCount;
    IntParam*       IntParams;
    uint32_t        FloatCount;
    FloatParam*     FloatParams;
    uint32_t        Float2Count;
    Float2Param*    Float2Params;
    uint32_t        Float3Count;
    Float3Param*    Float3Params;
    uint32_t        Float4Count;
    Float4Param*    Float4Params;
    uint32_t        Color3Count;
    Color3Param*    Color3Params;
    uint32_t        Color4Count;
    Color4Param*    Color4Params;
    uint32_t        Bit32Count;
    Bit32Param*     Bit32Params;
    uint32_t        Texture2DCount;
    TextureParam*   Texture2DParams;
};

struct ExportContext
{
    char*           OutputPath;
    uint32_t        MaterialCount;
    MaterialData*   Materials;
};

} // namespace MaterialEditor

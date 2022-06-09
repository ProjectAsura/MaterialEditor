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
// EXPORT_PARAMETER_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum EXPORT_PARAMETER_TYPE
{
    EXPORT_PARAMETER_TYPE_BOOL,
    EXPORT_PARAMETER_TYPE_INT,
    EXPORT_PARAMETER_TYPE_FLOAT,
    EXPORT_PARAMETER_TYPE_FLOAT2,
    EXPORT_PARAMETER_TYPE_FLOAT3,
    EXPORT_PARAMETER_TYPE_FLOAT4,
    EXPORT_PARAMETER_TYPE_COLOR3,
    EXPORT_PARAMETER_TYPE_COLOR4,
};

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
// Vec4s structure
///////////////////////////////////////////////////////////////////////////////
struct Vec4s
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t w;
};

///////////////////////////////////////////////////////////////////////////////
// ExportParameter structure
///////////////////////////////////////////////////////////////////////////////
struct ExportParameter
{
    EXPORT_PARAMETER_TYPE   Type;
    const char*             Name;
    union Value
    {
        bool    Bool;
        int     Int;
        float   Float;
        Vec2    Float2;
        Vec3    Float3;
        Vec4    Float4;
        Vec3    Color3;
        Vec4    Color4;
    } Value;
};

///////////////////////////////////////////////////////////////////////////////
// ExportTexture structure
///////////////////////////////////////////////////////////////////////////////
struct ExportTexture
{
    const char*     Name;
    const char*     Path;
};

///////////////////////////////////////////////////////////////////////////////
// ExportMaterial structure
///////////////////////////////////////////////////////////////////////////////
struct ExportMaterial
{
    const char*         Name;               //!< マテリアル名.
    const char*         ShaderPath;         //!< シェーダファイルパス.
    bool                CastShadow;         //!< シャドウキャスト.
    bool                ReceiveShadow;      //!< シャドウレシーブ.
    int                 BlendState;         //!< ブレンドステート.
    int                 RasterizerState;    //!< ラスタライザーステート.
    int                 DepthState;         //!< 深度ステート.
    uint32_t            ParamCount;         //!< パラメータ数.
    ExportParameter*    pParams;            //!< パラメータ.
    uint32_t            Texture2DCount;     //!< テクスチャ数.
    ExportTexture*      Texture2Ds;         //!< テクスチャ.
};

///////////////////////////////////////////////////////////////////////////////
// ExportMesh structure
///////////////////////////////////////////////////////////////////////////////
struct ExportMesh
{
    const char*     Name;
    const char*     MateiralName;
    uint32_t        VertexCount;
    const Vec3*     Positions;
    const Vec3*     Normals;
    const Vec3*     Tangents;
    const Vec4*     Colors;
    const Vec2*     TexCoord0;
    const Vec2*     TexCoord1;
    const Vec2*     TexCoord2;
    const Vec2*     TexCoord3;
    const Vec4s*    BoneIndices;
    const Vec4*     BoneWeights;
    uint32_t        IndexCount;
    const uint32_t* Indices;
};

///////////////////////////////////////////////////////////////////////////////
// ExportContext structure
///////////////////////////////////////////////////////////////////////////////
struct ExportContext
{
    char*               OutputPath;     //!< 出力ファイルパス.
    uint32_t            MaterialCount;  //!< マテリアル数.
    ExportMaterial*     Materials;      //!< マテリアル.
    uint32_t            MeshCount;      //!< メッシュ数.
    ExportMesh*         Meshes;         //!< メッシュ.
};


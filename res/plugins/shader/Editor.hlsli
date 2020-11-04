//-----------------------------------------------------------------------------
// File : Editor.hlsli
// Desc : Shader Definition For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#ifndef EDITOR_HLSLI
#define EDITOR_HLSLI

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Math.hlsli"
#include "BRDF.hlsli"


///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position   : SV_POSITION;    // 位置座標.
    float4 Color      : COLOR;          // カラー.
    float3 Normal     : NORMAL;         // 法線ベクトル.
    float3 Tangent    : TANGENT;        // 接線ベクトル.
    float4 TexCoord01 : TEXCOORD0;      // テクスチャ座標0, テクスチャ座標1.
    float4 TexCoord23 : TEXCOORD1;      // テクスチャ座標2, テクスチャ座標3.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0;
    float4 NRM   : SV_TARGET1;  // 法線/ラフネス/メタルネス.
};

///////////////////////////////////////////////////////////////////////////////
// CbScene constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbScene : register(b0)
{
    float4x4    View        : packoffset(c0);
    float4x4    Proj        : packoffset(c4);
    float4x4    InvView     : packoffset(c8);
    float4x4    InvProj     : packoffset(c12);
    float3      CameraPos   : packoffset(c16);
    float       Timer       : packoffset(c16.w);
    float       NearClip    : packoffset(c17);
    float       FarClip     : packoffset(c17.y);
    float2      UVToView    : packoffset(c17.z);
}

///////////////////////////////////////////////////////////////////////////////
// CbMesh constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0);
};

///////////////////////////////////////////////////////////////////////////////
// CbLight constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b3)
{
    float3   SunLightDir        : packoffset(c0);
    float    SunLightIntensity  : packoffset(c0.w);
    float    IBLIntensity       : packoffset(c1);
    float    IBLRotate          : packoffset(c1.y);
    float    PreExposureScale   : packoffset(c1.z);
};

///////////////////////////////////////////////////////////////////////////////
// CbShadow constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbShadow : register(b4)
{
    float4x4 ShadowMatrix   : packoffset(c0);
};

///////////////////////////////////////////////////////////////////////////////
// CbMaterialCommon constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMaterialCommon : register(b5)
{
    int ShadowReceive   : packoffset(c0);       // 0:レシーブしない, 1:レシーブする.
    int DisplayFace     : packoffset(c0.y);     // 0:両面表示, 1:背面表示, 2:前面表示 3:ワイヤーフレーム.
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D   EnvBRDF         : register(t0);
TextureCube DiffuseLD       : register(t1);
TextureCube SpecularLD      : register(t2);
Texture2D   SceneDepthMap   : register(t3);
Texture2D   SceneColorMap   : register(t4);
Texture2D   ShadowIndexMap  : register(t5);
Texture2D   ShadowAtlasMap  : register(t6);

SamplerState LinearClamp        : register(s0);
SamplerState LinearWrap         : register(s1);
SamplerState LinearMirror       : register(s2);
SamplerState PointClamp         : register(s3);
SamplerState PointWrap          : register(s4);
SamplerState PointMirror        : register(s5);
SamplerState AnisotropicClamp   : register(s6);
SamplerState AnisotropicWrap    : register(s7);
SamplerState AnisotropicMirror  : register(s8);


float2 GetTexCoord0(VSOutput value)
{ return value.TexCoord01.xy; }

float2 GetTexCoord1(VSOutput value)
{ return value.TexCoord01.zw; }

float2 GetTexCoord2(VSOutput value)
{ return value.TexCoord23.xy; }

float2 GetTexCoord3(VSOutput value)
{ return value.TexCoord23.zw; }

float3 GetBitangent(VSOutput value)
{ return -normalize(cross(value.Normal, value.Tangent)); }

float4 EncodeNRM(float3 normal, float roughness, float metalness)
{ return float4(PackNormal(normal), roughness, metalness); }

#endif//EDITOR_DEF_HLSLI

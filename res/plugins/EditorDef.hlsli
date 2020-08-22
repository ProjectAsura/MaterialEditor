//-----------------------------------------------------------------------------
// File : EditorDef.hlsli
// Desc : Shader Definition For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#ifndef EDITOR_DEF_HLSLI
#define EDITOR_DEF_HLSLI

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Math.hlsli"
#include "BRDF.hlsli"


///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct MSOutput
{
    float4 Position     : SV_POSITION;      //!< 位置座標です.
    float4 TexCoord01   : TEXCOORD01;       //!< テクスチャ座標0と1です(xy: TexCoord0, zw:TexCoord1)
    float4 TexCoord23   : TEXCOORD23;       //!< テクスチャ座標2と3です(xy: TexCoord2, zw:TexCoord3)
    float4 Color        : COLOR;            //!< 頂点カラーです.
    float3 Normal       : NORMAL;           //!< 法線ベクトルです.
    float3 Tangent      : TANGENT;          //!< 接線ベクトルです.
    float3 PositionWS   : POSITION_WS;      //!< ワールド空間の位置座標です.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0;
};

///////////////////////////////////////////////////////////////////////////////
// CbMesh constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b0)
{
    float4x4 World : packoffset(c0);
};

///////////////////////////////////////////////////////////////////////////////
// CbScene constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbScene : register(b1)
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
// CbMaterial constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMaterial : register(b2)
{
    int     CastShadow      : packoffset(c0);
    int     ReceiveShadow   : packoffset(c0.y);
    int     EnableDither    : packoffset(c0.z);
    int     DisplayFace     : packoffset(c0.w);
    float2  UVScale0        : packoffset(c1);
    float2  UVScale1        : packoffset(c1.z);
    float2  UVScale2        : packoffset(c2);
    float2  UVScale3        : packoffset(c2.z);
    float2  UVRotate0       : packoffset(c3);
    float2  UVRotate1       : packoffset(c3.z);
    float2  UVRotate2       : packoffset(c4);
    float2  UVRotate3       : packoffset(c4.z);
    float2  UVScroll0       : packoffset(c5);
    float2  UVScroll1       : packoffset(c5.z);
    float2  UVScroll2       : packoffset(c6);
    float2  UVScroll3       : packoffset(c6.z);
    float   AlphaThreshold0 : packoffset(c7);
    float   AlphaThreshold1 : packoffset(c7.y);
    float   AlphaThreshold2 : packoffset(c7.z);
    float   AlphaThreshold3 : packoffset(c7.w);
    float4  Color0          : packoffset(c8);
    float4  Color1          : packoffset(c9);
    float4  Color2          : packoffset(c10);
    float4  Color3          : packoffset(c11);
    float4  ColorScale0     : packoffset(c12);
    float4  ColorScale1     : packoffset(c13);
    float4  ColorScale2     : packoffset(c14);
    float4  ColorScale3     : packoffset(c15);
};

///////////////////////////////////////////////////////////////////////////////
// CbUser constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbUser : register(b3)
{
    int     CustomBool_0    : packoffset(c0);
    int     CustomBool_1    : packoffset(c0.y);
    int     CustomBool_2    : packoffset(c0.z);
    int     CustomBool_3    : packoffset(c0.w);
    int     CustomBool_4    : packoffset(c1);
    int     CustomBool_5    : packoffset(c1.y);
    int     CustomBool_6    : packoffset(c1.z);
    int     CustomBool_7    : packoffset(c1.w);
    float2  CustomFloat2_0  : packoffset(c2);
    float2  CustomFloat2_1  : packoffset(c2.z);
    float2  CustomFloat2_2  : packoffset(c3);
    float2  CustomFloat2_3  : packoffset(c3.z);
    float2  CustomFloat2_4  : packoffset(c4);
    float2  CustomFloat2_5  : packoffset(c4.z);
    float2  CustomFloat2_6  : packoffset(c5);
    float2  CustomFloat2_7  : packoffset(c5.z);
    float3  CustomFloat3_0  : packoffset(c6);
    float   CustomFloat_0   : packoffset(c6.w);
    float3  CustomFloat3_1  : packoffset(c7);
    float   CustomFloat_1   : packoffset(c7.w);
    float3  CustomFloat3_2  : packoffset(c8);
    float   CustomFloat_2   : packoffset(c8.w);
    float3  CustomFloat3_3  : packoffset(c9);
    float   CustomFloat_3   : packoffset(c9.w);
    float3  CustomFloat3_4  : packoffset(c10);
    float   CustomFloat_4   : packoffset(c10.w);
    float3  CustomFloat3_5  : packoffset(c11);
    float   CustomFloat_5   : packoffset(c11.w);
    float3  CustomFloat3_6  : packoffset(c12);
    float   CustomFloat_6   : packoffset(c12.w);
    float3  CustomFloat3_7  : packoffset(c13);
    float   CustomFloat_7   : packoffset(c13.w);
    float4  CustomFloat4_0 : packoffset(c14);
    float4  CustomFloat4_1 : packoffset(c15);
    float4  CustomFloat4_2 : packoffset(c16);
    float4  CustomFloat4_3 : packoffset(c17);
    float4  CustomFloat4_4 : packoffset(c18);
    float4  CustomFloat4_5 : packoffset(c19);
    float4  CustomFloat4_6 : packoffset(c20);
    float4  CustomFloat4_7 : packoffset(c21);
};

///////////////////////////////////////////////////////////////////////////////
// CbLight constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b4)
{
    float3   SunLightDir        : packoffset(c0);
    float    SunLightIntensity  : packoffset(c0.z);
    float    IBLIntensity       : packoffset(c1);
};

///////////////////////////////////////////////////////////////////////////////
// CbShadow constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbShadow : register(b5)
{
    float4   SplitDistance;
    float4x4 ShadowMatrix[4];
}

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D   EnvBRDF         : register(t0);
TextureCube DiffuseLD       : register(t1);
TextureCube SpecularLD      : register(t2);
Texture2D   SceneDepthMap   : register(t3);
Texture2D   SceneColorMap   : register(t4);
Texture2D   BaseColorMap0   : register(t5);
Texture2D   BaseColorMap1   : register(t6);
Texture2D   BaseColorMap2   : register(t7);
Texture2D   BaseColorMap3   : register(t8);
Texture2D   ORMMap0         : register(t9);
Texture2D   ORMMap1         : register(t10);
Texture2D   ORMMap2         : register(t11);
Texture2D   ORMMap3         : register(t12);
Texture2D   NormalMap0      : register(t13);
Texture2D   NormalMap1      : register(t14);
Texture2D   NormalMap2      : register(t15);
Texture2D   NormalMap3      : register(t16);
Texture2D   OpacityMap0     : register(t17);
Texture2D   OpacityMap1     : register(t18);
Texture2D   OpacityMap2     : register(t19);
Texture2D   OpacityMap3     : register(t20);
Texture2D   MaskMap0        : register(t21);
Texture2D   MaskMap1        : register(t22);
Texture2D   MaskMap2        : register(t23);
Texture2D   MaskMap3        : register(t24);
Texture2D   EmissiveMap0    : register(t25);
Texture2D   EmissiveMap1    : register(t26);
Texture2D   EmissiveMap2    : register(t27);
Texture2D   EmissiveMap3    : register(t28);
Texture2D   CurvatureMap    : register(t29);
Texture2D   DistortionMap   : register(t30);
Texture2D   FlowMap         : register(t31);
Texture2D   CustomMap0      : register(t32);
Texture2D   CustomMap1      : register(t33);
Texture2D   CustomMap2      : register(t34);
Texture2D   CustomMap3      : register(t35);
Texture2D   CustomMap4      : register(t36);
Texture2D   CustomMap5      : register(t37);
Texture2D   CustomMap6      : register(t38);
Texture2D   CustomMap7      : register(t39);

SamplerState LinearClamp        : register(s0);
SamplerState LinearWrap         : register(s1);
SamplerState LinearMirror       : register(s2);
SamplerState PointClamp         : register(s3);
SamplerState PointWrap          : register(s4);
SamplerState PointMirror        : register(s5);
SamplerState AnisotropicClamp   : register(s6);
SamplerState AnisotropicWrap    : register(s7);
SamplerState AnisotropicMirror  : register(s8);


float2 GetTexCoord0(MSOutput value)
{ return value.TexCoord01.xy; }

float2 GetTexCoord1(MSOutput value)
{ return value.TexCoord01.zw; }

float2 GetTexCoord2(MSOutput value)
{ return value.TexCoord23.xy; }

float2 GetTexCoord3(MSOutput value)
{ return value.TexCoord23.zw; }


#endif//EDITOR_DEF_HLSLI
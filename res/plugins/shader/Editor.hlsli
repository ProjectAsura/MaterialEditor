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

/* 
    b0 - b10 : �V�X�e���g�p.
    t0 - t10 : �V�X�e���g�p.
    s0 - s8  : �V�X�e���g�p.
*/

///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position   : SV_POSITION;    // �ʒu���W.
    float4 Color      : COLOR;          // �J���[.
    float3 Normal     : NORMAL;         // �@���x�N�g��.
    float3 Tangent    : TANGENT;        // �ڐ��x�N�g��.
    float4 TexCoord01 : TEXCOORD0;      // �e�N�X�`�����W0, �e�N�X�`�����W1.
    float4 TexCoord23 : TEXCOORD1;      // �e�N�X�`�����W2, �e�N�X�`�����W3.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0;
    float4 NRM   : SV_TARGET1;  // �@��/���t�l�X/���^���l�X.
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
    int ShadowReceive   : packoffset(c0);       // 0:���V�[�u���Ȃ�, 1:���V�[�u����.
    int DisplayFace     : packoffset(c0.y);     // 0:���ʕ\��, 1:�w�ʕ\��, 2:�O�ʕ\�� 3:���C���[�t���[��.
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
//Texture2D   ReservedMap0    : register(t7);
//Texture2D   ReservedMap1    : register(t8);
//Texture2D   ReservedMap2    : register(t9);
//Texture2D   ReservedMap3    : register(t10);

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

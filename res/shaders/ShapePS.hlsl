//-----------------------------------------------------------------------------
// File : ShapePS.hlsl
// Desc : Pixel Shader For Shape.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#include "Math.hlsli"

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal   : NORMAL;
    float4 Color    : COLOR;
};


///////////////////////////////////////////////////////////////////////////////
// CbScene constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbScene : register(b0)
{
    float4x4    View            : packoffset(c0);
    float4x4    Proj            : packoffset(c4);
    float4x4    InvView         : packoffset(c8);
    float4x4    InvProj         : packoffset(c12);
    float3      CameraPos       : packoffset(c16);
    float       Timer           : packoffset(c16.w);
    float       NearClip        : packoffset(c17);
    float       FarClip         : packoffset(c17.y);
    float2      UVToView        : packoffset(c17.z);
    float2      ScreenSize      : packoffset(c18);
    float2      InvScreenSize   : packoffset(c18.z);
};

///////////////////////////////////////////////////////////////////////////////
// CbLight constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b1)
{
    float3   SunLightDir        : packoffset(c0);
    float    SunLightIntensity  : packoffset(c0.w);
    float    IBLIntensity       : packoffset(c1);
    float    IBLRotate          : packoffset(c1.y);
    float    PreExposureScale   : packoffset(c1.z);
};

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
float4 main(const VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(SunLightDir);

    return float4(input.Color.rgb * saturate(dot(N, L)), input.Color.a);
}

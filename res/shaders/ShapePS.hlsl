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
//      スクリーンUVを取得します.
//-----------------------------------------------------------------------------
float2 GetScreenUV(VSOutput value)
{
    float2 uv = value.Position.xy * InvScreenSize;
    return float2(uv.x, 1.0f - uv.y);
}

//-----------------------------------------------------------------------------
//      ビュー空間深度を取得します.
//-----------------------------------------------------------------------------
float GetViewDepth(VSOutput value)
{ return ToViewDepth(value.Position.z, NearClip, FarClip); }

//-----------------------------------------------------------------------------
//      ビュー空間位置座標を取得します.
//-----------------------------------------------------------------------------
float3 GetViewPosition(VSOutput value)
{
    float viewDepth = GetViewDepth(value);
    return ToViewPos(GetScreenUV(value), viewDepth, UVToView);
}

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
float4 main(const VSOutput input) : SV_TARGET
{
    float3 viewPos = GetViewPosition(input);

    float3 N = normalize(cross(ddx_fine(viewPos), ddy_fine(viewPos)));
    float3 L = normalize(SunLightDir);

    return float4(input.Color.rgb * saturate(dot(N, L)), input.Color.a);
}

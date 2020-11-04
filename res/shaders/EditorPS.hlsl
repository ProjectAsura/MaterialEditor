//-----------------------------------------------------------------------------
// File : EditorPS.hlsl
// Desc : Editor Default Pixel Shader.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#include "Math.hlsli"

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
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
    float4 Diffuse  : SV_TARGET0;  // ディフューズライティング結果.
    float4 Specular : SV_TARGET1;  // スペキュラーライティング結果.
    float4 NRM      : SV_TARGET2;  // 法線/ラフネス/メタルネス.
};

///////////////////////////////////////////////////////////////////////////////
// CbLight constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b0)
{
    float3   SunLightDir        : packoffset(c0);
    float    SunLightIntensity  : packoffset(c0.w);
    float    IBLIntensity       : packoffset(c1);
    float    IBLRotate          : packoffset(c1.y);
    float    PreExposureScale   : packoffset(c1.z);
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       CheckerBoard : register(t0);
SamplerState    LinearWrap   : register(s0);

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
PSOutput main(const VSOutput input)
{
    PSOutput output = (PSOutput)0;
    float2 uv = input.TexCoord01.xy;
    float3 N = normalize(input.Normal);
    float3 L = normalize(SunLightDir);

    output.Diffuse  = CheckerBoard.Sample(LinearWrap, uv) * saturate(dot(N, L));
    output.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.NRM      = float4(PackNormal(N), 1.0f, 0.0f);
    return output;
}

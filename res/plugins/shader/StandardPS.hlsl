//-----------------------------------------------------------------------------
// File : StandardPS.hlsl
// Desc : Standard Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Editor.hlsli"

/* ユーザー定義データはスロット番号11から開始してください. 10番まではシステム使用とします.*/

///////////////////////////////////////////////////////////////////////////////
// CbUser constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbUser : register(b11)
{
    float2 UVOffset         : packoffset(c0);
    float2 UVScale          : packoffset(c0.z);
    float  UVRotate         : packoffset(c1);
    float  AlphaThreshold   : packoffset(c1.y);
};

//-----------------------------------------------------------------------------
// Textures
//-----------------------------------------------------------------------------
Texture2D BaseColor : register(t11);
Texture2D ORM       : register(t12);
Texture2D Normal    : register(t13);
Texture2D Emissive  : register(t14);


//-----------------------------------------------------------------------------
//      ライティング処理です.
//-----------------------------------------------------------------------------
PSOutput LightingPS(const VSOutput input)
{
    PSOutput output = (PSOutput)0;

    // UVアニメーション.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // ベースカラー取得.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }

    // 法線／オクルージョン／ラフネス／メタルネスを取得.
    float3 normal = Normal.Sample(AnisotropicWrap, uv).xyz * 2.0f - 1.0f;
    float3 orm    = ORM.Sample(AnisotropicWrap, uv).rgb;

    // 従接線ベクトルを計算.
    float3 bitangent = GetBitangent(input);
    float3 N = FromTangentSpaceToWorld(normal, input.Tangent, bitangent, input.Normal);

    // ビュー空間の深度と位置座標を取得.
    float3 viewPos = GetViewPosition(input);

    // ワールド空間の位置を復元.
    float3 worldPos = ToWorldPosition(viewPos);

    // 視線ベクトルライトベクトルを算出.
    float3 V = normalize(CameraPos - worldPos);
    float3 L = normalize(SunLightDir);

    float3 kd = ToKd(baseColor.rgb, orm.z);
    float3 ks = ToKs(baseColor.rgb, orm.z);

    // ライティング.
    output.Color.rgb += EvaluateIBLIsotropy(N, V, kd, ks, orm.x, orm.y, orm.z);
    output.Color.rgb += EvaluateDirectLightIsotropicGGX(N, V, L, kd, ks, orm.y);
    output.Color.rgb += Emissive.Sample(AnisotropicWrap, uv).rgb;
    output.Color.a = 1.0f;

    output.NRM = EncodeNRM(N, orm.y, orm.z);

    return output;
}

//-----------------------------------------------------------------------------
//      シャドウイング処理です.
//-----------------------------------------------------------------------------
void ShadowingPS(const VSOutput input)
{
    // UVアニメーション.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // ベースカラー取得.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }
}
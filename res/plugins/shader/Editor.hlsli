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
    b0 - b10 : システム使用.
    t0 - t10 : システム使用.
    s0 - s8  : システム使用.
*/

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
    float4x4    View          : packoffset(c0);
    float4x4    Proj          : packoffset(c4);
    float4x4    InvView       : packoffset(c8);
    float4x4    InvProj       : packoffset(c12);
    float3      CameraPos     : packoffset(c16);
    float       Timer         : packoffset(c16.w);
    float       NearClip      : packoffset(c17);
    float       FarClip       : packoffset(c17.y);
    float2      UVToView      : packoffset(c17.z);
    float2      ScreenSize    : packoffset(c18);
    float2      InvScreenSize : packoffset(c18.z);
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

//-----------------------------------------------------------------------------
//      スクリーンUVを取得します.
//-----------------------------------------------------------------------------
float2 GetScreenUV(VSOutput value)
{
    float2 uv = value.Position.xy * InvScreenSize;
    return float2(uv.x, 1.0f - uv.y);
}

//-----------------------------------------------------------------------------
//      テクスチャ座標0を取得します.
//-----------------------------------------------------------------------------
float2 GetTexCoord0(VSOutput value)
{ return value.TexCoord01.xy; }

//-----------------------------------------------------------------------------
//      テクスチャ座標1を取得します.
//-----------------------------------------------------------------------------
float2 GetTexCoord1(VSOutput value)
{ return value.TexCoord01.zw; }

//-----------------------------------------------------------------------------
//      テクスチャ座標2を取得します.
//-----------------------------------------------------------------------------
float2 GetTexCoord2(VSOutput value)
{ return value.TexCoord23.xy; }

//-----------------------------------------------------------------------------
//      テクスチャ座標3を取得します.
//-----------------------------------------------------------------------------
float2 GetTexCoord3(VSOutput value)
{ return value.TexCoord23.zw; }

//-----------------------------------------------------------------------------
//      従接線ベクトルを取得します.
//-----------------------------------------------------------------------------
float3 GetBitangent(VSOutput value)
{ return -normalize(cross(value.Normal, value.Tangent)); }

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
//      ビュー空間位置座標からワールド空間位置座標に変換します.
//-----------------------------------------------------------------------------
float3 ToWorldPosition(float3 viewPos)
{
    float4 worldPos = mul(InvView, float4(viewPos, 1.0f));
    worldPos.xyz /= worldPos.w;
    return worldPos.xyz;
}

//-----------------------------------------------------------------------------
//      ワールド空間位置座標を取得します.
//-----------------------------------------------------------------------------
float3 GetWorldPosition(VSOutput value)
{
    float3 viewPos = GetViewPosition(value);
    return ToWorldPosition(viewPos);
}

//-----------------------------------------------------------------------------
//      法線/ラフネス/メタルネスをエンコードします.
//-----------------------------------------------------------------------------
float4 EncodeNRM(float3 normal, float roughness, float metalness)
{ return float4(PackNormal(normal), roughness, metalness); }

//-----------------------------------------------------------------------------
//      法線/ラフネス/メタルネスをでコードします.
//------------------------------------------------------------------------------
void DecodeNRM(float4 encoded, out float3 normal, out float roughness, out float metalness)
{
    normal = UnpackNormal(encoded.xy);
    roughness = encoded.z;
    metalness = encoded.w;
}

//-----------------------------------------------------------------------------
//      事前計算済みDFV項を取得します.
//-----------------------------------------------------------------------------
float4 GetDFG(float roughness, float NoV)
{
    float2 uv = float2(NoV, roughness);
    uv.y = 1.0f - uv.y;    // DirectX系に変換.
    return EnvBRDF.SampleLevel(LinearClamp, uv, 0.0f);
}

//-----------------------------------------------------------------------------
//      ディフューズを評価します.
//-----------------------------------------------------------------------------
float3 EvaluateDiffuseIBL(float3 N, float3 Kd, float occlusion)
{
    // Lambert Diffuse.
    return Kd * DiffuseLD.SampleLevel(LinearClamp, N, 0).rgb * IBLIntensity * occlusion;
}

//-----------------------------------------------------------------------------
//      スペキュラーを評価します.
//-----------------------------------------------------------------------------
float3 EvaluateSpecularIBL
(
    float3  N,          // 法線ベクトル.
    float3  V,          // 視線ベクトル.
    float3  R,          // 反射ベクトル.
    float   NoV,        // 法線ベクトルと視線ベクトルの内積.
    float   roughness,  // 線形ラフネス.
    float   occlusion   // オクルージョン.
)
{
    // ミップマップレベル数を取得.
    float2 size;
    float mipLevels;
    SpecularLD.GetDimensions( 0, size.x, size.y, mipLevels );

    // 参照ミップマップレベルをroughnessから算出.
    float mipIndex = roughness * (mipLevels - 1.0f);

    float a2 = roughness * roughness;
    float3 dominantR = GetSpecularDomiantDir(N, R, a2);
    float  specularAO = CalcSpecularAO(NoV, occlusion, roughness);

    return SpecularLD.SampleLevel(LinearClamp, dominantR, mipIndex).rgb * IBLIntensity * specularAO;
}

//-----------------------------------------------------------------------------
//      等方性IBLを評価します.
//-----------------------------------------------------------------------------
float3 EvaluateIBLIsotropy
(
    float3  N,              // 法線ベクトル.
    float3  V,              // 視線ベクトル.
    float3  Kd,             // 拡散反射率.
    float3  Ks,             // 鏡面反射率.
    float   occlusion,      // オクルージョン.
    float   roughness,      // 線形ラフネス.
    float   metalness       // メタルネス.
)
{
    float  NoV = abs(dot(N, V));
    float3 R   = -reflect(V, N);
    float3 diffuse  = EvaluateDiffuseIBL(N, Kd, occlusion);
    float3 specular = EvaluateSpecularIBL(N, V, R, NoV, roughness, occlusion);

    float2 preDFG = GetDFG(roughness, NoV);

    // 多重散乱モデル.
    // Carmelo J. Fdez-Agüera, ”A Multiple-Scattering Microfacet Model for Real-Time Image-based Lighting”
    // Journal of Computer Graphics Techniques (JCGT), vol. 8, no. 1, 45-55, 2019
    float3 FssEss = Ks * preDFG.x + preDFG.y;
    float  Ess  = preDFG.x + preDFG.y;
    float  Ems  = 1.0f - Ess;
    float3 Fave = Ks + (1.0f / 21.0f) * (1.0f - Ks);
    float3 Fms  = FssEss * Fave / (1.0f - (1.0f - Ess) * Fave);
    float3 Edss = 1.0f - (FssEss + Fms * Ems);
    float3 subSurface = lerp(Kd * Edss, 0.0f, metalness);

    return diffuse + (FssEss * specular + (Fms * Ems + subSurface) * diffuse);
}

//-----------------------------------------------------------------------------
//      異方性IBLを評価します.
//-----------------------------------------------------------------------------
float3 EvaluateIBLAnisotropy
(
    float3  T,              // 接線ベクトル.
    float3  B,              // 従接線ベクトル.
    float3  N,              // 法線ベクトル.
    float3  V,              // 視線ベクトル.
    float3  Kd,             // 拡散反射率.
    float3  Ks,             // 鏡面反射率.
    float   occlusion,      // アンビエントオクルージョン.
    float   roughness,      // 線形ラフネス.
    float   metalness,      // メタルネス.
    float   anisotropy      // 異方率.
)
{
    float  NoV = abs(dot(N, V));
    float3 R   = BentReflection(T, B, N, -V, anisotropy);
    float3 diffuse  = EvaluateDiffuseIBL(N, Kd, occlusion);
    float3 specular = EvaluateSpecularIBL(N, V, R, NoV, roughness, occlusion);

    float2 preDFG = GetDFG(roughness, NoV);

    // 多重散乱モデル.
    // Carmelo J. Fdez-Agüera, ”A Multiple-Scattering Microfacet Model for Real-Time Image-based Lighting”
    // Journal of Computer Graphics Techniques (JCGT), vol. 8, no. 1, 45-55, 2019
    float3 FssEss = Ks * preDFG.x + preDFG.y;
    float  Ess  = preDFG.x + preDFG.y;
    float  Ems  = 1.0f - Ess;
    float3 Fave = Ks + (1.0f / 21.0f) * (1.0f - Ks);
    float3 Fms  = FssEss * Fave / (1.0f - (1.0f - Ess) * Fave);
    float3 Edss = 1.0f - (FssEss + Fms * Ems);
    float3 subSurface = lerp(Kd * Edss, 0.0f, metalness);

    return diffuse + (FssEss * specular + (Fms * Ems + subSurface) * diffuse);
}

//-----------------------------------------------------------------------------
//      直接光を評価します.
//-----------------------------------------------------------------------------
float3 EvaluateDirectLightIsotropicGGX
(
    float3 N,           // 法線ベクトル.
    float3 V,           // 視線ベクトル.
    float3 L,           // ライトベクトル(ライトに向かう方向).
    float3 Kd,          // 拡散反射率.
    float3 Ks,          // 鏡面反射率.
    float  roughness    // 線形ラフネス.
)
{
    float3 H     = normalize(V + L);
    float  NdotV = saturate(dot(N, V));
    float  LdotH = saturate(dot(L, H));
    float  NdotH = saturate(dot(N, H));
    float  NdotL = saturate(dot(N, L));
    float  VdotH = saturate(dot(V, H));
    float  a2    = max(roughness * roughness, 0.01);
    float  f90   = saturate(50.0f * dot(Ks, 0.33f));

    float3 diffuse = Kd / F_PI;
    float  D = D_GGX(NdotH, a2);
    float  G = G_SmithGGX(NdotL, NdotV, a2);
    float3 F = F_Schlick(Ks, f90, LdotH);
    float3 specular = (D * G * F) / F_PI;

    return (diffuse + specular) * NdotL * SunLightIntensity;
}

//-----------------------------------------------------------------------------
//      異方性マテリアルの直接光を評価します.
//-----------------------------------------------------------------------------
float3 EvaluateDirectLightAnisotropicGGX
(
    float3 T,           // 接線ベクトル.
    float3 B,           // 従法線ベクトル.
    float3 N,           // 法線ベクトル.
    float3 V,           // 視線ベクトル.
    float3 L,           // ライトベクトル.
    float3 Kd,          // 拡散反射率.
    float3 Ks,          // 鏡面反射率.
    float  roughness,   // ラフネス.
    float  anisotropy   // 異方率.
)
{
    float3 H     = normalize(V + L);
    float  NdotV = saturate(dot(N, V));
    float  LdotH = saturate(dot(L, H));
    float  NdotH = saturate(dot(N, H));
    float  NdotL = saturate(dot(N, L));
    float  a2    = max(roughness * roughness, 0.01);
    float  f90   = saturate(50.0f * dot(Ks, 0.33f));

    float at = max(roughness * (1.0f + anisotropy), 0.001f);
    float ab = max(roughness * (1.0f - anisotropy), 0.001f);

    float3 diffuse = Kd / F_PI;
    float  D = D_GGXAnisotropic(at, ab, NdotH, H, T, B);
    float  G = V_SmithGGXHeightCorrelatedAnisotropic(at, ab, V, L, T, B, NdotV, NdotL);
    float3 F = F_Schlick(Ks, f90, LdotH);
    float3 specular = (D * G * F) / F_PI;

    return (diffuse + specular) * NdotL * SunLightIntensity;
}


#endif//EDITOR_DEF_HLSLI

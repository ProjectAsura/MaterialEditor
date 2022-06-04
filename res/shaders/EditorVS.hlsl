//-----------------------------------------------------------------------------
// File : EditorVS.hlsl
// Desc : Editor Vertex Shader.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Math.hlsli"


///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position     : POSITION;        // 位置座標.
    float3  Normal       : NORMAL;          // 法線ベクトル.
    float3  Tangent      : TANGENT;         // 接線ベクトル.
    float4  Color        : COLOR;           // カラー.
    float2  TexCoord0    : TEXCOORD0;       // テクスチャ座標0.
    float2  TexCoord1    : TEXCOORD1;       // テクスチャ座標1
    float2  TexCoord2    : TEXCOORD2;       // テクスチャ座標2.
    float2  TexCoord3    : TEXCOORD3;       // テクスチャ座標3.
};

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
// CbScene constant buffer.
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
};

///////////////////////////////////////////////////////////////////////////////
// CbMesh structure
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b1)
{
    float4x4 World;     // ワールド行列.
};

//-----------------------------------------------------------------------------
// Resources.
//-----------------------------------------------------------------------------
StructuredBuffer<float4x4>  InstanceMatrix : register(t1);


//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
VSOutput main(const VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    worldPos = mul(InstanceMatrix[instanceId], worldPos);

    float4 viewPos  = mul(View, worldPos);
    float4 projPos  = mul(Proj, viewPos);

    float3 worldNormal  = mul((float3x3)World, input.Normal);
    float3 worldTangent = mul((float3x3)World, input.Tangent);

    output.Position     = projPos;
    output.Normal       = worldNormal;
    output.Tangent      = worldTangent;
    output.Color        = input.Color;
    output.TexCoord01   = float4(input.TexCoord0, input.TexCoord1);
    output.TexCoord23   = float4(input.TexCoord2, input.TexCoord3);
    return output;
}
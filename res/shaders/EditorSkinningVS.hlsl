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
    float4  Color        : COLOR;           // カラー.
    uint    TangentSpace : TANGENT_SPACE;   // 接線空間.
    uint4   TexCoord     : TEXCOORD;        // テクスチャ座標.
    uint4   BoneIndex    : BONE_INDEX;
    float4  BoneWeight   : BONE_WEIGHT;
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
// Textures and Samplers.
//-----------------------------------------------------------------------------
StructuredBuffer<float4x4>  BoneTransforms : register(t0);

//-----------------------------------------------------------------------------
//      スキニング処理を行います.
//-----------------------------------------------------------------------------
float4 Skinning(float4 value, uint4 boneIndex, float4 boneWeight)
{
    float4 ret = 0;
    ret += mul(BoneTransforms[boneIndex.x], value) * boneWeight.x;
    ret += mul(BoneTransforms[boneIndex.y], value) * boneWeight.y;
    ret += mul(BoneTransforms[boneIndex.z], value) * boneWeight.z;
    ret += mul(BoneTransforms[boneIndex.w], value) * boneWeight.w;
    return ret;
}


//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
VSOutput main(const VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 skinningPos = Skinning(localPos, input.BoneIndex, input.BoneWeight);
    float4 worldPos = mul(World, localPos);
    float4 viewPos  = mul(View, worldPos);
    float4 projPos  = mul(Proj, viewPos);

    float3 tangent, normal;
    UnpackTN(input.TangentSpace, tangent, normal);

    float4 skinningNormal  = Skinning(float4(normal,  0), input.BoneIndex, input.BoneWeight);
    float4 skinningTangent = Skinning(float4(tangent, 0), input.BoneIndex, input.BoneWeight);

    output.Position      = projPos;
    output.Color         = input.Color;
    output.Normal        = skinningNormal;
    output.Tangent       = skinningTangent;
    output.TexCoord01.xy = UnpackHalf2(input.TexCoord.x);
    output.TexCoord01.zw = UnpackHalf2(input.TexCoord.y);
    output.TexCoord23.xy = UnpackHalf2(input.TexCoord.z);
    output.TexCoord23.zw = UnpackHalf2(input.TexCoord.w);

    return output;
}
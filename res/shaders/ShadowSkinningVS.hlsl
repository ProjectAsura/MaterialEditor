//-----------------------------------------------------------------------------
// File : ShadowSkinningVS.hlsl
// Desc : Editor Skinning Vertex Shader for Shadow.
// Copyright(c) Project Asura. All right reserved.
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
    float3  Position     : POSITION;        // �ʒu���W.
    float4  Color        : COLOR;           // �J���[.
    uint    TangentSpace : TANGENT_SPACE;   // �ڐ����.
    uint4   TexCoord     : TEXCOORD;        // �e�N�X�`�����W.
    uint4   BoneIndex    : BONE_INDEX;
    float4  BoneWeight   : BONE_WEIGHT;
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
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
// CbMesh constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b1)
{
    float4x4 World;     // ���[���h�s��.
};

///////////////////////////////////////////////////////////////////////////////
// CbShadow constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbShadow : register(b2)
{
    float4x4 ShadowMatrix;  // �V���h�E�p�r���[�ˉe�s��.
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
StructuredBuffer<float4x4>  BoneTransforms : register(t0);

//-----------------------------------------------------------------------------
//      �X�L�j���O�������s���܂�.
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
//      ���C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
VSOutput main(const VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 skinningPos = Skinning(localPos, input.BoneIndex, input.BoneWeight);
    float4 projPos = mul(ShadowMatrix, localPos);

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
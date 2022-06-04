//-----------------------------------------------------------------------------
// File : ShadowVS.hlsl
// Desc : Editor Vertex Shader for Shadow
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
    float3  Normal       : NORMAL;          // �@���x�N�g��.
    float3  Tangent      : TANGENT;         // �ڐ��x�N�g��.
    float4  Color        : COLOR;           // �J���[.
    float2  TexCoord0    : TEXCOORD0;       // �e�N�X�`�����W0.
    float2  TexCoord1    : TEXCOORD1;       // �e�N�X�`�����W1
    float2  TexCoord2    : TEXCOORD2;       // �e�N�X�`�����W2.
    float2  TexCoord3    : TEXCOORD3;       // �e�N�X�`�����W3.
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
// CbMesh constant buffer
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
    float4x4 ShadowMatrix; // �V���h�E�p�r���[�ˉe�s��.
};

//-----------------------------------------------------------------------------
// Resources.
//-----------------------------------------------------------------------------
StructuredBuffer<float4x4>  InstanceMatrix : register(t1);

//-----------------------------------------------------------------------------
//      ���C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
VSOutput main(const VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    worldPos = mul(InstanceMatrix[instanceId], worldPos);
    float4 projPos = mul(ShadowMatrix, worldPos);

    float3 normal  = mul((float3x3)World, input.Normal);
    float3 tangent = mul((float3x3)World, input.Tangent);

    output.Position      = projPos;
    output.Normal        = normal;
    output.Tangent       = tangent;
    output.Color         = input.Color;
    output.TexCoord01    = float4(input.TexCoord0, input.TexCoord1);
    output.TexCoord23    = float4(input.TexCoord2, input.TexCoord3);

    return output;
}
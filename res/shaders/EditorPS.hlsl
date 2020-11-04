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
    float4 Diffuse  : SV_TARGET0;  // �f�B�t���[�Y���C�e�B���O����.
    float4 Specular : SV_TARGET1;  // �X�y�L�����[���C�e�B���O����.
    float4 NRM      : SV_TARGET2;  // �@��/���t�l�X/���^���l�X.
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
//      ���C���G���g���[�|�C���g�ł�.
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

//-----------------------------------------------------------------------------
// File : StandardPS.hlsl
// Desc : Standard Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Editor.hlsli"

/* ���[�U�[��`�f�[�^�̓X���b�g�ԍ�11����J�n���Ă�������. 10�Ԃ܂ł̓V�X�e���g�p�Ƃ��܂�.*/

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
//      ���C�e�B���O�����ł�.
//-----------------------------------------------------------------------------
PSOutput LightingPS(const VSOutput input)
{
    PSOutput output = (PSOutput)0;

    // UV�A�j���[�V����.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // �x�[�X�J���[�擾.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }

    // �@���^�I�N���[�W�����^���t�l�X�^���^���l�X���擾.
    float3 normal = Normal.Sample(AnisotropicWrap, uv).xyz * 2.0f - 1.0f;
    float3 orm    = ORM.Sample(AnisotropicWrap, uv).rgb;

    // �]�ڐ��x�N�g�����v�Z.
    float3 bitangent = GetBitangent(input);
    float3 N = FromTangentSpaceToWorld(normal, input.Tangent, bitangent, input.Normal);

    // �r���[��Ԃ̐[�x�ƈʒu���W���擾.
    float3 viewPos = GetViewPosition(input);

    // ���[���h��Ԃ̈ʒu�𕜌�.
    float3 worldPos = ToWorldPosition(viewPos);

    // �����x�N�g�����C�g�x�N�g�����Z�o.
    float3 V = normalize(CameraPos - worldPos);
    float3 L = normalize(SunLightDir);

    float3 kd = ToKd(baseColor.rgb, orm.z);
    float3 ks = ToKs(baseColor.rgb, orm.z);

    // ���C�e�B���O.
    output.Color.rgb += EvaluateIBLIsotropy(N, V, kd, ks, orm.y, orm.x);
    output.Color.rgb += EvaluateDirectLightIsotropicGGX(N, V, L, kd, ks, orm.y);
    output.Color.rgb += Emissive.Sample(AnisotropicWrap, uv).rgb;
    output.Color.a = 1.0f;

    output.NRM = EncodeNRM(N, orm.y, orm.z);

    return output;
}

//-----------------------------------------------------------------------------
//      �V���h�E�C���O�����ł�.
//-----------------------------------------------------------------------------
void ShadowingPS(const VSOutput input)
{
    // UV�A�j���[�V����.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // �x�[�X�J���[�擾.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }
}
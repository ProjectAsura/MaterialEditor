//-----------------------------------------------------------------------------
// File : StandardPS.hlsl
// Desc : Standard Material.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Editor.hlsli"


///////////////////////////////////////////////////////////////////////////////
// CbUser constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbUser
{
    float2 UVOffset;
    float2 UVScale;
    float  UVRotate;
    float  AlphaThreshold;
};

//-----------------------------------------------------------------------------
// Textures
//-----------------------------------------------------------------------------
Texture2D BaseColor;
Texture2D ORM;
Texture2D Normal;
Texture2D Emissive;


//-----------------------------------------------------------------------------
//      ���O�v�Z�ς�DFV�����擾���܂�.
//-----------------------------------------------------------------------------
float4 GetDFG(float roughness, float NoV)
{
    float2 uv = float2(NoV, roughness);
    uv.y = 1.0f - uv.y;    // DirectX�n�ɕϊ�.
    return EnvBRDF.SampleLevel(LinearClamp, uv, 0.0f);
}

//-------------------------------------------------------------------------------------------------
//      �f�B�t���[�Y��]�����܂�.
//-------------------------------------------------------------------------------------------------
float3 EvaluateDiffuseIBL(float3 N, float3 Kd)
{
    // Lambert Diffuse.
    return Kd * DiffuseLD.SampleLevel(LinearClamp, N, 0).rgb * IBLIntensity;
}

//-----------------------------------------------------------------------------
//      �X�y�L�����[��]�����܂�.
//-----------------------------------------------------------------------------
float3 EvaluateSpecularIBL(float3 N, float3 V, float3 R, float roughness)
{
    // �~�b�v�}�b�v���x�������擾.
    float2 size;
    float mipLevels;
    SpecularLD.GetDimensions( 0, size.x, size.y, mipLevels );

    // �Q�ƃ~�b�v�}�b�v���x����roughness����Z�o.
    float mipIndex = roughness * (mipLevels - 1.0f);

    float a2 = roughness * roughness;
    float3 dominantR = GetSpecularDomiantDir(N, R, a2);

    return SpecularLD.SampleLevel(LinearClamp, dominantR, mipIndex).rgb * IBLIntensity;
}

//-----------------------------------------------------------------------------
//      IBL��]�����܂�.
//-----------------------------------------------------------------------------
float3 EvaluateIBL(float3 N, float3 V, float3 Kd, float3 Ks, float roughness, float ao)
{
    float  NoV     = abs(dot(V, N));
    float3 R       = -reflect(V, N);
    float3 diffuse = EvaluateDiffuseIBL(N, Kd);
    diffuse *= ao;

    float3 specular   = EvaluateSpecularIBL(N, V, R, roughness);
    float  specularAO = CalcSpecularAO(NoV, ao, roughness);
    specular *= specularAO;

    float2 preDFG = GetDFG(roughness, NoV).rg;
    return (diffuse + specular * (Ks * preDFG.r + preDFG.g));
}

//-----------------------------------------------------------------------------
//      ���ڌ���]�����܂�.
//-----------------------------------------------------------------------------
float3 EvaluateDirectLight(float3 N, float3 V, float3 L, float3 Kd, float3 Ks, float roughness)
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
    float  viewDepth = ToViewDepth(input.Position.z, NearClip, FarClip);
    float3 viewPos   = ToViewPos(GetTexCoord0(input), viewDepth, UVToView);

    // ���[���h��Ԃ̈ʒu�𕜌�.
    float4 worldPos = mul(InvView, float4(viewPos, 1.0f));
    worldPos.xyz /= worldPos.w;

    // �����x�N�g�����C�g�x�N�g�����Z�o.
    float3 V = normalize(CameraPos.xyz - worldPos.xyz);
    float3 L = normalize(SunLightDir);

    float3 kd = ToKd(baseColor.rgb, orm.z);
    float3 ks = ToKs(baseColor.rgb, orm.z);

    // ���C�e�B���O.
    output.Color.rgb += EvaluateIBL(N, V, kd, ks, orm.y, orm.x);
    output.Color.rgb += EvaluateDirectLight(N, V, L, kd, ks, orm.y);
    output.Color.rgb += Emissive.Sample(AnisotropicWrap, uv);

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
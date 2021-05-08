//-----------------------------------------------------------------------------
// File : LightingCompositePS.hlsl
// Desc : Lighting Composite.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       DiffuseBuffer   : register(t0);
Texture2D       SpecularBuffer  : register(t1);
Texture2D       DepthBuffer     : register(t2);
SamplerState    PointSampler    : register(s0);


//-----------------------------------------------------------------------------
//      エントリーポイントです.
//-----------------------------------------------------------------------------
float4 main(VSOutput input) : SV_TARGET
{
    float depth = DepthBuffer.SampleLevel(PointSampler, input.TexCoord, 0.0f).r;
    if (depth >= 1.0f)
    { discard; }

    float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
    result.rgb += DiffuseBuffer .SampleLevel(PointSampler, input.TexCoord, 0.0f).rgb;
    result.rgb += SpecularBuffer.SampleLevel(PointSampler, input.TexCoord, 0.0f).rgb;
    return result;
}

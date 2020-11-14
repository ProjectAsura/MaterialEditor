//-----------------------------------------------------------------------------
// File : ShapeVS.hlsl
// Desc : Vertex Shader For Shape.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal   : NORMAL;
    float4 Color    : COLOR;
};

///////////////////////////////////////////////////////////////////////////////
// CbScene constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbScene : register(b0)
{
    float4x4    View            : packoffset(c0);
    float4x4    Proj            : packoffset(c4);
    float4x4    InvView         : packoffset(c8);
    float4x4    InvProj         : packoffset(c12);
    float3      CameraPos       : packoffset(c16);
    float       Timer           : packoffset(c16.w);
    float       NearClip        : packoffset(c17);
    float       FarClip         : packoffset(c17.y);
    float2      UVToView        : packoffset(c17.z);
    float2      ScreenSize      : packoffset(c18);
    float2      InvScreenSize   : packoffset(c18.z);
};


///////////////////////////////////////////////////////////////////////////////
// CbMesh Constant Buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b1)
{
    float4x4    World;
    float4      Color;
};

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
VSOutput main(const VSInput input )
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul( World, localPos );
    float4 viewPos  = mul( View,  worldPos );
    float4 projPos  = mul( Proj,  viewPos );

    float3 worldNormal = mul((float3x3)World, input.Normal);

    output.Position = projPos;
    output.Normal = worldNormal;
    output.Color    = Color;

    return output;
}
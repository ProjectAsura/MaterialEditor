//-----------------------------------------------------------------------------
// File : EditorMS.hlsl
// Desc : Mesh Shader For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Math.hlsli"


///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct MSOutput
{
    float4 Position     : SV_POSITION;      //!< �ʒu���W�ł�.
    float4 TexCoord01   : TEXCOORD01;       //!< �e�N�X�`�����W0��1�ł�(xy: TexCoord0, zw:TexCoord1)
    float4 TexCoord23   : TEXCOORD23;       //!< �e�N�X�`�����W2��3�ł�(xy: TexCoord2, zw:TexCoord3)
    float4 Color        : COLOR;            //!< ���_�J���[�ł�.
    float3 Normal       : NORMAL;           //!< �@���x�N�g���ł�.
    float3 Tangent      : TANGENT;          //!< �ڐ��x�N�g���ł�.
    float3 PositionWS   : POSITION_WS;      //!< ���[���h��Ԃ̈ʒu���W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// ParamMesh structure
///////////////////////////////////////////////////////////////////////////////
struct ParamMesh
{
    float4x4 World;             //!< ���[���h�s��ł�.
    float    MeshTimer;         //!< ���b�V���^�C�}�[�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// ParamScene structure
///////////////////////////////////////////////////////////////////////////////
struct ParamScene
{
    float4x4 View;              //!< �r���[�s��ł�.
    float4x4 Proj;              //!< �ˉe�s��ł�.
    float3   CameraPos;         //!< �J�����ʒu�ł�.
    float    CameraFovY;        //!< �J�����̐�����p(�P�ʂ̓��W�A��).
    float    NearClip;          //!< �߃N���b�v���ʂ܂ł̋����ł�.
    float    FarClip;           //!< ���N���b�v���ʂ܂ł̋����ł�.
    float2   ScreenSize;        //!< �X�N���[���T�C�Y�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// Meshlet structure
///////////////////////////////////////////////////////////////////////////////
struct Meshlet
{
    uint VertexOffset;      // ���_�ԍ��I�t�Z�b�g.
    uint VertexCount;       // �o�͒��_��.
    uint PrimitiveOffset;   // �v���~�e�B�u�ԍ��I�t�Z�b�g.
    uint PrimitiveCount;    // �o�̓v���~�e�B�u��.
};


//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------
ConstantBuffer<ParamMesh>       CbMesh          : register(b0);
ConstantBuffer<ParamScene>      CbScene         : register(b1);
StructuredBuffer<float3>        Positions       : register(t0);
StructuredBuffer<uint>          TangentSpaces   : register(t1);
StructuredBuffer<uint>          Colors          : register(t2);
StructuredBuffer<uint>          TexCoords0      : register(t3);
StructuredBuffer<uint>          TexCoords1      : register(t4);
StructuredBuffer<uint>          TexCoords2      : register(t5);
StructuredBuffer<uint>          TexCoords3      : register(t6);
StructuredBuffer<uint>          Indices         : register(t7);
StructuredBuffer<Meshlet>       Meshlets        : register(t8);
StructuredBuffer<uint>          Primitives      : register(t9);


//-----------------------------------------------------------------------------
//      ���C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main
(
    uint groupThreadId  : SV_GroupThreadID,
    uint groupId        : SV_GroupID,
    out  vertices MSOutput   verts[64],
    out  indices  uint3      polys[126]
)
{
    Meshlet m = Meshlets[groupId];

    SetMeshOutputCounts(m.VertexCount, m.PrimitiveCount);

    if (groupThreadId < m.PrimitiveCount)
    {
        uint packedIndex     = Primitives[m.PrimitiveOffset + groupThreadId];
        polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
    }
    
    if (groupThreadId < m.VertexCount)
    {
        uint        index  = Indices[m.VertexOffset + groupThreadId];
        MSOutput    output = (MSOutput)0;

        float4 localPos = float4(Positions[index], 1.0f);
        float4 worldPos = mul(CbMesh.World, localPos);
        float4 viewPos  = mul(CbScene.View, worldPos);
        float4 projPos  = mul(CbScene.Proj, viewPos);

        float3 N, T;
        UnpackTN(TangentSpaces[index], N, T);

        float3 worldN = normalize(mul((float3x3)CbMesh.World, N));
        float3 worldT = normalize(mul((float3x3)CbMesh.World, T));

        output.Position         = projPos;
        output.TexCoord01.xy    = UnpackHalf2(TexCoords0[index]);
        output.TexCoord01.zw    = UnpackHalf2(TexCoords1[index]);
        output.TexCoord23.xy    = UnpackHalf2(TexCoords2[index]);
        output.TexCoord23.zw    = UnpackHalf2(TexCoords3[index]);
        output.Color            = UnpackUnorm(Colors[index]);
        output.PositionWS       = worldPos.xyz;
        output.Tangent          = worldT;
        output.Normal           = worldN;

        verts[groupThreadId] = output;
    }
}

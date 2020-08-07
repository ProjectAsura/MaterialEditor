//-----------------------------------------------------------------------------
// File : EditorMS.hlsl
// Desc : Mesh Shader For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// MSInput structure
///////////////////////////////////////////////////////////////////////////////
struct MSInput
{
    float3 Position;        //!< �ʒu���W�ł�.
    float2 TexCoord0;       //!< �e�N�X�`�����W0�ł�.
    float2 TexCoord1;       //!< �e�N�X�`�����W1�ł�.
    float2 TexCoord2;       //!< �e�N�X�`�����W2�ł�.
    float2 TexCoord3;       //!< �e�N�X�`�����W3�ł�.
    float3 Normal;          //!< �@���x�N�g���ł�(���[�J�����W�n).
    float3 Tangent;         //!< �ڐ��x�N�g���ł�(���[�J�����W�n).
    float4 Color;           //!< ���_�J���[�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct MSOutput
{
    float4 Position     : SV_POSITION;      //!< �ʒu���W�ł�.
    float2 TexCorod0    : TEXCOORD0;        //!< �e�N�X�`�����W0�ł�.
    float2 TexCoord1    : TEXCOORD1;        //!< �e�N�X�`�����W1�ł�.
    float2 TexCoord2    : TEXCOORD2;        //!< �e�N�X�`�����W2�ł�.
    float2 TexCoord3    : TEXCOORD3;        //!< �e�N�X�`�����W3�ł�.
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
    int      MeshId;            //!< ���b�V��ID�ł�.
    float    MeshTimer;         //!< ���b�V���^�C�}�[�ł�.
    int      CustomProcessId;   //!< �J�X�^������ID�ł�.
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
ConstantBuffer<ParamMesh>       CbMesh      : register(b0);
ConstantBuffer<ParamScene>      CbScene     : register(b1);
StructuredBuffer<MSInput>       Vertices    : register(t0);
StructuredBuffer<uint>          Indices     : register(t1);
StructuredBuffer<Meshlet>       Meshlets    : register(t2);
StructuredBuffer<uint>          Primitives  : register(t3);


//-----------------------------------------------------------------------------
//      �p�b�L���O���ꂽ�C���f�b�N�X�f�[�^��W�J����.
//-----------------------------------------------------------------------------
uint3 UnpackPrimitiveIndex(uint packedIndex)
{
    return uint3(
        packedIndex & 0x3FF,
        (packedIndex >> 10) & 0x3FF,
        (packedIndex >> 20) & 0x3FF);
}

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
        MSInput     input  = Vertices[index];
        MSOutput    output = (MSOutput)0;

        float4 localPos = float4(input.Position, 1.0f);
        float4 worldPos = mul(CbMesh.World, localPos);
        float4 viewPos  = mul(CbScene.View, worldPos);
        float4 projPos  = mul(CbScene.Proj, viewPos);

        output.Position     = projPos;
        output.TexCorod0    = input.TexCoord0;
        output.TexCoord1    = input.TexCoord1;
        output.TexCoord2    = input.TexCoord2;
        output.TexCoord3    = input.TexCoord3;
        output.Color        = input.Color;
        output.PositionWS   = worldPos;

        // ���x�N�g��.
        float3 N = normalize(mul((float3x3)CbMesh.World, input.Normal));
        float3 T = normalize(mul((float3x3)CbMesh.World, input.Tangent));

        output.Tangent      = T;
        output.Normal       = N;

        verts[groupThreadId] = output;
    }
}

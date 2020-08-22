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
    float4 Position     : SV_POSITION;      //!< 位置座標です.
    float4 TexCoord01   : TEXCOORD01;       //!< テクスチャ座標0と1です(xy: TexCoord0, zw:TexCoord1)
    float4 TexCoord23   : TEXCOORD23;       //!< テクスチャ座標2と3です(xy: TexCoord2, zw:TexCoord3)
    float4 Color        : COLOR;            //!< 頂点カラーです.
    float3 Normal       : NORMAL;           //!< 法線ベクトルです.
    float3 Tangent      : TANGENT;          //!< 接線ベクトルです.
    float3 PositionWS   : POSITION_WS;      //!< ワールド空間の位置座標です.
};

///////////////////////////////////////////////////////////////////////////////
// ParamMesh structure
///////////////////////////////////////////////////////////////////////////////
struct ParamMesh
{
    float4x4 World;             //!< ワールド行列です.
    float    MeshTimer;         //!< メッシュタイマーです.
};

///////////////////////////////////////////////////////////////////////////////
// ParamScene structure
///////////////////////////////////////////////////////////////////////////////
struct ParamScene
{
    float4x4 View;              //!< ビュー行列です.
    float4x4 Proj;              //!< 射影行列です.
    float3   CameraPos;         //!< カメラ位置です.
    float    CameraFovY;        //!< カメラの垂直画角(単位はラジアン).
    float    NearClip;          //!< 近クリップ平面までの距離です.
    float    FarClip;           //!< 遠クリップ平面までの距離です.
    float2   ScreenSize;        //!< スクリーンサイズです.
};

///////////////////////////////////////////////////////////////////////////////
// Meshlet structure
///////////////////////////////////////////////////////////////////////////////
struct Meshlet
{
    uint VertexOffset;      // 頂点番号オフセット.
    uint VertexCount;       // 出力頂点数.
    uint PrimitiveOffset;   // プリミティブ番号オフセット.
    uint PrimitiveCount;    // 出力プリミティブ数.
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
//      メインエントリーポイントです.
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

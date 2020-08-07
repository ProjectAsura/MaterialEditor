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
    float3 Position;        //!< 位置座標です.
    float2 TexCoord0;       //!< テクスチャ座標0です.
    float2 TexCoord1;       //!< テクスチャ座標1です.
    float2 TexCoord2;       //!< テクスチャ座標2です.
    float2 TexCoord3;       //!< テクスチャ座標3です.
    float3 Normal;          //!< 法線ベクトルです(ローカル座標系).
    float3 Tangent;         //!< 接線ベクトルです(ローカル座標系).
    float4 Color;           //!< 頂点カラーです.
};

///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct MSOutput
{
    float4 Position     : SV_POSITION;      //!< 位置座標です.
    float2 TexCorod0    : TEXCOORD0;        //!< テクスチャ座標0です.
    float2 TexCoord1    : TEXCOORD1;        //!< テクスチャ座標1です.
    float2 TexCoord2    : TEXCOORD2;        //!< テクスチャ座標2です.
    float2 TexCoord3    : TEXCOORD3;        //!< テクスチャ座標3です.
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
    int      MeshId;            //!< メッシュIDです.
    float    MeshTimer;         //!< メッシュタイマーです.
    int      CustomProcessId;   //!< カスタム処理IDです.
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
ConstantBuffer<ParamMesh>       CbMesh      : register(b0);
ConstantBuffer<ParamScene>      CbScene     : register(b1);
StructuredBuffer<MSInput>       Vertices    : register(t0);
StructuredBuffer<uint>          Indices     : register(t1);
StructuredBuffer<Meshlet>       Meshlets    : register(t2);
StructuredBuffer<uint>          Primitives  : register(t3);


//-----------------------------------------------------------------------------
//      パッキングされたインデックスデータを展開する.
//-----------------------------------------------------------------------------
uint3 UnpackPrimitiveIndex(uint packedIndex)
{
    return uint3(
        packedIndex & 0x3FF,
        (packedIndex >> 10) & 0x3FF,
        (packedIndex >> 20) & 0x3FF);
}

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

        // 基底ベクトル.
        float3 N = normalize(mul((float3x3)CbMesh.World, input.Normal));
        float3 T = normalize(mul((float3x3)CbMesh.World, input.Tangent));

        output.Tangent      = T;
        output.Normal       = N;

        verts[groupThreadId] = output;
    }
}

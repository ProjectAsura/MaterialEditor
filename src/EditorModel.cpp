//-----------------------------------------------------------------------------
// File : EditorModel.cpp
// Desc : Model For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <EditorModel.h>
#include <MeshLoader.h>
#include <asdxLogger.h>
#include <asdxDeviceContext.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    asdx::Vector3   Position;
    uint32_t        Color;
    uint32_t        TangentSpace;
    uint32_t        TexCoord0;
    uint32_t        TexCoord1;
    uint32_t        TexCoord2;
    uint32_t        TexCoord3;
};

///////////////////////////////////////////////////////////////////////////////
// SkinningData structure
///////////////////////////////////////////////////////////////////////////////
struct SkinningData
{
    asdx::ResBoneIndex  Indices;
    asdx::Vector4       Weights;
};

} // namespace


///////////////////////////////////////////////////////////////////////////////
// EditorMesh class 
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorMesh::EditorMesh()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorMesh::~EditorMesh()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool EditorMesh::Init(ID3D11Device* pDevice, const asdx::ResMesh& mesh)
{
    if (pDevice == nullptr)
    {
        ELOG("Error : Invalid Argument");
        return false;
    }

    std::vector<Vertex> vertices;
    vertices.resize(mesh.Positions.size());

    m_MeshName      = mesh.MeshName;
    m_MaterialName  = mesh.MaterialName;

    m_Box.maxi = m_Box.mini = mesh.Positions[0];

    for(size_t i=0; i<vertices.size(); ++i)
    {
        auto& vertex = vertices[i];
        vertex.Position     = mesh.Positions[i];
        vertex.Color        = (mesh.Colors.empty()) ? 0xFFFFFFFF : mesh.Colors[i];
        vertex.TangentSpace = mesh.TangentSpaces[i];
        vertex.TexCoord0    = (mesh.TexCoords[0].empty()) ? 0 : mesh.TexCoords[0][i];
        vertex.TexCoord1    = (mesh.TexCoords[1].empty()) ? 0 : mesh.TexCoords[1][i];
        vertex.TexCoord2    = (mesh.TexCoords[2].empty()) ? 0 : mesh.TexCoords[2][i];
        vertex.TexCoord3    = (mesh.TexCoords[3].empty()) ? 0 : mesh.TexCoords[3][i];

        m_Box.maxi = asdx::Vector3::Max(m_Box.maxi, vertex.Position);
        m_Box.mini = asdx::Vector3::Min(m_Box.mini, vertex.Position);
    }

    if (!m_VB.Init(
        pDevice,
        sizeof(Vertex) * vertices.size(),
        sizeof(Vertex),
        vertices.data()))
    {
        ELOG("Error : VertexBuffer::Init() Failed.");
        return false;
    }

    m_HasSkinningData = false;

    if (mesh.BoneWeights.size() > 0)
    {
        std::vector<SkinningData> skinningData;
        skinningData.resize(mesh.BoneWeights.size());

        for(size_t i=0; i<mesh.BoneWeights.size(); ++i)
        {
            auto& vertex = skinningData[i];
            vertex.Indices = mesh.BoneIndices[i];
            vertex.Weights = mesh.BoneWeights[i];
        }

        if (!m_SkinVB.Init(
            pDevice,
            sizeof(SkinningData) * skinningData.size(),
            sizeof(SkinningData),
            skinningData.data()))
        {
            ELOG("Error : VertexBuffer::Init() Failed.");
            return false;
        }

        m_HasSkinningData = true;
    }

    if (!m_IB.Init(
        pDevice,
        sizeof(uint32_t) * mesh.Indices.size(),
        mesh.Indices.data()))
    {
        ELOG("Error : IndexBuffer::Init() Failed.");
        return false;
    }
    m_IndexCount = uint32_t(mesh.Indices.size());

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void EditorMesh::Term()
{
    m_IndexCount        = 0;
    m_HasSkinningData   = false;

    m_IB    .Term();
    m_VB    .Term();
    m_SkinVB.Term();
}

//-----------------------------------------------------------------------------
//      メッシュ名を取得します.
//-----------------------------------------------------------------------------
const std::string& EditorMesh::GetMeshName() const
{ return m_MeshName; }

//-----------------------------------------------------------------------------
//      マテリアル名を取得します.
//-----------------------------------------------------------------------------
const std::string& EditorMesh::GetMaterialName() const
{ return m_MaterialName; }

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void EditorMesh::Draw(ID3D11DeviceContext* pContext) const
{
    auto pVB0    = m_VB.GetBuffer();
    auto stride0 = m_VB.GetStride();
    auto offset0 = 0u;

    pContext->IASetVertexBuffers(0, 1, &pVB0, &stride0, &offset0);

    if (m_HasSkinningData)
    {
        auto pVB1    = m_SkinVB.GetBuffer();
        auto stride1 = m_SkinVB.GetStride();
        auto offset1 = 0u;

        pContext->IASetVertexBuffers(1, 1, &pVB1, &stride1, &offset1);
    }

    pContext->IASetIndexBuffer(m_IB.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
    pContext->DrawIndexed(m_IndexCount, 0, 0);
}

//-----------------------------------------------------------------------------
//      スキニングデータを持つかどうか?
//-----------------------------------------------------------------------------
bool EditorMesh::HasSkinningData() const
{ return m_HasSkinningData; }

//-----------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-----------------------------------------------------------------------------
const BoundingBox& EditorMesh::GetBox() const
{ return m_Box; }


///////////////////////////////////////////////////////////////////////////////
// EditorModel class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorModel::EditorModel()
: m_Scale       (1.0f, 1.0f, 1.0f)
, m_Rotation    (0.0f, 0.0f, 0.0f)
, m_Translation (0.0f, 0.0f, 0.0f)
, m_World       (asdx::Matrix::CreateIdentity())
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorModel::~EditorModel()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool EditorModel::Init(const char* path)
{
    auto pDevice = asdx::DeviceContext::Instance().GetDevice();

    asdx::ResModel res;
    MeshLoader loader;
    if (!loader.Load(path, res))
    {
        ELOGA("Error : MeshLoader::Load() Failed.");
        return false;
    }

    m_Meshes.resize(res.Meshes.size());

    for(size_t i=0; i<res.Meshes.size(); ++i)
    {
        if (!m_Meshes[i].Init(pDevice, res.Meshes[i]))
        {
            ELOG("Error : EditorMesh::Init() Failed.");
            return false;
        }

        if (i == 0)
        {
            m_Box = m_Meshes[i].GetBox();
        }
        else
        {
            m_Box.mini = asdx::Vector3::Min(m_Box.mini, m_Meshes[i].GetBox().mini);
            m_Box.maxi = asdx::Vector3::Max(m_Box.maxi, m_Meshes[i].GetBox().maxi);
        }
    }

    m_Path        = path;
    m_Scale       = asdx::Vector3(1.0f, 1.0f, 1.0f);
    m_Rotation    = asdx::Vector3(0.0f, 0.0f, 0.0f);
    m_Translation = asdx::Vector3(0.0f, 0.0f, 0.0f);
    m_World       = asdx::Matrix::CreateIdentity();

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void EditorModel::Term()
{
    for(size_t i=0; i<m_Meshes.size(); ++i)
    { m_Meshes[i].Term(); }

    m_Meshes.clear();

    m_Scale       = asdx::Vector3(1.0f, 1.0f, 1.0f);
    m_Rotation    = asdx::Vector3(0.0f, 0.0f, 0.0f);
    m_Translation = asdx::Vector3(0.0f, 0.0f, 0.0f);
    m_World       = asdx::Matrix::CreateIdentity();
}

//-----------------------------------------------------------------------------
//      メッシュ数を取得します.
//-----------------------------------------------------------------------------
uint32_t EditorModel::GetMeshCount() const
{ return uint32_t(m_Meshes.size()); }

//-----------------------------------------------------------------------------
//      メッシュを取得します.
//-----------------------------------------------------------------------------
const EditorMesh& EditorModel::GetMesh(uint32_t index) const
{ return m_Meshes[index]; }

//-----------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-----------------------------------------------------------------------------
const BoundingBox& EditorModel::GetBox() const
{ return m_Box; }

//-----------------------------------------------------------------------------
//      ファイルパスを取得します.
//-----------------------------------------------------------------------------
const std::string& EditorModel::GetPath() const
{ return m_Path; }

//-----------------------------------------------------------------------------
//      ワールド行列を取得します.
//-----------------------------------------------------------------------------
const asdx::Matrix& EditorModel::GetWorld() const
{ return m_World; }

//-----------------------------------------------------------------------------
//      スケールを設定します.
//-----------------------------------------------------------------------------
void EditorModel::SetScale(const asdx::Vector3& value)
{
    m_Scale = value;
    UpdateWorld();
}

//-----------------------------------------------------------------------------
//      回転角を設定します.
//-----------------------------------------------------------------------------
void EditorModel::SetRotate(const asdx::Vector3& value)
{
    m_Rotation = value;
    UpdateWorld();
}

//-----------------------------------------------------------------------------
//      平行移動量を設定します.
//-----------------------------------------------------------------------------
void EditorModel::SetTranslation(const asdx::Vector3& value)
{
    m_Translation = value;
    UpdateWorld();
}

//-----------------------------------------------------------------------------
//      スケールを取得します.
//-----------------------------------------------------------------------------
const asdx::Vector3& EditorModel::GetScale() const
{ return m_Scale; }

//-----------------------------------------------------------------------------
//      回転角を取得します.
//-----------------------------------------------------------------------------
const asdx::Vector3& EditorModel::GetRotate() const
{ return m_Rotation; }

//-----------------------------------------------------------------------------
//      平行移動量を取得します.
//-----------------------------------------------------------------------------
const asdx::Vector3& EditorModel::GetTranslation() const
{ return m_Translation; }

//-----------------------------------------------------------------------------
//      ワールド行列を更新します.
//-----------------------------------------------------------------------------
void EditorModel::UpdateWorld()
{
    m_World  = asdx::Matrix::CreateScale(m_Scale); 
    m_World *= asdx::Matrix::CreateRotationX(asdx::ToRadian(m_Rotation.x));
    m_World *= asdx::Matrix::CreateRotationY(asdx::ToRadian(m_Rotation.y));
    m_World *= asdx::Matrix::CreateRotationZ(asdx::ToRadian(m_Rotation.z));
    m_World._41 = m_Translation.x;
    m_World._42 = m_Translation.y;
    m_World._43 = m_Translation.z;
}
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
#include <asdxMisc.h>
#include <asdxDeviceContext.h>
#include <OBJLoader.h>
#include <FBXLoader.h>
#include <GLTFLoader.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    asdx::Vector3   Position;
    asdx::Vector3   Normal;
    asdx::Vector3   Tangent;
    asdx::Vector4   Color;
    asdx::Vector2   TexCoord0;
    asdx::Vector2   TexCoord1;
    asdx::Vector2   TexCoord2;
    asdx::Vector2   TexCoord3;
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
: m_InstanceCount(1)
, m_MaterialId   (0)
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
        assert(mesh.Positions.empty() == false);
        assert(mesh.Normals  .empty() == false);
        assert(mesh.Tangents .empty() == false);

        auto& vertex = vertices[i];
        vertex.Position     = mesh.Positions[i];
        vertex.Color        = (mesh.Colors.empty()) ? asdx::Vector4(1.0f, 1.0f, 1.0f, 1.0f) : mesh.Colors[i];
        vertex.Normal       = mesh.Normals[i];
        vertex.Tangent      = mesh.Tangents[i];
        vertex.TexCoord0    = (mesh.TexCoords[0].empty()) ? asdx::Vector2(0.0f, 0.0f) : mesh.TexCoords[0][i];
        vertex.TexCoord1    = (mesh.TexCoords[1].empty()) ? asdx::Vector2(0.0f, 0.0f) : mesh.TexCoords[1][i];
        vertex.TexCoord2    = (mesh.TexCoords[2].empty()) ? asdx::Vector2(0.0f, 0.0f) : mesh.TexCoords[2][i];
        vertex.TexCoord3    = (mesh.TexCoords[3].empty()) ? asdx::Vector2(0.0f, 0.0f) : mesh.TexCoords[3][i];

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

    m_InstanceCount = 1;
    {
        // 単位行列で初期化.
        for(auto i=0u; i<kMaxInstanceCount; ++i)
        { m_InstanceMatrix[i] = asdx::Matrix::CreateIdentity(); }

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth              = sizeof(asdx::Matrix) * kMaxInstanceCount;
        desc.Usage                  = D3D11_USAGE_DYNAMIC;
        desc.BindFlags              = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags         = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags              = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride    = sizeof(asdx::Matrix);

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem             = reinterpret_cast<const void*>(m_InstanceMatrix);
        res.SysMemPitch         = sizeof(asdx::Matrix) * kMaxInstanceCount;
        res.SysMemSlicePitch    = res.SysMemSlicePitch;

        auto hr = pDevice->CreateBuffer(&desc, &res, m_InstanceMatrixResource.GetAddress()); 
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements  = kMaxInstanceCount;

        hr = pDevice->CreateShaderResourceView(
            m_InstanceMatrixResource.GetPtr(), &srvDesc, m_InstanceMatrixSRV.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreateShaderResourceView() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void EditorMesh::Term()
{
    m_MeshName      .clear();
    m_MaterialName  .clear();

    m_IndexCount        = 0;
    m_HasSkinningData   = false;

    m_IB    .Term();
    m_VB    .Term();
    m_SkinVB.Term();

    m_InstanceCount = 0;
    m_InstanceMatrixResource.Reset();
    m_InstanceMatrixSRV.Reset();

    m_MaterialId = 0;
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
    auto pSRV    = m_InstanceMatrixSRV.GetPtr();

    pContext->IASetVertexBuffers(0, 1, &pVB0, &stride0, &offset0);
    pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetShaderResources(1, 1, &pSRV);

    if (m_HasSkinningData)
    {
        auto pVB1    = m_SkinVB.GetBuffer();
        auto stride1 = m_SkinVB.GetStride();
        auto offset1 = 0u;

        pContext->IASetVertexBuffers(1, 1, &pVB1, &stride1, &offset1);
    }

    pContext->IASetIndexBuffer(m_IB.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
    pContext->DrawIndexedInstanced(m_IndexCount, m_InstanceCount, 0, 0, 0);

    ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
    pContext->VSSetShaderResources(1, 1, pNullSRV);
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

//-----------------------------------------------------------------------------
//      ポリゴン数を取得します.
//-----------------------------------------------------------------------------
uint32_t EditorMesh::GetPolygonCount() const
{ return m_IndexCount / 3; }

//-----------------------------------------------------------------------------
//      インスタンス数を設定します.
//-----------------------------------------------------------------------------
void EditorMesh::SetInstanceCount(uint32_t count)
{
    if (count == 0 || count > kMaxInstanceCount)
    { return; }

    m_InstanceCount = count;
}

//-----------------------------------------------------------------------------
//      インスタンス数を取得します.
//-----------------------------------------------------------------------------
uint32_t EditorMesh::GetInstanceCount() const
{ return m_InstanceCount; }

//-----------------------------------------------------------------------------
//      インスタンス行列を取得します.
//-----------------------------------------------------------------------------
const asdx::Matrix& EditorMesh::GetInstanceMatrix(uint32_t index) const
{
    assert(index < kMaxInstanceCount);
    return m_InstanceMatrix[index];
}

//-----------------------------------------------------------------------------
//      インスタンス行列を設定します.
//-----------------------------------------------------------------------------
void EditorMesh::SetInstanceMatrix(uint32_t index, const asdx::Matrix& matrix)
{
    assert(index < kMaxInstanceCount);
    m_InstanceMatrix[index] = matrix;
}

//-----------------------------------------------------------------------------
//      マテリアル番号を設定します.
//-----------------------------------------------------------------------------
void EditorMesh::SetMaterialId(uint32_t index)
{ m_MaterialId = index; }

//-----------------------------------------------------------------------------
//      マテリアル番号を取得します.
//-----------------------------------------------------------------------------
uint32_t EditorMesh::GetMaterialId() const
{ return m_MaterialId; }


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
, m_DirtyWorld  (false)
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
    auto ext = asdx::GetExtA(path);
    auto pDevice = asdx::DeviceContext::Instance().GetDevice();

    asdx::ResModel model;

    // OBJファイル.
    if (_stricmp(ext.c_str(), "obj") == 0)
    {
        OBJLoader loader;
        if (!loader.Load(path, model))
        {
            ELOGA("Error : OBJLoader::Load() Failed. path = %s", path);
            return false;
        }
    }
    // FBXファイル.
    else if (_stricmp(ext.c_str(), "fbx") == 0)
    {
        FBXLoader loader;
        if (!loader.Load(path, model))
        {
            ELOGA("Error : FBXLoader::Load() Failed. path = %s", path);
            return false;
        }
    }
    // GLTFファイル.
    else if (_stricmp(ext.c_str(), "gltf") == 0 || _stricmp(ext.c_str(), "glb") == 0)
    {
        GLTFLoader loader;
        if (!loader.Load(path, model))
        {
            ELOGA("Error : GLTFLoader::Load() Failed. path = %s", path);
            return false;
        }
    }
    else
    {
        // 非サポートフォーマット.
        ELOGA("Error : Unsupported File Format. path = %s", path);
        return false;
    }


    m_Meshes.resize(model.Meshes.size());

    for(size_t i=0; i<model.Meshes.size(); ++i)
    {
        if (!m_Meshes[i].Init(pDevice, model.Meshes[i]))
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
EditorMesh& EditorModel::GetMesh(uint32_t index)
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
    m_DirtyWorld = true;
}

//-----------------------------------------------------------------------------
//      回転角を設定します.
//-----------------------------------------------------------------------------
void EditorModel::SetRotation(const asdx::Vector3& value)
{
    m_Rotation = value;
    m_DirtyWorld = true;
}

//-----------------------------------------------------------------------------
//      平行移動量を設定します.
//-----------------------------------------------------------------------------
void EditorModel::SetTranslation(const asdx::Vector3& value)
{
    m_Translation = value;
    m_DirtyWorld = true;
}

//-----------------------------------------------------------------------------
//      スケールを取得します.
//-----------------------------------------------------------------------------
const asdx::Vector3& EditorModel::GetScale() const
{ return m_Scale; }

//-----------------------------------------------------------------------------
//      回転角を取得します.
//-----------------------------------------------------------------------------
const asdx::Vector3& EditorModel::GetRotation() const
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
    if (!m_DirtyWorld)
    { return; }

    m_World  = asdx::Matrix::CreateScale(m_Scale); 
    m_World *= asdx::Matrix::CreateRotationX(asdx::ToRadian(m_Rotation.x));
    m_World *= asdx::Matrix::CreateRotationY(asdx::ToRadian(m_Rotation.y));
    m_World *= asdx::Matrix::CreateRotationZ(asdx::ToRadian(m_Rotation.z));
    m_World._41 = m_Translation.x;
    m_World._42 = m_Translation.y;
    m_World._43 = m_Translation.z;

    m_DirtyWorld = false;
}

//-----------------------------------------------------------------------------
//      ポリゴン数を計算します.
//-----------------------------------------------------------------------------
uint64_t EditorModel::CalcPolygonCount() const
{
    uint64_t count = 0;
    for(size_t i=0; i<m_Meshes.size(); ++i)
    { count += m_Meshes[i].GetPolygonCount(); }

    return count;
}
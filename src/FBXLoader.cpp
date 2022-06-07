//-------------------------------------------------------------------------------------------------
// File : FBXLoader.cpp
// Desc : Film Box Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <FBXLoader.h>
#include <asdxLogger.h>
#include <asdxMisc.h>

#if ENABLE_FBX
#include <fbxsdk.h>
#endif

//-------------------------------------------------------------------------------------------------
// Using Statements.
//-------------------------------------------------------------------------------------------------
using namespace fbxsdk;


namespace {

#if ENABLE_FBX
///////////////////////////////////////////////////////////////////////////////////////////////////
// Counters structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Counters
{
    uint32_t mesh       = 0;
    uint32_t skeleton   = 0;
    uint32_t camera     = 0;
    uint32_t light      = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Face structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Face
{
    uint32_t IndexP[ 3 ];       // 位置座標インデックス.
    uint32_t IndexC[ 3 ];       // 頂点カラーインデックス.
    uint32_t IndexN[ 3 ];       // 法線ベクトルインデックス.
    uint32_t IndexT[ 3 ];       // 接線ベクトルインデックス.
    uint32_t IndexU[ 4 ][ 3 ];  // テクスチャ座標インデックス.
};

//-------------------------------------------------------------------------------------------------
//      ノード別の数を解析します.
//-------------------------------------------------------------------------------------------------
void ParseCount(FbxNode* pNode, Counters& counters)
{
    if (pNode == nullptr)
    { return; }

    if (pNode->GetNodeAttribute() != nullptr)
    {
        auto type = pNode->GetNodeAttribute()->GetAttributeType();
        switch(type)
        {
        case FbxNodeAttribute::eMesh:
            { counters.mesh++; }
            break;

        case FbxNodeAttribute::eSkeleton:
            { counters.skeleton++; }
            break;

        case FbxNodeAttribute::eCamera:
            { counters.camera++; }
            break;

        case FbxNodeAttribute::eLight:
            { counters.light++; }
            break;
        }
    }

    auto count = pNode->GetChildCount();
    for(auto i=0; i<count; ++i)
    { ParseCount( pNode->GetChild(i), counters); }
}

//-------------------------------------------------------------------------------------------------
//      FbxMatrixを変換します.
//-------------------------------------------------------------------------------------------------
asdx::Matrix FromFbxMatrix(const FbxMatrix& matrix)
{
    asdx::Matrix result;
    result._11 = static_cast<float>(matrix.Get( 0, 0 ));
    result._12 = static_cast<float>(matrix.Get( 0, 1 ));
    result._13 = static_cast<float>(matrix.Get( 0, 2 ));
    result._14 = static_cast<float>(matrix.Get( 0, 3 ));

    result._21 = static_cast<float>(matrix.Get( 1, 0 ));
    result._22 = static_cast<float>(matrix.Get( 1, 1 ));
    result._23 = static_cast<float>(matrix.Get( 1, 2 ));
    result._24 = static_cast<float>(matrix.Get( 1, 3 ));

    result._31 = static_cast<float>(matrix.Get( 2, 0 ));
    result._32 = static_cast<float>(matrix.Get( 2, 1 ));
    result._33 = static_cast<float>(matrix.Get( 2, 2 ));
    result._34 = static_cast<float>(matrix.Get( 2, 3 ));

    result._41 = static_cast<float>(matrix.Get( 3, 0 ));
    result._42 = static_cast<float>(matrix.Get( 3, 1 ));
    result._43 = static_cast<float>(matrix.Get( 3, 2 ));
    result._44 = static_cast<float>(matrix.Get( 3, 3 ));

    return result;
}

//-------------------------------------------------------------------------------------------------
//      FbxMatrixを変換します.
//-------------------------------------------------------------------------------------------------
asdx::Matrix FromFbxMatrix(const FbxAMatrix& matrix)
{
    asdx::Matrix result;
    result._11 = static_cast<float>(matrix.Get( 0, 0 ));
    result._12 = static_cast<float>(matrix.Get( 0, 1 ));
    result._13 = static_cast<float>(matrix.Get( 0, 2 ));
    result._14 = static_cast<float>(matrix.Get( 0, 3 ));

    result._21 = static_cast<float>(matrix.Get( 1, 0 ));
    result._22 = static_cast<float>(matrix.Get( 1, 1 ));
    result._23 = static_cast<float>(matrix.Get( 1, 2 ));
    result._24 = static_cast<float>(matrix.Get( 1, 3 ));

    result._31 = static_cast<float>(matrix.Get( 2, 0 ));
    result._32 = static_cast<float>(matrix.Get( 2, 1 ));
    result._33 = static_cast<float>(matrix.Get( 2, 2 ));
    result._34 = static_cast<float>(matrix.Get( 2, 3 ));

    result._41 = static_cast<float>(matrix.Get( 3, 0 ));
    result._42 = static_cast<float>(matrix.Get( 3, 1 ));
    result._43 = static_cast<float>(matrix.Get( 3, 2 ));
    result._44 = static_cast<float>(matrix.Get( 3, 3 ));

    return result;
}

//-------------------------------------------------------------------------------------------------
//      FbxVector2からVector2に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector2 ToVector2(const FbxVector2& value)
{
    return asdx::Vector2(
        float(value[0]),
        float(value[1]));
}

//-------------------------------------------------------------------------------------------------
//      FbxVector4からVector3に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector3 ToVector3(const FbxVector4& value)
{
    return asdx::Vector3(
        float(value[0]),
        float(value[1]),
        float(value[2]));
}

//-------------------------------------------------------------------------------------------------
//      FbxVector4からVector4に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector4 ToVector4(const FbxVector4& value)
{
    return asdx::Vector4(
        float(value[0]),
        float(value[1]),
        float(value[2]),
        float(value[3]));
}

//-------------------------------------------------------------------------------------------------
//      Vector2に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector2 FromFbxProperty2(const FbxPropertyT<FbxDouble2>& value)
{
    auto src = value.Get();
    return asdx::Vector2(float(src[0]), float(src[1]));
}

//-------------------------------------------------------------------------------------------------
//      Vector3に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector3 FromFbxProperty3(const FbxPropertyT<FbxDouble3>& value)
{
    auto src = value.Get();
    return asdx::Vector3(
        float(src[0]),
        float(src[1]),
        float(src[2]));
}

//-------------------------------------------------------------------------------------------------
//      Vector4に変換します.
//-------------------------------------------------------------------------------------------------
asdx::Vector4 FromFbxProperty4(const FbxPropertyT<FbxDouble4>& value)
{
    auto src = value.Get();
    return asdx::Vector4(
        float(src[0]),
        float(src[1]),
        float(src[2]),
        float(src[3]));
}

//-------------------------------------------------------------------------------------------------
//      テクスチャリストに追加します.
//-------------------------------------------------------------------------------------------------
void AddTexture(FbxSurfaceMaterial* pMaterial, const char* propName, std::vector<asdx::ResTexturePath>& textures)
{
    auto prop = pMaterial->FindProperty(propName);
    auto count = prop.GetSrcObjectCount<FbxTexture>();

    for(auto i=0; i<count; ++i)
    {
        auto pLayeredTexture = prop.GetSrcObject<FbxLayeredTexture>(i);
        
        // 単体テクスチャ.
        if (pLayeredTexture == nullptr)
        {
            auto pBaseTexture = prop.GetSrcObject<FbxTexture>(i);
            if (pBaseTexture == nullptr)
            { continue; }

            auto pFileTexture = FbxCast<FbxFileTexture>(pBaseTexture);
            if (pFileTexture == nullptr)
            { continue; }

            // 使用されていない場合はスキップ.
            if (!pFileTexture->UseMaterial.Get())
            { continue; }

            // テクスチャ名を格納.
            asdx::ResTexturePath item;
            item.Name = propName;
            item.Path = pFileTexture->GetRelativeFileName(); // UTF-8で格納.

            textures.emplace_back(item);
        }
        // レイヤーテクスチャ.
        else
        {
            auto layerCount = pLayeredTexture->GetSrcObjectCount<FbxTexture>();
            for(auto l=0; l<layerCount; ++l)
            {
                auto pBaseTexture = pLayeredTexture->GetSrcObject<FbxTexture>(l);
                if (pBaseTexture == nullptr)
                { continue; }

                auto pFileTexture = FbxCast<FbxFileTexture>(pBaseTexture);
                if (pFileTexture == nullptr)
                { continue; }

                // 使用されていない場合はスキップ.
                if (pFileTexture->UseMaterial.Get())
                { continue; }

                // テクスチャ名を格納.
                asdx::ResTexturePath item;
                item.Name = propName;
                item.Path = pFileTexture->GetRelativeFileName(); // UTF-8で格納.
                textures.emplace_back(item);
            }
        }
    }
}
#endif//ENABLE_FBX

} // namespace


///////////////////////////////////////////////////////////////////////////////////////////////////
// FBXLoader class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
FBXLoader::FBXLoader()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
FBXLoader::~FBXLoader()
{ /* DO_NOTHING */ }

#if ENABLE_FBX
//-------------------------------------------------------------------------------------------------
//      読み込み処理です.
//-------------------------------------------------------------------------------------------------
bool FBXLoader::Load(const char* path, asdx::ResModel& model, bool skipTriangulate)
{
    if (path == nullptr)
    {
        ELOG("Error : FBXLoader::Init() Failed. path = %s", path);
        return false;
    }

    // フォルダパス取得.
    m_FolderPath = asdx::GetDirectoryPathA(path);

    // マネージャ生成.
    m_pMgr = FbxManager::Create();
    if (m_pMgr == nullptr)
    {
        ELOG("Error : FbxManager::Create() Failed.");
        Term();
        return false;
    }

    // シーンを生成.
    m_pScene = FbxScene::Create(m_pMgr, "PainterScene");
    if (m_pScene == nullptr)
    {
        ELOG("Error : FbxScene::Create() Failed.");
        Term();
        return false;
    }

    // インポータ―生成.
    m_pImporter = FbxImporter::Create(m_pMgr, "FBXLoader");
    if (m_pImporter == nullptr)
    {
        ELOG("Error : FbxImporter::Create() Failed.");
        Term();
        return false;
    }

    // UTF-8に変換して，インポータ―を初期化.
    {
        char* filePath;
        FbxAnsiToUTF8(path, filePath, 0);

        if (!m_pImporter->Initialize(filePath))
        {
            ELOG("Error : FbxImpoter::Initialize() Failed.");
            Term();
            FbxDeleteArray(filePath);
            return false;
        }

        // 不要なメモリを解放.
        FbxDeleteArray(filePath);
    }

    // インポート.
    if (!m_pImporter->Import(m_pScene))
    {
        ELOG("Error : FbxImport::Import() Failed.");
        return false;
    }

    // 単位を[cm]に変換.
    {
        auto unit = m_pScene->GetGlobalSettings().GetSystemUnit();
        if (unit.GetScaleFactor() != 1.0f)
        { FbxSystemUnit::cm.ConvertScene(m_pScene); }
    }

    // 三角形化しておく.
    if (!skipTriangulate)
    {
        FbxGeometryConverter converter(m_pMgr);
        if (!converter.Triangulate(m_pScene, true))
        {
            ELOG("Error : FbxGeometryConverter::Triangulate() Failed.");
            return false;
        }

        // 変なポリゴンもついでに取り除いておく.
        converter.RemoveBadPolygonsFromMeshes(m_pScene);
    }

    // コンテンツを解析.
    ParseContent(model);

    m_pMgr->Destroy();
    m_pMgr      = nullptr;
    m_pImporter = nullptr;
    m_pScene    = nullptr;

    return true;
}

//-------------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------------
void FBXLoader::Term()
{
    if (m_pMgr != nullptr)
    { m_pMgr->Destroy(); }

    m_pMgr      = nullptr;
    m_pImporter = nullptr;
    m_pScene    = nullptr;

    m_FolderPath.clear();
}

//-------------------------------------------------------------------------------------------------
//      コンテンツを解析します
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseContent(asdx::ResModel& model)
{
    // ルートノード取得.
    FbxNode* pRootNode = m_pScene->GetRootNode();
    if (pRootNode == nullptr)
    {
        ELOG("Error : RootNode is null.");
        return;
    }

    // ノードを解析.
    ParseNode(pRootNode, model);
}

//-------------------------------------------------------------------------------------------------
//      ノードを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseNode(FbxNode* pNode, asdx::ResModel& model)
{
    if (pNode == nullptr)
    {
        ELOG("Error : Node is null.");
        return;
    }

    if (pNode->GetNodeAttribute() == nullptr)
    {
        ELOG("Error : Node Attribute is null.");
    }
    else
    {
        auto type = pNode->GetNodeAttribute()->GetAttributeType();
        switch(type)
        {
        case FbxNodeAttribute::eMesh:
            { ParseMesh(pNode, model); }
            break;

#if 0
        //case FbxNodeAttribute::eSkeleton:
        //   { ParseSkeleton(pNode); }
        //   break;

        //case FbxNodeAttribute::eCamera:
        //   { ParseCamera(pNode); }
        //   break;

        //case FbxNodeAttribute::eLight:
        //    { ParseLight(pNode); }
        //    break;
#endif
        }
    }

    auto count = pNode->GetChildCount();
    for(auto i=0; i<count; ++i)
    { ParseNode( pNode->GetChild(i), model); }
}

//-------------------------------------------------------------------------------------------------
//      メッシュを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseMesh(FbxNode* pNode, asdx::ResModel& model)
{
    auto pMesh = pNode->GetMesh();
    assert( pMesh != nullptr );

    MeshFBX dst;
    dst.Name = pNode->GetInitialName();

    asdx::ResMaterial mat;

    ParseVertex  ( pMesh, dst );
    ParseMaterial( pMesh, model );
    ParseSubset  ( pMesh, dst );

    for(size_t i=0; i<dst.Subsets.size(); ++i)
    {
        asdx::ResMesh mesh;
        mesh.MeshName = dst.Name;
        mesh.MaterialName = dst.Subsets[i].MaterialName;

        auto count = dst.Subsets[i].Count;
        mesh.Positions.resize(count);

        if (dst.Normals.size() > 0)
        { mesh.Normals.resize(count); }

        if (dst.Tangents.size() > 0)
        { mesh.Tangents.resize(count); }

        if (dst.BoneIndices.size() > 0)
        { mesh.BoneIndices.resize(count); }

        if (dst.BoneWeights.size() > 0)
        { mesh.BoneWeights.resize(count); }

        if (dst.Colors.size() > 0)
        { mesh.Colors.resize(count); }

        for(auto j=0; j<MAX_LAYER_COUNT; ++j)
        {
            if (dst.TexCoords[j].size() == 0)
            { continue; }

            mesh.TexCoords[j].resize(count);
        }

        mesh.Indices.resize(count);

        for(uint32_t j=0; j<count; ++j)
        {
            auto idx = dst.Subsets[i].Offset + j;
            auto vid = dst.Indices[idx];

            mesh.Indices[j] = vid;
            mesh.Positions[j] = dst.Positions[vid];

            if (dst.Normals.size() > 0)
            { mesh.Normals[j] = dst.Normals[vid]; }

            if (dst.Tangents.size() > 0)
            { mesh.Tangents[j] = dst.Tangents[vid]; }

            if (dst.BoneIndices.size() > 0)
            { mesh.BoneIndices[j] = dst.BoneIndices[vid]; }

            if (dst.BoneWeights.size() > 0)
            { mesh.BoneWeights[j] = dst.BoneWeights[vid]; }

            if (dst.Colors.size() > 0)
            { mesh.Colors[j] = dst.Colors[vid]; }

            for(auto k=0; k<MAX_LAYER_COUNT; ++k)
            {
                if (dst.TexCoords[k].size() == 0)
                { continue; }

                // V方向をフリップ.
                mesh.TexCoords[k][j].x = dst.TexCoords[k][j].x;
                mesh.TexCoords[k][j].y = 1.0f - dst.TexCoords[k][j].y;
            }
        }

        for(size_t j=0; j<count; j+=3)
        {
            auto i0 = mesh.Indices[j + 0];
            auto i1 = mesh.Indices[j + 1];
            auto i2 = mesh.Indices[j + 2];
            mesh.Indices[j + 0] = i0;
            mesh.Indices[j + 1] = i2;
            mesh.Indices[j + 2] = i1;
        }

        model.Meshes.emplace_back(mesh);
    }
}

//-------------------------------------------------------------------------------------------------
//      頂点データを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseVertex(FbxMesh* pSrcMesh, MeshFBX& dstMesh)
{
    auto pPosition  = pSrcMesh->GetControlPoints();
    auto pNormal    = pSrcMesh->GetElementNormal();
    auto pTangent   = pSrcMesh->GetElementTangent();
    auto pColors    = pSrcMesh->GetElementVertexColor();

    // 接線データが無ければ生成する.
    if (pTangent == nullptr)
    {
        pSrcMesh->GenerateTangentsData(0, true);
        pTangent = pSrcMesh->GetElementTangent();
        assert(pTangent != nullptr);
    }

    auto world = FromFbxMatrix(pSrcMesh->GetNode()->EvaluateGlobalTransform());

    FbxGeometryElementUV* pUV[4];
    auto layerCount = pSrcMesh->GetElementUVCount();

    const auto MaxLayerCount = 4;

    if ( layerCount > MaxLayerCount )
    {
        ILOG( "Info : Texture Layer Count Clamp %d ---> %d", layerCount, MaxLayerCount );
        layerCount = MaxLayerCount;
    }

    for( auto i=0; i<MaxLayerCount; ++i )
    { pUV[i] = pSrcMesh->GetElementUV(i); }

    // 面データのメモリを確保.
    std::vector<Face> faces;
    faces.resize( pSrcMesh->GetPolygonCount() );

    // 位置座標インデックスを格納.
    for( size_t i=0; i<faces.size(); ++i )
    {
        auto count = pSrcMesh->GetPolygonSize(int(i));
        assert( count == 3 );  // 事前に三角形しているの必ず3になっていることをチェック.

        for( auto j=0; j<count; ++j )
        {
            faces[i].IndexP[j] = pSrcMesh->GetPolygonVertex(int(i), j);
            faces[i].IndexN[j] = 0;
            faces[i].IndexT[j] = 0;

            for( auto k=0; k<MaxLayerCount; ++k )
            { faces[i].IndexU[k][j] = 0; }
        }
    }

    // 頂点カラーが存在する場合.
    if ( pColors != nullptr )
    {
        auto mapMode = pColors->GetMappingMode();
        auto refMode = pColors->GetReferenceMode();

        switch( mapMode )
        {
        case FbxGeometryElement::eByControlPoint:
            {
                std::vector<uint32_t> indices;
                indices.resize( pSrcMesh->GetControlPointsCount() );

                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(i); }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pColors->GetIndexArray();
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(container.GetAt(i)); }
                }

                for( size_t i=0; i<faces.size(); ++i )
                {
                    for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                    { faces[i].IndexC[j] = indices[ faces[i].IndexP[j] ]; }
                }

                indices.clear();
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int idx = 0;
                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexC[j] = static_cast<uint32_t>(idx);
                            idx++;
                        }
                    }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pColors->GetIndexArray();
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexC[j] = static_cast<uint32_t>(container.GetAt(idx));
                            idx++;
                        }
                    }
                }
            }
            break;
        }
    }

    // 法線ベクトルが存在する場合.
    if ( pNormal != nullptr )
    {
        auto mapMode = pNormal->GetMappingMode();
        auto refMode = pNormal->GetReferenceMode();

        switch( mapMode )
        {
        case FbxGeometryElement::eByControlPoint:
            {
                std::vector<uint32_t> indices;
                indices.resize( pSrcMesh->GetControlPointsCount() );

                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(i); }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pNormal->GetIndexArray();
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(container.GetAt(i)); }
                }

                for( size_t i=0; i<faces.size(); ++i )
                {
                    for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                    { faces[i].IndexN[j] = indices[ faces[i].IndexP[j] ]; }
                }

                indices.clear();
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int idx = 0;
                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexN[j] = static_cast<uint32_t>(idx);
                            idx++;
                        }
                    }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pNormal->GetIndexArray();
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexN[j] = static_cast<uint32_t>(container.GetAt(idx));
                            idx++;
                        }
                    }
                }
            }
            break;
        }
    }

    // 接線ベクトル
    if ( pTangent != nullptr )
    {
        auto mapMode = pTangent->GetMappingMode();
        auto refMode = pTangent->GetReferenceMode();

        switch( mapMode )
        {
        case FbxGeometryElement::eByControlPoint:
            {
                std::vector<uint32_t> indices;
                indices.resize( pSrcMesh->GetControlPointsCount() );

                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(i); }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pTangent->GetIndexArray();
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(container.GetAt(i)); }
                }

                for( size_t i=0; i<faces.size(); ++i )
                {
                    for( size_t j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                    { faces[i].IndexT[j] = indices[ faces[i].IndexP[j] ]; }
                }

                indices.clear();
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int idx = 0;
                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexT[j] = static_cast<uint32_t>(idx);
                            idx++;
                        }
                    }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pTangent->GetIndexArray();
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexT[j] = static_cast<uint32_t>(container.GetAt(idx));
                            idx++;
                        }
                    }
                }
            }
            break;

        default:
            ELOG("Unexpect case.");
            break;
        }
    }

    // テクスチャ座標.
    for( auto layer=0; layer<layerCount; ++layer )
    {
        if ( pUV[layer] == nullptr )
        { continue; }

        auto pTexCoord = pUV[layer];
        auto mapMode = pTexCoord->GetMappingMode();
        auto refMode = pTexCoord->GetReferenceMode();

        switch( mapMode )
        {
        case FbxGeometryElement::eByControlPoint:
            {
                std::vector<uint32_t> indices;
                indices.resize( pSrcMesh->GetControlPointsCount() );

                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( auto i=-0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(i); }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pTexCoord->GetIndexArray();
                    for( auto i=0; i<pSrcMesh->GetControlPointsCount(); ++i )
                    { indices[i] = static_cast<uint32_t>(container.GetAt(i)); }
                }

                for( size_t i=0; i<faces.size(); ++i )
                {
                    for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                    { faces[i].IndexU[layer][j] = indices[ faces[i].IndexP[j] ]; }
                }

                indices.clear();
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int idx = 0;

                if ( refMode == FbxGeometryElement::eDirect )
                {
                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexU[layer][j] = static_cast<uint32_t>(idx);
                            idx++;
                        }
                    }
                }
                else if ( refMode == FbxGeometryElement::eIndexToDirect )
                {
                    auto& container = pTexCoord->GetIndexArray();

                    for( size_t i=0; i<faces.size(); ++i )
                    {
                        for( auto j=0; j<pSrcMesh->GetPolygonSize(int(i)); ++j )
                        {
                            faces[i].IndexU[layer][j] = static_cast<uint32_t>(container.GetAt(idx));
                            idx++;
                        }
                    }
                }
            }
            break;
        }
    }

    // 頂点を組み立てる.
    {
        dstMesh.Positions.resize( faces.size() * 3 );
        if (pNormal != nullptr)
        { dstMesh.Normals  .resize( faces.size() * 3 ); }
        if (pTangent != nullptr)
        { dstMesh.Tangents.resize( faces.size() * 3 ); }
        for( auto i=0; i<layerCount; ++i )
        { dstMesh.TexCoords[i].resize( faces.size() * 3 ); }
        if (pColors != nullptr)
        { dstMesh.Colors.resize( faces.size() * 3 ); }

        auto worldT = asdx::Matrix::Transpose(world);

        uint32_t idx = 0;
        for( size_t i=0; i<faces.size(); ++i )
        {
            for( auto j=0; j<3; ++j )
            {
                auto& position = pPosition[ faces[i].IndexP[j] ];
                dstMesh.Positions[idx].x = static_cast<float>( position[0] );
                dstMesh.Positions[idx].y = static_cast<float>( position[1] );
                dstMesh.Positions[idx].z = static_cast<float>( position[2] );
                dstMesh.Positions[idx] = asdx::Vector3::TransformCoord(dstMesh.Positions[idx], world);

                if ( pColors != nullptr )
                {
                    const auto& color = pColors->GetDirectArray()[ faces[i].IndexC[j] ];
                    dstMesh.Colors[idx].x = static_cast<float>( color[0] );
                    dstMesh.Colors[idx].y = static_cast<float>( color[1] );
                    dstMesh.Colors[idx].z = static_cast<float>( color[2] );
                    dstMesh.Colors[idx].w = static_cast<float>( color[3] );
                }

                if ( pNormal != nullptr )
                {
                    const auto& normal = pNormal->GetDirectArray()[ faces[i].IndexN[j] ];
                    dstMesh.Normals[idx].x = static_cast<float>( normal[0] );
                    dstMesh.Normals[idx].y = static_cast<float>( normal[1] );
                    dstMesh.Normals[idx].z = static_cast<float>( normal[2] );
                    dstMesh.Normals[idx] = asdx::Vector3::TransformNormal(dstMesh.Normals[idx], worldT);
                    dstMesh.Normals[idx] = asdx::Vector3::SafeNormalize(dstMesh.Normals[idx], dstMesh.Normals[idx]);
                }

                if ( pTangent != nullptr )
                {
                    const auto& tangent = pTangent->GetDirectArray()[ faces[i].IndexT[j] ];
                    dstMesh.Tangents[idx].x = static_cast<float>( tangent[0] );
                    dstMesh.Tangents[idx].y = static_cast<float>( tangent[1] );
                    dstMesh.Tangents[idx].z = static_cast<float>( tangent[2] );
                    dstMesh.Tangents[idx] = asdx::Vector3::TransformNormal(dstMesh.Tangents[idx], worldT);
                    dstMesh.Tangents[idx] = asdx::Vector3::SafeNormalize(dstMesh.Tangents[idx], dstMesh.Tangents[idx]);
                }

                for( auto k=0; k<layerCount; ++k)
                {
                    if ( pUV[k] != nullptr )
                    {
                        const auto& texcoord = pUV[k]->GetDirectArray()[ faces[i].IndexU[k][j] ];
                        dstMesh.TexCoords[k][idx].x = static_cast<float>( texcoord[0] );
                        dstMesh.TexCoords[k][idx].y = static_cast<float>( texcoord[1] );
                    }
                }

                idx++;
            }
        }
    }

    // 頂点インデックス設定.
    {
        dstMesh.Indices.resize( faces.size() * 3 );
        for( size_t i=0; i<faces.size() * 3; ++i )
        { dstMesh.Indices[i] = static_cast<uint32_t>(i); }
    }

    // ボーンIDとボーンの重み設定.
    {
        dstMesh.BoneIndices.resize( faces.size() * 3 );
        dstMesh.BoneWeights.resize( faces.size() * 3 );

        for(size_t i=0; i<faces.size() * 3; ++i)
        {
            dstMesh.BoneIndices[i].x = 0;
            dstMesh.BoneIndices[i].y = 0;
            dstMesh.BoneIndices[i].z = 0;
            dstMesh.BoneIndices[i].w = 0;

            dstMesh.BoneWeights[i].x = -1.0f;
            dstMesh.BoneWeights[i].y = -1.0f;
            dstMesh.BoneWeights[i].z = -1.0f;
            dstMesh.BoneWeights[i].w = -1.0f;
        }

        auto deformerCount = pSrcMesh->GetDeformerCount();
        for(auto i=0; i<deformerCount; ++i)
        {
            auto pSkin = FbxCast<FbxSkin>(pSrcMesh->GetDeformer(i, FbxDeformer::eSkin));
            if (pSkin == nullptr)
            { continue; }

            auto clusterCount = pSkin->GetClusterCount();
            for(auto j=0; j<clusterCount; ++j)
            {
                auto pCluster = pSkin->GetCluster(j);
                if (pCluster == nullptr)
                { continue; }

                auto pointCount = pCluster->GetControlPointIndicesCount();
                auto indices = pCluster->GetControlPointIndices();
                auto weights = pCluster->GetControlPointWeights();


                for(auto k=0l; k<pointCount; ++k)
                {
                    auto idx = indices[k];
                    auto weight = float(weights[k]);

                    // 空いているところから順番に入れていく.
                    if (dstMesh.BoneWeights[idx].x < 0.0f)
                    {
                        dstMesh.BoneIndices[idx].x = j;
                        dstMesh.BoneWeights[idx].x = weight;
                    }
                    else if (dstMesh.BoneWeights[idx].y < 0.0f)
                    {
                        dstMesh.BoneIndices[idx].y = j;
                        dstMesh.BoneWeights[idx].y = weight;
                    }
                    else if (dstMesh.BoneWeights[idx].z < 0.0f)
                    {
                        dstMesh.BoneIndices[idx].z = j;
                        dstMesh.BoneWeights[idx].z = weight;
                    }
                    else if (dstMesh.BoneWeights[idx].w < 0.0f)
                    {
                        dstMesh.BoneIndices[idx].w = j;
                        dstMesh.BoneWeights[idx].w = weight;
                    }
                    else
                    {
                        // 空きが無ければ一番ウェイトが小さいところを置き換える.
                        auto miniIdx = 0;
                        auto miniWeight = dstMesh.BoneWeights[idx].x;

                        if (dstMesh.BoneIndices[idx].y < miniWeight)
                        {
                            miniWeight = dstMesh.BoneWeights[idx].y;
                            miniIdx    = 1;
                        }
                        if (dstMesh.BoneIndices[idx].z < miniWeight)
                        {
                            miniWeight = dstMesh.BoneWeights[idx].z;
                            miniIdx    = 2;
                        }
                        if (dstMesh.BoneIndices[idx].w < miniWeight)
                        {
                            miniWeight = dstMesh.BoneWeights[idx].w;
                            miniIdx    = 3;
                        }

                        if (miniWeight < weight)
                        {
                            switch(miniIdx)
                            {
                            case 0:
                                {
                                    dstMesh.BoneIndices[idx].x = j;
                                    dstMesh.BoneWeights[idx].x = weight;
                                }
                                break;

                            case 1:
                                {
                                    dstMesh.BoneIndices[idx].y = j;
                                    dstMesh.BoneWeights[idx].y = weight;
                                }
                                break;

                            case 2:
                                {
                                    dstMesh.BoneIndices[idx].z = j;
                                    dstMesh.BoneWeights[idx].z = weight;
                                }
                                break;

                            case 3:
                                {
                                    dstMesh.BoneIndices[idx].w = j;
                                    dstMesh.BoneWeights[idx].w = weight;
                                }
                                break;

                            }
                        }
                    }
                }

            }
        }
    }

    for(size_t i=0; i<dstMesh.BoneWeights.size(); ++i)
    {
        if (dstMesh.BoneWeights[i].x < 0.0f)
        { dstMesh.BoneWeights[i].x = 0.0f; }

        if (dstMesh.BoneWeights[i].y < 0.0f)
        { dstMesh.BoneWeights[i].y = 0.0f; }

        if (dstMesh.BoneWeights[i].z < 0.0f)
        { dstMesh.BoneWeights[i].z = 0.0f; }

        if (dstMesh.BoneWeights[i].w < 0.0f)
        { dstMesh.BoneWeights[i].w = 0.0f; }
    }

    faces.clear();
}

//-------------------------------------------------------------------------------------------------
//      マテリアルを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseMaterial(FbxMesh* pSrcMesh, asdx::ResModel& dstModel)
{
    auto count = pSrcMesh->GetNode()->GetMaterialCount();

    for(auto i=0; i<count; ++i)
    {
        auto pBase = pSrcMesh->GetNode()->GetMaterial(i);
        if (pBase == nullptr)
        { continue; }

        asdx::ResMaterial dstMaterial;
        dstMaterial.Name = pBase->GetName();

        // Lambert
        if (pBase->GetClassId().Is(FbxSurfaceLambert::ClassId))
        {
            auto pLambert = FbxCast<FbxSurfaceLambert>(pBase);
            AddTexture(pLambert, FbxSurfaceMaterial::sAmbient,   dstMaterial.Textures);
            AddTexture(pLambert, FbxSurfaceMaterial::sDiffuse,   dstMaterial.Textures);
            AddTexture(pLambert, FbxSurfaceMaterial::sEmissive,  dstMaterial.Textures);
            AddTexture(pLambert, FbxSurfaceMaterial::sNormalMap, dstMaterial.Textures);

        }
        else if (pBase->GetClassId().Is(FbxSurfacePhong::ClassId))
        {
            auto pPhong = FbxCast<FbxSurfacePhong>(pBase);
            AddTexture(pPhong, FbxSurfaceMaterial::sAmbient,   dstMaterial.Textures);
            AddTexture(pPhong, FbxSurfaceMaterial::sDiffuse,   dstMaterial.Textures);
            AddTexture(pPhong, FbxSurfaceMaterial::sSpecular,  dstMaterial.Textures);
            AddTexture(pPhong, FbxSurfaceMaterial::sShininess, dstMaterial.Textures);
            AddTexture(pPhong, FbxSurfaceMaterial::sEmissive,  dstMaterial.Textures);
            AddTexture(pPhong, FbxSurfaceMaterial::sNormalMap, dstMaterial.Textures);
        }

        dstMaterial.Textures.shrink_to_fit();

        dstModel.Materials.emplace_back(dstMaterial);
    }
}

//-------------------------------------------------------------------------------------------------
//      サブセットを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseSubset(FbxMesh* pSrcMesh, MeshFBX& dstMesh)
{
    FbxLayerElementArrayTemplate<int>* pIndices = nullptr;
    auto mappingMode = FbxGeometryElement::eNone;

    if ( pSrcMesh->GetElementMaterial() != nullptr )
    {
        pSrcMesh->GetMaterialIndices( &pIndices );
        mappingMode = pSrcMesh->GetElementMaterial()->GetMappingMode();
    }

    switch( mappingMode )
    {
    case FbxGeometryElement::eNone:
        { /* DO_NOTHING */ }
        break;

    case FbxGeometryElement::eAllSame:
        {
            dstMesh.Subsets.resize(1);

            dstMesh.Subsets[0].Offset     = 0;
            dstMesh.Subsets[0].Count      = pSrcMesh->GetPolygonCount() * 3;
            //dstMesh.Subsets[0].MaterialId = pIndices->GetAt(0);
            dstMesh.Subsets[0].MaterialName = pSrcMesh->GetNode()->GetMaterial(pIndices->GetAt(0))->GetName();
        }
        break;

    case FbxGeometryElement::eByPolygon:
        {
            int prevId = -1;
            int subset = -1;
            for(auto i=0; i<pSrcMesh->GetPolygonCount(); ++i)
            {
                auto id = pIndices->GetAt(i);

                if ( subset >= 0 )
                { dstMesh.Subsets[subset].Count += 3; }

                if ( prevId != id )
                {
                    SubsetFBX data;
                    data.Count      = 0;
                    data.Offset     = static_cast<uint32_t>(i * 3);
                    //data.MaterialId = static_cast<uint32_t>(id);
                    data.MaterialName = pSrcMesh->GetNode()->GetMaterial(id)->GetName();

                    dstMesh.Subsets.emplace_back( data );

                    prevId = id;
                    subset++;
                }
            }
        }
        break;
    }
}

#if 0
//-------------------------------------------------------------------------------------------------
//      スケルトンを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseSkeleton(FbxNode* pNode)
{
    auto pSkeleton = pNode->GetSkeleton();
    assert(pSkeleton != nullptr);

    FbxMatrix transform    = m_pScene->GetAnimationEvaluator()->GetNodeLocalTransform(pNode);
    FbxMatrix invTransform = m_pScene->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode).Inverse();

    ResBone dst;
    dst.Name         = pNode->GetInitialName();
    dst.IsRoot       = pSkeleton->IsSkeletonRoot();
    dst.Length       = float(pSkeleton->GetLimbLengthDefaultValue());
    dst.Transform    = FromFbxMatrix(transform);
    dst.InvTransform = FromFbxMatrix(invTransform);

    m_Bones.push_back(dst);
}

//-------------------------------------------------------------------------------------------------
//      モーションを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseMotion(FbxAnimStack* pSrcStack, ResMotion& dstMotion)
{
    FbxString stackName = pSrcStack->GetName();
    dstMotion.Name = stackName;

    auto takeInfo = m_pScene->GetTakeInfo(stackName);
    assert( takeInfo != nullptr );

    // ゲーム用途に合わせて60FPSとしておく.
    auto timeUnit = FbxTime::eFrames60;

    auto start = takeInfo->mLocalTimeSpan.GetStart().GetFrameCount(timeUnit);
    auto end   = takeInfo->mLocalTimeSpan.GetStop() .GetFrameCount(timeUnit);

    dstMotion.Duration = static_cast<uint32_t>(end - start + 1);

    // 各ボーン分だけ回す.
    for(size_t i=0; i<m_Bones.size(); ++i)
    {
        // 名前からリンクノード名を取得.
        auto pLinkNode = m_pScene->FindNodeByName(FbxString(m_Bones[i].Name.c_str()));
        if ( pLinkNode == nullptr )
        {
            ILOG("Info : Not Found. name = %s", m_Bones[i].Name.c_str());
            continue;
        }

        ResBoneMotion boneMotion;
        boneMotion.BoneName = m_Bones[i].Name;
        boneMotion.KeyFrames.reserve(dstMotion.Duration);

        FbxAMatrix prevTransform;
        prevTransform.SetIdentity();

        // キーフレームを設定.
        for(auto idx=start; idx<=end; ++idx)
        {
            FbxTime curTime;
            curTime.SetFrame(idx, timeUnit);

            // 変換行列を取得.
            auto transform = pLinkNode->EvaluateGlobalTransform(curTime);

            // 同一変換行列であれば設定しない.
            if (prevTransform != transform)
            {
                auto t = transform.GetT();  // 平行移動.
                auto s = transform.GetS();  // スケール.
                auto q = transform.GetQ();  // 回転.

                ResKeyPose key;
                key.FrameIndex  = static_cast<uint32_t>(idx);
                key.Translation = asdx::Vector3(t[0], t[1], t[2]);
                key.Scale       = asdx::Vector3(s[0], s[1], s[2]);
                key.Rotation    = asdx::Quaternion(q[0], q[1], q[2], q[3]);

            #if 0
                // 行列で取りたい場合はこちら.
                //key.Transform  = FromFbxMatrix(transform);
            #endif

                boneMotion.KeyFrames.push_back(key);
                prevTransform = transform;
            }
        }

        // メモリ最適化.
        boneMotion.KeyFrames.shrink_to_fit();

        // 格納.
        dstMotion.Bones.push_back(boneMotion);
    }
}


//-------------------------------------------------------------------------------------------------
//      カメラを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseCamera(FbxNode* pNode)
{
    auto pCamera = pNode->GetCamera();
    assert(pCamera != nullptr);

    ResCamera dst;
    dst.Position            = FromFbxProperty3(pCamera->Position);
    dst.Target              = FromFbxProperty3(pCamera->InterestPosition);
    dst.Upward              = FromFbxProperty3(pCamera->UpVector);
    dst.FieldOfView         = float(pCamera->FieldOfView.Get());
    dst.NearClip            = float(pCamera->NearPlane.Get());
    dst.FarClip             = float(pCamera->FarPlane.Get());
    dst.OrthoZoom           = float(pCamera->OrthoZoom.Get());
    dst.ApertureWidth       = float(pCamera->GetApertureWidth());
    dst.ApertureHeight      = float(pCamera->GetApertureHeight());
    dst.FilmWidth           = float(pCamera->FilmWidth.Get());
    dst.FilmHeight          = float(pCamera->FilmHeight.Get());
    dst.FilmAspectRatio     = float(pCamera->FilmAspectRatio.Get());
    dst.FilmSqueezeRatio    = float(pCamera->FilmSqueezeRatio.Get());
    dst.FocalLength         = float(pCamera->FocalLength.Get());

    switch(pCamera->ProjectionType.Get())
    {
    case FbxCamera::ePerspective:
        { dst.ProjectionType = PROJECTION_TYPE_PERSPECTIVE; }
        break;

    case FbxCamera::eOrthogonal:
        { dst.ProjectionType = PROJECTION_TYPE_ORTHO; }
        break;
    }

    m_Cameras.push_back(dst);
}

//-------------------------------------------------------------------------------------------------
//      ライトを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseLight(FbxNode* pNode)
{
    auto pLight = pNode->GetLight();
    assert(pLight != nullptr);

    ResLight dst;
    dst.Color = FromFbxProperty3(pLight->Color);
    dst.Intensity = float(pLight->Intensity.Get());
    dst.InnerAngle = float(pLight->InnerAngle.Get());
    dst.OuterAngle = float(pLight->OuterAngle.Get());

    switch(pLight->LightType.Get())
    {
    case FbxLight::ePoint:
        { dst.Type = LIGHT_TYPE_POINT; }
        break;

    case FbxLight::eDirectional:
        { dst.Type = LIGHT_TYPE_DIRECTIONAL; }
        break;

    case FbxLight::eSpot:
        { dst.Type = LIGHT_TYPE_SPOT; }
        break;

    case FbxLight::eArea:
        { dst.Type = LIGHT_TYPE_AREA; }
        break;
    }

    switch(pLight->AreaLightShape.Get())
    {
    case FbxLight::eRectangle:
        { dst.AreaLightShape = LIGHT_SHAPE_RECTANGLE; }
        break;

    case FbxLight::eSphere:
        { dst.AreaLightShape = LIGHT_SHAPE_SPHERE; }
        break;
    }

    m_Lights.push_back(dst);
}

#endif

#else   // FBX無効時.

//-------------------------------------------------------------------------------------------------
//      読み込み処理です.
//-------------------------------------------------------------------------------------------------
bool FBXLoader::Load(const char*, asdx::ResModel&, bool)
{
    ELOGA("Error : FBX Not Supported.");
    return false;
}

// 空関数として実装.
void FBXLoader::Term()                                                  { /* DO_NOTHING */ }
void FBXLoader::ParseContent   ( asdx::ResModel& )                      { /* DO_NOTHING */ }
void FBXLoader::ParseNode      ( fbxsdk::FbxNode*, asdx::ResModel& )    { /* DO_NOTHING */ }
void FBXLoader::ParseMesh      ( fbxsdk::FbxNode*, asdx::ResModel& )    { /* DO_NOTHING */ }
void FBXLoader::ParseVertex    ( fbxsdk::FbxMesh*, MeshFBX& )           { /* DO_NOTHING */ }
void FBXLoader::ParseSubset    ( fbxsdk::FbxMesh*, MeshFBX& )           { /* DO_NOTHING */ }
void FBXLoader::ParseMaterial  ( fbxsdk::FbxMesh*, asdx::ResModel& )    { /* DO_NOTHING */ }

#endif//ENABLE_FBX
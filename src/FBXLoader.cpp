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
#include <algorithm>
#include <tuple>

#if ENABLE_FBX
#include <fbxsdk.h>
#endif

namespace {

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
    uint32_t IndexU[ MaxLayerCount ][ 3 ];  // テクスチャ座標インデックス.
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
void AddTexture(FbxSurfaceMaterial* pMaterial, const char* propName, std::vector<std::string>& textures)
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
            textures.push_back(pFileTexture->GetRelativeFileName()); // UTF-8で格納.
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
                textures.push_back(pFileTexture->GetRelativeFileName()); // UTF-8で格納.
            }
        }
    }
}

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

//-------------------------------------------------------------------------------------------------
//      読み込み処理です.
//-------------------------------------------------------------------------------------------------
bool FBXLoader::Load(const char* path, bool skipTriangulate)
{
    if (path == nullptr)
    {
        ELOG("Error : FBXLoader::Init() Failed. path = %s", path);
        return false;
    }

    m_ErrorString.clear();

    // ファイルパス.
    m_Path = path;

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
            ELOG("Error : FbxImpoter::Initialize() Failed. path = %s", filePath);
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

    // 単位を[m]に変換.
    {
        auto unit = m_pScene->GetGlobalSettings().GetSystemUnit();

        if (unit.GetScaleFactor() != 1.0f)
        { FbxSystemUnit::m.ConvertScene(m_pScene); }
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
    ParseContent();

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

    m_Meshes.clear();
    m_Meshes.shrink_to_fit();

    m_Bones.clear();
    m_Bones.shrink_to_fit();

    m_Motions.clear();
    m_Motions.shrink_to_fit();

    m_Cameras.clear();
    m_Cameras.shrink_to_fit();

    m_Lights.clear();
    m_Lights.shrink_to_fit();

    m_Materials.clear();
    m_Materials.shrink_to_fit();

    m_NullNodes.clear();
    m_NullNodes.shrink_to_fit();

    m_FolderPath.clear();
}

//-------------------------------------------------------------------------------------------------
//      コンテンツを解析します
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseContent()
{
    // ルートノード取得.
    FbxNode* pRootNode = m_pScene->GetRootNode();
    if (pRootNode == nullptr)
    {
        ELOG("Error : RootNode is null.");
        return;
    }

    // カウントを求めます.
    Counters counters;
    ParseCount(pRootNode, counters);

    m_Meshes .reserve( counters.mesh );
    m_Bones  .reserve( counters.skeleton );
    m_Cameras.reserve( counters.camera );
    m_Lights .reserve( counters.light );

    // マテリアル数を取得.
    auto materialCount = m_pScene->GetMaterialCount();
    m_Materials.resize(materialCount);

    // マテリアルを解析.
    for(auto i=0; i<materialCount; ++i)
    {
        auto mat = m_pScene->GetMaterial(i);
        ParseMaterial(mat, m_Materials[i]);
    }

    // ノードを解析.
    std::string parent = "";
    ParseNode(pRootNode, parent);
}

//-------------------------------------------------------------------------------------------------
//      ノードを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseNode(FbxNode* pNode, const std::string& parent)
{
    if (pNode == nullptr)
    {
        ELOG("Error : Node is null.");
        return;
    }

    std::string parentName = pNode->GetInitialName();
    if (parentName.find("spl_") != 0)
    { parentName = parent; }

    if (pNode->GetNodeAttribute() == nullptr)
    {
        ParseNull(pNode);
    }
    else
    {
        auto type = pNode->GetNodeAttribute()->GetAttributeType();
        switch(type)
        {
        case FbxNodeAttribute::eMesh:
            { ParseMesh(pNode, parentName); }
            break;

        case FbxNodeAttribute::eSkeleton:
           { ParseSkeleton(pNode); }
           break;

        case FbxNodeAttribute::eCamera:
           { ParseCamera(pNode); }
           break;

        case FbxNodeAttribute::eLight:
            { ParseLight(pNode); }
            break;

        case FbxNodeAttribute::eNull:
            { ParseNull(pNode); }
            break;
        }
    }

    auto count = pNode->GetChildCount();
    for(auto i=0; i<count; ++i)
    { ParseNode( pNode->GetChild(i), parentName); }
}

//-----------------------------------------------------------------------------
//      アトリビュート無しノードを解析します.
//-----------------------------------------------------------------------------
void FBXLoader::ParseNull(FbxNode* pNode)
{
    auto T = pNode->LclTranslation.Get();
    auto S = pNode->LclScaling.Get();
    auto R = pNode->LclRotation.Get();

    ResNullNodeFBX dst = {};
    dst.Name        = pNode->GetInitialName();
    dst.Translation = asdx::Vector3(float(T[0]), float(T[1]), float(T[2]));
    dst.Scale       = asdx::Vector3(float(S[0]), float(S[1]), float(S[2]));
    dst.Rotation    = asdx::Vector3(
        asdx::ToRadian(float(R[0])),
        asdx::ToRadian(float(R[1])),
        asdx::ToRadian(float(R[2])));

    m_NullNodes.push_back(dst);
}

//-------------------------------------------------------------------------------------------------
//      メッシュを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseMesh(FbxNode* pNode, const std::string& parentNode)
{
    auto pMesh = pNode->GetMesh();
    assert( pMesh != nullptr );

    ResMeshFBX dst;
    dst.ParentNode = parentNode;
    dst.Name       = pNode->GetInitialName();

    ParseVertex( pMesh, dst );
    ParseSubset( pMesh, dst );

    m_Meshes.push_back(dst);
}

//-------------------------------------------------------------------------------------------------
//      スケルトンを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseSkeleton(FbxNode* pNode)
{
    auto pSkeleton = pNode->GetSkeleton();
    assert(pSkeleton != nullptr);

    {
        FbxMatrix transform    = m_pScene->GetAnimationEvaluator()->GetNodeLocalTransform(pNode);
        FbxMatrix invTransform = m_pScene->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode).Inverse();
    
        ResBoneFBX dst;
        dst.Name         = pNode->GetInitialName();
        dst.IsRoot       = pSkeleton->IsSkeletonRoot();
        dst.Length       = float(pSkeleton->GetLimbLengthDefaultValue());
        dst.Transform    = FromFbxMatrix(transform);
        dst.InvTransform = FromFbxMatrix(invTransform);

        m_Bones.push_back(dst);
    }
}

//-------------------------------------------------------------------------------------------------
//      カメラを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseCamera(FbxNode* pNode)
{
    auto pCamera = pNode->GetCamera();
    assert(pCamera != nullptr);

    ResCameraFBX dst;
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

    ResLightFBX dst;
    dst.Color      = FromFbxProperty3(pLight->Color);
    dst.Intensity  = float(pLight->Intensity.Get());
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

//-------------------------------------------------------------------------------------------------
//      頂点データを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseVertex(FbxMesh* pSrcMesh, ResMeshFBX& dstMesh)
{
    auto pPosition  = pSrcMesh->GetControlPoints();
    auto pNormal    = pSrcMesh->GetElementNormal();
    auto pTangent   = pSrcMesh->GetElementTangent();
    auto pBinormal  = pSrcMesh->GetElementBinormal();
    auto pColors    = pSrcMesh->GetElementVertexColor();

    auto world = FromFbxMatrix(pSrcMesh->GetNode()->EvaluateGlobalTransform());

    FbxGeometryElementUV* pUV[MaxLayerCount];
    auto layerCount = pSrcMesh->GetElementUVCount();

    if ( layerCount > MaxLayerCount )
    {
        ILOG( "Info : Texture Layer Count Clamp %d ---> %d", layerCount, MaxLayerCount );
        layerCount = MaxLayerCount;
    }

    for( auto i=0; i<MaxLayerCount; ++i )
    { pUV[i] = pSrcMesh->GetElementUV(i); }

    // 接線データが無ければ生成する.
    if (pTangent == nullptr && layerCount > 0)
    {
        pSrcMesh->GenerateTangentsData(0, true);
        pTangent = pSrcMesh->GetElementTangent();
        assert(pTangent != nullptr);
    }

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
            auto indexP = pSrcMesh->GetPolygonVertex(int(i), j);
            if (indexP < 0)
            {
                m_ErrorString += "Invalid Data Index. Polygon Vertex Index (";
                m_ErrorString += std::to_string(int(i));
                m_ErrorString += " , ";
                m_ErrorString += std::to_string(j);
                m_ErrorString += "), Mesh name = ";
                m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                m_ErrorString += ", UniqueID = ";
                m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                m_ErrorString += "\n";
                indexP = 0;
            }
            faces[i].IndexP[j] = uint32_t(indexP);
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
                    {
                        auto indexC = container.GetAt(i);
                        if (indexC < 0)
                        {
                            m_ErrorString += "Invalid Data Index. VertexColor index (";
                            m_ErrorString += std::to_string(i);
                            m_ErrorString += "), Mesh name = ";
                            m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                            m_ErrorString += ", UniqueID = ";
                            m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                            m_ErrorString += "\n";
                            indexC = 0;
                        }
                        indices[i] = static_cast<uint32_t>(indexC);
                    }
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
                            auto indexC = container.GetAt(idx);
                            if (indexC < 0)
                            {
                                m_ErrorString += "Invalid Data Index. VertexColor index (";
                                m_ErrorString += std::to_string(idx);
                                m_ErrorString += "), Mesh name = ";
                                m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                                m_ErrorString += ", UniqueID = ";
                                m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                                m_ErrorString += "\n";
                                indexC = 0;
                            }

                            faces[i].IndexC[j] = uint32_t(indexC);
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
                    {
                        auto indexN = container.GetAt(i);
                        if (indexN < 0)
                        {
                            m_ErrorString += "Invalid Data Index. Normal index (";
                            m_ErrorString += std::to_string(i);
                            m_ErrorString += "), Mesh name = ";
                            m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                            m_ErrorString += ", UniqueID = ";
                            m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                            m_ErrorString += "\n";
                            indexN = 0;
                        }

                        indices[i] = uint32_t(indexN);
                    }
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
                            auto indexN = container.GetAt(idx);
                            if (indexN < 0)
                            {
                                m_ErrorString += "Invalid Data Index. Normal index (";
                                m_ErrorString += std::to_string(i);
                                m_ErrorString += "), Mesh name = ";
                                m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                                m_ErrorString += ", UniqueID = ";
                                m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                                m_ErrorString += "\n";
                                indexN = 0;
                            }

                            faces[i].IndexN[j] = uint32_t(indexN);
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
                    {
                        auto indexT = container.GetAt(i);
                        if (indexT < 0)
                        {
                            m_ErrorString += "Invalid Data Index. Tangent index (";
                            m_ErrorString += std::to_string(i);
                            m_ErrorString += "), Mesh name = ";
                            m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                            m_ErrorString += ", UniqueID = ";
                            m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                            m_ErrorString += "\n";
                            indexT = 0;
                        }

                        indices[i] = uint32_t(indexT);
                    }
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
                            auto indexT = container.GetAt(idx);
                            if (indexT < 0)
                            {
                                m_ErrorString += "Invalid Data Index. Tangent index (";
                                m_ErrorString += std::to_string(idx);
                                m_ErrorString += "), Mesh name = ";
                                m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                                m_ErrorString += ", UniqueID = ";
                                m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                                m_ErrorString += "\n";
                                indexT = 0;
                            }
                            faces[i].IndexT[j] = uint32_t(indexT);
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
                    {
                        auto indexU = container.GetAt(i);
                        if (indexU < 0)
                        {
                            m_ErrorString += "Invalid Data Index. TexCoord(";
                            m_ErrorString += std::to_string(layer);
                            m_ErrorString += ") index (";
                            m_ErrorString += std::to_string(i);
                            m_ErrorString += "), Mesh name = ";
                            m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                            m_ErrorString += ", UniqueID = ";
                            m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                            m_ErrorString += "\n";
                            indexU = 0;
                        }
                        indices[i] = uint32_t(indexU);
                    }
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
                            auto indexU = container.GetAt(idx);
                            if (indexU < 0)
                            {
                                m_ErrorString += "Invalid Data Index. TexCoord(";
                                m_ErrorString += std::to_string(layer);
                                m_ErrorString += ") index (";
                                m_ErrorString += std::to_string(idx);
                                m_ErrorString += "), Mesh name = ";
                                m_ErrorString += pSrcMesh->GetNode()->GetInitialName();
                                m_ErrorString += ", UniqueID = ";
                                m_ErrorString += std::to_string(pSrcMesh->GetUniqueID());
                                m_ErrorString += "\n";
                                indexU = 0;
                            }
                            faces[i].IndexU[layer][j] = uint32_t(indexU);
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
                    dstMesh.Normals[idx].x = static_cast<float>( normal[0] * normal[3] );
                    dstMesh.Normals[idx].y = static_cast<float>( normal[1] * normal[3] );
                    dstMesh.Normals[idx].z = static_cast<float>( normal[2] * normal[3] );
                    dstMesh.Normals[idx] = asdx::Vector3::TransformNormal(dstMesh.Normals[idx], world);
                    dstMesh.Normals[idx] = asdx::Vector3::SafeNormalize(dstMesh.Normals[idx], dstMesh.Normals[idx]);
                }

                if ( pTangent != nullptr )
                {
                    const auto& tangent = pTangent->GetDirectArray()[ faces[i].IndexT[j] ];
                    asdx::Vector3 T;
                    T.x = static_cast<float>( tangent[0] * tangent[3] );
                    T.y = static_cast<float>( tangent[1] * tangent[3] );
                    T.z = static_cast<float>( tangent[2] * tangent[3] );
                    T = asdx::Vector3::TransformNormal(T, world);
                    T = asdx::Vector3::SafeNormalize(T, T);
                    dstMesh.Tangents[idx].x = T.x;
                    dstMesh.Tangents[idx].y = T.y;
                    dstMesh.Tangents[idx].z = T.z;
                    dstMesh.Tangents[idx].w = 1.0f;
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
        for( size_t i=0; i<faces.size() * 3; i+=3 )
        {
            dstMesh.Indices[i + 0] = static_cast<uint32_t>(i + 0);
            dstMesh.Indices[i + 1] = static_cast<uint32_t>(i + 1);
            dstMesh.Indices[i + 2] = static_cast<uint32_t>(i + 2);
        }
    }

    // ボーンIDとボーンの重み設定.
    {
        dstMesh.BoneIds.resize( faces.size() * 3 );
        dstMesh.BoneWeights.resize( faces.size() * 3 );

        std::vector<ResBoneIdFBX>   boneIndices;
        std::vector<asdx::Vector4>  boneWeights;

        auto point_count = pSrcMesh->GetControlPointsCount();
        boneIndices.resize(point_count);
        boneWeights.resize(point_count);

        auto deformerCount = pSrcMesh->GetDeformerCount(FbxDeformer::eSkin);
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

                auto count   = pCluster->GetControlPointIndicesCount();
                auto indices = pCluster->GetControlPointIndices();
                auto weights = pCluster->GetControlPointWeights();

                if (count == 0)
                { continue; }

                // ここのカウントは pSrcMesh->GetControlPointsCount() と同じになる.
                for(auto k=0l; k<count; ++k)
                {
                    auto id  = indices[k];
                    auto w   = float(weights[k]);

                    // 空いているところから順番に入れていく.
                    if (boneIndices[id].x < 0)
                    {
                        boneIndices[id].x = j;
                        boneWeights[id].x = w;
                    }
                    else if (boneIndices[id].y < 0)
                    {
                        boneIndices[id].y = j;
                        boneWeights[id].y = w;
                    }
                    else if (boneIndices[id].z < 0)
                    {
                        boneIndices[id].z = j;
                        boneWeights[id].z = w;
                    }
                    else if (boneIndices[id].w < 0)
                    {
                        boneIndices[id].w = j;
                        boneWeights[id].w = w;
                    }
                    else
                    {
                        // 空きが無ければ一番ウェイトが小さいところを置き換える.
                        auto miniIndex  = 0;
                        auto miniWeight = boneWeights[id].x;

                        if (boneIndices[id].y < miniWeight)
                        {
                            miniWeight = boneWeights[id].y;
                            miniIndex  = 1;
                        }
                        if (boneIndices[id].z < miniWeight)
                        {
                            miniWeight = boneWeights[id].z;
                            miniIndex  = 2;
                        }
                        if (boneIndices[id].w < miniWeight)
                        {
                            miniWeight = boneWeights[id].w;
                            miniIndex  = 3;
                        }

                        if (miniWeight < w)
                        {
                            switch(miniIndex)
                            {
                            case 0:
                                {
                                    boneIndices[id].x = j;
                                    boneWeights[id].x = w;
                                }
                                break;

                            case 1:
                                {
                                    boneIndices[id].y = j;
                                    boneWeights[id].y = w;
                                }
                                break;

                            case 2:
                                {
                                    boneIndices[id].z = j;
                                    boneWeights[id].z = w;
                                }
                                break;

                            case 3:
                                {
                                    boneIndices[id].w = j;
                                    boneWeights[id].w = w;
                                }
                                break;

                            }
                        }

                    } // end else
                } // end for
            } // end for
        }

        // 全部ボーンをなめたら最終的なデータを入れる.
        auto idx = 0;
        for(size_t i=0; i<faces.size(); ++i)
        {
            for(auto j=0; j<3; ++j)
            {
                auto bi = boneIndices[ faces[i].IndexP[j] ];
                auto bw = boneWeights[ faces[i].IndexP[j] ];

                if (bi.x < 0)
                {
                    bi.x = 0;
                    bw.x = 0.0f;
                }
                if (bi.y < 0)
                {
                    bi.y = 0;
                    bw.y = 0.0f;
                }
                if (bi.z < 0)
                {
                    bi.z = 0;
                    bw.z = 0.0f;
                }
                if (bi.w < 0)
                {
                    bi.w = 0;
                    bw.w = 0.0f;
                }

                auto total_weight = 0.0f;
                total_weight += bw.x;
                total_weight += bw.y;
                total_weight += bw.z;
                total_weight += bw.w;

                dstMesh.BoneIds[idx].x     = bi.x;
                dstMesh.BoneWeights[idx].x = bw.x / total_weight;

                dstMesh.BoneIds[idx].y     = bi.y;
                dstMesh.BoneWeights[idx].y = bw.y / total_weight;

                dstMesh.BoneIds[idx].z     = bi.z;
                dstMesh.BoneWeights[idx].z = bw.z / total_weight;

                dstMesh.BoneIds[idx].w     = bi.w;
                dstMesh.BoneWeights[idx].w = bw.w / total_weight;

                idx++;
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
void FBXLoader::ParseMaterial(FbxSurfaceMaterial* pSrcMaterial, ResMaterialFBX& dstMaterial)
{
    dstMaterial.Name = pSrcMaterial->GetName();

    if (pSrcMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
    {
        auto pLambert = FbxCast<FbxSurfaceLambert>(pSrcMaterial);
        AddTexture(pLambert, FbxSurfaceMaterial::sAmbient,   dstMaterial.Textures);
        AddTexture(pLambert, FbxSurfaceMaterial::sDiffuse,   dstMaterial.Textures);
        AddTexture(pLambert, FbxSurfaceMaterial::sEmissive,  dstMaterial.Textures);
        AddTexture(pLambert, FbxSurfaceMaterial::sNormalMap, dstMaterial.Textures);
    }
    else if (pSrcMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
    {
        auto pPhong = FbxCast<FbxSurfacePhong>(pSrcMaterial);
        AddTexture(pPhong, FbxSurfaceMaterial::sAmbient,    dstMaterial.Textures);
        AddTexture(pPhong, FbxSurfaceMaterial::sDiffuse,    dstMaterial.Textures);
        AddTexture(pPhong, FbxSurfaceMaterial::sSpecular,   dstMaterial.Textures);
        AddTexture(pPhong, FbxSurfaceMaterial::sShininess,  dstMaterial.Textures);
        AddTexture(pPhong, FbxSurfaceMaterial::sEmissive,   dstMaterial.Textures);
        AddTexture(pPhong, FbxSurfaceMaterial::sNormalMap,  dstMaterial.Textures);
    }

    dstMaterial.Textures.shrink_to_fit();
}

//-------------------------------------------------------------------------------------------------
//      サブセットを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseSubset(FbxMesh* pSrcMesh, ResMeshFBX& dstMesh)
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
            auto id         = pIndices->GetAt(0);
            auto material   = pSrcMesh->GetNode()->GetMaterial(id);
            auto index      = -1;
            
            for(int i=0; i<int(m_Materials.size()); ++i)
            {
                if (strcmp(material->GetName(), m_Materials[i].Name.c_str()) == 0)
                {
                    index = i;
                    break;
                }
            }

            assert(index >= 0);

            dstMesh.Subsets.resize(1);

            dstMesh.Subsets[0].Offset     = 0;
            dstMesh.Subsets[0].Count      = pSrcMesh->GetPolygonCount() * 3;
            dstMesh.Subsets[0].MaterialId = index;
        }
        break;

    case FbxGeometryElement::eByPolygon:
        {
            int prevId = -1;
            int subset = -1;
            for(auto i=0; i<pSrcMesh->GetPolygonCount(); ++i)
            {
                auto id         = pIndices->GetAt(i);
                auto material   = pSrcMesh->GetNode()->GetMaterial(id);
                auto index      = -1;

                for(int i=0; i<int(m_Materials.size()); ++i)
                {
                    if (strcmp(material->GetName(), m_Materials[i].Name.c_str()) == 0)
                    {
                        index = i;
                        break;
                    }
                }

                assert(index >= 0);

                if ( prevId != index )
                {
                    ResSubsetFBX data;
                    data.Count      = 0;
                    data.Offset     = uint32_t(i * 3);
                    data.MaterialId = index;

                    dstMesh.Subsets.push_back( data );

                    prevId = index;
                    subset++;
                }

                if ( subset >= 0 )
                { dstMesh.Subsets[subset].Count += 3; }
            }
        }
        break;
    }
}

//-------------------------------------------------------------------------------------------------
//      モーションを解析します.
//-------------------------------------------------------------------------------------------------
void FBXLoader::ParseMotion(FbxAnimStack* pSrcStack, ResMotionFBX& dstMotion)
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

        ResBoneMotionFBX boneMotion;
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

                ResKeyPoseFBX key;
                key.FrameIndex  = static_cast<uint32_t>(idx);
                key.Translation = asdx::Vector3(float(t[0]), float(t[1]), float(t[2]));
                key.Scale       = asdx::Vector3(float(s[0]), float(s[1]), float(s[2]));
                key.Rotation    = asdx::Quaternion(float(q[0]), float(q[1]), float(q[2]), float(q[3]));

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

bool FBXLoader::Load(const char* path, asdx::ResModel& model)
{
    if (!Load(path))
    {
        return false;
    }

    model.Materials.resize(m_Materials.size());
    for(size_t i=0; i<m_Materials.size(); ++i)
    {
        auto& srcMat = m_Materials[i];
        auto& dstMat = model.Materials[i];

        dstMat.Name = srcMat.Name;
    }

    auto meshId = 0u;

    for(size_t i=0; i<m_Meshes.size(); ++i)
    {
        std::sort(m_Meshes[i].Subsets.begin(), m_Meshes[i].Subsets.end(),
            [](const ResSubsetFBX& lhs, const ResSubsetFBX& rhs)
            {
                return std::tie(lhs.MaterialId, lhs.Offset) < std::tie(rhs.MaterialId, rhs.Offset);
            });

        auto hasNormal      = m_Meshes[i].Normals.size() > 0;
        auto hasTangent     = m_Meshes[i].Tangents.size() > 0;
        auto hasUV0         = m_Meshes[i].TexCoords[0].size() > 0;
        auto hasUV1         = m_Meshes[i].TexCoords[1].size() > 0;
        auto hasUV2         = m_Meshes[i].TexCoords[2].size() > 0;
        auto hasUV3         = m_Meshes[i].TexCoords[3].size() > 0;
        auto hasBoneId      = m_Meshes[i].BoneIds.size() > 0;
        auto hasBoneWeight  = m_Meshes[i].BoneWeights.size() > 0;
        auto hasColor       = m_Meshes[i].Colors.size() > 0;

        auto materialId = -1;
        auto index = 0;

        auto dstMesh = asdx::ResMesh();

        for(size_t j=0; j<m_Meshes[i].Subsets.size(); ++j)
        {
            auto& subset = m_Meshes[i].Subsets[j];
            auto count = subset.Count;

            if (materialId != subset.MaterialId)
            {
                if (materialId >= 0)
                {
                    if (!hasNormal)
                    { asdx::CalcNormals(dstMesh); }

                    if (!hasTangent)
                    { asdx::CalcTangents(dstMesh); }

                    model.Meshes.emplace_back(dstMesh);
                    dstMesh = asdx::ResMesh();
                }

                dstMesh.MeshName     = "mesh";
                dstMesh.MeshName     += std::to_string(meshId);
                dstMesh.MaterialName = m_Materials[subset.MaterialId].Name;

                materialId = subset.MaterialId;

                index = 0;
                meshId++;
            }

            for(size_t k=0; k<count; ++k)
            {
                auto idx = k + subset.Offset;

                dstMesh.Positions.push_back(m_Meshes[i].Positions[idx]);

                if (hasColor)
                { dstMesh.Colors.push_back(m_Meshes[i].Colors[idx]); }

                if (hasNormal)
                { dstMesh.Normals.push_back(m_Meshes[i].Normals[idx]); }

                if (hasTangent)
                {
                    dstMesh.Tangents.push_back(asdx::Vector3(
                        m_Meshes[i].Tangents[idx].x,
                        m_Meshes[i].Tangents[idx].y,
                        m_Meshes[i].Tangents[idx].z));
                }

                if (hasUV0)
                { dstMesh.TexCoords[0].push_back(m_Meshes[i].TexCoords[0][idx]); }

                if (hasUV1)
                { dstMesh.TexCoords[1].push_back(m_Meshes[i].TexCoords[1][idx]); }

                if (hasUV2)
                { dstMesh.TexCoords[2].push_back(m_Meshes[i].TexCoords[2][idx]); }

                if (hasUV3)
                { dstMesh.TexCoords[3].push_back(m_Meshes[i].TexCoords[3][idx]); }

                if (hasBoneId)
                {
                    dstMesh.BoneIndices.push_back(asdx::ResBoneIndex(
                        uint16_t(m_Meshes[i].BoneIds[idx].x),
                        uint16_t(m_Meshes[i].BoneIds[idx].y),
                        uint16_t(m_Meshes[i].BoneIds[idx].z),
                        uint16_t(m_Meshes[i].BoneIds[idx].w)));
                }

                if (hasBoneWeight)
                { dstMesh.BoneWeights.push_back(m_Meshes[i].BoneWeights[idx]); }

                dstMesh.Indices.push_back(index);
                index++;
            }
        }

        if (materialId > 0)
        {
            if (!hasNormal)
            { asdx::CalcNormals(dstMesh); }

            if (!hasTangent)
            { asdx::CalcTangents(dstMesh); }

            model.Meshes.emplace_back(dstMesh);
        }
    }

    return true;
}
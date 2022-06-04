//-----------------------------------------------------------------------------
// File : FBXLoader.h
// Desc : Flim Box Loader.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxResModel.h>
#include <asdxResMaterial.h>

//-----------------------------------------------------------------------------
// Forward Declarations.
//-----------------------------------------------------------------------------
namespace fbxsdk {
class FbxManager;
class FbxImporter;
class FbxScene;
class FbxNode;
class FbxMesh;
} // namesapce fbxsdk


///////////////////////////////////////////////////////////////////////////////
// SubsetFBX structure
///////////////////////////////////////////////////////////////////////////////
struct SubsetFBX
{
    std::string MaterialName;
    uint32_t    Offset;
    uint32_t    Count;
};

///////////////////////////////////////////////////////////////////////////////
// MeshFBX structure
///////////////////////////////////////////////////////////////////////////////
struct MeshFBX
{
    std::string                     Name;
    std::vector<asdx::Vector3>      Positions;
    std::vector<asdx::Vector3>      Normals;
    std::vector<asdx::Vector3>      Tangents;
    std::vector<asdx::Vector2>      TexCoords[MAX_LAYER_COUNT];
    std::vector<asdx::ResBoneIndex> BoneIndices;
    std::vector<asdx::Vector4>      BoneWeights;
    std::vector<asdx::Vector4>      Colors;
    std::vector<SubsetFBX>          Subsets;
    std::vector<uint32_t>           Indices;
};

///////////////////////////////////////////////////////////////////////////////
// FBXLoader class
///////////////////////////////////////////////////////////////////////////////
class FBXLoader
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです。
    //-------------------------------------------------------------------------
    FBXLoader();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~FBXLoader();

    //-------------------------------------------------------------------------
    //! @brief      モデルをロードします.
    //-------------------------------------------------------------------------
    bool Load(const char* path, asdx::ResModel& model, bool skipTriangulate = false);

    //-------------------------------------------------------------------------
    //! @brief
    //-------------------------------------------------------------------------
    const char* GetFolderPath() const { return m_FolderPath.c_str(); }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    fbxsdk::FbxManager*     m_pMgr      = nullptr;
    fbxsdk::FbxImporter*    m_pImporter = nullptr;
    fbxsdk::FbxScene*       m_pScene    = nullptr;
    std::string             m_FolderPath;

    //=========================================================================
    // private methods.
    //=========================================================================
    void Term           ();
    void ParseContent   ( asdx::ResModel& model );
    void ParseNode      ( fbxsdk::FbxNode* pNode, asdx::ResModel& model );
    void ParseMesh      ( fbxsdk::FbxNode* pNode, asdx::ResModel& model );
    void ParseVertex    ( fbxsdk::FbxMesh* pSrcMesh, MeshFBX& dstMesh );
    void ParseSubset    ( fbxsdk::FbxMesh* pSrcMesh, MeshFBX& dstMesh );
    void ParseMaterial  ( fbxsdk::FbxMesh* pSrcMesh, asdx::ResModel& dstModel );

#if 0
    //void ParseSkeleton  ( FbxNode* pNode );
    //void ParseCamera    ( FbxNode* pNode );
    //void ParseLight     ( FbxNode* pNode );
#endif
};

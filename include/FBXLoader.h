//-------------------------------------------------------------------------------------------------
// File : FBXLoader.h
// Desc : Flim Box Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once


//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <asdxResModel.h>
#include <asdxResMaterial.h>

namespace fbxsdk {
class FbxManager;
class FbxImporter;
class FbxScene;
class FbxNode;
class FbxMesh;
} // namesapce fbxsdk

////-------------------------------------------------------------------------------------------------
//// Constant Values.
////-------------------------------------------------------------------------------------------------
//constexpr int MaxLayerCount = 4;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//// PROJECTION_TYPE
/////////////////////////////////////////////////////////////////////////////////////////////////////
//enum PROJECTION_TYPE
//{
//    PROJECTION_TYPE_PERSPECTIVE,
//    PROJECTION_TYPE_ORTHO,
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// LIGHT_TYPE 
/////////////////////////////////////////////////////////////////////////////////////////////////////
//enum LIGHT_TYPE
//{
//    LIGHT_TYPE_POINT,
//    LIGHT_TYPE_DIRECTIONAL,
//    LIGHT_TYPE_SPOT,
//    LIGHT_TYPE_AREA
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// LIGHT_SHAPE
/////////////////////////////////////////////////////////////////////////////////////////////////////
//enum LIGHT_SHAPE
//{
//    LIGHT_SHAPE_RECTANGLE,
//    LIGHT_SHAPE_SPHERE,
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResMaterial structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResMaterial
//{
//    std::string                 Name;
//    std::vector<std::string>    Textures;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResSubset structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResSubset
//{
//    int         MaterialId;
//    uint32_t    Offset;
//    uint32_t    Count;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResBone structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResBone
//{
//    std::string     Name;
//    bool            IsRoot;
//    float           Length;
//    asdx::Matrix    Transform;
//    asdx::Matrix    InvTransform;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// BoneId structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResBoneId
//{
//    int     x = -1;
//    int     y = -1;
//    int     z = -1;
//    int     w = -1;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResMesh structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResMesh
//{
//    std::string                     Name;
//    std::vector<asdx::Vector3>      Positions;
//    std::vector<asdx::Vector3>      Normals;
//    std::vector<asdx::Vector3>      Tangents;
//    std::vector<asdx::Vector2>      TexCoords[MaxLayerCount];
//    std::vector<ResBoneId>          BoneIds;
//    std::vector<asdx::Vector4>      BoneWeights;
//    std::vector<asdx::Vector4>      Colors;
//    std::vector<ResSubset>          Subsets;
//    std::vector<uint32_t>           Indices;
//    std::vector<ResMaterial>        Materials;
//};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// KeyPose structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResKeyPose
//{
//    uint32_t            FrameIndex;
//    asdx::Vector3       Translation;
//    asdx::Vector3       Scale;
//    asdx::Quaternion    Rotation;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// KeyPose structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResBoneMotion
//{
//    std::string             BoneName;
//    std::vector<ResKeyPose> KeyFrames;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// Motion structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResMotion
//{
//    std::string                 Name;
//    uint32_t                    Duration;
//    std::vector<ResBoneMotion>  Bones;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResCamera structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResCamera
//{
//    asdx::Vector3   Position;
//    asdx::Vector3   Target;
//    asdx::Vector3   Upward;
//    float           FieldOfView;
//    float           NearClip;
//    float           FarClip;
//    float           OrthoZoom;
//    PROJECTION_TYPE ProjectionType;
//    float           ApertureWidth;
//    float           ApertureHeight;
//    float           FilmWidth;
//    float           FilmHeight;
//    float           FilmAspectRatio;
//    float           FilmSqueezeRatio;
//    float           FocalLength;
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// ResLight structure
/////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ResLight
//{
//    LIGHT_TYPE      Type;
//    asdx::Vector3   Color;
//    float           Intensity;
//    float           InnerAngle;
//    float           OuterAngle;
//    LIGHT_SHAPE     AreaLightShape;
//};


///////////////////////////////////////////////////////////////////////////////////////////////////
// FBXLoader class
///////////////////////////////////////////////////////////////////////////////////////////////////
class FBXLoader
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================
    FBXLoader();
    ~FBXLoader();

    bool Load(const char* path, asdx::ResModel& model, asdx::ResMaterialSet& materialSet, bool skipTriangulate = false);
    void Term();

    const char* GetFolderPath() const { return m_FolderPath.c_str(); }

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    fbxsdk::FbxManager*                 m_pMgr      = nullptr;
    fbxsdk::FbxImporter*                m_pImporter = nullptr;
    fbxsdk::FbxScene*                   m_pScene    = nullptr;
    std::string                 m_FolderPath;

    //=============================================================================================
    // private methods.
    //=============================================================================================
    void ParseContent   ();
    void ParseNode      ( fbxsdk::FbxNode* pNode );
    void ParseMesh      ( fbxsdk::FbxNode* pNode );
    //void ParseSkeleton  ( FbxNode* pNode );
    //void ParseCamera    ( FbxNode* pNode );
    //void ParseLight     ( FbxNode* pNode );

    void ParseVertex    ( fbxsdk::FbxMesh* pSrcMesh, asdx::ResMesh& dstMesh );
    void ParseMaterial  ( fbxsdk::FbxMesh* pSrcMesh, asdx::ResMesh& dstMesh );
    void ParseSubset    ( fbxsdk::FbxMesh* pSrcMesh, asdx::ResMesh& dstMesh );
};

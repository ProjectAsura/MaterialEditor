//-------------------------------------------------------------------------------------------------
// File : FBXLoader.h
// Desc : Flim Box Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once


//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <fbxsdk.h>
#include <asdxMath.h>
#include <asdxResModel.h>


//-------------------------------------------------------------------------------------------------
// Constant Values.
//-------------------------------------------------------------------------------------------------
constexpr int MaxLayerCount = 4;


///////////////////////////////////////////////////////////////////////////////////////////////////
// PROJECTION_TYPE
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PROJECTION_TYPE
{
    PROJECTION_TYPE_PERSPECTIVE,
    PROJECTION_TYPE_ORTHO,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT_TYPE 
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LIGHT_TYPE
{
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_AREA
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT_SHAPE
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LIGHT_SHAPE
{
    LIGHT_SHAPE_RECTANGLE,
    LIGHT_SHAPE_SPHERE,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResMaterialFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResMaterialFBX
{
    std::string                 Name;
    std::vector<std::string>    Textures;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResSubsetFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResSubsetFBX
{
    int         MaterialId;
    uint32_t    Offset;
    uint32_t    Count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResBoneFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResBoneFBX
{
    std::string     Name;
    bool            IsRoot;
    float           Length;
    asdx::Matrix    Transform;
    asdx::Matrix    InvTransform;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResBoneIdFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResBoneIdFBX
{
    int     x = -1;
    int     y = -1;
    int     z = -1;
    int     w = -1;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResMeshFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResMeshFBX
{
    std::string                     ParentNode;
    std::string                     Name;
    std::vector<asdx::Vector3>      Positions;
    std::vector<asdx::Vector3>      Normals;
    std::vector<asdx::Vector4>      Tangents;       // tangent.x, tangent.y, tangent.z, binormal_sign
    std::vector<asdx::Vector2>      TexCoords[MaxLayerCount];
    std::vector<ResBoneIdFBX>       BoneIds;
    std::vector<asdx::Vector4>      BoneWeights;
    std::vector<asdx::Vector4>      Colors;
    std::vector<ResSubsetFBX>       Subsets;
    std::vector<uint32_t>           Indices;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResKeyPoseFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResKeyPoseFBX
{
    uint32_t            FrameIndex;
    asdx::Vector3       Translation;
    asdx::Vector3       Scale;
    asdx::Quaternion    Rotation;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResBoneMotionFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResBoneMotionFBX
{
    std::string                 BoneName;
    std::vector<ResKeyPoseFBX>  KeyFrames;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResMotionFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResMotionFBX
{
    std::string                     Name;
    uint32_t                        Duration;
    std::vector<ResBoneMotionFBX>   Bones;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResCameraFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResCameraFBX
{
    asdx::Vector3   Position;
    asdx::Vector3   Target;
    asdx::Vector3   Upward;
    float           FieldOfView;
    float           NearClip;
    float           FarClip;
    float           OrthoZoom;
    PROJECTION_TYPE ProjectionType;
    float           ApertureWidth;
    float           ApertureHeight;
    float           FilmWidth;
    float           FilmHeight;
    float           FilmAspectRatio;
    float           FilmSqueezeRatio;
    float           FocalLength;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResLightFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResLightFBX
{
    LIGHT_TYPE      Type;
    asdx::Vector3   Color;
    float           Intensity;
    float           InnerAngle;
    float           OuterAngle;
    LIGHT_SHAPE     AreaLightShape;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResNullNodeFBX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResNullNodeFBX
{
    std::string     Name;
    asdx::Vector3   Scale;
    asdx::Vector3   Rotation;
    asdx::Vector3   Translation;
};


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

    bool Load(const char* path, bool skipTriangulate = false);
    void Term();

    const std::vector<ResMeshFBX>&     GetMeshes   () const { return m_Meshes;    }
    const std::vector<ResMotionFBX>&   GetMotions  () const { return m_Motions;   }
    const std::vector<ResBoneFBX>&     GetBones    () const { return m_Bones;     }
    const std::vector<ResLightFBX>&    GetLights   () const { return m_Lights;    }
    const std::vector<ResCameraFBX>&   GetCameras  () const { return m_Cameras;   }
    const std::vector<ResMaterialFBX>& GetMaterials() const { return m_Materials; }
    const std::vector<ResNullNodeFBX>& GetNullNodes() const { return m_NullNodes; }
    const char* GetFolderPath   () const { return m_FolderPath.c_str(); }
    const char* GetPath         () const { return m_Path.c_str(); }
    bool        HasError        () const { return m_ErrorString.empty() == false; }
    const char* GetErrorString  () const { return m_ErrorString.c_str(); }

    bool Load(const char* path, asdx::ResModel& model);

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    FbxManager*                 m_pMgr      = nullptr;
    FbxImporter*                m_pImporter = nullptr;
    FbxScene*                   m_pScene    = nullptr;
    std::vector<ResMeshFBX>     m_Meshes;
    std::vector<ResBoneFBX>     m_Bones;
    std::vector<ResMotionFBX>   m_Motions;
    std::vector<ResCameraFBX>   m_Cameras;
    std::vector<ResLightFBX>    m_Lights;
    std::vector<ResMaterialFBX> m_Materials;
    std::vector<ResNullNodeFBX> m_NullNodes;
    std::string                 m_FolderPath;
    std::string                 m_Path;
    std::string                 m_ErrorString;

    //=============================================================================================
    // private methods.
    //=============================================================================================
    void ParseContent   ();
    void ParseNode      ( FbxNode* pNode, const std::string& parentName );
    void ParseMesh      ( FbxNode* pNode, const std::string& parentName );
    void ParseSkeleton  ( FbxNode* pNode );
    void ParseCamera    ( FbxNode* pNode );
    void ParseLight     ( FbxNode* pNode );
    void ParseNull      ( FbxNode* pNode );

    void ParseVertex    ( FbxMesh* pSrcMesh, ResMeshFBX& dstMesh );
    void ParseSubset    ( FbxMesh* pSrcMesh, ResMeshFBX& dstMesh );
    void ParseMotion    ( FbxAnimStack* pSrcStack, ResMotionFBX& dstMotion );
    void ParseMaterial  ( FbxSurfaceMaterial* pSrcMaterial, ResMaterialFBX& dstMaterial );

};
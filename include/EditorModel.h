﻿//-----------------------------------------------------------------------------
// File : EditorModel.h
// Desc : Model For Mateiral Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <string>
#include <asdxMath.h>
#include <asdxResModel.h>
#include <asdxVertexBuffer.h>
#include <asdxIndexBuffer.h>


//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const uint32_t kMaxInstanceCount = 32;   // 最大32.


///////////////////////////////////////////////////////////////////////////////
// BoundingBox structure
///////////////////////////////////////////////////////////////////////////////
struct BoundingBox
{
    asdx::Vector3   mini;
    asdx::Vector3   maxi;
};


///////////////////////////////////////////////////////////////////////////////
// EditorMesh class
///////////////////////////////////////////////////////////////////////////////
class EditorMesh
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
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    EditorMesh();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~EditorMesh();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(ID3D11Device* pDevice, const asdx::ResMesh& mesh);

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      メッシュ名を取得します.
    //-------------------------------------------------------------------------
    const std::string& GetMeshName() const;

    //-------------------------------------------------------------------------
    //! @brief      マテリアル名を取得します.
    //-------------------------------------------------------------------------
    const std::string& GetMaterialName() const;

    //-------------------------------------------------------------------------
    //! @brief      描画処理を行います.
    //-------------------------------------------------------------------------
    void Draw(ID3D11DeviceContext* pContext) const;

    //-------------------------------------------------------------------------
    //! @brief      スキニングデータを持つかどうか?
    //-------------------------------------------------------------------------
    bool HasSkinningData() const;

    //-------------------------------------------------------------------------
    //! @brief      バウンディングボックスを取得します.
    //-------------------------------------------------------------------------
    const BoundingBox& GetBox() const;

    //-------------------------------------------------------------------------
    //! @brief      ポリゴン数を取得します.
    //-------------------------------------------------------------------------
    uint32_t GetPolygonCount() const;

    void SetInstanceCount(uint32_t count);

    uint32_t GetInstanceCount() const;

    const asdx::Matrix& GetInstanceMatrix(uint32_t index) const;

    void SetInstanceMatrix(uint32_t index, const asdx::Matrix& matrix);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::string                 m_MeshName;
    std::string                 m_MaterialName;
    BoundingBox                 m_Box;
    bool                        m_HasSkinningData;
    uint32_t                    m_IndexCount;
    asdx::VertexBuffer          m_VB;
    asdx::VertexBuffer          m_SkinVB;
    asdx::IndexBuffer           m_IB;
    uint32_t                    m_InstanceCount;
    asdx::Matrix                m_InstanceMatrix[kMaxInstanceCount];

    asdx::RefPtr<ID3D11Buffer>              m_InstanceMatrixBuffer;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_InstanceMatrixSRV;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};

///////////////////////////////////////////////////////////////////////////////
// EditorModel class
///////////////////////////////////////////////////////////////////////////////
class EditorModel
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variable.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    EditorModel();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~EditorModel();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //!
    //! @param[in]      path        ファイルパスです.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool Init(const char* model);

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      メッシュ数を取得します.
    //!
    //! @return     メッシュ数を返却します.
    //-------------------------------------------------------------------------
    uint32_t GetMeshCount() const;

    //-------------------------------------------------------------------------
    //! @brief      メッシュを取得します.
    //!
    //! @param[in]      index       メッシュ番号.
    //! @return     メッシュを返却します.
    //-------------------------------------------------------------------------
    const EditorMesh& GetMesh(uint32_t index) const;

    //-------------------------------------------------------------------------
    //! @brief      バウンディングボックスを取得します.
    //!
    //! @return     バウンディングボックスを返却します.
    //-------------------------------------------------------------------------
    const BoundingBox& GetBox() const;

    //-------------------------------------------------------------------------
    //! @brief      ファイルパスを取得します.
    //!
    //! @return     ファイルパスを返却します.
    //-------------------------------------------------------------------------
    const std::string& GetPath() const;

    //-------------------------------------------------------------------------
    //! @brief      ワールド行列を取得します.
    //!
    //! @return     ワールド行列を返却します.
    //-------------------------------------------------------------------------
    const asdx::Matrix& GetWorld() const;

    //-------------------------------------------------------------------------
    //! @brief      スケールを設定します.
    //-------------------------------------------------------------------------
    void SetScale(const asdx::Vector3& scale);

    //-------------------------------------------------------------------------
    //! @brief      回転角を設定します(度単位).
    //-------------------------------------------------------------------------
    void SetRotation(const asdx::Vector3& rotate);

    //-------------------------------------------------------------------------
    //! @brief      平行移動量を設定します.
    //-------------------------------------------------------------------------
    void SetTranslation(const asdx::Vector3& translation);

    //-------------------------------------------------------------------------
    //! @brief      スケールを取得します.
    //-------------------------------------------------------------------------
    const asdx::Vector3& GetScale() const;

    //-------------------------------------------------------------------------
    //! @brief      回転角を取得します(度単位).
    //-------------------------------------------------------------------------
    const asdx::Vector3& GetRotation() const;

    //-------------------------------------------------------------------------
    //! @brief      平行移動量を取得します.
    //-------------------------------------------------------------------------
    const asdx::Vector3& GetTranslation() const;

    //-------------------------------------------------------------------------
    //! @brief      ポリゴン数を計算します.
    //-------------------------------------------------------------------------
    uint64_t CalcPolygonCount() const;

    //-------------------------------------------------------------------------
    //! @brief      ワールド行列を更新します.
    //-------------------------------------------------------------------------
    void UpdateWorld();


private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::string             m_Path;         //!< ファイルパス.
    asdx::Vector3           m_Scale;        //!< スケール.
    asdx::Vector3           m_Rotation;     //!< 回転角(度単位).
    asdx::Vector3           m_Translation;  //!< 平行移動量.
    asdx::Matrix            m_World;        //!< ワールド行列.
    BoundingBox             m_Box;          //!< バウンディングボックス.
    std::vector<EditorMesh> m_Meshes;       //!< メッシュ.
    bool                    m_DirtyWorld;   //!< ダーティフラグ.

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};

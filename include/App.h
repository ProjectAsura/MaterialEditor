﻿//-----------------------------------------------------------------------------
// File : App.h
// Desc : Material Editor Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxCameraUtil.h>
#include <asdxConstantBuffer.h>
#include <asdxVertexBuffer.h>
#include <asdxShader.h>
#include <asdxRenderState.h>
#include <WorkSpace.h>
#include <Config.h>
#include <DebugPrimitive.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////
class App : public asdx::Application
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
    App();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~App();

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::VertexShader                  m_EditorVS;
    asdx::VertexShader                  m_EditorSkinningVS;
    asdx::VertexShader                  m_ShadowVS;
    asdx::VertexShader                  m_ShadowSkinningVS;
    asdx::VertexShader                  m_TriangleVS;
    asdx::VertexShader                  m_GuideVS;
    asdx::VertexShader                  m_ShapeVS;
    asdx::PixelShader                   m_DefaultPS;
    asdx::PixelShader                   m_CopyPS;
    asdx::PixelShader                   m_OETFPS;
    asdx::PixelShader                   m_GuidePS;
    asdx::PixelShader                   m_ShapePS;
    asdx::PixelShader                   m_CompositePS;

    asdx::ConstantBuffer                m_SceneCB;
    asdx::ConstantBuffer                m_GuideCB;
    asdx::ConstantBuffer                m_LightCB;
    asdx::ConstantBuffer                m_MeshCB;
    asdx::ConstantBuffer                m_OETFCB;

    asdx::VertexBuffer                  m_AxisVB;
    asdx::VertexBuffer                  m_GridVB;
    asdx::VertexBuffer                  m_TriangleVB;

    uint32_t                            m_AxisVertexCount = 6;
    uint32_t                            m_GridVertexCount = 0;
    asdx::CameraUpdater                 m_CameraController;
    WorkSpace                           m_WorkSpace;
    Config                              m_Config;
    bool                                m_CameraControl = false;
    asdx::Matrix                        m_Proj = asdx::Matrix::CreateIdentity();

    asdx::ColorTarget2D                 m_DiffuseBuffer;
    asdx::ColorTarget2D                 m_SpecularBuffer;
    asdx::ColorTarget2D                 m_NRMBuffer;
    asdx::ColorTarget2D                 m_DummyColorBuffer;
    asdx::ColorTarget2D                 m_LightingBuffer;
    asdx::DepthTarget2D                 m_DepthBuffer;
    asdx::DepthTarget2D                 m_ShadowBuffer;
    bool                                m_ReloadShader  = false; // リロード要求.
    bool                                m_PrevLoadState = false; // 前フレームのロードフラグ.
    float                               m_LoadingPos    = 0.0f;
    float                               m_AutoRotationY = 0.0f;
    int                                 m_GuizmoOperation = -1;
    ArrowShape                          m_ArrowShape;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      初期化時の処理です.
    //-------------------------------------------------------------------------
    bool OnInit() override;

    //-------------------------------------------------------------------------
    //! @brief      終了時の処理です.
    //-------------------------------------------------------------------------
    void OnTerm() override;

    //-------------------------------------------------------------------------
    //! @brief      フレーム遷移処理です.
    //-------------------------------------------------------------------------
    void OnFrameMove(asdx::FrameEventArgs& args) override;

    //-------------------------------------------------------------------------
    //! @brief      フレーム描画処理です.
    //-------------------------------------------------------------------------
    void OnFrameRender(asdx::FrameEventArgs& args) override;

    //-------------------------------------------------------------------------
    //! @brief      リサイズ時の処理です.
    //-------------------------------------------------------------------------
    void OnResize(const asdx::ResizeEventArgs& args) override;

    //-------------------------------------------------------------------------
    //! @brief      キー処理です.
    //-------------------------------------------------------------------------
    void OnKey(const asdx::KeyEventArgs& args) override;

    //-------------------------------------------------------------------------
    //! @brief      マウス処理です.
    //-------------------------------------------------------------------------
    void OnMouse(const asdx::MouseEventArgs& args) override;

    //-------------------------------------------------------------------------
    //! @brief      タイピング処理です.
    //-------------------------------------------------------------------------
    void OnTyping(uint32_t keyCode) override;

    //-------------------------------------------------------------------------
    //! @brief      ファイルドロップ時の処理です.
    //-------------------------------------------------------------------------
    void OnDrop(const std::vector<std::string>& dropFiles) override;

    //-------------------------------------------------------------------------
    //! @brief      GUIを描画します.
    //-------------------------------------------------------------------------
    void DrawGui();

    //-------------------------------------------------------------------------
    //! @brief      3D描画を行います.
    //-------------------------------------------------------------------------
    void Draw3D();

    //-------------------------------------------------------------------------
    //! @brief      モデルの描画を行います.
    //-------------------------------------------------------------------------
    void DrawModel(bool lightingPass, asdx::BlendType blendType);

    //-------------------------------------------------------------------------
    //! @brief      ガイドオブジェクトを描画します.
    //-------------------------------------------------------------------------
    void DrawGuide();

    //-------------------------------------------------------------------------
    //! @brief      フルスクリーン矩形を描画します.
    //-------------------------------------------------------------------------
    void DrawQuad();

    //-------------------------------------------------------------------------
    //! @brief      シャドウマップを描画します.
    //-------------------------------------------------------------------------
    void DrawShadowMap();

};
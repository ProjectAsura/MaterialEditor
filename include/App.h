//-----------------------------------------------------------------------------
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

    //-------------------------------------------------------------------------
    //! @brief      メッシュを出力します.
    //-------------------------------------------------------------------------
    bool ExportMesh(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      マテリアルを出力します.
    //-------------------------------------------------------------------------
    bool ExportMaterial(const char* path);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::RefPtr<ID3D11VertexShader>    m_VS;
    asdx::RefPtr<ID3D11VertexShader>    m_SkinningVS;
    asdx::RefPtr<ID3D11VertexShader>    m_TriangleVS;
    asdx::RefPtr<ID3D11PixelShader>     m_DefaultPS;
    asdx::RefPtr<ID3D11PixelShader>     m_CopyPS;
    asdx::RefPtr<ID3D11InputLayout>     m_IL;
    asdx::RefPtr<ID3D11InputLayout>     m_SkinningIL;
    asdx::RefPtr<ID3D11InputLayout>     m_TriangleIL;
    asdx::ConstantBuffer                m_SceneCB;
    asdx::ConstantBuffer                m_GuideCB;
    asdx::ConstantBuffer                m_LightCB;
    asdx::ConstantBuffer                m_MeshCB;
    asdx::VertexBuffer                  m_AxisVB;
    asdx::VertexBuffer                  m_GridVB;
    asdx::VertexBuffer                  m_TriangleVB;
    asdx::VertexShader                  m_GuideVS;
    asdx::PixelShader                   m_GuidePS;
    uint32_t                            m_AxisVertexCount = 6;
    uint32_t                            m_GridVertexCount = 0;
    asdx::CameraUpdater                 m_CameraController;
    WorkSpace                           m_WorkSpace;
    Config                              m_Config;
    bool                                m_CameraControl = false;
    asdx::Matrix                        m_Proj = asdx::Matrix::CreateIdentity();
    asdx::ColorTarget2D                 m_LightingBuffer;
    asdx::ColorTarget2D                 m_NRMBuffer;
    asdx::ColorTarget2D                 m_DummyColorBuffer;
    asdx::DepthTarget2D                 m_DepthBuffer;
    asdx::DepthTarget2D                 m_ShadowBuffer;

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

    void DrawGuide();

    void DrawQuad();
};
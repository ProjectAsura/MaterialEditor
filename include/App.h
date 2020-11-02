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
#include <WorkSpace.h>


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
    WorkSpace                           m_WorkSpace;

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
    void OnDrop(const wchar_t** dropFiles, uint32_t fileCount) override;

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
    void DrawModel();
};
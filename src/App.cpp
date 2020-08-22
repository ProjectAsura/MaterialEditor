//-----------------------------------------------------------------------------
// File : App.cpp
// Desc : Material Editor Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <asdxGuiMgr.h>
#include <asdxLogger.h>
#include <asdxCmdHelper.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
App::App()
: asdx::Application(L"MaterailEditor", 1920, 1080, nullptr, nullptr, nullptr)
{
    m_SwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
    m_ClearColor[0] = 0.18f;
    m_ClearColor[1] = 0.18f;
    m_ClearColor[2] = 0.18f;
    m_ClearColor[3] = 1.0f;
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
App::~App()
{
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool App::OnInit()
{
    m_pGraphicsQueue = asdx::GfxDevice().GetGraphicsQueue();

    const char* fontPath = "../res/fonts/07やさしさゴシック.ttf";
    if (!asdx::GuiMgr::Instance().Init(
        m_hWnd,
        m_Width,
        m_Height,
        m_SwapChainFormat,
        fontPath))
    {
        ELOGA("Error : GuiMgr::Init() Failed.");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void App::OnTerm()
{

    asdx::GuiMgr::Instance().Term();
}

//-----------------------------------------------------------------------------
//      描画時の処理です.
//-----------------------------------------------------------------------------
void App::OnFrameRender(asdx::FrameEventArgs& args)
{
    auto idx  = GetCurrentBackBufferIndex();
    auto pCmd = m_GfxCmdList.Reset();

    asdx::GfxDevice().SetUploadCommand(pCmd);

    asdx::BarrierTransition(
        pCmd,
        m_ColorTarget[idx].GetResource(),
        0,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    auto pRTV = m_ColorTarget[idx].GetRTV();
    auto pDSV = m_DepthTarget.GetDSV();

    asdx::ClearRTV(pCmd, pRTV, m_ClearColor);
    asdx::ClearDSV(pCmd, pDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

    asdx::SetRenderTarget(pCmd, pRTV, pDSV);
    asdx::SetViewport(pCmd, m_ColorTarget[idx].GetResource());

    {
        //DrawShadow(pCmd);

        //DrawBackground(pCmd);

        //DrawForwardLighting(pCmd);

        // GUI描画.
        DrawGui(pCmd);
    }

    asdx::BarrierTransition(
        pCmd,
        m_ColorTarget[idx].GetResource(),
        0,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    pCmd->Close();

    ID3D12CommandList* pCmds[] = {
        pCmd
    };

    // 前フレームの描画の完了を待機.
    m_pGraphicsQueue->WaitIdle();

    // コマンドを実行.
    m_pGraphicsQueue->Execute(1, pCmds);

    // 画面に表示.
    Present(0);

    // フレーム同期.
    asdx::GfxDevice().FrameSync();
}

//-----------------------------------------------------------------------------
//      リサイズ時の処理です.
//-----------------------------------------------------------------------------
void App::OnResize(const asdx::ResizeEventArgs& args)
{
}

//-----------------------------------------------------------------------------
//      キー処理です.
//-----------------------------------------------------------------------------
void App::OnKey(const asdx::KeyEventArgs& args)
{
    asdx::GuiMgr::Instance().OnKey(args.IsKeyDown, args.IsKeyDown, args.KeyCode);
}

//-----------------------------------------------------------------------------
//      マウス処理です.
//-----------------------------------------------------------------------------
void App::OnMouse(const asdx::MouseEventArgs& args)
{
    asdx::GuiMgr::Instance().OnMouse(
        args.X,
        args.Y,
        args.WheelDelta,
        args.IsLeftButtonDown,
        args.IsMiddleButtonDown,
        args.IsRightButtonDown);
}

//-----------------------------------------------------------------------------
//      タイピング処理です.
//-----------------------------------------------------------------------------
void App::OnTyping(uint32_t keyCode)
{
    asdx::GuiMgr::Instance().OnTyping(keyCode);
}

//-----------------------------------------------------------------------------
//      ドロップ時の処理です.
//-----------------------------------------------------------------------------
void App::OnDrop(const wchar_t** dropFiles, uint32_t fileCount)
{
}


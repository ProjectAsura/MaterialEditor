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
#include <asdxDeviceContext.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
#include "../res/shaders/Compiled/EditorVS.inc"
#include "../res/shaders/Compiled/EditorSkinningVS.inc"


//-----------------------------------------------------------------------------
// Global Varaibles.
//-----------------------------------------------------------------------------
D3D11_INPUT_ELEMENT_DESC kElements[] = {
    { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",         0, DXGI_FORMAT_R8G8B8A8_UNORM,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_SPACE", 0, DXGI_FORMAT_R32_UINT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC kSkinningElements[] = {
    { "BONE_INDEX",  0, DXGI_FORMAT_R16G16B16A16_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

///////////////////////////////////////////////////////////////////////////////
// SceneBuffer structure
///////////////////////////////////////////////////////////////////////////////
struct SceneBuffer
{
    asdx::Matrix    View;           // ビュー行列.
    asdx::Matrix    Proj;           // 射影行列.
    asdx::Matrix    InvView;        // ビュー行列の逆行列.
    asdx::Matrix    InvProj;        // 射影行列の逆行列.
    asdx::Vector3   CameraPos;      // カメラ位置.
    float           Timer;          // グローバルタイマー.
    float           NearClip;       // ニア平面.
    float           FarClip;        // ファー平面.
    asdx::Vector2   UVToView;       // UVからビュー空間への変換パラメータ.
};

} // namespace


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
    const char* fontPath = "../res/fonts/07やさしさゴシック.ttf";
    if (!asdx::GuiMgr::GetInstance().Init(
        m_pDevice,
        m_pDeviceContext,
        m_hWnd,
        m_Width,
        m_Height,
        fontPath))
    {
        ELOGA("Error : GuiMgr::Init() Failed.");
        return false;
    }

    // 頂点シェーダ.
    {
        auto hr = m_pDevice->CreateVertexShader(
            EditorVS, sizeof(EditorVS), nullptr, m_VS.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateVertexShader() Failed. errcode = 0x%x", hr);
            return false;
        }

        hr = m_pDevice->CreateVertexShader(
            EditorSkinningVS, sizeof(EditorSkinningVS), nullptr, m_SkinningVS.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateVertexShader() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    // プラグインマネージャー初期化.
    {
        if (!PluginMgr::Instance().Load())
        {
            ELOG("Error : PluginMgr::Load() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void App::OnTerm()
{
    PluginMgr::Instance().Term();
    m_VS.Reset();
    m_SkinningVS.Reset();
    asdx::GuiMgr::GetInstance().Term();
}

//-----------------------------------------------------------------------------
//      描画時の処理です.
//-----------------------------------------------------------------------------
void App::OnFrameRender(asdx::FrameEventArgs& args)
{
    // シーン定数バッファの更新.
    {
        auto fov = asdx::ToRadian(37.5f);
        auto camera = m_CameraController.GetCamera();

        auto aspect = float(m_Width) / float(m_Height);
        auto proj = asdx::Matrix::CreatePerspectiveFieldOfView(
            fov,
            aspect,
            camera.GetMinDist(),
            camera.GetMaxDist());

        SceneBuffer res = {};
        res.View        = m_CameraController.GetView();
        res.Proj        = proj;
        res.InvView     = asdx::Matrix::Invert(res.View);
        res.InvProj     = asdx::Matrix::Invert(proj);
        res.CameraPos   = camera.GetPosition();
        res.Timer       = float(m_Timer.GetTime());
        res.NearClip    = camera.GetMinDist();
        res.FarClip     = camera.GetMaxDist();
        res.UVToView.x  = float(1.0 / double(proj._11));
        res.UVToView.y  = float(1.0 / double(proj._22));

        auto pCB = m_SceneBuffer.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    Draw3D();

    auto pRTV = m_ColorTarget2D.GetTargetView();
    m_pDeviceContext->ClearRenderTargetView(pRTV, m_ClearColor);
    m_pDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
    m_pDeviceContext->RSSetViewports(1, &m_Viewport);
    m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

    // スワップチェインに描画.
    {
        DrawGui();
    }

    // 画面に表示.
    Present(0);
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
    asdx::GuiMgr::GetInstance().OnKey(
        args.IsKeyDown, args.IsKeyDown, args.KeyCode);

    m_CameraController.OnKey(args.KeyCode, args.IsKeyDown, args.IsAltDown);
}

//-----------------------------------------------------------------------------
//      マウス処理です.
//-----------------------------------------------------------------------------
void App::OnMouse(const asdx::MouseEventArgs& args)
{
    auto isAltDown  = ( GetKeyState( VK_MENU ) & 0x8000 ) != 0;
    auto isCtrlDown = ( GetKeyState( VK_CONTROL ) & 0x800 ) != 0;

    asdx::GuiMgr::GetInstance().OnMouse(
        args.X,
        args.Y,
        args.WheelDelta,
        args.IsLeftButtonDown,
        args.IsMiddleButtonDown,
        args.IsRightButtonDown);

    m_CameraControl = false;

    if (isAltDown)
    {
        m_CameraControl = true;
        m_CameraController.OnMouse(
            args.X,
            args.Y,
            args.WheelDelta,
            args.IsLeftButtonDown,
            args.IsRightButtonDown,
            args.IsMiddleButtonDown,
            args.IsSideButton1Down,
            args.IsSideButton2Down);
    }
}

//-----------------------------------------------------------------------------
//      タイピング処理です.
//-----------------------------------------------------------------------------
void App::OnTyping(uint32_t keyCode)
{
    asdx::GuiMgr::GetInstance().OnTyping(keyCode);
}

//-----------------------------------------------------------------------------
//      ドロップ時の処理です.
//-----------------------------------------------------------------------------
void App::OnDrop(const wchar_t** dropFiles, uint32_t fileCount)
{
}


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
#include <asdxRenderState.h>
#include <asdxMisc.h>
#include <asdxAppHistoryMgr.h>

namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
#include "../res/shaders/Compiled/EditorVS.inc"
#include "../res/shaders/Compiled/EditorSkinningVS.inc"
#include "../res/shaders/Compiled/EditorPS.inc"
#include "../res/shaders/Compiled/GuideVS.inc"
#include "../res/shaders/Compiled/GuidePS.inc"
#include "../res/shaders/Compiled/TriangleVS.inc"
#include "../res/shaders/Compiled/CopyPS.inc"

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
    { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",         0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_SPACE", 0, DXGI_FORMAT_R32_UINT,           0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_INDEX",    0, DXGI_FORMAT_R16G16B16A16_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC kTriangleElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

///////////////////////////////////////////////////////////////////////////////
// LightBuffer structure
///////////////////////////////////////////////////////////////////////////////
struct LightBuffer
{
    asdx::Vector3   SunLightDir         = asdx::Vector3(0.0f, 0.0f, 1.0f);
    float           SunLightIntensity   = 1.0f;
    float           IBLIntensity        = 1.0f;
    float           IBLRotation         = 0.0f;
    float           PreExposureScale    = 1.0f;
    float           Padding0;
};

///////////////////////////////////////////////////////////////////////////////
// GuideVertex structure
///////////////////////////////////////////////////////////////////////////////
struct GuideVertex
{
    asdx::Vector3     Position;     //!< 位置座標です.
    asdx::Vector4     Color;        //!< 頂点カラーです.

    GuideVertex()
    { /* DO_NOTHING */ }

    GuideVertex( const asdx::Vector3& position, const asdx::Vector4& color )
    : Position( position )
    , Color ( color )
    { /* DO_NOTHING */ }
};

///////////////////////////////////////////////////////////////////////////////
// MeshBuffer structure
///////////////////////////////////////////////////////////////////////////////
struct MeshBuffer
{
    asdx::Matrix  World = asdx::Matrix::CreateIdentity();
};

///////////////////////////////////////////////////////////////////////////////
// CbTransform structure
///////////////////////////////////////////////////////////////////////////////
struct CbTransform
{
    asdx::Matrix  World = asdx::Matrix::CreateIdentity();
    asdx::Matrix  View  = asdx::Matrix::CreateIdentity();
    asdx::Matrix  Proj  = asdx::Matrix::CreateIdentity();
};


//-----------------------------------------------------------------------------
//      軸を生成します.
//-----------------------------------------------------------------------------
bool CreateAxis
(
    ID3D11Device*       pDevice,
    float               dist,
    asdx::VertexBuffer& vb,
    uint32_t&           count
)
{
    const auto kR = asdx::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    const auto kG = asdx::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    const auto kB = asdx::Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    GuideVertex vertices[] = {
        GuideVertex(asdx::Vector3(0.0f, 0.0f, 0.0f), kR),
        GuideVertex(asdx::Vector3(dist, 0.0f, 0.0f), kR),
        GuideVertex(asdx::Vector3(0.0f, 0.0f, 0.0f), kB),
        GuideVertex(asdx::Vector3(0.0f, dist, 0.0f), kB),
        GuideVertex(asdx::Vector3(0.0f, 0.0f, 0.0f), kG),
        GuideVertex(asdx::Vector3(0.0f, 0.0f, dist), kG)
    };

    if (!vb.Init(
        pDevice,
        sizeof(GuideVertex) * 6,
        sizeof(GuideVertex),
        vertices))
    {
        ELOG("Error : VertexBuffer::Init() Failed.");
        return false;
    }

    count = 6;

    return true;
}

//-----------------------------------------------------------------------------
//      グリッドを生成します.
//-----------------------------------------------------------------------------
bool CreateGrid
(
    ID3D11Device*       pDevice,
    float               step,
    uint32_t            count,
    asdx::VertexBuffer& vb,
    uint32_t&           vertexCount
)
{
    vertexCount = (count + 1) * 4;

    std::vector<GuideVertex> vertices;
    vertices.resize(vertexCount);

    auto w = (count * 0.5f) * step;
    auto h = (count * 0.5f) * step;
    auto idx = 0;

    auto color0 = asdx::Vector4(0.75f, 0.75f, 0.75f, 1.0f);
    auto color1 = asdx::Vector4(0.25f, 0.25f, 0.25f, 1.0f);

    const auto kBlock = 10;

    // 縦線.
    for( auto i=0u; i<=count; i++ )
    {
        auto split = (i % kBlock) == 0;
        auto x = (i * step) - w;

        vertices[idx].Position = asdx::Vector3(x, 0.0f, -h);
        vertices[idx].Color    = split ? color0 : color1;
        idx++;

        vertices[idx].Position = asdx::Vector3(x, 0.0f, h);
        vertices[idx].Color    = split ? color0 : color1;
        idx++;
    }

    // 横線.
    for( auto i=0u; i<=count; i++ )
    {
        auto split = (i % kBlock) == 0;
        auto z = (i * step) - h;

        vertices[idx].Position = asdx::Vector3(-w, 0.0f, z);
        vertices[idx].Color    = split ? color0 : color1;
        idx++;

        vertices[idx].Position = asdx::Vector3(w, 0.0f, z);
        vertices[idx].Color    = split ? color0 : color1;
        idx++;
    }

    auto ret = vb.Init(
        pDevice,
        sizeof(GuideVertex) * vertexCount,
        sizeof(GuideVertex),
        vertices.data());

    vertices.clear();
    vertices.shrink_to_fit();

    if (!ret)
    {
        ELOG("Error : VertexBuffer::Init() Failed.");
        return false;
    }

    return true;
}


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

    if (!asdx::AppHistoryMgr::GetInstance().Init(100))
    {
        ELOG("Error : AppHistoryMgr::Init() Failed.");
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

        hr = m_pDevice->CreateVertexShader(
            TriangleVS, sizeof(TriangleVS), nullptr, m_TriangleVS.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateVertexShader() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    // ピクセルシェーダ.
    {
        auto hr = m_pDevice->CreatePixelShader(
            EditorPS, sizeof(EditorPS), nullptr, m_DefaultPS.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
            return false;
        }

        hr = m_pDevice->CreatePixelShader(
            CopyPS, sizeof(CopyPS), nullptr, m_CopyPS.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    // 入力レイアウト.
    {
        auto hr = m_pDevice->CreateInputLayout(
            kElements, _countof(kElements), EditorVS, sizeof(EditorVS), m_IL.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateInputLayout() Failed. errcode = 0x%x", hr);
            return false;
        }

        hr = m_pDevice->CreateInputLayout(
            kSkinningElements, _countof(kSkinningElements), EditorSkinningVS, sizeof(EditorSkinningVS), m_SkinningIL.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateInputLayout() Failed. errcode = 0x%x", hr);
            return false;
        }

        hr = m_pDevice->CreateInputLayout(
            kTriangleElements, _countof(kTriangleElements), TriangleVS, sizeof(TriangleVS), m_TriangleIL.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateInputLayout() Failed. errcode = 0x%x", hr);
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

    // 定数バッファ初期化.
    {
        if (!m_SceneCB.Init(m_pDevice, sizeof(SceneBuffer)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }

        if (!m_LightCB.Init(m_pDevice, sizeof(LightBuffer)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }

        if (!m_MeshCB.Init(m_pDevice, sizeof(MeshBuffer)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }
    }

    // 頂点バッファ.
    {
        struct Vertex
        {
            asdx::Vector2 pos;
            asdx::Vector2 tex;
        };

        Vertex vertices[] = {
            { asdx::Vector2(-1.0f,  1.0f), asdx::Vector2(0.0f, 0.0f) },
            { asdx::Vector2( 3.0f,  1.0f), asdx::Vector2(2.0f, 0.0f) },
            { asdx::Vector2(-1.0f, -3.0f), asdx::Vector2(0.0f, 2.0f) }
        };

        if (!m_TriangleVB.Init(m_pDevice, sizeof(vertices), sizeof(Vertex), vertices))
        {
            ELOG("Error : VertexBuffer::Init() Failed.");
            return false;
        }
    }

    // ガイドオブジェクト初期化.
    {
        if (!CreateAxis(m_pDevice, 100.0f, m_AxisVB, m_AxisVertexCount))
        {
            ELOG("Error : CreateAxis() Failed.");
            return false;
        }

        if (!CreateGrid(m_pDevice, 1.0f, 200, m_GridVB, m_GridVertexCount))
        {
            ELOG("Error : CreateGrid() Failed.");
            return false;
        }

        if (!m_GuideCB.Init(m_pDevice, sizeof(CbTransform)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        if (!m_GuideVS.Init(m_pDevice, ASDX_SHADER_BIN(GuideVS), 2, elements))
        {
            ELOG("Error : GuideVS Init Failed.");
            return false;
        }

        if (!m_GuidePS.Init(m_pDevice, ASDX_SHADER_BIN(GuidePS)))
        {
            ELOG("Error : GuidPS Init Failed.");
            return false;
        }
    }

    // ライティング用.
    {
        asdx::TargetDesc2D desc = {};
        desc.Width              = m_Width;
        desc.Height             = m_Height;
        desc.ArraySize          = 1;
        desc.MipLevels          = 1;
        desc.Format             = DXGI_FORMAT_R11G11B10_FLOAT;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        if (!m_LightingBuffer.Create(m_pDevice, desc))
        {
            ELOG("Error : ColorTarget2D::Create() Failed.");
            return false;
        }
    }

    // 法線/ラフネス/メタルネス用.
    {
        asdx::TargetDesc2D desc = {};
        desc.Width = m_Width;
        desc.Height = m_Height;
        desc.ArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        if (!m_NRMBuffer.Create(m_pDevice, desc))
        {
            ELOG("Error : ColorTarget2D::Create() Failed.");
            return false;
        }
    }

    // 深度用.
    {
        asdx::TargetDesc2D desc = {};
        desc.Width              = m_Width;
        desc.Height             = m_Height;
        desc.ArraySize          = 1;
        desc.MipLevels          = 1;
        desc.Format             = DXGI_FORMAT_D32_FLOAT;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        if (!m_DepthBuffer.Create(m_pDevice, desc))
        {
            ELOG("Error : DepthTarget2D::Create() Failed.");
            return false;
        }
    }


    // シャドウマップ用.
    {
        asdx::TargetDesc2D desc = {};
        desc.Width              = 2048;
        desc.Height             = 2048;
        desc.ArraySize          = 1;
        desc.MipLevels          = 1;
        desc.Format             = DXGI_FORMAT_D32_FLOAT;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        if (!m_ShadowBuffer.Create(m_pDevice, desc))
        {
            ELOG("Error : DepthTarget2D::Create() Failed.");
            return false;
        }

        desc.Format = DXGI_FORMAT_R8_UINT;
        if (!m_DummyColorBuffer.Create(m_pDevice, desc))
        {
            ELOG("Error : ColorTarget2D::Create() Failed.");
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
    m_VS        .Reset();
    m_SkinningVS.Reset();
    m_TriangleVS.Reset();
    m_DefaultPS .Reset();
    m_IL        .Reset();
    m_SkinningIL.Reset();
    m_TriangleIL.Reset();

    m_SceneCB.Term();
    m_GuideCB.Term();
    m_LightCB.Term();
    m_MeshCB .Term();

    m_AxisVB    .Term();
    m_GridVB    .Term();
    m_TriangleVB.Term();
    m_GuideVS   .Term();
    m_GuidePS   .Term();
    m_AxisVertexCount = 0;
    m_GridVertexCount = 0;

    m_LightingBuffer.Release();
    m_NRMBuffer     .Release();
    m_ShadowBuffer  .Release();
    m_DepthBuffer   .Release();
    m_DummyColorBuffer.Release();

    PluginMgr::Instance().Term();
    asdx::GuiMgr::GetInstance().Term();

    asdx::AppHistoryMgr::GetInstance().Term();
}

//-----------------------------------------------------------------------------
//      描画時の処理です.
//-----------------------------------------------------------------------------
void App::OnFrameRender(asdx::FrameEventArgs& args)
{
    auto pRTV = m_ColorTarget2D.GetTargetView();
    auto pDSV = m_DepthTarget2D.GetTargetView();

    // レンダーターゲットが初期化されていない場合は処理しない.
    if (pRTV == nullptr || pDSV == nullptr)
    { return; }

    // ターゲットをクリア.
    m_pDeviceContext->ClearRenderTargetView(pRTV, m_ClearColor);
    m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // シーン定数バッファの更新.
    {
        auto fov = asdx::ToRadian(37.5f);
        auto camera = m_CameraController.GetCamera();

        auto aspect = float(m_Width) / float(m_Height);
        m_Proj = asdx::Matrix::CreatePerspectiveFieldOfView(
            fov,
            aspect,
            camera.GetMinDist(),
            camera.GetMaxDist());

        SceneBuffer res = {};
        res.View        = m_CameraController.GetView();
        res.Proj        = m_Proj;
        res.InvView     = asdx::Matrix::Invert(res.View);
        res.InvProj     = asdx::Matrix::Invert(m_Proj);
        res.CameraPos   = camera.GetPosition();
        res.Timer       = float(m_Timer.GetTime());
        res.NearClip    = camera.GetMinDist();
        res.FarClip     = camera.GetMaxDist();
        res.UVToView.x  = float(1.0 / double(m_Proj._11));
        res.UVToView.y  = float(1.0 / double(m_Proj._22));

        auto pCB = m_SceneCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // ライトバッファ更新.
    {
        asdx::Vector3 lightDir;
        //auto theta = asdx::ToRadian(setting.DirectLightAngle.y);
        //auto phi   = asdx::ToRadian(setting.DirectLightAngle.x);
        //lightDir.x = cos(theta) * cos(phi);
        //lightDir.y = sin(theta);
        //lightDir.z = cos(theta) * sin(phi);

        LightBuffer res = {};
        //res.SunLightDir = lightDir;


        if (res.SunLightIntensity < 0.0f)
        { res.SunLightIntensity = 0.0f; }

        if (res.IBLIntensity < 0.0f)
        { res.IBLIntensity = 0.0f; }

        if (res.PreExposureScale < 0.0f)
        { res.PreExposureScale = 0.0f;}

        auto pCB = m_LightCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // メッシュバッファ更新.
    {
        MeshBuffer res = {};

        if (m_WorkSpace.GetModel() != nullptr)
        { res.World = m_WorkSpace.GetModel()->GetWorld(); }

        auto pCB = m_MeshCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // 3D描画.
    Draw3D();

    m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
    m_pDeviceContext->RSSetViewports(1, &m_Viewport);
    m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

    // スワップチェインに描画.
    {
        // コンポジット.
        {
            auto pSRV = m_LightingBuffer.GetShaderResource();
            auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::LinearClamp);
            m_pDeviceContext->PSSetShader(m_CopyPS.GetPtr(), nullptr, 0);
            m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
            m_pDeviceContext->PSSetSamplers(0, 1, &pSmp);
            DrawQuad();
        }

        // GUI描画.
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
    m_LightingBuffer.Resize(m_pDevice, args.Width, args.Height);
    m_NRMBuffer     .Resize(m_pDevice, args.Width, args.Height);
    m_ShadowBuffer  .Resize(m_pDevice, args.Width, args.Height);
    m_DepthBuffer   .Resize(m_pDevice, args.Width, args.Height);
}

//-----------------------------------------------------------------------------
//      キー処理です.
//-----------------------------------------------------------------------------
void App::OnKey(const asdx::KeyEventArgs& args)
{
    asdx::GuiMgr::GetInstance().OnKey(
        args.IsKeyDown, args.IsKeyDown, args.KeyCode);

    m_CameraController.OnKey(
        args.KeyCode, args.IsKeyDown, args.IsAltDown);
}

//-----------------------------------------------------------------------------
//      マウス処理です.
//-----------------------------------------------------------------------------
void App::OnMouse(const asdx::MouseEventArgs& args)
{
    auto isAltDown  = !!(GetKeyState(VK_MENU) & 0x8000);
    auto isCtrlDown = !!(GetKeyState(VK_CONTROL) & 0x800);
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

    asdx::GuiMgr::GetInstance().OnMouse(
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
    asdx::GuiMgr::GetInstance().OnTyping(keyCode);
}

//-----------------------------------------------------------------------------
//      ドロップ時の処理です.
//-----------------------------------------------------------------------------
void App::OnDrop(const std::vector<std::string>& dropFiles)
{
    for (size_t i=0; i<dropFiles.size(); ++i)
    {
        auto ext = asdx::GetExtA(dropFiles[i].c_str());

        if (ext == "fbx" || ext == "obj")
        {
            if (m_WorkSpace.New(dropFiles[i].c_str()))
            { return; }
        }
        else if (ext == "work")
        {
            if (m_WorkSpace.LoadAsync(dropFiles[i].c_str()))
            { return; }
        }
    }
}

//-----------------------------------------------------------------------------
//      ガイドを描画します.
//-----------------------------------------------------------------------------
void App::DrawGuide()
{
    CbTransform res;
    res.World = asdx::Matrix::CreateIdentity();
    res.View  = m_CameraController.GetView();
    res.Proj  = m_Proj;

    auto pCB = m_GuideCB.GetBuffer();
    m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);

    auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Readonly);
    m_GuideVS.Bind(m_pDeviceContext);
    m_GuidePS.Bind(m_pDeviceContext);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &pCB);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);

    // グリッドの描画.
    if (m_GridVertexCount > 0 /*&& m_Config.Debug.DrawGrid*/)
    {
        auto pVB    = m_GridVB.GetBuffer();
        auto stride = m_GridVB.GetStride();
        auto offset = 0u;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
        m_pDeviceContext->Draw(m_GridVertexCount, 0);
    }

    // 軸の描画.
    if (m_AxisVertexCount > 0 /*&& m_Config.Debug.DrawAxis*/)
    {
        auto pVB    = m_AxisVB.GetBuffer();
        auto stride = m_AxisVB.GetStride();
        auto offset = 0u;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
        m_pDeviceContext->Draw(m_AxisVertexCount, 0);
    }

    m_GuidePS.UnBind(m_pDeviceContext);
    m_GuideVS.UnBind(m_pDeviceContext);
}

//-----------------------------------------------------------------------------
//      フルスクリーン矩形を描画します.
//-----------------------------------------------------------------------------
void App::DrawQuad()
{
    auto pVB    = m_TriangleVB.GetBuffer();
    auto stride = m_TriangleVB.GetStride();
    auto offset = 0u;

    auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::None);
    auto pBS  = asdx::RenderState::GetInstance().GetBS(asdx::BlendType::Opaque);
    auto pRS  = asdx::RenderState::GetInstance().GetRS(asdx::RasterizerType::CullNone);

    float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32_t sampleMask = 0xffff;

    m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
    m_pDeviceContext->OMSetBlendState(pBS, blendFactor, sampleMask);
    m_pDeviceContext->RSSetState(pRS);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    m_pDeviceContext->IASetInputLayout(m_TriangleIL.GetPtr());
    m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->VSSetShader(m_TriangleVS.GetPtr(), nullptr, 0);
    m_pDeviceContext->Draw(3, 0);
}


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
#include <LightMgr.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
#include "../res/shaders/Compiled/EditorVS.inc"
#include "../res/shaders/Compiled/EditorSkinningVS.inc"
#include "../res/shaders/Compiled/EditorPS.inc"
#include "../res/shaders/Compiled/GuideVS.inc"
#include "../res/shaders/Compiled/GuidePS.inc"
#include "../res/shaders/Compiled/ShadowVS.inc"
#include "../res/shaders/Compiled/ShadowSkinningVS.inc"
#include "../res/shaders/Compiled/TriangleVS.inc"
#include "../res/shaders/Compiled/ShapeVS.inc"
#include "../res/shaders/Compiled/ShapePS.inc"
#include "../../asdx11/res/shaders/Compiled/CopyPS.inc"
#include "../../asdx11/res/shaders/Compiled/OETFPS.inc"


//-----------------------------------------------------------------------------
// Global Varaibles.
//-----------------------------------------------------------------------------
static const D3D11_INPUT_ELEMENT_DESC kElements[] = {
    { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",         0, DXGI_FORMAT_R8G8B8A8_UNORM,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_SPACE", 0, DXGI_FORMAT_R32_UINT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static const D3D11_INPUT_ELEMENT_DESC kSkinningElements[] = {
    { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",         0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_SPACE", 0, DXGI_FORMAT_R32_UINT,           0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_INDEX",    0, DXGI_FORMAT_R16G16B16A16_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static const D3D11_INPUT_ELEMENT_DESC kTriangleElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static const D3D11_INPUT_ELEMENT_DESC kShapeElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static const D3D11_INPUT_ELEMENT_DESC kGuideElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
    asdx::Vector2   ScreenSize;     // スクリーンサイズ.
    asdx::Vector2   InvScreenSize;  // スクリーンサイズの逆数.
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
// OETFBuffer structure
///////////////////////////////////////////////////////////////////////////////
struct OETFBuffer
{
    int   OETFType          = 1;
    float BaseLuminance     = 100.0f;
    float TargetLuminance   = 100.0f;
    float Padding0;
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
        GuideVertex(asdx::Vector3(0.0f, 0.0f, 0.0f), kG),
        GuideVertex(asdx::Vector3(0.0f, dist, 0.0f), kG),
        GuideVertex(asdx::Vector3(0.0f, 0.0f, 0.0f), kB),
        GuideVertex(asdx::Vector3(0.0f, 0.0f, dist), kB)
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

//-----------------------------------------------------------------------------
//      方位角から方向ベクトルを求めます.
//-----------------------------------------------------------------------------
asdx::Vector3 ToDirection(const asdx::Vector2& angle)
{
    auto theta = asdx::ToRadian(angle.y);
    auto phi   = asdx::ToRadian(angle.x);
    return asdx::Vector3(
        cos(theta) * cos(phi),
        sin(theta),
        cos(theta) * sin(phi));
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
        if (!m_EditorVS.Init(m_pDevice, ASDX_SHADER_BIN(EditorVS), _countof(kElements), kElements))
        {
            ELOG("Error : EditorVS Init Failed.");
            return false;
        }

        if (!m_EditorSkinningVS.Init(m_pDevice, ASDX_SHADER_BIN(EditorSkinningVS), _countof(kSkinningElements), kSkinningElements))
        {
            ELOG("Error : EditorSkinningVS Init Failed.");
            return false;
        }

        if (!m_ShadowVS.Init(m_pDevice, ASDX_SHADER_BIN(ShadowVS), _countof(kElements), kElements))
        {
            ELOG("Error : ShadowVS Init Failed.");
            return false;
        }

        if (!m_ShadowSkinningVS.Init(m_pDevice, ASDX_SHADER_BIN(ShadowSkinningVS), _countof(kSkinningElements), kSkinningElements))
        {
            ELOG("Error : ShadowSkinningVS Init Failed.");
            return false;
        }

        if (!m_TriangleVS.Init(m_pDevice, ASDX_SHADER_BIN(TriangleVS), _countof(kTriangleElements), kTriangleElements))
        {
            ELOG("Error : TriangleVS Init Failed.");
            return false;
        }

        if (!m_GuideVS.Init(m_pDevice, ASDX_SHADER_BIN(GuideVS), _countof(kGuideElements), kGuideElements))
        {
            ELOG("Error : GuideVS Init Failed.");
            return false;
        }

        if (!m_ShapeVS.Init(m_pDevice, ASDX_SHADER_BIN(ShapeVS), _countof(kShapeElements), kShapeElements))
        {
            ELOG("Error : ShadpeVS Init Failed.");
            return false;
        }
    }

    // ピクセルシェーダ.
    {
        if (!m_DefaultPS.Init(m_pDevice, ASDX_SHADER_BIN(EditorPS)))
        {
            ELOG("Error : DefaultPS Init Failed.");
            return false;
        }

        if (!m_CopyPS.Init(m_pDevice, ASDX_SHADER_BIN(CopyPS)))
        {
            ELOG("Error : CopyPS Init Failed.");
            return false;
        }

        if (!m_OETFPS.Init(m_pDevice, ASDX_SHADER_BIN(OETFPS)))
        {
            ELOG("Error : OETFPS Init Failed.");
            return false;
        }

        if (!m_GuidePS.Init(m_pDevice, ASDX_SHADER_BIN(GuidePS)))
        {
            ELOG("Error : GuidePS Init Failed.");
            return false;
        }

        if (!m_ShapePS.Init(m_pDevice, ASDX_SHADER_BIN(ShapePS)))
        {
            ELOG("Error : ShapePS Init Failed.");
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

        if (!m_OETFCB.Init(m_pDevice, sizeof(OETFBuffer)))
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
    }

    // プリミティブ初期化.
    {
        if (!m_ArrowShape.Init(m_pDevice, 0.5f, 6.0f, 1.0f, 2.0f, 20))
        {
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

    // ライトマネージャー初期化.
    if (!LightMgr::Instance().Init())
    {
        ELOG("Error : LightMgr::Init() Failed.");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void App::OnTerm()
{
    m_EditorVS.Term();
    m_EditorSkinningVS.Term();
    m_ShadowVS.Term();
    m_ShadowSkinningVS.Term();
    m_TriangleVS.Term();
    m_GuideVS.Term();
    m_ShapeVS.Term();
    m_DefaultPS.Term();
    m_CopyPS.Term();
    m_OETFPS.Term();
    m_GuidePS.Term();
    m_ShapePS.Term();

    m_SceneCB.Term();
    m_GuideCB.Term();
    m_LightCB.Term();
    m_MeshCB .Term();
    m_OETFCB .Term();

    m_AxisVB    .Term();
    m_GridVB    .Term();
    m_TriangleVB.Term();
    m_GuideVS   .Term();
    m_GuidePS   .Term();
    m_AxisVertexCount = 0;
    m_GridVertexCount = 0;

    m_ArrowShape.Term();

    m_LightingBuffer.Release();
    m_NRMBuffer     .Release();
    m_ShadowBuffer  .Release();
    m_DepthBuffer   .Release();
    m_DummyColorBuffer.Release();

    PluginMgr::Instance().Term();
    asdx::GuiMgr::GetInstance().Term();
    LightMgr::Instance().Term();

    asdx::AppHistoryMgr::GetInstance().Term();
}

//-----------------------------------------------------------------------------
//      フレーム遷移処理です.
//-----------------------------------------------------------------------------
void App::OnFrameMove(asdx::FrameEventArgs& args)
{
    auto isLoading = m_WorkSpace.IsLoading();
    if (m_PrevLoadState != isLoading)
    {
        // 読み込み終わった時.
        if (!isLoading && m_WorkSpace.GetModel() != nullptr)
        {
            m_LoadingPos = 0;

            auto nearClip = m_Config.Camera.NearClip.GetValue();
            auto farClip  = m_Config.Camera.FarClip .GetValue();

            // カメラ位置をモデルに合わせて再設定.
            auto& box   = m_WorkSpace.GetModel()->GetBox();
            auto size   = box.maxi - box.mini;
            auto length = size.Length();
            auto target = (box.maxi + box.mini) * 0.5f;
            auto pos    = asdx::Vector3(target.x, target.y, target.z + nearClip + length);
            auto upward = asdx::Vector3(0.0f, 1.0f, 0.0f);

            m_CameraController.GetCamera().Reset();
            m_CameraController.Init(pos, target, upward, nearClip, farClip);
        }
    }

    m_PrevLoadState = isLoading;

    // シーン定数バッファの更新.
    {
        auto fov      = asdx::ToRadian(m_Config.Camera.FieldOfView.GetValue());
        auto camera   = m_CameraController.GetCamera();
        auto nearClip = m_Config.Camera.NearClip.GetValue();
        auto farClip  = m_Config.Camera.FarClip .GetValue();
        auto w        = float(m_Width);
        auto h        = float(m_Height);

        auto aspect = w / h;
        m_Proj = asdx::Matrix::CreatePerspectiveFieldOfView(
            fov, aspect, nearClip, farClip);

        SceneBuffer res = {};
        res.View            = m_CameraController.GetView();
        res.Proj            = m_Proj;
        res.InvView         = asdx::Matrix::Invert(res.View);
        res.InvProj         = asdx::Matrix::Invert(m_Proj);
        res.CameraPos       = camera.GetPosition();
        res.Timer           = float(m_Timer.GetTime());
        res.NearClip        = nearClip;
        res.FarClip         = farClip;
        res.UVToView.x      = float(1.0 / double(m_Proj._11));
        res.UVToView.y      = float(1.0 / double(m_Proj._22));
        res.ScreenSize      = asdx::Vector2(w, h);
        res.InvScreenSize   = asdx::Vector2(1.0f / w, 1.0f / h);

        auto pCB = m_SceneCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // ライトバッファ更新.
    {
        LightBuffer res = {};

        auto light = LightMgr::Instance().GetLight();
        if (light != nullptr)
        {
            res.SunLightDir         = ToDirection(light->SunLightAngle);
            res.SunLightIntensity   = light->SunLightIntensity;
            res.IBLIntensity        = light->IBLIntensity;

            if (res.SunLightIntensity < 0.0f)
            { res.SunLightIntensity = 0.0f; }

            if (res.IBLIntensity < 0.0f)
            { res.IBLIntensity = 0.0f; }

            if (res.PreExposureScale < 0.0f)
            { res.PreExposureScale = 0.0f;}
        }

        auto pCB = m_LightCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // メッシュバッファ更新.
    {
        MeshBuffer res = {};

        auto model = m_WorkSpace.GetModel();
        if (model != nullptr)
        {
            auto rotation = m_Config.ModelPreview.Rotation.GetValue();
            model->SetScale(m_Config.ModelPreview.Scale.GetValue());
            model->SetRotation(rotation);
            model->SetTranslation(m_Config.ModelPreview.Translation.GetValue());

            // 自動回転.
            if (m_Config.ModelPreview.AutoRotation.GetValue())
            {
                auto speed = m_Config.ModelPreview.AutoRotationSpeed.GetValue();
                m_AutoRotationY += speed * float(m_Timer.GetElapsedTime() * 1000.0f);
                model->SetRotation(asdx::Vector3(0.0f, m_AutoRotationY, 0.0f) + rotation);
            }

            model->UpdateWorld();
            res.World = model->GetWorld();
        }

        auto pCB = m_MeshCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }

    // OETFバッファ更新.
    {
        OETFBuffer res = {};


        auto pCB = m_OETFCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    }
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
            auto pCB  = m_OETFCB.GetBuffer();
            m_OETFPS.Bind(m_pDeviceContext);
            m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
            m_pDeviceContext->PSSetConstantBuffers(0, 1, &pCB);
            m_pDeviceContext->PSSetSamplers(0, 1, &pSmp);
            DrawQuad();
            m_OETFPS.UnBind(m_pDeviceContext);
        }

        // GUI描画.
        DrawGui();
    }

    // 画面に表示.
    Present(0);

    // 同期を取る.
    PluginMgr::Instance().Sync();

    // シェーダをリロード.
    if (m_ReloadShader)
    {
        PluginMgr::Instance().ReloadShader();
        m_ReloadShader = false;
    }
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

    auto isCtrlDown = !!(GetAsyncKeyState(VK_CONTROL) & 0x8000);

    if (args.IsKeyDown)
    {
        switch (args.KeyCode)
        {
        case VK_F5:
            // シェーダをリロード.
            { m_ReloadShader = true; }
            break;

        case 'Z':
            // 元に戻す.
            if (isCtrlDown)
            { asdx::AppHistoryMgr::GetInstance().Undo(); }
            break;

        case 'Y':
            // やり直す.
            if (isCtrlDown)
            { asdx::AppHistoryMgr::GetInstance().Redo(); }
            break;

        case 'Q':
            // Rotate
            if (isCtrlDown && !ImGui::IsAnyWindowHovered())
            {
                if (m_GuizmoOperation == 1)
                { m_GuizmoOperation = -1; }
                else
                { m_GuizmoOperation = 1; }
            }
            break;

        case 'W':
            // Translate
            if (isCtrlDown && !ImGui::IsAnyWindowHovered())
            {
                if (m_GuizmoOperation == 0)
                { m_GuizmoOperation = -1; }
                else
                { m_GuizmoOperation = 0; }
            }
            break;

        case 'E':
            // Scale
            if (isCtrlDown && !ImGui::IsAnyWindowHovered())
            {
                if (m_GuizmoOperation == 2)
                { m_GuizmoOperation = -1; }
                else 
                { m_GuizmoOperation = 2; }
            }
            break;
        }
    }
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

        if (ext == "fbx" 
         || ext == "obj"
         || ext == "3ds"
         || ext == "dae"
         || ext == "collada"
         || ext == "blend"
         || ext == "dxf"
         || ext == "gltf"
         || ext == "glb"
         || ext == "lwo"
         || ext == "mdl"
         || ext == "md2"
         || ext == "md3"
         || ext == "md5"
         || ext == "ms3d"
         || ext == "ply"
         || ext == "stl"
         || ext == "x")
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
//      モデルを描画します.
//-----------------------------------------------------------------------------
void App::DrawModel(bool lightingPass, asdx::BlendType blendType)
{
    if (m_WorkSpace.IsLoading())
    { return; }

    auto model = m_WorkSpace.GetModel();
    if (model == nullptr)
    { return; }

    auto materials = m_WorkSpace.GetMaterials();
    auto count     = model->GetMeshCount();

    auto pSceneCB = m_SceneCB.GetBuffer();
    auto pLightCB = m_LightCB.GetBuffer();
    auto pMeshCB  = m_MeshCB .GetBuffer();

    auto light = LightMgr::Instance().GetLight();

    for(auto i=0u; i<count; ++i)
    {
        auto& mesh      = model->GetMesh(i);
        auto& material  = materials->GetMaterial(i);
    
        if (!lightingPass && !material.CastShadow())
        { continue; }

        if (material.GetBlendState() != blendType)
        { continue; }

        if (mesh.HasSkinningData())
        {
            if (lightingPass)
            { m_EditorSkinningVS.Bind(m_pDeviceContext); }
            else
            { m_ShadowSkinningVS.Bind(m_pDeviceContext); }
        }
        else
        {
            if (lightingPass)
            { m_EditorVS.Bind(m_pDeviceContext); }
            else
            { m_ShadowVS.Bind(m_pDeviceContext); }
        }

        m_pDeviceContext->VSSetConstantBuffers(0, 1, &pSceneCB);
        m_pDeviceContext->VSSetConstantBuffers(1, 1, &pMeshCB);

        // マテリアル更新 & 設定.
        auto shader = material.Bind(m_pDeviceContext, lightingPass);
        if (shader != nullptr)
        {
            // 定数バッファ設定.
            shader->SetCBV(m_pDeviceContext, "CbScene", pSceneCB);
            shader->SetCBV(m_pDeviceContext, "CbLight", pLightCB);

            if (lightingPass && light != nullptr)
            {
                // IBL設定.
                shader->SetSRV(m_pDeviceContext, "EnvBRDF", LightMgr::Instance().GetEnvBRDF());
                shader->SetSRV(m_pDeviceContext, "DiffuseLD",  light->GetDiffuseLD());
                shader->SetSRV(m_pDeviceContext, "SpecularLD", light->GetSpecularLD());

                // シャドウマップ設定.
            }

            // メッシュを描画.
            mesh.Draw(m_pDeviceContext);

            // マテリアル設定を解除.
            material.Unbind(m_pDeviceContext, shader);
        }
        else
        {
            if (lightingPass)
            {
                auto pSRV = PluginMgr::Instance().GetDefaultSRV(DEFAULT_TEXTURE_CHECKER_BOARD);
                auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::LinearClamp);

                m_DefaultPS.Bind(m_pDeviceContext);
                m_pDeviceContext->PSSetConstantBuffers(0, 1, &pLightCB);
                m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
                m_pDeviceContext->PSSetSamplers(0, 1, &pSmp);
            }
            else
            {
                m_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
            }

            float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            uint32_t sampleMask = 0xffff;
            auto pBS  = asdx::RenderState::GetInstance().GetBS(blendType);
            auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Default);
            auto pRS  = asdx::RenderState::GetInstance().GetRS(asdx::RasterizerType::CullNone);
            m_pDeviceContext->OMSetBlendState(pBS, blendFactor, sampleMask);
            m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
            m_pDeviceContext->RSSetState(pRS);

            mesh.Draw(m_pDeviceContext);

            ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
            ID3D11SamplerState* pNullSmp[] = { nullptr };

            m_pDeviceContext->PSSetShaderResources(0, 1, pNullSRV);
            m_pDeviceContext->PSSetSamplers(0, 1, pNullSmp);
            m_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
        }

        ID3D11Buffer* pNullCBV[] = { nullptr, nullptr };
        m_pDeviceContext->VSGetConstantBuffers(0, 2, pNullCBV);
        m_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
    }
}

//-----------------------------------------------------------------------------
//      ガイドを描画します.
//-----------------------------------------------------------------------------
void App::DrawGuide()
{
    {
        CbTransform res;
        res.World = asdx::Matrix::CreateIdentity();
        res.View = m_CameraController.GetView();
        res.Proj = m_Proj;

        auto pCB = m_GuideCB.GetBuffer();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);

        auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Readonly);
        m_GuideVS.Bind(m_pDeviceContext);
        m_GuidePS.Bind(m_pDeviceContext);
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &pCB);
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
    }

    // グリッドの描画.
    if (m_GridVertexCount > 0 && m_Config.Debug.DrawGrid.GetValue())
    {
        auto pVB    = m_GridVB.GetBuffer();
        auto stride = m_GridVB.GetStride();
        auto offset = 0u;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
        m_pDeviceContext->Draw(m_GridVertexCount, 0);
    }

    // 軸の描画.
    if (m_AxisVertexCount > 0 && m_Config.Debug.DrawAxis.GetValue())
    {
        auto pVB    = m_AxisVB.GetBuffer();
        auto stride = m_AxisVB.GetStride();
        auto offset = 0u;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
        m_pDeviceContext->Draw(m_AxisVertexCount, 0);
    }

    m_GuidePS.UnBind(m_pDeviceContext);
    m_GuideVS.UnBind(m_pDeviceContext);

    // ライト方向の描画.
    if (m_Config.Debug.DrawLightDir.GetValue())
    {
        m_ShapeVS.Bind(m_pDeviceContext);
        m_ShapePS.Bind(m_pDeviceContext);

        auto light = LightMgr::Instance().GetLight();
        asdx::Vector2 angle(0.0f, 0.0f);
        if (light != nullptr)
        { angle = light->SunLightAngle; }

        // カメラに追従するようにする.
        const auto kDistanceToDraw = 50.0f;
        auto cameraReleative = m_CameraController.GetCamera().GetPosition();
        cameraReleative += (-m_CameraController.GetCamera().GetAxisZ() * kDistanceToDraw);

        auto world = 
            asdx::Matrix::CreateRotationZ(asdx::F_PIDIV2) *
            asdx::Matrix::CreateRotationY(asdx::ToRadian(angle.y)) *
            asdx::Matrix::CreateRotationX(asdx::ToRadian(angle.x)) *
            asdx::Matrix::CreateTranslation(cameraReleative);


        auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::None);
        auto pRS  = asdx::RenderState::GetInstance().GetRS(asdx::RasterizerType::CullCounterClockWise);
        m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
        m_pDeviceContext->RSSetState(pRS);

        auto pSceneCB = m_SceneCB.GetBuffer();
        auto pLightCB = m_LightCB.GetBuffer();

        m_pDeviceContext->VSSetConstantBuffers(0, 1, &pSceneCB);
        m_pDeviceContext->PSSetConstantBuffers(0, 1, &pSceneCB);
        m_pDeviceContext->PSSetConstantBuffers(1, 1, &pLightCB);

        m_ArrowShape.Draw(m_pDeviceContext, world, asdx::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

        m_ShapePS.UnBind(m_pDeviceContext);
        m_ShapeVS.UnBind(m_pDeviceContext);
    }
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
    auto pBS  = asdx::RenderState::GetInstance().GetBS (asdx::BlendType::Opaque);
    auto pRS  = asdx::RenderState::GetInstance().GetRS (asdx::RasterizerType::CullNone);

    float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32_t sampleMask = 0xffff;

    m_TriangleVS.Bind(m_pDeviceContext);
    m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
    m_pDeviceContext->OMSetBlendState(pBS, blendFactor, sampleMask);
    m_pDeviceContext->RSSetState(pRS);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->Draw(3, 0);
    m_TriangleVS.UnBind(m_pDeviceContext);
}


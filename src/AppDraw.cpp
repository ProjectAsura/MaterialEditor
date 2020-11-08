//-----------------------------------------------------------------------------
// File : AppDraw.cpp
// Desc : Draw.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <LightMgr.h>


//-----------------------------------------------------------------------------
//      3D描画します.
//-----------------------------------------------------------------------------
void App::Draw3D()
{
    // シャドウマップ描画.
#if 0
    {
        auto pRTV = m_DummyColorTarget.GetTargetView();
        auto pDSV = m_ShadowTarget.GetTargetView();

        m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // レンダーターゲットは本来不要だが，
        // 設定しないとピクセルシェーダが起動せずアルファ抜きが出来ないため設定する.
        m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX   = 0.0f;
        viewport.TopLeftY   = 0.0f;
        viewport.Width      = 2048.0f;
        viewport.Height     = 2048.0f;
        viewport.MinDepth   = 0.0f;
        viewport.MaxDepth   = 1.0f;

        D3D11_RECT scissor = {};
        scissor.left    = 0;
        scissor.right   = 2048;
        scissor.top     = 0;
        scissor.bottom  = 2048;

        m_pDeviceContext->RSSetViewports(1, &viewport);
        m_pDeviceContext->RSSetScissorRects(1, &scissor);

        DrawModel(false, asdx::BlendType::Opaque);
    }
#endif

    // 不透明モデル描画.
    {
        ID3D11RenderTargetView* pRTV[] = {
            m_LightingBuffer.GetTargetView(),
            m_NRMBuffer     .GetTargetView(),
        };

        auto pDSV = m_DepthBuffer.GetTargetView();

        auto cc = m_Config.Background.ClearColor.GetValue();
        float clearColor0[] = { cc.x, cc.y, cc.z, 1.0f };
        float clearColor1[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_pDeviceContext->ClearRenderTargetView(pRTV[0], clearColor0);
        m_pDeviceContext->ClearRenderTargetView(pRTV[1], clearColor1);
        m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
        m_pDeviceContext->RSSetViewports(1, &m_Viewport);
        m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

        if (m_Config.Background.ShowTexture.GetValue())
        {
            m_pDeviceContext->OMSetRenderTargets(1, pRTV, nullptr);
            LightMgr::Instance().Draw(
                m_pDeviceContext,
                m_CameraController.GetCamera().GetPosition(),
                m_CameraController.GetView(),
                m_Proj,
                m_Config.Camera.FarClip.GetValue());
        }


        m_pDeviceContext->OMSetRenderTargets(2, pRTV, pDSV);

        DrawModel(true, asdx::BlendType::Opaque);
    }


    // GTAO & GTSO


    // SSR


    // 半透明モデル描画
    {

    }


    // トーンマップ.


    // Bloom


    // TAA


    // ガイドオブジェクト描画.
    DrawGuide();
}


void App::DrawShadowMap()
{

}

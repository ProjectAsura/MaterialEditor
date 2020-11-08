﻿//-----------------------------------------------------------------------------
// File : AppDraw.cpp
// Desc : Draw.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>


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
            {
                m_pDeviceContext->VSSetShader(m_SkinningVS.GetPtr(), nullptr, 0);
                m_pDeviceContext->IASetInputLayout(m_SkinningIL.GetPtr());
            }
            else
            {
                m_pDeviceContext->VSSetShader(m_ShadowSkinningVS.GetPtr(), nullptr, 0);
                m_pDeviceContext->IASetInputLayout(m_ShadowSkinningIL.GetPtr());
            }
        }
        else
        {
            if (lightingPass)
            {
                m_pDeviceContext->VSSetShader(m_VS.GetPtr(), nullptr, 0);
                m_pDeviceContext->IASetInputLayout(m_IL.GetPtr());
            }
            else
            {
                m_pDeviceContext->VSSetShader(m_ShadowVS.GetPtr(), nullptr, 0);
                m_pDeviceContext->IASetInputLayout(m_ShadowIL.GetPtr());
            }
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

            if (lightingPass)
            {
                // IBL設定.

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

                m_pDeviceContext->PSSetShader(m_DefaultPS.GetPtr(), nullptr, 0);
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
    }
}

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

    // フォワードレンダリング.
    {
        ID3D11RenderTargetView* pRTV[] = {
            m_LightingBuffer.GetTargetView(),
            m_NRMBuffer     .GetTargetView(),
        };

        auto pDSV = m_DepthBuffer.GetTargetView();

        float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_pDeviceContext->ClearRenderTargetView(pRTV[0], m_ClearColor);
        m_pDeviceContext->ClearRenderTargetView(pRTV[1], clearColor);
        m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        m_pDeviceContext->OMSetRenderTargets(2, pRTV, pDSV);
        m_pDeviceContext->RSSetViewports(1, &m_Viewport);
        m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

        // モデルを描画.
        DrawModel(true, asdx::BlendType::Opaque);
    }


    // GTAO & GTSO


    // SSR


    // トーンマップ.


    // Bloom


    // TAA


    // ガイドオブジェクト描画.
    DrawGuide();
}


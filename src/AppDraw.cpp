//-----------------------------------------------------------------------------
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
            m_pDeviceContext->VSSetShader(m_SkinningVS.GetPtr(), nullptr, 0);
            m_pDeviceContext->IASetInputLayout(m_SkinningIL.GetPtr());
        }
        else
        {
            m_pDeviceContext->VSSetShader(m_VS.GetPtr(), nullptr, 0);
            m_pDeviceContext->IASetInputLayout(m_IL.GetPtr());
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


    // フォワードレンダリング.
    {
        ID3D11RenderTargetView* pRTV[] = {
            m_LightingTarget.GetTargetView(),
            m_NRMTarget.GetTargetView(),
        };

        float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_pDeviceContext->ClearRenderTargetView(pRTV[0], clearColor);
        m_pDeviceContext->ClearRenderTargetView(pRTV[1], clearColor);

        auto pDSV = m_DepthTarget2D.GetTargetView();

        m_pDeviceContext->OMSetRenderTargets(2, pRTV, pDSV);
        m_pDeviceContext->RSSetViewports(1, &m_Viewport);
        m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

        DrawModel(true, asdx::BlendType::Opaque);
    }


    // SSAO.


    // SSR


    // トーンマップ.


    // Bloom


    // TAA

}


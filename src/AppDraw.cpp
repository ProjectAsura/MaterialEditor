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
void App::DrawModel()
{
    auto model     = m_WorkSpace.GetModel();
    auto materials = m_WorkSpace.GetMaterials();
    auto count     = model->GetMeshCount();

    for(auto i=0u; i<count; ++i)
    {
        auto& mesh = model->GetMesh(i);
    
        EditorMaterial material;
        if (!materials->FindMaterial(mesh.GetMaterialName(), material))
        { continue; }

        // マテリアル更新 & 設定.
        auto shader = material.Bind(m_pDeviceContext);

        // 定数バッファ設定.
        shader->SetCBV(m_pDeviceContext, "CbScene", m_SceneCB.GetBuffer());
        shader->SetCBV(m_pDeviceContext, "CbLight", m_LightCB.GetBuffer());

        // テクスチャを設定.
        //shader->SetSRV();

        // メッシュを描画.
        mesh.Draw(m_pDeviceContext);

        // マテリアル設定を解除.
        material.Unbind(m_pDeviceContext, shader);
    }
}

//-----------------------------------------------------------------------------
//      3D描画します.
//-----------------------------------------------------------------------------
void App::Draw3D()
{
    // シャドウマップ描画.


    // フォワードレンダリング.


    // SSAO.


    // SSR


    // トーンマップ.


    // Bloom


    // TAA

}


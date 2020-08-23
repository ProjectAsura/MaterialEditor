//-----------------------------------------------------------------------------
// File : EditorMaterial.cpp
// Desc : Material For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <EditorMaterial.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
const char* kDisplayFace[] = {
    u8"両面",
    u8"表面",
    u8"裏面"
};

const char* kState[] = {
    u8"不透明",
    u8"半透明",
    u8"歪み"
};

///////////////////////////////////////////////////////////////////////////////
// CbMaterial structure
///////////////////////////////////////////////////////////////////////////////
struct CbMaterial
{
    int     CastShadow;
    int     ReceiveShadow;
    int     EnableDither;
    int     DisplayFace;
    
    asdx::Vector2   UVScale0;
    asdx::Vector2   UVScale1;
    asdx::Vector2   UVScale2;
    asdx::Vector2   UVScale3;

    asdx::Vector2   UVRotate0;
    asdx::Vector2   UVRotate1;
    asdx::Vector2   UVRotate2;
    asdx::Vector2   UVRotate3;

    asdx::Vector2   UVScroll0;
    asdx::Vector2   UVScroll1;
    asdx::Vector2   UVScroll2;
    asdx::Vector2   UVScroll3;

    float   AlphaThreshold0;
    float   AlphaThreshold1;
    float   AlphaThreshold2;
    float   AlphaThreshold3;

    asdx::Vector4   Color0;
    asdx::Vector4   Color1;
    asdx::Vector4   Color2;
    asdx::Vector4   Color3;

    asdx::Vector4   ColorScale0;
    asdx::Vector4   ColorScale1;
    asdx::Vector4   ColorScale2;
    asdx::Vector4   ColorScale3;
};

///////////////////////////////////////////////////////////////////////////////
// CbUser structure
///////////////////////////////////////////////////////////////////////////////
struct CbUser
{
    int     CustomBool_0;
    int     CustomBool_1;
    int     CustomBool_2;
    int     CustomBool_3;
    int     CustomBool_4;
    int     CustomBool_5;
    int     CustomBool_6;
    int     CustomBool_7;

    asdx::Vector2   CustomFloat2_0;
    asdx::Vector2   CustomFloat2_1;
    asdx::Vector2   CustomFloat2_2;
    asdx::Vector2   CustomFloat2_3;
    asdx::Vector2   CustomFloat2_4;
    asdx::Vector2   CustomFloat2_5;
    asdx::Vector2   CustomFloat2_6;
    asdx::Vector2   CustomFloat2_7;

    asdx::Vector3   CustomFloat3_0;
    float           CustomFloat_0;
    asdx::Vector3   CustomFloat3_1;
    float           CustomFloat_1;
    asdx::Vector3   CustomFloat3_2;
    float           CustomFloat_2;
    asdx::Vector3   CustomFloat3_3;
    float           CustomFloat_3;
    asdx::Vector3   CustomFloat3_4;
    float           CustomFloat_4;
    asdx::Vector3   CustomFloat3_5;
    float           CustomFloat_5;
    asdx::Vector3   CustomFloat3_6;
    float           CustomFloat_6;
    asdx::Vector3   CustomFloat3_7;
    float           CustomFloat_7;

    asdx::Vector4   CustomFloat4_0;
    asdx::Vector4   CustomFloat4_1;
    asdx::Vector4   CustomFloat4_2;
    asdx::Vector4   CustomFloat4_3;
    asdx::Vector4   CustomFloat4_4;
    asdx::Vector4   CustomFloat4_5;
    asdx::Vector4   CustomFloat4_6;
    asdx::Vector4   CustomFloat4_7;
};

//-----------------------------------------------------------------------------
//      ラジアンに変換します.
//-----------------------------------------------------------------------------
asdx::Vector2 ToRadian(const asdx::Vector2& value)
{
    return asdx::Vector2(
        asdx::ToRadian(value.x),
        asdx::ToRadian(value.y));
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// EditorMaterial class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EditorMaterial::EditorMaterial()
: m_Hash(0)
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EditorMaterial::~EditorMaterial()
{
}

//-----------------------------------------------------------------------------
//      初期化処理です.
//-----------------------------------------------------------------------------
bool EditorMaterial::Init(const char* path)
{

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理です.
//-----------------------------------------------------------------------------
void EditorMaterial::Term()
{
}

//-----------------------------------------------------------------------------
//      マテリアルハッシュを取得します.
//-----------------------------------------------------------------------------
uint32_t EditorMaterial::GetHash() const
{ return m_Hash; }

//-----------------------------------------------------------------------------
//      定数バッファを更新します.
//-----------------------------------------------------------------------------
void EditorMaterial::Update()
{
    m_CB    .SwapBuffer();
    m_UserCB.SwapBuffer();

    {
        auto ptr = m_CB.MapAs<CbMaterial>();
        ptr->CastShadow         = CastShadow    .GetValue() ? 1 : 0;
        ptr->ReceiveShadow      = ReceiveShadow .GetValue() ? 1 : 0;
        ptr->EnableDither       = Dither        .GetValue() ? 1 : 0;
        ptr->DisplayFace        = DisplayFace   .GetValue();
        ptr->UVScale0           = UVScale0.GetValue();
        ptr->UVScale1           = UVScale1.GetValue();
        ptr->UVScale2           = UVScale2.GetValue();
        ptr->UVScale3           = UVScale3.GetValue();
        ptr->UVRotate0          = ToRadian(UVRotate0.GetValue());
        ptr->UVRotate1          = ToRadian(UVRotate1.GetValue());
        ptr->UVRotate2          = ToRadian(UVRotate2.GetValue());
        ptr->UVRotate3          = ToRadian(UVRotate3.GetValue());
        ptr->UVScroll0          = UVScroll0.GetValue();
        ptr->UVScroll1          = UVScroll1.GetValue();
        ptr->UVScroll2          = UVScroll2.GetValue();
        ptr->UVScroll3          = UVScroll3.GetValue();
        ptr->AlphaThreshold0    = AlphaThreshold0.GetValue();
        ptr->AlphaThreshold1    = AlphaThreshold1.GetValue();
        ptr->AlphaThreshold2    = AlphaThreshold2.GetValue();
        ptr->AlphaThreshold3    = AlphaThreshold3.GetValue();
        ptr->Color0             = Color0.GetValue();
        ptr->Color1             = Color1.GetValue();
        ptr->Color2             = Color2.GetValue();
        ptr->Color3             = Color3.GetValue();
        ptr->ColorScale0        = ColorScale0.GetValue();
        ptr->ColorScale1        = ColorScale1.GetValue();
        ptr->ColorScale2        = ColorScale2.GetValue();
        ptr->ColorScale3        = ColorScale3.GetValue();
        m_CB.Unmap();
    }

    {
        auto ptr = m_UserCB.MapAs<CbUser>();
        ptr->CustomBool_0 = CustomBool_0.GetValue() ? 1 : 0;
        ptr->CustomBool_1 = CustomBool_1.GetValue() ? 1 : 0;
        ptr->CustomBool_2 = CustomBool_2.GetValue() ? 1 : 0;
        ptr->CustomBool_3 = CustomBool_3.GetValue() ? 1 : 0;
        ptr->CustomBool_4 = CustomBool_4.GetValue() ? 1 : 0;
        ptr->CustomBool_5 = CustomBool_5.GetValue() ? 1 : 0;
        ptr->CustomBool_6 = CustomBool_6.GetValue() ? 1 : 0;
        ptr->CustomBool_7 = CustomBool_7.GetValue() ? 1 : 0;

        ptr->CustomFloat_0 = CustomFloat_0.GetValue();
        ptr->CustomFloat_1 = CustomFloat_1.GetValue();
        ptr->CustomFloat_2 = CustomFloat_2.GetValue();
        ptr->CustomFloat_3 = CustomFloat_3.GetValue();
        ptr->CustomFloat_4 = CustomFloat_4.GetValue();
        ptr->CustomFloat_5 = CustomFloat_5.GetValue();
        ptr->CustomFloat_6 = CustomFloat_6.GetValue();
        ptr->CustomFloat_7 = CustomFloat_7.GetValue();

        ptr->CustomFloat2_0 = CustomFloat2_0.GetValue();
        ptr->CustomFloat2_1 = CustomFloat2_1.GetValue();
        ptr->CustomFloat2_2 = CustomFloat2_2.GetValue();
        ptr->CustomFloat2_3 = CustomFloat2_3.GetValue();
        ptr->CustomFloat2_4 = CustomFloat2_4.GetValue();
        ptr->CustomFloat2_5 = CustomFloat2_5.GetValue();
        ptr->CustomFloat2_6 = CustomFloat2_6.GetValue();
        ptr->CustomFloat2_7 = CustomFloat2_7.GetValue();

        ptr->CustomFloat3_0 = CustomFloat3_0.GetValue();
        ptr->CustomFloat3_1 = CustomFloat3_1.GetValue();
        ptr->CustomFloat3_2 = CustomFloat3_2.GetValue();
        ptr->CustomFloat3_3 = CustomFloat3_3.GetValue();
        ptr->CustomFloat3_4 = CustomFloat3_4.GetValue();
        ptr->CustomFloat3_5 = CustomFloat3_5.GetValue();
        ptr->CustomFloat3_6 = CustomFloat3_6.GetValue();
        ptr->CustomFloat3_7 = CustomFloat3_7.GetValue();

        ptr->CustomFloat4_0 = CustomFloat4_0.GetValue();
        ptr->CustomFloat4_1 = CustomFloat4_1.GetValue();
        ptr->CustomFloat4_2 = CustomFloat4_2.GetValue();
        ptr->CustomFloat4_3 = CustomFloat4_3.GetValue();
        ptr->CustomFloat4_4 = CustomFloat4_4.GetValue();
        ptr->CustomFloat4_5 = CustomFloat4_5.GetValue();
        ptr->CustomFloat4_6 = CustomFloat4_6.GetValue();
        ptr->CustomFloat4_7 = CustomFloat4_7.GetValue();

        m_UserCB.Unmap();
    }
}

//-----------------------------------------------------------------------------
//      テクスチャと定数バッファを設定します.
//-----------------------------------------------------------------------------
void EditorMaterial::Bind(ID3D12GraphicsCommandList* pCmd)
{
    //  エディタ
}

//-----------------------------------------------------------------------------
//      GUIを描画します.
//-----------------------------------------------------------------------------
void DrawParam(EditorMaterial& material, const EditorShader& shader, const char* defaultPath)
{
    if (material.GetHash() != shader.GetHash())
    { return; }

    auto& setting = shader.GetSetting();

    material.Visble         .DrawCheckbox(u8"表示");
    material.CastShadow     .DrawCheckbox(u8"シャドウキャスト");
    material.ReceiveShadow  .DrawCheckbox(u8"レシーブシャドウ");
    material.Dither         .DrawCheckbox(u8"ディザ抜き");
    material.DisplayFace    .DrawCombo(u8"表示面", 3, kDisplayFace);
    material.State          .DrawCombo(u8"ステート", 3, kState);

    if (setting.EnableBaseColorMap0) { material.BaseColorMap0.DrawControl(u8"ベースカラー0", defaultPath); }
    if (setting.EnableBaseColorMap1) { material.BaseColorMap1.DrawControl(u8"ベースカラー1", defaultPath); }
    if (setting.EnableBaseColorMap2) { material.BaseColorMap2.DrawControl(u8"ベースカラー2", defaultPath); }
    if (setting.EnableBaseColorMap3) { material.BaseColorMap3.DrawControl(u8"ベースカラー3", defaultPath); }

    if (setting.EnableORMMap0) { material.ORMMap0.DrawControl(u8"ORMマップ0", defaultPath); }
    if (setting.EnableORMMap1) { material.ORMMap1.DrawControl(u8"ORMマップ1", defaultPath); }
    if (setting.EnableORMMap2) { material.ORMMap2.DrawControl(u8"ORMマップ2", defaultPath); }
    if (setting.EnableORMMap3) { material.ORMMap3.DrawControl(u8"ORMマップ3", defaultPath); }

    if (setting.EnableNormalMap0) { material.NormalMap0.DrawControl(u8"法線マップ0", defaultPath); }
    if (setting.EnableNormalMap1) { material.NormalMap1.DrawControl(u8"法線マップ0", defaultPath); }
    if (setting.EnableNormalMap2) { material.NormalMap2.DrawControl(u8"法線マップ0", defaultPath); }
    if (setting.EnableNormalMap3) { material.NormalMap3.DrawControl(u8"法線マップ0", defaultPath); }

    if (setting.EnableOpacityMap0) { material.OpacityMap0.DrawControl(u8"不透明0", defaultPath); }
    if (setting.EnableOpacityMap1) { material.OpacityMap1.DrawControl(u8"不透明1", defaultPath); }
    if (setting.EnableOpacityMap2) { material.OpacityMap2.DrawControl(u8"不透明2", defaultPath); }
    if (setting.EnableOpacityMap3) { material.OpacityMap3.DrawControl(u8"不透明3", defaultPath); }

    if (setting.EnableMaskMap0) { material.MaskMap0.DrawControl(u8"マスク0", defaultPath); }
    if (setting.EnableMaskMap1) { material.MaskMap1.DrawControl(u8"マスク1", defaultPath); }
    if (setting.EnableMaskMap2) { material.MaskMap2.DrawControl(u8"マスク2", defaultPath); }
    if (setting.EnableMaskMap3) { material.MaskMap3.DrawControl(u8"マスク3", defaultPath); }

    if (setting.EnableEmissiveMap0) { material.EmissiveMap0.DrawControl(u8"エミッシブ0", defaultPath); }
    if (setting.EnableEmissiveMap1) { material.EmissiveMap1.DrawControl(u8"エミッシブ1", defaultPath); }
    if (setting.EnableEmissiveMap2) { material.EmissiveMap2.DrawControl(u8"エミッシブ2", defaultPath); }
    if (setting.EnableEmissiveMap3) { material.EmissiveMap3.DrawControl(u8"エミッシブ3", defaultPath); }

    if (setting.EnableCurvatureMap) { material.CurvatureMap.DrawControl(u8"曲率マップ", defaultPath); }
    if (setting.EnableDistortionMap) { material.DistortionMap.DrawControl(u8"歪みマップ", defaultPath); }
    if (setting.EnableFlowMap) { material.FlowMap.DrawControl(u8"フローマップ", defaultPath); }

    if (setting.CustomMap0.Enable) { material.CustomMap0.DrawControl(setting.CustomMap0.Label.c_str(), defaultPath); }
    if (setting.CustomMap1.Enable) { material.CustomMap0.DrawControl(setting.CustomMap1.Label.c_str(), defaultPath); }
    if (setting.CustomMap2.Enable) { material.CustomMap0.DrawControl(setting.CustomMap2.Label.c_str(), defaultPath); }
    if (setting.CustomMap3.Enable) { material.CustomMap0.DrawControl(setting.CustomMap3.Label.c_str(), defaultPath); }
    if (setting.CustomMap4.Enable) { material.CustomMap0.DrawControl(setting.CustomMap4.Label.c_str(), defaultPath); }
    if (setting.CustomMap5.Enable) { material.CustomMap0.DrawControl(setting.CustomMap0.Label.c_str(), defaultPath); }
    if (setting.CustomMap6.Enable) { material.CustomMap0.DrawControl(setting.CustomMap0.Label.c_str(), defaultPath); }
    if (setting.CustomMap7.Enable) { material.CustomMap0.DrawControl(setting.CustomMap0.Label.c_str(), defaultPath); }

    if (setting.EnableUVScale0) { material.UVScale0.DrawSlider(u8"UVスケール0"); }
    if (setting.EnableUVScale1) { material.UVScale1.DrawSlider(u8"UVスケール1"); }
    if (setting.EnableUVScale2) { material.UVScale2.DrawSlider(u8"UVスケール2"); }
    if (setting.EnableUVScale3) { material.UVScale3.DrawSlider(u8"UVスケール3"); }

    if (setting.EnableUVRotate0) { material.UVRotate0.DrawSlider(u8"UV回転0", 0.1f, -360.0f, 360.0f); }
    if (setting.EnableUVRotate1) { material.UVRotate1.DrawSlider(u8"UV回転1", 0.1f, -360.0f, 360.0f); }
    if (setting.EnableUVRotate2) { material.UVRotate2.DrawSlider(u8"UV回転2", 0.1f, -360.0f, 360.0f); }
    if (setting.EnableUVRotate3) { material.UVRotate3.DrawSlider(u8"UV回転3", 0.1f, -360.0f, 360.0f); }

    if (setting.EnableUVScroll0) { material.UVScroll0.DrawSlider(u8"UVスクロール0"); }
    if (setting.EnableUVScroll1) { material.UVScroll1.DrawSlider(u8"UVスクロール1"); }
    if (setting.EnableUVScroll2) { material.UVScroll2.DrawSlider(u8"UVスクロール2"); }
    if (setting.EnableUVScroll3) { material.UVScroll3.DrawSlider(u8"UVスクロール3"); }

    if (setting.EnableAlphaThreshold0) { material.AlphaThreshold0.DrawSlider(u8"α閾値0", 0.1f, 0.0f, 1.0f); }
    if (setting.EnableAlphaThreshold1) { material.AlphaThreshold1.DrawSlider(u8"α閾値1", 0.1f, 0.0f, 1.0f); }
    if (setting.EnableAlphaThreshold2) { material.AlphaThreshold2.DrawSlider(u8"α閾値2", 0.1f, 0.0f, 1.0f); }
    if (setting.EnableAlphaThreshold3) { material.AlphaThreshold3.DrawSlider(u8"α閾値3", 0.1f, 0.0f, 1.0f); }

    if (setting.EnableColor0) { material.Color0.DrawPicker(u8"カラー0"); }
    if (setting.EnableColor1) { material.Color1.DrawPicker(u8"カラー1"); }
    if (setting.EnableColor2) { material.Color2.DrawPicker(u8"カラー2"); }
    if (setting.EnableColor3) { material.Color3.DrawPicker(u8"カラー3"); }

    if (setting.EnableColorScale0) { material.ColorScale0.DrawSlider(u8"カラースケール0", 0.1f, 0.0f, FLT_MAX); }
    if (setting.EnableColorScale1) { material.ColorScale1.DrawSlider(u8"カラースケール1", 0.1f, 0.0f, FLT_MAX); }
    if (setting.EnableColorScale2) { material.ColorScale2.DrawSlider(u8"カラースケール2", 0.1f, 0.0f, FLT_MAX); }
    if (setting.EnableColorScale3) { material.ColorScale3.DrawSlider(u8"カラースケール3", 0.1f, 0.0f, FLT_MAX); }

    if (setting.CustomBool_0.Enable){ material.CustomBool_0.DrawCheckbox(setting.CustomBool_0.Label.c_str()); }
    if (setting.CustomBool_1.Enable){ material.CustomBool_1.DrawCheckbox(setting.CustomBool_1.Label.c_str()); }
    if (setting.CustomBool_2.Enable){ material.CustomBool_2.DrawCheckbox(setting.CustomBool_2.Label.c_str()); }
    if (setting.CustomBool_3.Enable){ material.CustomBool_3.DrawCheckbox(setting.CustomBool_3.Label.c_str()); }
    if (setting.CustomBool_4.Enable){ material.CustomBool_4.DrawCheckbox(setting.CustomBool_4.Label.c_str()); }
    if (setting.CustomBool_5.Enable){ material.CustomBool_5.DrawCheckbox(setting.CustomBool_5.Label.c_str()); }
    if (setting.CustomBool_6.Enable){ material.CustomBool_6.DrawCheckbox(setting.CustomBool_6.Label.c_str()); }
    if (setting.CustomBool_7.Enable){ material.CustomBool_7.DrawCheckbox(setting.CustomBool_7.Label.c_str()); }

    if (setting.CustomFloat_0.Enable){ material.CustomFloat_0.DrawSlider(setting.CustomFloat_0.Label.c_str(), setting.CustomFloat_0.Step, setting.CustomFloat_0.Min, setting.CustomFloat_0.Max); }
    if (setting.CustomFloat_1.Enable){ material.CustomFloat_1.DrawSlider(setting.CustomFloat_1.Label.c_str(), setting.CustomFloat_1.Step, setting.CustomFloat_1.Min, setting.CustomFloat_1.Max); }
    if (setting.CustomFloat_2.Enable){ material.CustomFloat_2.DrawSlider(setting.CustomFloat_2.Label.c_str(), setting.CustomFloat_2.Step, setting.CustomFloat_2.Min, setting.CustomFloat_2.Max); }
    if (setting.CustomFloat_3.Enable){ material.CustomFloat_3.DrawSlider(setting.CustomFloat_3.Label.c_str(), setting.CustomFloat_3.Step, setting.CustomFloat_3.Min, setting.CustomFloat_3.Max); }
    if (setting.CustomFloat_4.Enable){ material.CustomFloat_4.DrawSlider(setting.CustomFloat_4.Label.c_str(), setting.CustomFloat_4.Step, setting.CustomFloat_4.Min, setting.CustomFloat_4.Max); }
    if (setting.CustomFloat_5.Enable){ material.CustomFloat_5.DrawSlider(setting.CustomFloat_5.Label.c_str(), setting.CustomFloat_5.Step, setting.CustomFloat_5.Min, setting.CustomFloat_5.Max); }
    if (setting.CustomFloat_6.Enable){ material.CustomFloat_6.DrawSlider(setting.CustomFloat_6.Label.c_str(), setting.CustomFloat_6.Step, setting.CustomFloat_6.Min, setting.CustomFloat_6.Max); }
    if (setting.CustomFloat_7.Enable){ material.CustomFloat_7.DrawSlider(setting.CustomFloat_7.Label.c_str(), setting.CustomFloat_7.Step, setting.CustomFloat_7.Min, setting.CustomFloat_7.Max); }

    if (setting.CustomFloat2_0.Enable){ material.CustomFloat2_0.DrawSlider(setting.CustomFloat2_0.Label.c_str(), setting.CustomFloat2_0.Step, setting.CustomFloat2_0.Min, setting.CustomFloat2_0.Max); }
    if (setting.CustomFloat2_1.Enable){ material.CustomFloat2_1.DrawSlider(setting.CustomFloat2_1.Label.c_str(), setting.CustomFloat2_1.Step, setting.CustomFloat2_1.Min, setting.CustomFloat2_1.Max); }
    if (setting.CustomFloat2_2.Enable){ material.CustomFloat2_2.DrawSlider(setting.CustomFloat2_2.Label.c_str(), setting.CustomFloat2_2.Step, setting.CustomFloat2_2.Min, setting.CustomFloat2_2.Max); }
    if (setting.CustomFloat2_3.Enable){ material.CustomFloat2_3.DrawSlider(setting.CustomFloat2_3.Label.c_str(), setting.CustomFloat2_3.Step, setting.CustomFloat2_3.Min, setting.CustomFloat2_3.Max); }
    if (setting.CustomFloat2_4.Enable){ material.CustomFloat2_4.DrawSlider(setting.CustomFloat2_4.Label.c_str(), setting.CustomFloat2_4.Step, setting.CustomFloat2_4.Min, setting.CustomFloat2_4.Max); }
    if (setting.CustomFloat2_5.Enable){ material.CustomFloat2_5.DrawSlider(setting.CustomFloat2_5.Label.c_str(), setting.CustomFloat2_5.Step, setting.CustomFloat2_5.Min, setting.CustomFloat2_5.Max); }
    if (setting.CustomFloat2_6.Enable){ material.CustomFloat2_6.DrawSlider(setting.CustomFloat2_6.Label.c_str(), setting.CustomFloat2_6.Step, setting.CustomFloat2_6.Min, setting.CustomFloat2_6.Max); }
    if (setting.CustomFloat2_7.Enable){ material.CustomFloat2_7.DrawSlider(setting.CustomFloat2_7.Label.c_str(), setting.CustomFloat2_7.Step, setting.CustomFloat2_7.Min, setting.CustomFloat2_7.Max); }

    if (setting.CustomFloat3_0.Enable){ material.CustomFloat3_0.DrawSlider(setting.CustomFloat3_0.Label.c_str(), setting.CustomFloat3_0.Step, setting.CustomFloat3_0.Min, setting.CustomFloat3_0.Max); }
    if (setting.CustomFloat3_1.Enable){ material.CustomFloat3_1.DrawSlider(setting.CustomFloat3_1.Label.c_str(), setting.CustomFloat3_1.Step, setting.CustomFloat3_1.Min, setting.CustomFloat3_1.Max); }
    if (setting.CustomFloat3_2.Enable){ material.CustomFloat3_2.DrawSlider(setting.CustomFloat3_2.Label.c_str(), setting.CustomFloat3_2.Step, setting.CustomFloat3_2.Min, setting.CustomFloat3_2.Max); }
    if (setting.CustomFloat3_3.Enable){ material.CustomFloat3_3.DrawSlider(setting.CustomFloat3_3.Label.c_str(), setting.CustomFloat3_3.Step, setting.CustomFloat3_3.Min, setting.CustomFloat3_3.Max); }
    if (setting.CustomFloat3_4.Enable){ material.CustomFloat3_4.DrawSlider(setting.CustomFloat3_4.Label.c_str(), setting.CustomFloat3_4.Step, setting.CustomFloat3_4.Min, setting.CustomFloat3_4.Max); }
    if (setting.CustomFloat3_5.Enable){ material.CustomFloat3_5.DrawSlider(setting.CustomFloat3_5.Label.c_str(), setting.CustomFloat3_5.Step, setting.CustomFloat3_5.Min, setting.CustomFloat3_5.Max); }
    if (setting.CustomFloat3_6.Enable){ material.CustomFloat3_6.DrawSlider(setting.CustomFloat3_6.Label.c_str(), setting.CustomFloat3_6.Step, setting.CustomFloat3_6.Min, setting.CustomFloat3_6.Max); }
    if (setting.CustomFloat3_7.Enable){ material.CustomFloat3_7.DrawSlider(setting.CustomFloat3_7.Label.c_str(), setting.CustomFloat3_7.Step, setting.CustomFloat3_7.Min, setting.CustomFloat3_7.Max); }

    if (setting.CustomFloat4_0.Enable){ material.CustomFloat4_0.DrawSlider(setting.CustomFloat4_0.Label.c_str(), setting.CustomFloat4_0.Step, setting.CustomFloat4_0.Min, setting.CustomFloat4_0.Max); }
    if (setting.CustomFloat4_1.Enable){ material.CustomFloat4_1.DrawSlider(setting.CustomFloat4_1.Label.c_str(), setting.CustomFloat4_1.Step, setting.CustomFloat4_1.Min, setting.CustomFloat4_1.Max); }
    if (setting.CustomFloat4_2.Enable){ material.CustomFloat4_2.DrawSlider(setting.CustomFloat4_2.Label.c_str(), setting.CustomFloat4_2.Step, setting.CustomFloat4_2.Min, setting.CustomFloat4_2.Max); }
    if (setting.CustomFloat4_3.Enable){ material.CustomFloat4_3.DrawSlider(setting.CustomFloat4_3.Label.c_str(), setting.CustomFloat4_3.Step, setting.CustomFloat4_3.Min, setting.CustomFloat4_3.Max); }
    if (setting.CustomFloat4_4.Enable){ material.CustomFloat4_4.DrawSlider(setting.CustomFloat4_4.Label.c_str(), setting.CustomFloat4_4.Step, setting.CustomFloat4_4.Min, setting.CustomFloat4_4.Max); }
    if (setting.CustomFloat4_5.Enable){ material.CustomFloat4_5.DrawSlider(setting.CustomFloat4_5.Label.c_str(), setting.CustomFloat4_5.Step, setting.CustomFloat4_5.Min, setting.CustomFloat4_5.Max); }
    if (setting.CustomFloat4_6.Enable){ material.CustomFloat4_6.DrawSlider(setting.CustomFloat4_6.Label.c_str(), setting.CustomFloat4_6.Step, setting.CustomFloat4_6.Min, setting.CustomFloat4_6.Max); }
    if (setting.CustomFloat4_7.Enable){ material.CustomFloat4_7.DrawSlider(setting.CustomFloat4_7.Label.c_str(), setting.CustomFloat4_7.Step, setting.CustomFloat4_7.Min, setting.CustomFloat4_7.Max); }
}


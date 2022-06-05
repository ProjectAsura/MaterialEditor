//---------------------------------------------------------"--------------------
// File : Config.cpp
// Desc : Configuration.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Config.h>
#include <imgui.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxLocalization.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const asdx::Localization kCameraTypeLabel[] = {
    asdx::Localization(u8"注視点モード", u8"LookAt Mode"),
    asdx::Localization(u8"ウォークスルーモード", u8"Walktrough Mode"),
};
static const asdx::Localization kTagBackGround(u8"背景", u8"Background");
static const asdx::Localization kTagClearColor(u8"クリアカラー", u8"Clear Color");
static const asdx::Localization kTagShowTexture(u8"背景画像表示", u8"Show Texture");
static const asdx::Localization kTagCamera(u8"カメラ", u8"Camera");
static const asdx::Localization kTagType(u8"タイプ", u8"Type");
static const asdx::Localization kTagFovY(u8"垂直画角", u8"Field Of View Y");
static const asdx::Localization kTagNearClip(u8"ニアクリップ平面", u8"Near Clip");
static const asdx::Localization kTagFarClip(u8"ファークリップ平面", u8"Far Clip");
static const asdx::Localization kTagRotationGain(u8"回転量", u8"Rotation Gain");
static const asdx::Localization kTagDollyGain(u8"ドリー量", u8"Dolly Gain");
static const asdx::Localization kTagMoveGain(u8"移動量", u8"Move Gain");
static const asdx::Localization kTagWheelGain(u8"ホイール量", u8"Wheel Gain");
static const asdx::Localization kTagModelPreview(u8"モデルプレビュー", u8"Preview Model");
static const asdx::Localization kTagScale(u8"拡大縮小", u8"Scale");
static const asdx::Localization kTagRotation(u8"回転", u8"Rotation");
static const asdx::Localization kTagTranslation(u8"平行移動", u8"Transliation");
static const asdx::Localization kTagAutoTurn(u8"自動回転", u8"Auto Turn");
static const asdx::Localization kTagAutoTurnSpeed(u8"自動回転速度", u8"Auto Turn Speed");
static const asdx::Localization kTagDebug(u8"デバッグ", u8"Debug");
static const asdx::Localization kTagDrawBone(u8"ボーンの表示", u8"Draw Bone");
static const asdx::Localization kTagDrawSunLightDir(u8"サンライト方向の表示", u8"Sun Light Direction");
static const asdx::Localization kTagDrawGrid(u8"グリッドの表示", u8"Draw Grid");
static const asdx::Localization kTagDrawAxis(u8"座標軸の表示", u8"Draw Axis");

} // namespace


///////////////////////////////////////////////////////////////////////////////
// PanelSetting structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* PanelSetting::Serialize(tinyxml2::XMLDocument* doc, const char* tag)
{
    auto e = doc->NewElement(tag);
    e->SetAttribute("open", Open);
    e->SetAttribute("x", Pos.x);
    e->SetAttribute("y", Pos.y);
    e->SetAttribute("w", Size.x);
    e->SetAttribute("h", Size.y);
    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void PanelSetting::Deserialize(tinyxml2::XMLElement* element, const char* tag)
{
    auto e = element->FirstChildElement(tag);
    if (e == nullptr)
    { return; }

    Open    = e->BoolAttribute("open");
    Pos.x   = e->FloatAttribute("x");
    Pos.y   = e->FloatAttribute("y");
    Size.x  = e->FloatAttribute("w");
    Size.y  = e->FloatAttribute("h");
}

///////////////////////////////////////////////////////////////////////////////
// BackgroundSetting
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
BackgroundSetting::BackgroundSetting()
: ClearColor (0.18f, 0.18f, 0.18f)
, ShowTexture(false)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* BackgroundSetting::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("BackgroundSetting");
    e->InsertEndChild(asdx::Serialize(doc, "ClearColor",  ClearColor));
    e->InsertEndChild(asdx::Serialize(doc, "ShowTexture", ShowTexture));
    return e;
}


//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void BackgroundSetting::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("BackgroundSetting");
    if (e == nullptr)
    { return; }

    asdx::Deserialize(e, "ClearColor",  ClearColor);
    asdx::Deserialize(e, "ShowTexture", ShowTexture);
}

//-----------------------------------------------------------------------------
//      編集処理を行います.
//-----------------------------------------------------------------------------
void BackgroundSetting::Edit()
{
    if (!ImGui::CollapsingHeader(kTagBackGround.c_str()))
    { return; }

    ClearColor.DrawPicker(kTagClearColor.c_str());
    ShowTexture.DrawCheckbox(kTagShowTexture.c_str());
}


///////////////////////////////////////////////////////////////////////////////
// CameraSetting
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
CameraSetting::CameraSetting()
: Type          (0)
, FieldOfView   (37.5f)
, NearClip      (0.1f)
, FarClip       (100000.0f)
, RotateGain    (0.01f)
, DollyGain     (0.25)
, MoveGain      (0.1f)
, WheelGain     (20.0f)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* CameraSetting::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("CameraSetting");
    e->InsertEndChild(asdx::Serialize(doc, "Type", Type));
    e->InsertEndChild(asdx::Serialize(doc, "FieldOfView", FieldOfView));
    e->InsertEndChild(asdx::Serialize(doc, "NearClip", NearClip));
    e->InsertEndChild(asdx::Serialize(doc, "FarClip", FarClip));
    e->InsertEndChild(asdx::Serialize(doc, "RotateGain", RotateGain));
    e->InsertEndChild(asdx::Serialize(doc, "DollyGain", DollyGain));
    e->InsertEndChild(asdx::Serialize(doc, "MoveGain", MoveGain));
    e->InsertEndChild(asdx::Serialize(doc, "WheelGain", WheelGain));
    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void CameraSetting::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("CameraSetting");
    if (e == nullptr)
    { return; }

    asdx::Deserialize(e, "Type", Type);
    asdx::Deserialize(e, "FieldOfView", FieldOfView);
    asdx::Deserialize(e, "NearClip", NearClip);
    asdx::Deserialize(e, "FarClip", FarClip);
    asdx::Deserialize(e, "RotateGain", RotateGain);
    asdx::Deserialize(e, "DollyGain", DollyGain);
    asdx::Deserialize(e, "MoveGain", MoveGain);
    asdx::Deserialize(e, "WheelGain", WheelGain);
}

//-----------------------------------------------------------------------------
//      編集処理を行います.
//-----------------------------------------------------------------------------
void CameraSetting::Edit()
{
    if (!ImGui::CollapsingHeader(kTagCamera.c_str()))
    { return; }

    Type.DrawCombo(kTagType.c_str(), _countof(kCameraTypeLabel), kCameraTypeLabel);

    FieldOfView .DrawSlider(kTagFovY.c_str(), 0.1f, 1.0f, 360.0f);
    NearClip    .DrawSlider(kTagNearClip.c_str(), 0.1f, 0.01f, 1000000.0f);
    FarClip     .DrawSlider(kTagFarClip.c_str(), 1.0f, 0.01f, 10000000.0f);
    RotateGain  .DrawSlider(kTagRotationGain.c_str(), 0.001f, -100.0f, 100.0f);
    DollyGain   .DrawSlider(kTagDollyGain.c_str(), 0.001f, -100.0f, 100.0f);
    MoveGain    .DrawSlider(kTagMoveGain.c_str(), 0.001f, -100.0f, 100.0f);
    WheelGain   .DrawSlider(kTagWheelGain.c_str(), 0.001f, -100.0f, 100.0f);
}


///////////////////////////////////////////////////////////////////////////////
// ModelPreviewSetting structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
ModelPreviewSetting::ModelPreviewSetting()
: Scale             (1.0f, 1.0f, 1.0f)
, Rotation          (0.0f, 0.0f, 0.0f)
, Translation       (0.0f, 0.0f, 0.0f)
, AutoRotation      (false)
, AutoRotationSpeed (0.0f)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* ModelPreviewSetting::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("ModelPreview");
    e->InsertEndChild(asdx::Serialize(doc, "Scale", Scale));
    e->InsertEndChild(asdx::Serialize(doc, "Rotation", Rotation));
    e->InsertEndChild(asdx::Serialize(doc, "Translation", Translation));
    e->InsertEndChild(asdx::Serialize(doc, "AutoRotation", AutoRotation));
    e->InsertEndChild(asdx::Serialize(doc, "AutoRotationSpeed", AutoRotationSpeed));
    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void ModelPreviewSetting::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("ModelPreview");
    if (e == nullptr)
    { return; }

    asdx::Deserialize(e, "Scale", Scale);
    asdx::Deserialize(e, "Rotation", Rotation);
    asdx::Deserialize(e, "Translation", Translation);
    asdx::Deserialize(e, "AutoRotation", AutoRotation);
    asdx::Deserialize(e, "AutoRotationSpeed", AutoRotationSpeed);
}

//-----------------------------------------------------------------------------
//      編集処理を行います.
//-----------------------------------------------------------------------------
void ModelPreviewSetting::Edit()
{
    if (!ImGui::CollapsingHeader(kTagModelPreview.c_str()))
    { return; }

    Scale            .DrawSlider(kTagScale.c_str());
    Rotation         .DrawSlider(kTagRotation.c_str(), 0.1f, -360.0f, 360.0f);
    Translation      .DrawSlider(kTagTranslation.c_str());
    AutoRotation     .DrawCheckbox(kTagAutoTurn.c_str());
    AutoRotationSpeed.DrawSlider(kTagAutoTurnSpeed.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// DebugSetting structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
DebugSetting::DebugSetting()
: DrawBone      (false)
, DrawLightDir  (false)
, DrawGrid      (true)
, DrawAxis      (true)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      シリアライズします.
//-----------------------------------------------------------------------------
tinyxml2::XMLElement* DebugSetting::Serialize(tinyxml2::XMLDocument* doc)
{
    auto e = doc->NewElement("DebugSetting");
    e->InsertEndChild(asdx::Serialize(doc, "DrawBone", DrawBone));
    e->InsertEndChild(asdx::Serialize(doc, "DrawLightDir", DrawLightDir));
    e->InsertEndChild(asdx::Serialize(doc, "DrawGrid", DrawGrid));
    e->InsertEndChild(asdx::Serialize(doc, "DrawAxis", DrawAxis));
    return e;
}

//-----------------------------------------------------------------------------
//      デシリアライズします.
//-----------------------------------------------------------------------------
void DebugSetting::Deserialize(tinyxml2::XMLElement* element)
{
    auto e = element->FirstChildElement("DebugSetting");
    if (e == nullptr)
    { return; }

    asdx::Deserialize(e, "DrawBone", DrawBone);
    asdx::Deserialize(e, "DrawLightDir", DrawLightDir);
    asdx::Deserialize(e, "DrawGrid", DrawGrid);
    asdx::Deserialize(e, "DrawAxis", DrawAxis);
}

//-----------------------------------------------------------------------------
//      編集処理を行います.
//-----------------------------------------------------------------------------
void DebugSetting::Edit()
{
    if (!ImGui::CollapsingHeader(kTagDebug.c_str()))
    { return; }

    DrawBone    .DrawCheckbox(kTagDrawBone.c_str());
    DrawLightDir.DrawCheckbox(kTagDrawSunLightDir.c_str());
    DrawGrid    .DrawCheckbox(kTagDrawGrid.c_str());
    DrawAxis    .DrawCheckbox(kTagDrawAxis.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// Config structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      設定ファイルをロードします.
//-----------------------------------------------------------------------------
bool Config::Load(const char* path)
{
    std::string filePath;
    if (!asdx::SearchFilePathA(path, filePath))
    {
        ELOGA("Error : File Not Found. path = %s", path);
        return false;
    }

    tinyxml2::XMLDocument doc;
    auto ret = doc.LoadFile(filePath.c_str());
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : File Load Failed. path = %s", filePath.c_str());
        return false;
    }

    auto root = doc.FirstChildElement("Config");
    if (root == nullptr)
    {
        ELOGA("Error : Root Node Not Found.");
        return false;
    }

    {
        auto e = root->FirstChildElement("MainWindow");
        if (e != nullptr)
        {
            MainWindowWidth  = e->IntAttribute("w");
            MainWindowHeight = e->IntAttribute("h");
        }
    }

    {
        auto e = root->FirstChildElement("ShowFPS");
        if (e != nullptr)
        {
            ShowFPS = e->BoolAttribute("value");
        }
    }

    PanelEdit.Deserialize(root, "PanelEdit");
    PanelMesh.Deserialize(root, "PanelMesh");

    Background.Deserialize(root);
    Camera.Deserialize(root);
    ModelPreview.Deserialize(root);
    Debug.Deserialize(root);

    return true;
}

//-----------------------------------------------------------------------------
//      設定ファイルをセーブします.
//-----------------------------------------------------------------------------
bool Config::Save(const char* path)
{
    tinyxml2::XMLDocument doc;

    auto decl = doc.NewDeclaration();
    doc.InsertEndChild(decl);

    auto root = doc.NewElement("Config");

    {
        auto e = doc.NewElement("MainWindow");
        e->SetAttribute("w", MainWindowWidth);
        e->SetAttribute("h", MainWindowHeight);
        root->InsertEndChild(e);
    }

    {
        auto e = doc.NewElement("ShowFPS");
        e->SetAttribute("value", ShowFPS);
        root->InsertEndChild(e);
    }

    root->InsertEndChild(PanelEdit.Serialize(&doc, "PanelEdit"));
    root->InsertEndChild(PanelMesh.Serialize(&doc, "PanelMesh"));

    root->InsertEndChild(Background.Serialize(&doc));
    root->InsertEndChild(Camera.Serialize(&doc));
    root->InsertEndChild(ModelPreview.Serialize(&doc));
    root->InsertEndChild(Debug.Serialize(&doc));


    doc.InsertEndChild(root);
    auto ret = doc.SaveFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : Config Save Failed. path = %s", path);
        return false;
    }

    return true;
}
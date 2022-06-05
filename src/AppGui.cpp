//-----------------------------------------------------------------------------
// File : AppGui.cpp
// Desc : Draw GUI.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <asdxGuiMgr.h>
#include <asdxLogger.h>
#include <asdxMisc.h>
#include <asdxImGuiExtension.h>
#include <asdxLocalization.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <AppVersion.h>
#include <Config.h>
#include <LightMgr.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const char* kWorkFilter = "Work File(*.work)\0*.work\0\0";
static const char* kHelpLink = "https://github.com/ProjectAsura/MaterialEditor/blob/master/doc/help.md";
static const char* kReleaseNoteLink = "https://github.com/ProjectAsura/MaterialEditor/blob/master/doc/release_note.md";
static const ImVec4 kRed   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
static const ImVec4 kCyan  = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

static const asdx::Localization kTagPolygonCount(u8"ポリゴン数 : %lu", u8"Polygon Count : %lu");
static const asdx::Localization kTagConnect(u8"接続", u8"Connected");
static const asdx::Localization kTagDisconnect(u8"切断", u8"Disconnected");
static const asdx::Localization kTagFile(u8"ファイル", u8"File");
static const asdx::Localization kTagWorkSpace(u8"ワークスペース", u8"WorkSpace");
static const asdx::Localization kTagOpen(u8"開く", u8"Open");
static const asdx::Localization kTagSaveAs(u8"名前を付けて保存", u8"Save As");
static const asdx::Localization kTagSave(u8"上書き保存", u8"Save");
static const asdx::Localization kTagConnectionSettings(u8"接続設定", u8"Connection Settings");
static const asdx::Localization kTagLicense(u8"ライセンス", u8"License");
static const asdx::Localization kTagVersionInfo(u8"バージョン情報", u8"Version Info");
static const asdx::Localization kTagReleaseNotes(u8"リリースノート", u8"Release Notes");
static const asdx::Localization kTagHelp(u8"ヘルプ", u8"Help");
static const asdx::Localization kTagInformation(u8"情報", u8"Information");
static const asdx::Localization kTagRuntimeLinkage(u8"ランタイム連携", u8"Runtime Linkage");
static const asdx::Localization kTagExport(u8"エクスポート", u8"Export");
static const asdx::Localization kTagExportSetting(u8"エクスポート設定", u8"Export Settings");
static const asdx::Localization kTagMaterial(u8"マテリアル", u8"Material");
static const asdx::Localization kTagNoMaterial(u8"マテリアルが存在しません", u8"No Material");
static const asdx::Localization kTagNameFilter(u8"名前フィルタ", u8"Name Filter");
static const asdx::Localization kTagLight(u8"ライト", u8"Light");
static const asdx::Localization kTagConfig(u8"設定", u8"Config");
static const asdx::Localization kTagEdit(u8"編集", u8"Edit");
static const asdx::Localization kTagLanguage(u8"言語", u8"Language");
static const asdx::Localization kTagJapanese(u8"日本語", u8"Japanese");
static const asdx::Localization kTagEnglish(u8"英語", u8"English");


///////////////////////////////////////////////////////////////////////////////
// OSS structure
///////////////////////////////////////////////////////////////////////////////
struct OSS
{
    const char* Name;
    const char* Url;
};
static OSS kLicenses[] = {
    {"Dear Imgui"       , "https://github.com/ocornut/imgui/blob/master/LICENSE.txt"},
    {"ImgGuizmo"        , "https://github.com/CedricGuillemet/ImGuizmo/blob/master/LICENSE"},
    {"TinyXML-2"        , "https://github.com/leethomason/tinyxml2/blob/master/LICENSE.txt"},
    {"meshoptimizer"    , "https://github.com/zeux/meshoptimizer/blob/master/LICENSE.md"},
    {"DirectXTex"       , "https://github.com/microsoft/DirectXTex/blob/master/LICENSE"},
    {"assimp"           , "https://github.com/assimp/assimp/blob/master/LICENSE"},
};


///////////////////////////////////////////////////////////////////////////////
// MenuContext structure
///////////////////////////////////////////////////////////////////////////////
struct MenuContext
{
    bool                ShowLicense = false;
    WorkSpace*          pWorkSpace = nullptr;
    Config*             pConfig = nullptr;
};


//-----------------------------------------------------------------------------
//      ハイパーリンクを描画します.
//-----------------------------------------------------------------------------
void ImGuiHyperLink(const char* url)
{
    if (ImGuiClickableText(url))
    { ::ShellExecuteA(nullptr, "open", url, "", "", SW_SHOWNORMAL); }
}

//-----------------------------------------------------------------------------
//      FPSを表示します.
//-----------------------------------------------------------------------------
void DrawFPS(float fps, uint64_t polygonCount, bool connect)
{
    ImGui::SetNextWindowPos (ImVec2(10,  10));
    ImGui::SetNextWindowSize(ImVec2(160, 60));

    auto flags = ImGuiWindowFlags_NoMove
               | ImGuiWindowFlags_NoResize
               | ImGuiWindowFlags_NoTitleBar;
    if (!ImGui::Begin(u8"FrameInfo", nullptr, flags))
    { return; }

    // FPS
    ImGui::Text(u8"FPS : %.2f", fps);

    // ポリゴン数
    ImGui::Text(kTagPolygonCount.c_str(), polygonCount);

    if (connect)
    { ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), kTagConnect.c_str()); }
    else
    { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), kTagDisconnect.c_str()); }

    ImGui::End();
}

//-----------------------------------------------------------------------------
//      ファイルメニューを表示します.
//-----------------------------------------------------------------------------
void DrawFileMenu(WorkSpace* workSpace)
{
    if (!ImGui::BeginMenu(kTagFile.c_str()))
    { return; }

    if (ImGui::BeginMenu(kTagWorkSpace.c_str()))
    {
        if (ImGui::MenuItem(kTagOpen.c_str()))
        {
            std::string path;
            if (asdx::OpenFileDlg(kWorkFilter, path, ""))
            {
                // ワークスペースをロード.
                workSpace->LoadAsync(path.c_str());
            }
        }

        if (ImGui::MenuItem(kTagSaveAs.c_str()))
        {
            std::string base, ext;
            if (asdx::SaveFileDlg(kWorkFilter, base, ext))
            {
                auto path = base + ext;
                if (workSpace->SaveAs(path.c_str()))
                { ILOGA("Info : WorkSpace Save Success. path = %s", path.c_str()); }
                else
                { ELOGA("Error : WorkSpace Save Failed. path = %s", path.c_str()); }
            }
        }

        if (ImGui::MenuItem(kTagSave.c_str()))
        {
            if (workSpace->Save())
            { ILOGA("Info : WorkSpace Save Success."); }
            else
            { ELOGA("Error : WorkSpace Save Failed."); }
        }

        ImGui::EndMenu();
    }


    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      ランタイム連携メニューを表示します.
//-----------------------------------------------------------------------------
void DrawRuntimeLinkage(bool connect)
{
    if (!ImGui::BeginMenu(kTagRuntimeLinkage.c_str()))
        return;

    if (connect)
    {
        if (ImGui::MenuItem(kTagDisconnect.c_str()))
        {
        }
    }
    else
    {
        if (ImGui::MenuItem(kTagConnect.c_str()))
        {
        }
    }

    if (ImGui::MenuItem(kTagConnectionSettings.c_str()))
    {
    }

    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      情報メニューを表示します.
//-----------------------------------------------------------------------------
void DrawAppInfo(MenuContext& context)
{
    if (!ImGui::BeginMenu(kTagInformation.c_str()))
        return;

    if (ImGui::MenuItem(kTagHelp.c_str()))
    { ::ShellExecuteA(nullptr, "open", kHelpLink, "", "", SW_SHOWNORMAL); }

    if (ImGui::MenuItem(kTagReleaseNotes.c_str()))
    { ::ShellExecuteA(nullptr, "open", kReleaseNoteLink, "", "", SW_SHOWNORMAL); }

    if (ImGui::MenuItem(kTagVersionInfo.c_str()))
    {
        std::string msg = "Version : ";
        msg += std::to_string(APP_MAJOR_VERSION) + "." + std::to_string(APP_MINOR_VERSION);
        msg += "\n";
        msg += "Build : ";
        msg += APP_BUILD_VERSION_TAG;
        msg += "\n";
        msg += "Time  : ";
        msg += __DATE__;
        msg += " ";
        msg += __TIME__;

        asdx::InfoDlg("Version Information", msg.c_str());
    }

    if (ImGui::MenuItem(kTagLicense.c_str()))
    { context.ShowLicense = true; }

    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      エクスポートメニューを描画します.
//-----------------------------------------------------------------------------
void DrawExportMenu(MenuContext& context)
{
    if (!ImGui::MenuItem(kTagExport.c_str()))
    { return; }

    if (context.pWorkSpace->GetMaterials() == nullptr)
    {
        asdx::ErrorDlg("Export Failed.", "出力するマテリアルがありません");
        return;
    }

    auto exporter = context.pWorkSpace->GetExporter();
    if (exporter.empty() || exporter == "")
    {
        asdx::ErrorDlg("Export Failed.", "エクスポーターが設定されていません");
        return;
    }

    auto output = context.pWorkSpace->GetOutputPath();
    if (output.empty() || output == "")
    {
        std::string base, ext;
        if (asdx::SaveFileDlg(kWorkFilter, base, ext))
        {
            auto path = base + ext;
            context.pWorkSpace->SetOutputPath(path.c_str());
        }
        else
        {
            // キャンセルされた場合は何も表示せずに抜ける.
            return;
        }
    }

    //auto exportContext = context.pWorkSpace->GetMaterials()->CreateExportContext();
    //if (exportContext == nullptr)
    //{
    //    DisposeExportContext(exportContext);
    //    asdx::ErrorDlg("エクスポート失敗", "出力データの生成に失敗しました.");
    //    return;
    //}

    //auto ret = CallExporter(exporter.c_str(), exportContext);
    //if (ret)
    //{ ILOGA("Info : Export Success."); }
    //else
    //{ ELOGA("Error : Export Failed."); }

    //DisposeExportContext(exportContext);
}

//-----------------------------------------------------------------------------
//      言語メニューを描画します.
//-----------------------------------------------------------------------------
void DrawLanguageMenu()
{
    if (!ImGui::BeginMenu(kTagLanguage.c_str()))
    { return; }

    if (ImGui::MenuItem(kTagJapanese.c_str()))
    { asdx::SetCurrentLanguageType(asdx::LANGUAGE_JP); }

    if (ImGui::MenuItem(kTagEnglish.c_str()))
    { asdx::SetCurrentLanguageType(asdx::LANGUAGE_EN); }

    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      ポップアップメニューを描画します.
//-----------------------------------------------------------------------------
void DrawPopupMenu(MenuContext& context)
{
    if(!ImGui::BeginPopup(u8"PopupMenu"))
    { return; }

    // ファイルメニュー.
    DrawFileMenu(context.pWorkSpace);

    // エクスポートメニュー.
    DrawExportMenu(context);

    // ランタイム連携.
    DrawRuntimeLinkage(false);

    // 言語メニュー.
    DrawLanguageMenu();

    // 情報.
    DrawAppInfo(context);

    ImGui::EndPopup();
}

//-----------------------------------------------------------------------------
//      ライセンスを表示します.
//-----------------------------------------------------------------------------
void DrawLicenseDialog(MenuContext& context)
{
    if (context.ShowLicense)
    { ImGui::OpenPopup("License"); }

    if (ImGui::BeginPopupModal("License"))
    {
        auto count = _countof(kLicenses);
        for(auto i=0u; i<count; ++i)
        {
            ImGui::BulletText("%s :",kLicenses[i].Name);
            ImGui::SameLine();
            ImGuiHyperLink(kLicenses[i].Url);
        }

        if (ImGui::Button("OK"))
        { ImGui::CloseCurrentPopup(); }

        ImGui::EndPopup();
    }
}

//-----------------------------------------------------------------------------
//      モーダルダイアログを表示します.
//-----------------------------------------------------------------------------
void DrawModalDialog(MenuContext& context)
{
    // ライセンス表示.
    DrawLicenseDialog(context);
}

//-----------------------------------------------------------------------------
//      マテリアルタブを描画します.
//-----------------------------------------------------------------------------
void DrawMaterialTab(MenuContext& context)
{
    if (!ImGui::BeginTabItem(kTagMaterial.c_str()))
    { return; }

    auto pModel = context.pWorkSpace->GetModel();
    auto pMaterials = context.pWorkSpace->GetMaterials();

    if (pModel == nullptr || pMaterials == nullptr)
    {
        ImGui::TextColored(kRed, kTagNoMaterial.c_str());
        ImGui::EndTabItem();
        return;
    }

    // 名前フィルタ.
    pMaterials->NameFilter.Draw(kTagNameFilter.c_str());

    // マテリアルタイプフィルタ.
    pMaterials->TypeFilter = PluginMgr::Instance().DrawFilterCombo(pMaterials->TypeFilter);

    auto count = pMaterials->GetCount();
    for(auto i=0u; i<count; ++i)
    {
        auto& material = pMaterials->GetMaterial(i);

        // 名前でフィルタ.
        if (!pMaterials->NameFilter.PassFilter(material.GetName().c_str()))
        { continue; }

        // マテリアル編集.
        material.Edit(pMaterials->TypeFilter);
    }

    ImGui::EndTabItem();
}


//-----------------------------------------------------------------------------
//      ライトタブを描画します.
//-----------------------------------------------------------------------------
void DrawLightTab()
{
    if (!ImGui::BeginTabItem(kTagLight.c_str()))
    { return; }

    LightMgr::Instance().Edit();

    ImGui::EndTabItem();
}


//-----------------------------------------------------------------------------
//      設定タブを描画します.
//-----------------------------------------------------------------------------
void DrawConfigTab(MenuContext& context)
{
    if (!ImGui::BeginTabItem(kTagConfig.c_str()))
    { return; }

    // 背景.
    context.pConfig->Background.Edit();

    // エクスポーター
    if (ImGui::CollapsingHeader(kTagExportSetting.c_str()))
    { 
        auto prev = context.pWorkSpace->GetExporter();
        auto curr = PluginMgr::Instance().DrawExporterCombo(prev);

        if (prev != curr)
        { context.pWorkSpace->SetExporter(curr.c_str()); }
    }

    // カメラ.
    context.pConfig->Camera.Edit();

    // モデルプレビュー.
    context.pConfig->ModelPreview.Edit();


    // デバッグ.
    context.pConfig->Debug.Edit();

    ImGui::EndTabItem();
}

//-----------------------------------------------------------------------------
//      編集パネルを描画します.
//-----------------------------------------------------------------------------
void DrawEditorPanel(MenuContext& context)
{
    if (!context.pConfig->PanelEdit.Open)
    { return; }

    auto pos  = ToImVec2(context.pConfig->PanelEdit.Pos);
    auto size = ToImVec2(context.pConfig->PanelEdit.Size);

    ImGui::SetNextWindowPos (pos,  ImGuiCond_Once);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);

    if (!ImGui::Begin(kTagEdit.c_str(), &context.pConfig->PanelEdit.Open))
    { return; }

    int flag = 0;
    flag |= ImGuiTabBarFlags_TabListPopupButton;
    flag |= ImGuiTabBarFlags_FittingPolicyScroll;
    if (ImGui::BeginTabBar(u8"Panels", flag))
    {
        // マテリアルタブ.
        DrawMaterialTab(context);

        // 設定タブ.
        DrawConfigTab(context);

        // ライトタブ.
        DrawLightTab();

        // モーションタブ.


        // アイテムタブ


        ImGui::EndTabBar();
    }

    // ウィンドウ位置とサイズを更新.
    pos  = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();
    context.pConfig->PanelEdit.Pos  = FromImVec2(pos);
    context.pConfig->PanelEdit.Size = FromImVec2(size);

    ImGui::End();
}

//-----------------------------------------------------------------------------
//      ローディング表示を行います.
//-----------------------------------------------------------------------------
void DrawLoading(float x, float y)
{
    auto flag = 0;
    flag |= ImGuiWindowFlags_NoResize;
    flag |= ImGuiWindowFlags_NoMove;
    flag |= ImGuiWindowFlags_NoTitleBar;
    flag |= ImGuiWindowFlags_NoFocusOnAppearing;
    flag |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::Begin(u8"Loading", nullptr, flag);
    ImGui::TextColored(kCyan, u8"Now Loading ...");
    ImGui::End();
}

//-----------------------------------------------------------------------------
//      ギズモを表示します.
//-----------------------------------------------------------------------------
void DrawGuizmo
(
    MenuContext&        context,
    ImGuizmo::OPERATION operation,
    const asdx::Matrix& view,
    const asdx::Matrix& proj,
    float               w,
    float               h
)
{
    if (context.pWorkSpace->GetModel() == nullptr)
    { return; }

    asdx::Matrix matrix = context.pWorkSpace->GetModel()->GetWorld();

     auto flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoScrollbar;
    flags |= ImGuiWindowFlags_NoInputs;
    flags |= ImGuiWindowFlags_NoSavedSettings;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(w, h));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
    ImGui::PushStyleColor(ImGuiCol_Border,   0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin(u8"Gizmo", nullptr, flags);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(0, 0, w, h);

    ImGuizmo::Manipulate(
        view,
        proj,
        operation,
        ImGuizmo::MODE::LOCAL,
        matrix);

    asdx::Vector3 S, R, T;
    ImGuizmo::DecomposeMatrixToComponents(matrix, T, R, S);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    context.pConfig->ModelPreview.Translation.SetValue(T);
    context.pConfig->ModelPreview.Scale.SetValue(S);
    context.pConfig->ModelPreview.Rotation.SetValue(R);
}

} // namespace

//-----------------------------------------------------------------------------
//      GUIを描画します.
//-----------------------------------------------------------------------------
void App::DrawGui()
{
    asdx::GuiMgr::GetInstance().Update(m_Width, m_Height);
    {
        auto count = (m_WorkSpace.GetModel() != nullptr) 
            ? m_WorkSpace.GetModel()->CalcPolygonCount() : 0ull;

        // FPS表示.
        DrawFPS(GetFPS(), count, false);

        if(!ImGui::IsAnyItemHovered() & !m_CameraControl)
        {
            if(ImGui::IsMouseClicked(1))
            { ImGui::OpenPopup(u8"PopupMenu"); }
        }

        MenuContext context = {};
        context.pWorkSpace = &m_WorkSpace;
        context.pConfig    = &m_Config;

        auto w = float(m_Width);
        auto h = float(m_Height);

        // 右クリックメニュー.
        DrawPopupMenu(context);

        // 編集パネル.
        DrawEditorPanel(context);

        // モーダルダイアログ.
        DrawModalDialog(context);

        // ギズモ表示.
        if (m_GuizmoOperation >= 0)
        {
            DrawGuizmo(
                context,
                ImGuizmo::OPERATION(m_GuizmoOperation),
                m_CameraController.GetView(),
                m_Proj,
                w,
                h);
        }

        // ロード中.
        if (m_WorkSpace.IsLoading())
        {
            const auto speed = 300.0f;
            m_LoadingPos += speed * float(m_Timer.GetElapsedTime());
            auto x = w - float(int(m_LoadingPos) % m_Width);
            auto y = h - 100.0f;
            DrawLoading(x, y);
        }

    }
    // 描画コマンドを積む.
    asdx::GuiMgr::GetInstance().Draw();
}
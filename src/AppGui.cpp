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


///////////////////////////////////////////////////////////////////////////////
// OSS structure
///////////////////////////////////////////////////////////////////////////////
struct OSS
{
    const char* Name;
    const char* Url;
};
static OSS kLicenses[] = {
    {"Dear Imgui", "https://github.com/ocornut/imgui/blob/master/LICENSE.txt"},
    {"ImgGuizmo", "https://github.com/CedricGuillemet/ImGuizmo/blob/master/LICENSE"},
    {"TinyXML-2", "https://github.com/leethomason/tinyxml2/blob/master/LICENSE.txt"},
    {"meshoptimizer", "https://github.com/zeux/meshoptimizer/blob/master/LICENSE.md"},
    {"DirectXTex", "https://github.com/microsoft/DirectXTex/blob/master/LICENSE"},
    {"assimp", "https://github.com/assimp/assimp/blob/master/LICENSE"},
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
    ImGui::Text(u8"ポリゴン数 : %lu", polygonCount);

    if (connect)
    { ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"接続"); }
    else
    { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"切断"); }

    ImGui::End();
}

//-----------------------------------------------------------------------------
//      ファイルメニューを表示します.
//-----------------------------------------------------------------------------
void DrawFileMenu(WorkSpace* workSpace)
{
    if (!ImGui::BeginMenu(u8"ファイル"))
    { return; }

    if (ImGui::BeginMenu(u8"ワークスペース"))
    {
        if (ImGui::MenuItem(u8"開く"))
        {
            std::string path;
            if (asdx::OpenFileDlg(kWorkFilter, path, ""))
            {
                // ワークスペースをロード.
                workSpace->LoadAsync(path.c_str());
            }
        }

        if (ImGui::MenuItem(u8"名前を付けて保存"))
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

        if (ImGui::MenuItem(u8"上書き保存"))
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
//      外部連携メニューを表示します.
//-----------------------------------------------------------------------------
void DrawExternalLinkage(bool connect)
{
    if (!ImGui::BeginMenu(u8"外部連携"))
        return;

    if (connect)
    {
        if (ImGui::MenuItem(u8"切断"))
        {
        }
    }
    else
    {
        if (ImGui::MenuItem(u8"接続"))
        {
        }
    }

    if (ImGui::MenuItem(u8"接続設定"))
    {
    }

    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      情報メニューを表示します.
//-----------------------------------------------------------------------------
void DrawAppInfo(MenuContext& context)
{
    if (!ImGui::BeginMenu(u8"情報"))
        return;

    if (ImGui::MenuItem(u8"ヘルプ"))
    { ::ShellExecuteA(nullptr, "open", kHelpLink, "", "", SW_SHOWNORMAL); }

    if (ImGui::MenuItem(u8"リリースノート"))
    { ::ShellExecuteA(nullptr, "open", kReleaseNoteLink, "", "", SW_SHOWNORMAL); }

    if (ImGui::MenuItem(u8"バージョン情報"))
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

    if (ImGui::MenuItem(u8"ライセンス"))
    { context.ShowLicense = true; }

    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      エクスポートメニューを描画します.
//-----------------------------------------------------------------------------
void DrawExportMenu(MenuContext& context)
{
    if (!ImGui::MenuItem(u8"エクスポート"))
    { return; }

    if (context.pWorkSpace->GetMaterials() == nullptr)
    {
        asdx::ErrorDlg("エクスポート失敗", "出力するマテリアルがありません");
        return;
    }

    auto exporter = context.pWorkSpace->GetExporter();
    if (exporter.empty() || exporter == "")
    {
        asdx::ErrorDlg("エクスポート失敗", "エクスポーターが設定されていません");
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

    auto exportContext = context.pWorkSpace->GetMaterials()->CreateExportContext();
    if (exportContext == nullptr)
    {
        DisposeExportContext(exportContext);
        asdx::ErrorDlg("エクスポート失敗", "出力データの生成に失敗しました.");
        return;
    }

    auto ret = CallExporter(exporter.c_str(), exportContext);
    if (ret)
    { ILOGA("Info : Export Success."); }
    else
    { ELOGA("Error : Export Failed."); }

    DisposeExportContext(exportContext);
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

    // 外部連携.
    DrawExternalLinkage(false);

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
    if (!ImGui::BeginTabItem(u8"マテリアル"))
    { return; }

    auto pModel = context.pWorkSpace->GetModel();
    auto pMaterials = context.pWorkSpace->GetMaterials();

    if (pModel == nullptr || pMaterials == nullptr)
    {
        ImGui::TextColored(kRed, u8"マテリアルがありません");
        ImGui::EndTabItem();
        return;
    }

    // 名前フィルタ.
    pMaterials->NameFilter.Draw(u8"名前フィルタ");

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
    if (!ImGui::BeginTabItem(u8"ライト"))
    { return; }

    LightMgr::Instance().Edit();

    ImGui::EndTabItem();
}


//-----------------------------------------------------------------------------
//      設定タブを描画します.
//-----------------------------------------------------------------------------
void DrawConfigTab(MenuContext& context)
{
    if (!ImGui::BeginTabItem(u8"設定"))
    { return; }

    // 背景.
    context.pConfig->Background.Edit();

    // エクスポーター
    if (ImGui::CollapsingHeader(u8"エクスポート設定"))
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

    if (!ImGui::Begin(u8"編集", &context.pConfig->PanelEdit.Open))
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
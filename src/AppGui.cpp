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
#include <imgui.h>
#include <AppVersion.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const char* kWorkFilter = "Work File(*.work)\0*.work\0\0";


///////////////////////////////////////////////////////////////////////////////
// MenuContext structure
///////////////////////////////////////////////////////////////////////////////
struct MenuContext
{
    bool ShowLicense = false;
};


//-----------------------------------------------------------------------------
//      FPSを表示します.
//-----------------------------------------------------------------------------
void DrawFPS(float fps, uint64_t vertexCount, uint64_t meshletCount, bool connect)
{
    ImGui::SetNextWindowPos (ImVec2(10,  10));
    ImGui::SetNextWindowSize(ImVec2(150, 60));

    auto flags = ImGuiWindowFlags_NoMove
               | ImGuiWindowFlags_NoResize
               | ImGuiWindowFlags_NoTitleBar;
    if (!ImGui::Begin(u8"FrameInfo", nullptr, flags))
    { return; }

    // FPS
    ImGui::Text(u8"FPS : %.2f", fps);

    // 頂点数
    ImGui::Text(u8"頂点数 : %lu", vertexCount);

    if (connect)
    { ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"接続"); }
    else
    { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"切断"); }

    ImGui::End();
}

//-----------------------------------------------------------------------------
//      ファイルメニューを表示します.
//-----------------------------------------------------------------------------
void DrawFileMenu(WorkSpace& workSpace)
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
                workSpace.LoadAsync(path.c_str());
            }
        }

        if (ImGui::MenuItem(u8"名前を付けて保存"))
        {
            std::string base, ext;
            if (asdx::SaveFileDlg(kWorkFilter, base, ext))
            {
                auto path = base + ext;
                if (workSpace.SaveAs(path.c_str()))
                { ILOGA("Info : WorkSpace Save Success. path = %s", path.c_str()); }
                else
                { ELOGA("Error : WorkSpace Save Failed. path = %s", path.c_str()); }
            }
        }

        if (ImGui::MenuItem(u8"上書き保存"))
        {
            if (workSpace.Save())
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
    if (!ImGui::BeginMenu(u8"ランタイム連携"))
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
//      ヘルプメニューを表示します.
//-----------------------------------------------------------------------------
void DrawHelp(MenuContext& context)
{
    if (!ImGui::BeginMenu(u8"ヘルプ"))
        return;

    if (ImGui::MenuItem(u8"ヘルプファイル"))
    {
    }

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
    {
        context.ShowLicense = true;
    }


    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      ポップアップメニューを描画します.
//-----------------------------------------------------------------------------
void DrawPopupMenu(WorkSpace& workSpace, MenuContext& context)
{
    if(!ImGui::BeginPopup(u8"PopupMenu"))
    { return; }

    // ファイルメニュー.
    DrawFileMenu(workSpace);

    // ランタイム連携.
    //DrawRuntimeLinkage(false);

    // ヘルプ.
    DrawHelp(context);

    ImGui::EndPopup();
}

//-----------------------------------------------------------------------------
//      モーダルダイアログを表示します.
//-----------------------------------------------------------------------------
void DrawModalDialog(MenuContext& context)
{
    if (context.ShowLicense)
    { ImGui::OpenPopup("License"); }

    if (ImGui::BeginPopupModal("License"))
    {
        ImGui::BulletText("Dear Imgui (https://github.com/ocornut/imgui/blob/master/LICENSE.txt)");
        ImGui::BulletText("ImgGuizmo (https://github.com/CedricGuillemet/ImGuizmo/blob/master/LICENSE)");
        ImGui::BulletText("TinyXML-2 (https://github.com/leethomason/tinyxml2/blob/master/LICENSE.txt)");
        ImGui::BulletText("meshoptimizer (https://github.com/zeux/meshoptimizer/blob/master/LICENSE.md)");

        if (ImGui::Button("OK"))
        { ImGui::CloseCurrentPopup(); }

        ImGui::EndPopup();
    }
}

void DrawBackgroundSetting()
{
    // 背景色.

    // IBL画像.
}

void DrawLightingSetting()
{
    // 方向.

    // 強さ.
}

void DrawCameraSetting()
{
    // 
}

void DrawPostEffectSetting()
{
    // SSAO

    // トーンマップ.


    // Bloom


    // TAA
}

void DrawModelPreview()
{
}

void DrawShadowSetting()
{
}

void DrawDebugSetting()
{
}

void DrawEditorPanel()
{
}

} // namespace

//-----------------------------------------------------------------------------
//      GUIを描画します.
//-----------------------------------------------------------------------------
void App::DrawGui()
{
    asdx::GuiMgr::GetInstance().Update(m_Width, m_Height);
    {
        // FPS表示.
        DrawFPS(GetFPS(), 0, 0, false);

        if(!ImGui::IsAnyItemHovered())
        {
            if(ImGui::IsMouseClicked(1))
            { ImGui::OpenPopup(u8"PopupMenu"); }
        }

        MenuContext context = {};

        DrawPopupMenu(m_WorkSpace, context);

        DrawModalDialog(context);

    }
    // 描画コマンドを積む.
    asdx::GuiMgr::GetInstance().Draw();
}
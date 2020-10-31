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
#include <imgui.h>


namespace {

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
void DrawFileMenu()
{
    if (!ImGui::BeginMenu(u8"ファイル"))
    { return; }

    if (ImGui::BeginMenu(u8"ワークスペース"))
    {
        if (ImGui::MenuItem(u8"開く"))
        {
        }

        if (ImGui::MenuItem(u8"名前を付けて保存"))
        {
        }

        if (ImGui::MenuItem(u8"上書き保存"))
        {
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
void DrawHelp()
{
    if (!ImGui::BeginMenu(u8"ヘルプ"))
        return;

    if (ImGui::MenuItem(u8"ヘルプファイル"))
    {
    }

    if (ImGui::MenuItem(u8"バージョン情報"))
    {
    }

    if (ImGui::MenuItem(u8"ライセンス"))
    {
    }


    ImGui::EndMenu();
}

//-----------------------------------------------------------------------------
//      ポップアップメニューを描画します.
//-----------------------------------------------------------------------------
void DrawPopupMenu()
{
    if(!ImGui::BeginPopup(u8"PopupMenu"))
    { return; }

    // ファイルメニュー.
    DrawFileMenu();

    // ランタイム連携.
    DrawRuntimeLinkage(false);

    // ヘルプ.
    DrawHelp();

    ImGui::EndPopup();
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

        DrawPopupMenu();

    }
    // 描画コマンドを積む.
    asdx::GuiMgr::GetInstance().Draw();
}
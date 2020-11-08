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
#include <Config.h>
#include <LightMgr.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const char* kWorkFilter = "Work File(*.work)\0*.work\0\0";
static const char* kLicenses[] = {
    "Dear Imgui (https://github.com/ocornut/imgui/blob/master/LICENSE.txt)",
    "ImgGuizmo (https://github.com/CedricGuillemet/ImGuizmo/blob/master/LICENSE)",
    "TinyXML-2 (https://github.com/leethomason/tinyxml2/blob/master/LICENSE.txt)",
    "meshoptimizer (https://github.com/zeux/meshoptimizer/blob/master/LICENSE.md)",
    "DirectXTex ()",
    "assimp ()",
};
static const ImVec4 kRed   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
static const ImVec4 kCyan  = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

///////////////////////////////////////////////////////////////////////////////
// MenuContext structure
///////////////////////////////////////////////////////////////////////////////
struct MenuContext
{
    bool                ShowLicense = false;
    EditorModel*        pModel = nullptr;
    EditorMaterials*    pMaterials = nullptr;
    Config*             pConfig = nullptr;
};

ImVec2 ToImVec2(const asdx::Vector2& value)
{ return ImVec2(value.x, value.y); }

ImVec4 ToImVec4(const asdx::Vector4& value)
{ return ImVec4(value.x, value.y, value.z, value.w); }

asdx::Vector2 FromImVec2(const ImVec2& value)
{ return asdx::Vector2(value.x, value.y); }

asdx::Vector4 FromImVec4(const ImVec4& value)
{ return asdx::Vector4(value.x, value.y, value.z, value.w); }


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
        // ヘルプファイルを開く.
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
    { context.ShowLicense = true; }

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
    DrawRuntimeLinkage(false);

    // ヘルプ.
    DrawHelp(context);

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
        { ImGui::BulletText(kLicenses[i]); }

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

    if (context.pModel == nullptr || context.pMaterials == nullptr)
    {
        ImGui::TextColored(kRed, u8"マテリアルがありません");
        ImGui::EndTabItem();
        return;
    }

    // 名前フィルタ.

    // マテリアルタイプフィルタ.


    auto count = context.pMaterials->GetCount();
    for(auto i=0u; i<count; ++i)
    {
        auto& material = context.pMaterials->GetMaterial(i);

        // 名前でフィルタ.

        // タイプでフィルタ.

        // マテリアル編集.
        material.Edit();
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

    // カメラ.
    context.pConfig->Camera.Edit();

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


        // アイテムタブ/


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
        context.pModel     = m_WorkSpace.GetModel();
        context.pMaterials = m_WorkSpace.GetMaterials();
        context.pConfig    = &m_Config;

        // 右クリックメニュー.
        DrawPopupMenu(m_WorkSpace, context);

        // 編集パネル.
        DrawEditorPanel(context);

        // モーダルダイアログ.
        DrawModalDialog(context);

        // ロード中.
        if (m_WorkSpace.IsLoading())
        {
            const auto speed = 300.0f;
            m_LoadingPos += speed * float(m_Timer.GetElapsedTime());
            auto x = float(m_Width - int(m_LoadingPos) % m_Width);
            auto y = float(m_Height - 100);
            DrawLoading(x, y);
        }

    }
    // 描画コマンドを積む.
    asdx::GuiMgr::GetInstance().Draw();
}
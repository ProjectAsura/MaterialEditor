//-----------------------------------------------------------------------------
// File : WorkSpace.cpp
// Desc : Work Space File.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <WorkSpace.h>
#include <asdxLogger.h>
#include <asdxMisc.h>
#include <process.h>


///////////////////////////////////////////////////////////////////////////////
// WorkSpace class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
WorkSpace::WorkSpace()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
WorkSpace::~WorkSpace()
{ Clear(); }

//-----------------------------------------------------------------------------
//      ワークスペースを新規作成します.
//-----------------------------------------------------------------------------
bool WorkSpace::New(const char* modelPath)
{
    if (modelPath == nullptr || m_Loading)
    { return false; }

    std::string filePath;
    if (!asdx::SearchFilePathA(modelPath, filePath))
    {
        ELOG("Error : File Not Found. path = %s", modelPath);
        return false;
    }

    // データを破棄.
    Clear();

    // 読み込みフラグを立てる.
    m_Loading = true;

    // ロード引数.
    struct LoadArg
    {
        WorkSpace*      pWorkSpace;
        std::string     Path;
    };

    // ロード関数.
    auto loadFunc = [](void* args)
    {
        auto loadArg = static_cast<LoadArg*>(args);
        if (loadArg == nullptr)
        { return; }

        auto path = loadArg->Path.c_str();
        auto pWorkSpace = loadArg->pWorkSpace;

        // モデル読み込み.
        pWorkSpace->m_Model = new EditorModel();
        if (!pWorkSpace->m_Model->Init(path))
        {
            delete pWorkSpace->m_Model;
            pWorkSpace->m_Model = nullptr;

            pWorkSpace->m_Loading = false;
            return;
        }

        // マテリアル生成.
        pWorkSpace->m_Materials = new EditorMaterials();
        pWorkSpace->m_Materials->Resize(pWorkSpace->m_Model->GetMeshCount());

        // マテリアル名を設定.
        for(auto i=0u; i<pWorkSpace->m_Model->GetMeshCount(); ++i)
        {
            auto& mesh     = pWorkSpace->m_Model->GetMesh(i);
            auto& material = pWorkSpace->m_Materials->GetMaterial(i);
            material.SetName(mesh.GetMaterialName());
        }

        pWorkSpace->m_WorkDir    = asdx::ToFullPath(asdx::GetDirectoryPathA(path).c_str());
        pWorkSpace->m_ModelPath  = asdx::ToRelativePath(pWorkSpace->m_WorkDir.c_str(), path);
        pWorkSpace->m_Path       = "";
        pWorkSpace->m_OutputPath = "";

        // 読み込みフラグを落とす.
        pWorkSpace->m_Loading = false;

        loadArg->pWorkSpace = nullptr;
        delete loadArg;
    };

    // 引数準備.
    auto args = new LoadArg();
    args->pWorkSpace = this;
    args->Path       = filePath.c_str();

    // 読み込みスレッド開始.
    auto handle = _beginthread(loadFunc, 0, args);

    // 正常終了したかどうかチェック.
    if (handle == -1 || handle == 0)
    {
        // 失敗した場合は作成元でdeleteする.
        delete args;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      ロード処理を行います.
//-----------------------------------------------------------------------------
bool WorkSpace::Load(const char* path)
{
    // ロード中フラグを立てる.
    m_Loading = true;

    // 引数チェック.
    if (path == nullptr)
    {
        ELOG("Error : Invalid Arugment.");
        m_Loading = false;
        return false;
    }

    // ファイルを探索.
    std::string filePath;
    if (!asdx::SearchFilePathA(path, filePath))
    {
        ELOG("Error : File Not Found. path = %s", path);
        m_Loading = false;
        return false;
    }

    tinyxml2::XMLDocument doc;
    auto ret = doc.LoadFile(filePath.c_str());
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOG("Error : WorkSpace::Load() Failed. path = %s", filePath.c_str());
        m_Loading = false;
        return false;
    }

    // ルートノードを見つける.
    auto root = doc.FirstChildElement("WorkSpace");
    if (root == nullptr)
    {
        ELOG("Error : Invalid XML. Not Found Root.");
        m_Loading = false;
        return false;
    }

    // ワークファイルがあるディレクトリを取得.
    m_WorkDir = asdx::ToFullPath(asdx::GetDirectoryPathA(filePath.c_str()).c_str());

    // 出力パスを取得.
    {
        auto outputNode = root->FirstChildElement("OutputPath");
        if (outputNode != nullptr)
        {
            // 相対パス取得.
            m_OutputPath = outputNode->Attribute("value");
        }
    }

    // モデル
    {
        auto modelNode = root->FirstChildElement("EditorModel");
        if (modelNode != nullptr)
        {
            // 相対パス取得.
            m_ModelPath = modelNode->Attribute("value");

            // 絶対パスに変換.
            auto modelPath = asdx::ToFullPath((m_WorkDir + m_ModelPath).c_str());

            // モデルのロード.
            m_Model = new EditorModel();
            if (!m_Model->Init(modelPath.c_str()))
            {
                delete m_Model;
                m_Model = nullptr;

                m_WorkDir   .clear();
                m_OutputPath.clear();

                m_Loading = false;

                return false;
            }
        }
    }

    // 読み込み成功している場合は絶対にnullptrにならない.
    assert(m_Model != nullptr);

    m_Materials = new EditorMaterials();
    m_Materials->Resize(m_Model->GetMeshCount());

    // マテリアル名を設定.
    for(auto i=0u; i<m_Model->GetMeshCount(); ++i)
    {
        auto& mesh      = m_Model->GetMesh(i);
        auto& material  = m_Materials->GetMaterial(i);
        material.SetName(mesh.GetMaterialName());
    }

    // マテリアルを読み込み.
    m_Materials->Deserialize(root);

    m_Path    = filePath;
    m_Loading = false;
    return true;
}

//-----------------------------------------------------------------------------
//      非同期読み込みします.
//-----------------------------------------------------------------------------
bool WorkSpace::LoadAsync(const char* path)
{
    // 既にロード中の場合は受け付けない.
    if (m_Loading)
    { return false; }

    // ロード引数.
    struct LoadArg
    {
        WorkSpace*      pWorkSpace;
        std::string     Path;
    };

    // ロード関数.
    auto loadFunc = [](void* args)
    {
        auto loadArg = static_cast<LoadArg*>(args);
        if (loadArg == nullptr)
        { return; }

        auto path = loadArg->Path.c_str();

        if (!loadArg->pWorkSpace->Load(path))
        { ELOGA("Error : WorkSpace::Load() Failed. path = %s", path); }
        else
        { ILOGA("Info : WorkSpace::Load() Success. path = %s", path); }

        loadArg->pWorkSpace = nullptr;
        delete loadArg;
    };

    // 引数準備.
    auto args = new LoadArg();
    args->pWorkSpace = this;
    args->Path       = path;

    // 読み込みスレッド開始.
    auto handle = _beginthread(loadFunc, 0, args);

    // 正常終了したかどうかチェック.
    if (handle == -1 || handle == 0)
    {
        // 失敗した場合は作成元でdeleteする.
        delete args;
        return false;
    }

    // IsLoading()がfalseになったらロード終了.
    return true;
}

//-----------------------------------------------------------------------------
//      名前を付けて保存を行います.
//-----------------------------------------------------------------------------
bool WorkSpace::SaveAs(const char* path)
{
    /* 保存中にデータ変更されると困るので，非同期書き込みは用意しない. */

    if (m_Loading)
    { return false; }

    tinyxml2::XMLDocument doc;
    auto decl = doc.NewDeclaration();
    doc.InsertEndChild(decl);
    auto root = doc.NewElement("WorkSpace");

    auto outputNode = doc.NewElement("OutputPath");
    outputNode->SetAttribute("value", m_OutputPath.c_str());
    root->InsertEndChild(outputNode);

    auto modelNode = doc.NewElement("EditorModel");
    modelNode->SetAttribute("value", m_ModelPath.c_str());
    root->InsertEndChild(modelNode);

    if (m_Materials != nullptr)
    { root->InsertEndChild(m_Materials->Serialize(&doc)); }

    auto ret = doc.SaveFile(path);
    if (ret != tinyxml2::XML_SUCCESS)
    {
        ELOGA("Error : WorkSpace::Save() Failed. path = %s", path);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      上書き保存を行います.
//-----------------------------------------------------------------------------
bool WorkSpace::Save()
{ return SaveAs(m_Path.c_str()); }

//-----------------------------------------------------------------------------
//      データを破棄します.
//-----------------------------------------------------------------------------
void WorkSpace::Clear()
{
    if (m_Model != nullptr)
    {
        delete m_Model;
        m_Model = nullptr;
    }

    if (m_Materials != nullptr)
    {
        delete m_Materials;
        m_Materials = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      モデルを取得します.
//-----------------------------------------------------------------------------
EditorModel* WorkSpace::GetModel() const
{ return m_Model; }

//-----------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------
EditorMaterials* WorkSpace::GetMaterials() const
{ return m_Materials; }

//-----------------------------------------------------------------------------
//      ロード中かどうか?
//-----------------------------------------------------------------------------
bool WorkSpace::IsLoading() const
{ return m_Loading; }

//-----------------------------------------------------------------------------
//      出力パスを設定します.
//-----------------------------------------------------------------------------
void WorkSpace::SetOutputPath(const char* path)
{
    // フルパスに変換.
    auto fullPath = asdx::ToFullPath(path);

    // ワークディレクトリからの相対パスに変換する.
    m_OutputPath = asdx::ToRelativePath(m_WorkDir.c_str(), fullPath.c_str());
}

//-----------------------------------------------------------------------------
//      出力パスを取得します.
//-----------------------------------------------------------------------------
std::string WorkSpace::GetOutputPath() const
{ return asdx::ToFullPath((m_WorkDir + m_OutputPath).c_str()); }

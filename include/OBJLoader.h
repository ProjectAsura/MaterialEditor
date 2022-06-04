﻿//-----------------------------------------------------------------------------
// File : OBJLoader.h
// Desc : Wavefront Alias OBJ format.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxResModel.h>
#include <asdxResMaterial.h>
#include <map>


///////////////////////////////////////////////////////////////////////////////
// OBJLoader class
///////////////////////////////////////////////////////////////////////////////
class OBJLoader
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    OBJLoader() = default;

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~OBJLoader() = default;

    //-------------------------------------------------------------------------
    //! @brief      ファイルをロードします.
    //! 
    //! @param[in]      path        ファイルパスです.
    //! @param[out]     model       モデルの格納先です.
    //! @retval true    ロードに成功.
    //! @retval false   ロードに失敗.
    //-------------------------------------------------------------------------
    bool Load(const char* path, asdx::ResModel& model);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::string m_DirectoryPath;    //!< ディレクトリパス.

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      OBJファイルをロードします.
    //! 
    //! @param[in]      path        OBJファイルパスです.
    //! @param[out]     model       モデルの格納先です.
    //! @param[out]     materials   マテリアルの格納先です.
    //! @retval true    ロードに成功.
    //! @retval false   ロードに失敗.
    //-------------------------------------------------------------------------
    bool LoadOBJ(const char* path, asdx::ResModel& model);

    //-------------------------------------------------------------------------
    //! @brief      MTLファイルをロードします.
    //! 
    //! @param[in]      path        MTLファイルパスです.
    //! @param[out]     materials   マテリアルの格納先です.
    //! @retval true    ロードに成功.
    //! @retval false   ロードに失敗.
    //-------------------------------------------------------------------------
    bool LoadMTL(const char* path, asdx::ResModel& model);
};

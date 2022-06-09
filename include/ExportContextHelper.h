﻿//-----------------------------------------------------------------------------
// File : ExportContextHelper.h
// Desc : Export Context Helper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <ExportContext.h>
#include <WorkSpace.h>


//-----------------------------------------------------------------------------
//! @brief      出力コンテキストを生成します.
//! 
//! @param[in]      workSpace       ワークスペース.
//! @param[out]     context         出力コンテキスト.
//! @retval true    生成に成功.
//! @retval false   生成に失敗.
//-----------------------------------------------------------------------------
bool Create(const WorkSpace& workSpace, ExportContext** context);

//-----------------------------------------------------------------------------
//! @brief      出力コンテキストを破棄します.
//! 
//! @param[in]      context         出力コンテキスト.
//-----------------------------------------------------------------------------
void Dispose(ExportContext*& context);

//-----------------------------------------------------------------------------
// File : MeshLoader.cpp
// Desc : Resource Mesh Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxResModel.h>
#include <asdxResMaterial.h>


bool LoadFromGltf(const char* path, asdx::ResModel& model, asdx::ResMaterial& materials);
bool LoadFromObj(const char* path, asdx::ResModel& model, asdx::ResMaterial& materials);
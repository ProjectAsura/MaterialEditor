//-----------------------------------------------------------------------------
// File : MeshLoader.cpp
// Desc : Mesh Loader.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <MeshLoader.h>
#include <cassert>
#include <meshoptimizer.h>
#include <tiny_gltf.h>
#include <asdxLogger.h>


bool DummyLoadImageDataFunction(
    tinygltf::Image *, const int, std::string *,
    std::string *, int, int,
    const unsigned char *, int, void *user_pointer)
{
    return true;
}

bool DummyWriteImageDataFunction(
    const std::string*, const std::string*, tinygltf::Image*, bool, void*)
{
    return true;
}


bool LoadFromGltf(const char* path, asdx::ResModel& model, asdx::ResMaterialSet& materials)
{
    tinygltf::Model     tempModel;
    tinygltf::TinyGLTF  loader;
    std::string err;
    std::string warn;

    loader.SetImageLoader(DummyLoadImageDataFunction, nullptr);
    loader.SetImageWriter(DummyWriteImageDataFunction, nullptr);

    if (!loader.LoadASCIIFromFile(&tempModel, &err, &warn, path))
    {
        ELOGA("Error : LoadFromGltf() Failed. path = %s", path);
        return false;
    }

    return true;
}

bool LoadFromObj(const char* path, asdx::ResModel& model, asdx::ResMaterialSet& materials)
{
    return true;
}
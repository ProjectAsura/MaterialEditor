//-----------------------------------------------------------------------------
// File : ExportContextHelper.cpp
// Desc : Export Context Helper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <ExportContextHelper.h>


//-----------------------------------------------------------------------------
//      エクスポートコンテキストを生成します.
//-----------------------------------------------------------------------------
bool Create(const WorkSpace& workSpace, ExportContext** context)
{
    if(context == nullptr)
    { return false; }

    auto ctx = new ExportContext();

    // マテリアル生成.
    {
        auto count = workSpace.GetMaterials()->GetCount();

        ctx->MaterialCount = count;
        ctx->Materials     = new ExportMaterial[count];

        for(size_t i=0; i<count; ++i)
        {
            auto& src = workSpace.GetMaterials()->GetMaterial(uint32_t(i));
            src.SetExportData(&ctx->Materials[i]);
        }
    }

    // メッシュ生成.
    {
        auto count = workSpace.GetModel()->GetMeshCount();

        ctx->MeshCount = count;
        ctx->Meshes    = new ExportMesh[count];

        auto& srcModel = workSpace.GetModel()->GetResource();

        for(size_t i=0; i<count; ++i)
        {
            auto& src = srcModel.Meshes[i];
            auto& dst = ctx->Meshes[i];
            dst.VertexCount = uint32_t(src.Positions.size());

            dst.Positions   = reinterpret_cast<const Vec3*>(src.Positions.data());
            dst.Normals     = reinterpret_cast<const Vec3*>(src.Normals  .data());

            if (src.Tangents.empty())
            { dst.Tangents = nullptr; }
            else
            { dst.Tangents = reinterpret_cast<const Vec3*>(src.Tangents.data()); }

            if (src.Colors.empty())
            { dst.Colors = nullptr; }
            else
            { dst.Colors = reinterpret_cast<const Vec4*>(src.Colors.data()); }

            if (src.TexCoords[0].empty())
            { dst.TexCoord0 = nullptr; }
            else
            { dst.TexCoord0 = reinterpret_cast<const Vec2*>(src.TexCoords[0].data()); }

            if (src.TexCoords[1].empty())
            { dst.TexCoord1 = nullptr; }
            else
            { dst.TexCoord1 = reinterpret_cast<const Vec2*>(src.TexCoords[1].data()); }

            if (src.TexCoords[2].empty())
            { dst.TexCoord2 = nullptr; }
            else
            { dst.TexCoord2 = reinterpret_cast<const Vec2*>(src.TexCoords[2].data()); }

            if (src.TexCoords[3].empty())
            { dst.TexCoord3 = nullptr; }
            else
            { dst.TexCoord3 = reinterpret_cast<const Vec2*>(src.TexCoords[3].data()); }

            if (src.BoneIndices.empty())
            { dst.BoneIndices = nullptr; }
            else
            { dst.BoneIndices = reinterpret_cast<const Vec4s*>(src.BoneIndices.data()); }

            if (src.BoneWeights.empty())
            { dst.BoneWeights = nullptr; }
            else
            { dst.BoneWeights = reinterpret_cast<const Vec4*>(src.BoneWeights.data()); }

            dst.IndexCount  = uint32_t(src.Indices.size());
            dst.Indices     = src.Indices.data();
        }
    }

    *context = ctx;

    return true;
}

//-----------------------------------------------------------------------------
//      エクスポートコンテキストを破棄します.
//-----------------------------------------------------------------------------
void Dispose(ExportContext*& context)
{
    if (context == nullptr)
    { return; }

    if (context->Materials != nullptr)
    {
        for(auto i=0u; i<context->MaterialCount; ++i)
        {
            if (context->Materials[i].pParams != nullptr)
            {
                delete[] context->Materials[i].pParams;
                context->Materials[i].pParams = nullptr;
            }

            if (context->Materials[i].Texture2Ds != nullptr)
            {
                delete[] context->Materials[i].Texture2Ds;
                context->Materials[i].Texture2Ds = nullptr;
            }
        }

        delete[] context->Materials;
        context->Materials = nullptr;
    }

    if (context->Meshes != nullptr)
    {
        delete[] context->Meshes;
        context->Meshes = nullptr;
    }

    delete context;
    context = nullptr;
}

﻿//-----------------------------------------------------------------------------
// File : OBJLoader.cpp
// Desc : Wavefront Alias OBJ format.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <OBJLoader.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <fstream>

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const uint32_t OBJ_BUFFER_LENGTH = 2048;


///////////////////////////////////////////////////////////////////////////////
// SubsetOBJ structure
///////////////////////////////////////////////////////////////////////////////
struct SubsetOBJ
{
    std::string     MeshName;
    std::string     MaterialName;
    uint32_t        IndexStart;
    uint32_t        IndexCount;
};

///////////////////////////////////////////////////////////////////////////////
// IndexOBJ structure
///////////////////////////////////////////////////////////////////////////////
struct IndexOBJ
{
    uint32_t    P;
    uint32_t    T;
    uint32_t    N;
};

//-----------------------------------------------------------------------------
//      ロードします.
//-----------------------------------------------------------------------------
bool OBJLoader::Load(const char* path, asdx::ResModel& model, asdx::ResMaterialSet& materials)
{
    if (path == nullptr)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    // ディレクトリパス取得.
    m_DirectoryPath = asdx::GetDirectoryPathA(path);

    // OBJファイルをロード.
    return LoadOBJ(path, model, materials);
}

//-----------------------------------------------------------------------------
//      OBJファイルをロードします.
//-----------------------------------------------------------------------------
bool OBJLoader::LoadOBJ(const char* path, asdx::ResModel& model, asdx::ResMaterialSet& materials)
{
    std::ifstream stream;
    stream.open(path, std::ios::in);

    if (!stream.is_open())
    {
        ELOGA("Error : File Open Failed. path = %s", path);
        return false;
    }

    char buf[OBJ_BUFFER_LENGTH] = {};

    std::vector<asdx::Vector3> positions;
    std::vector<asdx::Vector2> texcoords;
    std::vector<asdx::Vector3> normals;
    std::vector<IndexOBJ>      indices;
    std::vector<SubsetOBJ>     subsets;
    std::string                group;

    uint32_t    faceIndex = 0;
    uint32_t    faceCount = 0;

    for(;;)
    {
        stream >> buf;
        if (!stream || stream.eof())
            break;

        if (0 == strcmp(buf, "#"))
        {
            /* DO_NOTHING */
        }
        else if (0 == strcmp(buf, "v"))
        {
            asdx::Vector3 v;
            stream >> v.x >> v.y >> v.z;
            positions.push_back(v);
        }
        else if (0 == strcmp(buf, "vt"))
        {
            asdx::Vector2 vt;
            stream >> vt.x >> vt.y;
            texcoords.push_back(vt);
        }
        else if (0 == strcmp(buf, "vn"))
        {
            asdx::Vector3 vn;
            stream >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        }
        else if (0 == strcmp(buf, "g"))
        {
            stream >> group;
        }
        else if (0 == strcmp(buf, "f"))
        {
            uint32_t ip, it, in;
            uint32_t p[4] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX };
            uint32_t t[4] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX };
            uint32_t n[4] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX };

            uint32_t count = 0;
            uint32_t index = 0;

            faceIndex++;
            faceCount++;

            for(auto i=0; i<4; ++i)
            {
                count++;

                // 位置座標インデックス.
                stream >> ip;
                p[i] = ip - 1;

                if ('/' == stream.peek())
                {
                    stream.ignore();

                    // テクスチャ座標インデックス.
                    if ('/' != stream.peek())
                    {
                        stream >> it;
                        t[i] = it - 1;
                    }

                    // 法線インデックス.
                    if ('/' == stream.peek())
                    {
                        stream.ignore();

                        stream >> in;
                        n[i] = in - 1;
                    }
                }

                if (count < 3)
                {
                    IndexOBJ f0 = { p[i], t[i], n[i] };
                    indices.push_back(f0);
                }

                if ('\n' == stream.peek() || '\r' == stream.peek())
                    break;
            }

            // 四角形.
            if (count > 3)
            {
                assert(count == 4);

                faceIndex++;
                faceCount++;

                IndexOBJ f0 = { p[2], t[2], n[2] };
                IndexOBJ f1 = { p[3], t[3], n[3] };
                IndexOBJ f2 = { p[0], t[0], n[0] };

                indices.push_back(f0);
                indices.push_back(f1);
                indices.push_back(f2);
            }
        }
        else if (0 == strcmp(buf, "mtllib"))
        {
            char path[256] = {};
            stream >> path;
            if (strlen(path) > 0)
            {
                if (!LoadMTL(path, materials))
                {
                    ELOGA("Error : Material Load Failed.");
                    return false;
                }
            }
        }
        else if (0 == strcmp(buf, "usemtl"))
        {
            SubsetOBJ subset = {};
            stream >> subset.MaterialName;
            subset.MeshName   = group;
            subset.IndexStart = faceIndex * 3;

            auto index = subsets.size();
            subsets.push_back(subset);

            if (subsets.size() > 1)
            {
                subsets[index].IndexCount = faceCount * 3;
                faceCount = 0;
            }
        }

        stream.ignore(OBJ_BUFFER_LENGTH, '\n');
    }

    if (subsets.size() > 0)
    {
        auto index = subsets.size();
        subsets[index - 1].IndexCount = faceCount * 3;
    }

    stream.close();

    // 出力データを組み立て.
    {
        uint32_t indexOffset = 0;
        model.Meshes.resize(subsets.size());

        for(size_t i=0; i<subsets.size(); ++i)
        {
            auto& mesh = model.Meshes[i];
            mesh.MeshName     = subsets[i].MeshName;
            mesh.MaterialName = subsets[i].MaterialName;

            for(size_t j=0; j<subsets[i].IndexCount; ++j)
            {
                const auto& f = indices[j + subsets[i].IndexStart];

                assert(f.P != UINT32_MAX);
                mesh.Positions.push_back(positions[f.P]);

                if (f.T != UINT32_MAX)
                { mesh.TexCoords[0].push_back(texcoords[f.T]); }

                if (f.N != UINT32_MAX)
                { mesh.Normals.push_back(normals[f.N]); }

                mesh.Indices.push_back(indexOffset);
                indexOffset++;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      MTLファイルをロードします.
//-----------------------------------------------------------------------------
bool OBJLoader::LoadMTL(const char* path, asdx::ResMaterialSet& set)
{
    std::ifstream stream;

    std::string filename = m_DirectoryPath + "/" + path;

    stream.open(filename.c_str(), std::ios::in);

    if (!stream.is_open())
    {
        ELOGA("Error : File Open Failed. path = %s", path);
        return false;
    }

    char buf[OBJ_BUFFER_LENGTH] = {};
    int32_t index = -1;

    for(;;)
    {
        stream >> buf;

        if (!stream || stream.eof())
            break;

        if (0 == strcmp(buf, "newmtl"))
        {
            index++;
            asdx::ResMaterial mat;
            set.Materials.push_back(mat);
            stream >> set.Materials[index].Name;
        }
        else if (0 == strcmp(buf, "Ka"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_AMBIENT;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT3;
            stream >> prop.Value.Float3.x >> prop.Value.Float3.y >> prop.Value.Float3.z;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "Kd"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_DIFFUSE;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT3;
            stream >> prop.Value.Float3.x >> prop.Value.Float3.y >> prop.Value.Float3.z;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "Ks"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_SPECULAR;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT3;
            stream >> prop.Value.Float3.x >> prop.Value.Float3.y >> prop.Value.Float3.z;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "Ke"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_EMISSIVE;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT3;
            stream >> prop.Value.Float3.x >> prop.Value.Float3.y >> prop.Value.Float3.z;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "d") || 0 == strcmp(buf, "Tr"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_ALPHA;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT;
            stream >> prop.Value.Float;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "Ns"))
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_SHININESS;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT;
            stream >> prop.Value.Float;

            set.Materials[index].Props.push_back(prop);
        }
        else if (0 == strcmp(buf, "map_Ka"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_AMBIENT;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == strcmp(buf, "map_Kd"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_DIFFUSE;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == strcmp(buf, "map_Ks"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_SPECULAR;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == _stricmp(buf, "map_bump") || 0 == strcmp(buf, "bump"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_BUMP;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == strcmp(buf, "disp"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_DISPLACEMENT;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == strcmp(buf, "norm"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_NORMAL;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }
        else if (0 == strcmp(buf, "map_ORM"))
        {
            asdx::ResTexturePath tex = {};
            tex.Name = asdx::TAG_ORM;
            stream >> tex.Path;

            set.Materials[index].Textures.push_back(tex);
        }

        stream.ignore(OBJ_BUFFER_LENGTH, '\n');
    }

    // ファイルを閉じる.
    stream.close();

    // 正常終了.
    return true;
}

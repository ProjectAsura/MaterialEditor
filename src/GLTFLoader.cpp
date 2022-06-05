//-----------------------------------------------------------------------------
// File : GLTFLoader.cpp
// Desc : gltf Loader.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <GLTFLoader.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <tiny_gltf.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const char* TAG_POSITION     = "POSITION";
static const char* TAG_NORMAL       = "NORMAL";
static const char* TAG_TANGENT      = "TANGENT";
static const char* TAG_COLOR        = "COLOR_0";
static const char* TAG_BONEINDEX    = "JOINTS_0";
static const char* TAG_BONEWEIGHT   = "WEIGHTS_0";
static const char* TAG_TEXCOORD[MAX_LAYER_COUNT] = { "TEXCOORD_0", "TEXCOORD_1", "TEXCOORD_2", "TEXCOORD_3" };


//-----------------------------------------------------------------------------
//      画像読み込みのダミー関数.
//-----------------------------------------------------------------------------
bool DummyLoadImage(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*)
{
    // 常に成功扱い.
    return true;
}

//-----------------------------------------------------------------------------
//      画像書き込みのダミー関数.
//-----------------------------------------------------------------------------
bool DummyWriteImage(const std::string*, const std::string*, tinygltf::Image*, bool, void*)
{
    // 常に成功扱い.
    return true;
}

//-----------------------------------------------------------------------------
//      クオータニオンから回転角に変換します.
//-----------------------------------------------------------------------------
static void QuatToAngleAxis
(
    const std::vector<double>   quaternion,
    float&                      outAngleRadian,
    float*                      axis
)
{
    double qx = quaternion[0];
    double qy = quaternion[1];
    double qz = quaternion[2];
    double qw = quaternion[3];
  
    double angleRadians = 2 * acos(qw);
    if (angleRadians == 0.0)
    {
        outAngleRadian = 0.0f;
        axis[0] = 0.0f;
        axis[1] = 0.0f;
        axis[2] = 1.0f;
        return;
    }

    double denom = sqrt(1 - qw * qw);
    outAngleRadian = float(angleRadians);
    axis[0] = float(qx / denom);
    axis[1] = float(qy / denom);
    axis[2] = float(qz / denom);
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// GLTFLoader class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      ファイルをロードします.
//-----------------------------------------------------------------------------
bool GLTFLoader::Load(const char* path, asdx::ResModel& model)
{
    auto ext = asdx::GetExtA(path);

    tinygltf::Model gltfModel;

    if (ext == "gltf")
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        loader.SetImageLoader(DummyLoadImage,  nullptr);
        loader.SetImageWriter(DummyWriteImage, nullptr);

        if (!loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path))
        {
            if (!warn.empty())
            { WLOGA("Warning : %s", warn.c_str()); }

            ELOGA("Error : TinyGLTF::LoadASCIIFromFile() Failed. err = %s", err.c_str());
            return false;
        }
    }
    else if (ext == "glb")
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        loader.SetImageLoader(DummyLoadImage,  nullptr);
        loader.SetImageWriter(DummyWriteImage, nullptr);

        if (!loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path))
        {
            if (!warn.empty())
            { WLOGA("Warning : %s", warn.c_str()); }

            ELOGA("Error : TinyGLTF::LoadBinaryFromFile() Failed. err = %s", err.c_str());
            return false;
        }
    }
    else
    {
        ELOGA("Error : Unsupported Extension.");
        return false;
    }

    auto meshCount = gltfModel.meshes.size();

    std::vector<asdx::Matrix>   transforms;
    transforms.resize(meshCount);

    // 行列初期化.
    for(size_t i=0; i<meshCount; ++i)
    {
        transforms[i].Identity();
    }

    for(size_t i=0; i<gltfModel.nodes.size(); ++i)
    {
        auto& srcNode = gltfModel.nodes[i];
        auto meshId = srcNode.mesh;
        if (meshId != -1)
        {
            if (srcNode.matrix.size() == 16)
            {
                transforms[i].row[0] = asdx::Vector4(float(srcNode.matrix[0]),  float(srcNode.matrix[1]),  float(srcNode.matrix[2]),  float(srcNode.matrix[3]));
                transforms[i].row[1] = asdx::Vector4(float(srcNode.matrix[4]),  float(srcNode.matrix[5]),  float(srcNode.matrix[6]),  float(srcNode.matrix[7]));
                transforms[i].row[2] = asdx::Vector4(float(srcNode.matrix[8]),  float(srcNode.matrix[9]),  float(srcNode.matrix[10]), float(srcNode.matrix[11]));
                transforms[i].row[3] = asdx::Vector4(float(srcNode.matrix[12]), float(srcNode.matrix[13]), float(srcNode.matrix[14]), float(srcNode.matrix[15]));
            }
            else
            {
                asdx::Matrix matrix = asdx::Matrix::CreateIdentity();

                if (srcNode.scale.size() == 3)
                {
                    matrix *= asdx::Matrix::CreateScale(
                        float(srcNode.scale[0]),
                        float(srcNode.scale[1]),
                        float(srcNode.scale[2]));
                }

                if (srcNode.rotation.size() == 4)
                {
                    float angleRad;
                    float axis[3];
                    QuatToAngleAxis(srcNode.rotation, angleRad, axis);

                    matrix *= asdx::Matrix::CreateFromAxisAngle(
                        asdx::Vector3(axis[0], axis[1], axis[2]), angleRad);
                }

                if (srcNode.translation.size() == 3)
                {
                    matrix *= asdx::Matrix::CreateTranslation(
                        float(srcNode.translation[0]),
                        float(srcNode.translation[1]),
                        float(srcNode.translation[2]));
                }

                transforms[i] = matrix;
            }
        }
    }


    // メッシュ変換.
    for(size_t i=0; i<meshCount; ++i)
    {
        auto& srcMesh = gltfModel.meshes[i];

        auto transformsT = asdx::Matrix::Transpose(transforms[i]);

        auto primitiveIndex = 0;
        for(auto& primitive : srcMesh.primitives)
        {
            asdx::ResMesh dstMesh;
            dstMesh.MeshName = srcMesh.name + std::to_string(primitiveIndex);
            dstMesh.MaterialName = gltfModel.materials[primitive.material].name;

            auto notFound = primitive.attributes.end();

            // 位置座標.
            auto attr = primitive.attributes.find(TAG_POSITION);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                const auto ptr = reinterpret_cast<asdx::Vector3*>(buffer.data.data() + offset);
                dstMesh.Positions.resize(accessor.count);

                for(size_t idx=0; idx<accessor.count; ++idx)
                {
                    dstMesh.Positions[idx] = ptr[idx];
                    dstMesh.Positions[idx] = asdx::Vector3::TransformCoord(dstMesh.Positions[idx], transforms[i]);
                }
            }

            // 法線ベクトル.
            attr = primitive.attributes.find(TAG_NORMAL);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                const auto ptr = reinterpret_cast<asdx::Vector3*>(buffer.data.data() + offset);
                dstMesh.Normals.resize(accessor.count);

                for(size_t idx=0; idx<accessor.count; ++idx)
                {
                    dstMesh.Normals[idx] = ptr[idx];
                    dstMesh.Normals[idx] = asdx::Vector3::TransformNormal(dstMesh.Normals[idx], transformsT);
                    dstMesh.Normals[idx] = asdx::Vector3::SafeNormalize(dstMesh.Normals[idx], dstMesh.Normals[idx]);
                }
            }

            // 接線ベクトル.
            attr = primitive.attributes.find(TAG_TANGENT);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                const auto ptr = reinterpret_cast<asdx::Vector3*>(buffer.data.data() + offset);
                dstMesh.Tangents.resize(accessor.count);

                for(size_t idx=0; idx<accessor.count; ++idx)
                {
                    dstMesh.Tangents[idx] = ptr[idx];
                    dstMesh.Tangents[idx] = asdx::Vector3::TransformNormal(dstMesh.Tangents[idx], transformsT);
                    dstMesh.Tangents[idx] = asdx::Vector3::SafeNormalize(dstMesh.Tangents[idx], dstMesh.Tangents[idx]);
                }

                memcpy(dstMesh.Tangents.data(), ptr, accessor.count * sizeof(asdx::Vector3));
            }

            // 頂点カラー.
            attr = primitive.attributes.find(TAG_COLOR);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                dstMesh.Tangents.resize(accessor.count);

                if (accessor.count == 4)
                {
                    switch(accessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        {
                            const auto ptr = buffer.data.data() + offset;
                            memcpy(dstMesh.Tangents.data(), ptr, accessor.count * sizeof(asdx::Vector4));
                        }
                        break;

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        {
                            const auto ptr = buffer.data.data() + offset;
                            for(size_t idx=0; idx<accessor.count; ++idx)
                            {
                                dstMesh.Colors[idx].x = float(ptr[idx * 4 + 0]) / 255.0f;
                                dstMesh.Colors[idx].y = float(ptr[idx * 4 + 1]) / 255.0f;
                                dstMesh.Colors[idx].z = float(ptr[idx * 4 + 2]) / 255.0f;
                                dstMesh.Colors[idx].w = float(ptr[idx * 4 + 3]) / 255.0f;
                            }
                        }
                        break;
                    }
                }
                else if (accessor.count == 3)
                {
                    switch(accessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        {
                            const auto ptr = reinterpret_cast<float*>(buffer.data.data() + offset);
                            for(size_t idx=0; idx<accessor.count; ++idx)
                            {
                                dstMesh.Colors[idx].x = ptr[idx * 3 + 0];
                                dstMesh.Colors[idx].y = ptr[idx * 3 + 1];
                                dstMesh.Colors[idx].z = ptr[idx * 3 + 2];
                                dstMesh.Colors[idx].w = 1.0f;
                            }
                        }
                        break;

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        {
                            const auto ptr = buffer.data.data() + offset;
                            for(size_t idx=0; idx<accessor.count; ++idx)
                            {
                                dstMesh.Colors[idx].x = float(ptr[idx * 3 + 0]) / 255.0f;
                                dstMesh.Colors[idx].y = float(ptr[idx * 3 + 1]) / 255.0f;
                                dstMesh.Colors[idx].z = float(ptr[idx * 3 + 2]) / 255.0f;
                                dstMesh.Colors[idx].w = 1.0f;
                            }
                        } 
                        break;
                    }
                }
            }

            // ボーンインデックス.
            attr = primitive.attributes.find(TAG_BONEINDEX);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                dstMesh.BoneIndices.resize(accessor.count);

                switch(accessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        const auto ptr = buffer.data.data() + offset;
                        for(size_t idx=0; idx<accessor.count; ++idx)
                        {
                            dstMesh.BoneIndices[idx].x = uint16_t(ptr[idx * 4 + 0]);
                            dstMesh.BoneIndices[idx].y = uint16_t(ptr[idx * 4 + 1]);
                            dstMesh.BoneIndices[idx].z = uint16_t(ptr[idx * 4 + 2]);
                            dstMesh.BoneIndices[idx].w = uint16_t(ptr[idx * 4 + 3]);
                        }
                    }
                    break;

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        const auto ptr = buffer.data.data() + offset;
                        memcpy(dstMesh.BoneIndices.data(), ptr, accessor.count * sizeof(asdx::ResBoneIndex));
                    }
                    break;
                }
            }

            // ボーンウェイト.
            attr = primitive.attributes.find(TAG_BONEWEIGHT);
            if (attr != notFound)
            {
                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                const auto ptr = buffer.data.data() + offset;
                dstMesh.BoneWeights.resize(accessor.count);
                memcpy(dstMesh.BoneWeights.data(), ptr, accessor.count * sizeof(asdx::Vector4));
            }

            // テクスチャ座標.
            for(auto layer = 0; layer<MAX_LAYER_COUNT; layer++)
            {
                attr = primitive.attributes.find(TAG_TEXCOORD[layer]);
                if (attr == notFound)
                { continue; }

                auto& accessor  = gltfModel.accessors[attr->second];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                dstMesh.TexCoords[layer].resize(accessor.count);

                switch(accessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_FLOAT:
                    {
                        const auto ptr = buffer.data.data() + offset;
                        memcpy(dstMesh.TexCoords[layer].data(), ptr, accessor.count * sizeof(asdx::Vector2));
                    }
                    break;

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        const auto ptr = buffer.data.data() + offset;
                        for(size_t idx=0; idx<accessor.count; ++idx)
                        {
                            dstMesh.TexCoords[layer][idx].x = float(ptr[idx * 2 + 0]) / 255.0f;
                            dstMesh.TexCoords[layer][idx].y = float(ptr[idx * 2 + 1]) / 255.0f;
                        }
                    }
                    break;

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        const auto ptr = reinterpret_cast<uint16_t*>(buffer.data.data() + offset);
                        for(size_t idx=0; idx<accessor.count; ++idx)
                        {
                            dstMesh.TexCoords[layer][idx].x = float(ptr[idx * 2 + 0]) / 65535.0f;
                            dstMesh.TexCoords[layer][idx].y = float(ptr[idx * 2 + 1]) / 65535.0f;
                        }
                    }
                    break;
                }
            }

            // 頂点インデックス.
            {
                auto& accessor  = gltfModel.accessors[primitive.indices];
                auto& view      = gltfModel.bufferViews[accessor.bufferView];
                auto& buffer    = gltfModel.buffers[view.buffer];
                auto offset     = accessor.byteOffset + view.byteOffset;

                dstMesh.Indices.resize(accessor.count);

                switch(accessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    {
                        const auto ptr = reinterpret_cast<uint32_t*>(buffer.data.data() + offset);
                        memcpy(dstMesh.Indices.data(), ptr, accessor.count * sizeof(uint32_t));
                    }
                    break;

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        const auto ptr = reinterpret_cast<uint16_t*>(buffer.data.data() + offset);
                        for(size_t idx=0; idx<accessor.count; ++idx)
                        {
                            dstMesh.Indices[idx] = uint32_t(ptr[idx]);
                        }
                    }
                    break;
                }
            }

            // 法線データが無ければ生成.
            if (dstMesh.Normals.empty())
            { asdx::CalcNormals(dstMesh); }

            // 接線データが無ければ生成を試みる.
            if (dstMesh.Tangents.empty())
            { asdx::CalcTangents(dstMesh); }

            // メッシュを追加.
            model.Meshes.emplace_back(dstMesh);

            // プリミティブ番号インクリメント.
            primitiveIndex++;
        }
    }

    auto materialCount = gltfModel.materials.size();
    model.Materials.resize(materialCount);

    // マテリアル変換
    for(size_t i=0; i<materialCount; ++i)
    {
        auto& dstMat = model.Materials[i];
        auto& srcMat = gltfModel.materials[i];

        dstMat.Name = srcMat.name;

        // エミッシブマップ.
        auto index = srcMat.emissiveTexture.index; 
        if (index != -1)
        {
            auto& texture = gltfModel.textures[index];

            asdx::ResTexturePath info;
            info.Name = asdx::TAG_EMISSIVE;
            info.Path = gltfModel.images[texture.source].uri;
            dstMat.Textures.emplace_back(info);
        }

        // 法線マップ.
        index = srcMat.normalTexture.index;
        if (index != -1)
        {
            auto& texture = gltfModel.textures[index];

            asdx::ResTexturePath info;
            info.Name = asdx::TAG_NORMAL;
            info.Path = gltfModel.images[texture.source].uri;
            dstMat.Textures.emplace_back(info);
        }

        // オクルージョンマップ.
        index = srcMat.occlusionTexture.index;
        if (index != -1)
        {
            auto& texture = gltfModel.textures[index];

            asdx::ResTexturePath info;
            info.Name = asdx::TAG_OCCLUSION;
            info.Path = gltfModel.images[texture.source].uri;
            dstMat.Textures.emplace_back(info);
        }

        // ベースカラーマップ.
        index = srcMat.pbrMetallicRoughness.baseColorTexture.index;
        if (index != -1)
        {
            auto& texture = gltfModel.textures[index];

            asdx::ResTexturePath info;
            info.Name = asdx::TAG_BASE_COLOR;
            info.Path = gltfModel.images[texture.source].uri;
            dstMat.Textures.emplace_back(info);
        }

        // メタリック・ラフネスマップ.
        index = srcMat.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (index != -1)
        {
            auto& texture = gltfModel.textures[index];

            asdx::ResTexturePath info;
            info.Name = asdx::TAG_ORM;
            info.Path = gltfModel.images[texture.source].uri;
            dstMat.Textures.emplace_back(info);
        }

        // アルファ値.
        {
            asdx::ResProperty prop = {};
            prop.Name           = asdx::TAG_ALPHA;
            prop.Type           = asdx::PROPERTY_TYPE_FLOAT;
            prop.Value.Float    = (srcMat.alphaMode == "MASK") ? float(srcMat.alphaCutoff) : 1.0f;

            dstMat.Props.emplace_back(prop);
        }

        // エミッシブスケール.
        {
            asdx::ResProperty prop = {};
            prop.Name   = asdx::TAG_EMISSIVE;
            prop.Type   = asdx::PROPERTY_TYPE_FLOAT3;
            prop.Value.Float3.x = float(srcMat.emissiveFactor[0]);
            prop.Value.Float3.y = float(srcMat.emissiveFactor[1]);
            prop.Value.Float3.z = float(srcMat.emissiveFactor[2]);

            dstMat.Props.emplace_back(prop);
        }

        // ベースカラースケール.
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_BASE_COLOR;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT4;
            prop.Value.Float4.x = float(srcMat.pbrMetallicRoughness.baseColorFactor[0]);
            prop.Value.Float4.y = float(srcMat.pbrMetallicRoughness.baseColorFactor[1]);
            prop.Value.Float4.z = float(srcMat.pbrMetallicRoughness.baseColorFactor[2]);
            prop.Value.Float4.w = float(srcMat.pbrMetallicRoughness.baseColorFactor[3]);

            dstMat.Props.emplace_back(prop);
        }

        // メタルネススケール.
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_METALNESS;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT;
            prop.Value.Float = float(srcMat.pbrMetallicRoughness.metallicFactor);

            dstMat.Props.emplace_back(prop);
        }

        // ラフネススケール.
        {
            asdx::ResProperty prop = {};
            prop.Name = asdx::TAG_ROUGHNESS;
            prop.Type = asdx::PROPERTY_TYPE_FLOAT;
            prop.Value.Float = float(srcMat.pbrMetallicRoughness.roughnessFactor);

            dstMat.Props.emplace_back(prop);
        }
    }

    return true;
}
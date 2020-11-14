//-----------------------------------------------------------------------------
// File : AppPrimitive.cpp
// Desc : Debug Primitive.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <AppPrimitive.h>
#include <vector>
#include <asdxLogger.h>



///////////////////////////////////////////////////////////////////////////////
// BoxShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
BoxShape::BoxShape()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
BoxShape::~BoxShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool BoxShape::Init(ID3D11Device* pDevice, float size)
{
    // 頂点バッファの生成.
    {
        auto s = size * 0.5f;
        ShapeVertex vertices[] = {
            { asdx::Vector3(-s,  s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },
            { asdx::Vector3(-s, -s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },
            { asdx::Vector3( s, -s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },

            { asdx::Vector3(-s,  s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },
            { asdx::Vector3( s, -s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },
            { asdx::Vector3( s,  s, -s), asdx::Vector3(0.0f, 0.0f, -1.0f) },

            { asdx::Vector3(s,  s, -s), asdx::Vector3(1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(s, -s, -s), asdx::Vector3(1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(s, -s,  s), asdx::Vector3(1.0f, 0.0f, 0.0f) },

            { asdx::Vector3(s,  s, -s), asdx::Vector3(1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(s, -s,  s), asdx::Vector3(1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(s,  s,  s), asdx::Vector3(1.0f, 0.0f, 0.0f) },

            { asdx::Vector3( s,  s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },
            { asdx::Vector3( s, -s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },
            { asdx::Vector3(-s, -s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },

            { asdx::Vector3( s,  s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },
            { asdx::Vector3(-s, -s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },
            { asdx::Vector3(-s,  s, s), asdx::Vector3(0.0f, 0.0f, 1.0f) },

            { asdx::Vector3(-s,  s,  s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(-s, -s,  s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(-s, -s, -s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },

            { asdx::Vector3(-s,  s,  s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(-s, -s, -s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },
            { asdx::Vector3(-s,  s, -s), asdx::Vector3(-1.0f, 0.0f, 0.0f) },

            { asdx::Vector3(-s, s,  s), asdx::Vector3(0.0f, 1.0f, 0.0f) },
            { asdx::Vector3(-s, s, -s), asdx::Vector3(0.0f, 1.0f, 0.0f) },
            { asdx::Vector3( s, s, -s), asdx::Vector3(0.0f, 1.0f, 0.0f) },

            { asdx::Vector3(-s, s,  s), asdx::Vector3(0.0f, 1.0f, 0.0f) },
            { asdx::Vector3( s, s, -s), asdx::Vector3(0.0f, 1.0f, 0.0f) },
            { asdx::Vector3( s, s,  s), asdx::Vector3(0.0f, 1.0f, 0.0f) },

            { asdx::Vector3(-s, -s, -s), asdx::Vector3(0.0f, -1.0f, 0.0f) },
            { asdx::Vector3(-s, -s,  s), asdx::Vector3(0.0f, -1.0f, 0.0f) },
            { asdx::Vector3( s, -s,  s), asdx::Vector3(0.0f, -1.0f, 0.0f) },

            { asdx::Vector3(-s, -s, -s), asdx::Vector3(0.0f, -1.0f, 0.0f) },
            { asdx::Vector3( s, -s,  s), asdx::Vector3(0.0f, -1.0f, 0.0f) },
            { asdx::Vector3( s, -s, -s), asdx::Vector3(0.0f, -1.0f, 0.0f) },
        };

        auto vertexCount = uint32_t(sizeof(vertices) / sizeof(vertices[0]));
 
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = sizeof(ShapeVertex) * vertexCount;
        desc.BindFlags  = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = &vertices[0];

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(CbMesh);
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        
        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void BoxShape::Term()
{
    m_CB.Reset();
    m_VB.Reset();
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void BoxShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    if (pVB == nullptr || pCB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color;
    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->Draw(36, 0);
}


///////////////////////////////////////////////////////////////////////////////
// SphereShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
SphereShape::SphereShape()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
SphereShape::~SphereShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool SphereShape::Init(ID3D11Device* pDevice, float radius, int divide)
{
    // 頂点バッファとインデックスバッファの生成.
    {
        std::vector<ShapeVertex>  vertices;
        std::vector<uint32_t>       indices;

        uint32_t verticalSegments   = divide;
        uint32_t horizontalSegments = divide * 2;

        float radius = 1.0f;

        // Create rings of vertices at progressively higher latitudes.
        for (size_t i = 0; i <= verticalSegments; i++)
        {
            float v = 1 - (float)i / verticalSegments;

            float theta = (i * asdx::F_PI / verticalSegments) - asdx::F_PIDIV2;

            auto st = sin(theta);
            auto ct = cos(theta);

            // Create a single ring of vertices at this latitude.
            for (size_t j = 0; j <= horizontalSegments; j++)
            {
                float u = (float)j / horizontalSegments;

                float phi = j * asdx::F_2PI / horizontalSegments;

                auto sp = sin(phi);
                auto cp = cos(phi);

                auto normal = asdx::Vector3(sp * ct, st, cp * ct);
                auto pos = normal * radius;

                ShapeVertex v;
                v.Position = pos;
                v.Normal   = normal;

                vertices.push_back(v);
            }
        }

        // Fill the index buffer with triangles joining each pair of latitude rings.
        uint32_t stride = horizontalSegments + 1;

        for (auto i = 0u; i < verticalSegments; i++)
        {
            for (auto j = 0u; j <= horizontalSegments; j++)
            {
                auto nextI = i + 1;
                auto nextJ = (j + 1) % stride;

                indices.push_back(i * stride + j);
                indices.push_back(nextI * stride + j);
                indices.push_back(i * stride + nextJ);

                indices.push_back(i * stride + nextJ);
                indices.push_back(nextI * stride + j);
                indices.push_back(nextI * stride + nextJ);
            }
        }

        // 頂点バッファの生成.
        {
            auto vertexCount = uint32_t(vertices.size());

            D3D11_BUFFER_DESC desc = {};
            desc.Usage     = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = sizeof(ShapeVertex) * vertexCount;
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA res = {};
            res.pSysMem = &vertices[0];

            auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
            if (FAILED(hr))
            {
                ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
                return false;
            }
        }

        // インデックスバッファの生成.
        {
            m_Count = uint32_t(indices.size());

            D3D11_BUFFER_DESC desc = {};
            desc.Usage     = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = sizeof(uint32_t) * m_Count;
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA res = {};
            res.pSysMem = &indices[0];

            auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
            if (FAILED(hr))
            {
                ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void SphereShape::Term()
{
    m_CB.Reset();
    m_IB.Reset();
    m_VB.Reset();
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void SphereShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(m_Count, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// PyradmidShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
PyramidShape::PyramidShape()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
PyramidShape::~PyramidShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool PyramidShape::Init(ID3D11Device* pDevice, float length, float width)
{
    auto s = width * 0.5f;
    ShapeVertex vertices[5] = {
        { asdx::Vector3(0.0f, length, 0.0f), asdx::Vector3(0.0f, 1.0f, 0.0f) },

        { asdx::Vector3(-s, 0.0f, -s), asdx::Vector3(-1.0f, 0.0f, -1.0f) },
        { asdx::Vector3( s, 0.0f, -s), asdx::Vector3( 1.0f, 0.0f, -1.0f) },
        { asdx::Vector3( s, 0.0f,  s), asdx::Vector3( 1.0f, 0.0f,  1.0f) },
        { asdx::Vector3(-s, 0.0f,  s), asdx::Vector3(-1.0f, 0.0f,  1.0f) },
    };

    for(auto i=0; i<5; ++i)
    {
        vertices[i].Normal = asdx::Vector3::Normalize(vertices[i].Normal);
    }

    uint32_t indices[12] = {
        0, 2, 1,
        0, 3, 2,
        0, 4, 3,
        0, 1, 4,
    };

    // 頂点バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(ShapeVertex) * 5;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = &vertices[0];

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // インデックスバッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(uint32_t) * 12;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = &indices[0];

        auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = sizeof(CbMesh);
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void PyramidShape::Term()
{
    m_CB.Reset();
    m_VB.Reset();
    m_IB.Reset();
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void PyramidShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(12, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// BoneShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
BoneShape::BoneShape()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
BoneShape::~BoneShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool BoneShape::Init(ID3D11Device* pDevice, float length, float width)
{
    auto s = width * 0.5f;
    ShapeVertex vertices[6] = {
        { asdx::Vector3(0.0f, length, 0.0f), asdx::Vector3(0.0f, 1.0f, 0.0f) },

        { asdx::Vector3(-s, length * 0.1f, -s), asdx::Vector3(-1.0f, 0.0f, -1.0f) },
        { asdx::Vector3( s, length * 0.1f, -s), asdx::Vector3( 1.0f, 0.0f, -1.0f) },
        { asdx::Vector3( s, length * 0.1f,  s), asdx::Vector3( 1.0f, 0.0f,  1.0f) },
        { asdx::Vector3(-s, length * 0.1f,  s), asdx::Vector3(-1.0f, 0.0f,  1.0f) },

        { asdx::Vector3(0.0f, 0.0f, 0.0f), asdx::Vector3(0.0f, -1.0f, 0.0f) },
    };

    // 基底変換行列
    auto basis = asdx::Matrix(
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    for(auto i=0; i<6; ++i)
    {
        vertices[i].Position = asdx::Vector3::Transform(vertices[i].Position, basis);
        vertices[i].Normal   = asdx::Vector3::Normalize(vertices[i].Normal);
    }

    uint32_t indices[24] = {
        0, 2, 1,
        0, 3, 2,
        0, 4, 3,
        0, 1, 4,

        1, 2, 5,
        2, 3, 5,
        4, 3, 5,
        4, 1, 5,
    };

    // 頂点バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(ShapeVertex) * 6;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = &vertices[0];

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // インデックスバッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(uint32_t) * 24;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = &indices[0];

        auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = sizeof(CbMesh);
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void BoneShape::Term()
{
    m_CB.Reset();
    m_IB.Reset();
    m_VB.Reset();
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void BoneShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(24, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// DiskShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
DiskShape::DiskShape()
: m_Count(0)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
DiskShape::~DiskShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool DiskShape::Init(ID3D11Device* pDevice, float radius, int divide)
{
    if (pDevice == nullptr || divide < 3 || radius <= 0.0f)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    std::vector<uint32_t> indices;
    std::vector<ShapeVertex> vertices;

    // 中心.
    ShapeVertex v;
    v.Position = asdx::Vector3(0.0f, 0.0f, 0.0f);
    v.Normal   = asdx::Vector3(0.0f, 1.0f, 0.0f);
    vertices.push_back(v);

    auto stepAngle = asdx::F_2PI / divide;
    auto angle = 0.0f;

    v.Position.x = sin(angle) * radius;
    v.Position.y = 0.0f;
    v.Position.z = cos(angle) * radius;
    vertices.push_back(v);

    auto prevIdx = 1;

    for(auto i=0; i<divide; i++)
    {
        auto currIdx = prevIdx + 1;
        angle = stepAngle * ((i + 1) % divide);

        v.Position.x = sin(angle) * radius;
        v.Position.y = 0.0f;
        v.Position.z = cos(angle) * radius;

        vertices.push_back(v);

        indices.push_back(0);
        indices.push_back(currIdx);
        indices.push_back(prevIdx);

        prevIdx = currIdx;
    }

    // 頂点バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(ShapeVertex) * vertices.size());
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = vertices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // インデックスバッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = indices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = sizeof(CbMesh);
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    m_Count = uint32_t(indices.size());

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void DiskShape::Term()
{
    m_VB.Reset();
    m_IB.Reset();
    m_CB.Reset();
    m_Count = 0;
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void DiskShape::Draw
(
    ID3D11DeviceContext* pContext,
    const asdx::Matrix& world,
    const asdx::Vector4 & color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(m_Count, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// ConeShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
ConeShape::ConeShape()
: m_Count(0)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
ConeShape::~ConeShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool ConeShape::Init
(
    ID3D11Device*   pDevice,
    float           radius,
    float           height,
    int             divide
)
{
    if (pDevice == nullptr || divide < 4 || radius <= 0.0f || height <= 0.0f)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    std::vector<uint32_t>    indices;
    std::vector<ShapeVertex> vertices;

    // 中心.
    ShapeVertex v;
    v.Position = asdx::Vector3(0.0f, 0.0f, 0.0f);
    v.Normal   = asdx::Vector3(0.0f, -1.0f, 0.0f);
    vertices.push_back(v);

    auto stepAngle = asdx::F_2PI / divide;
    auto angle = 0.0f;

    v.Normal.x = sin(angle);
    v.Normal.y = 0.0f;
    v.Normal.z = cos(angle);
    v.Position = v.Normal * radius;
    vertices.push_back(v);

    auto prevIdx = 1;

    // 底面.
    for(auto i=0; i<divide; i++)
    {
        auto currIdx = prevIdx + 1;
        angle = stepAngle * ((i + 1) % divide);

        v.Normal.x = sin(angle);
        v.Normal.y = 0.0f;
        v.Normal.z = cos(angle);
        v.Position = v.Normal * radius;

        vertices.push_back(v);

        indices.push_back(0);
        indices.push_back(prevIdx);
        indices.push_back(currIdx);

        prevIdx = currIdx;
    }

    auto centerIdx = UINT(vertices.size());
    v.Position = asdx::Vector3(0.0f, height, 0.0f);
    v.Normal   = asdx::Vector3(0.0f, 1.0f, 0.0f);
    vertices.push_back(v);

    prevIdx = 1;

    for (auto i=0; i<divide; ++i)
    {
        auto currIdx = prevIdx + 1;
        indices.push_back(centerIdx);
        indices.push_back(currIdx);
        indices.push_back(prevIdx);

        prevIdx = currIdx;
    }

    // 頂点バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(ShapeVertex) * vertices.size());
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = vertices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // インデックスバッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = indices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = UINT(sizeof(CbMesh));
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    m_Count = uint32_t(indices.size());

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void ConeShape::Term()
{
    m_VB.Reset();
    m_IB.Reset();
    m_CB.Reset();
    m_Count = 0;
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void ConeShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(m_Count, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// CylinderShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
CylinderShape::CylinderShape()
: m_Count(0)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
CylinderShape::~CylinderShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool CylinderShape::Init
(
    ID3D11Device*   pDevice,
    float           radius,
    float           height,
    int             divide
)
{
    if (pDevice == nullptr || divide < 4 || radius <= 0.0f || height <= 0.0f)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    if (pDevice == nullptr || divide < 4 || radius <= 0.0f || height <= 0.0f)
    {
        ELOGA("Error : Invalid Argument.");
        return false;
    }

    std::vector<uint32_t> indices;
    std::vector<ShapeVertex> vertices;

    // 底面中心.
    ShapeVertex v;
    v.Position = asdx::Vector3(0.0f, -height * 0.5f, 0.0f);
    v.Normal   = asdx::Vector3(0.0f, -1.0f, 0.0f);
    vertices.push_back(v);
    auto prevIdx = UINT(vertices.size());

    auto stepAngle = asdx::F_2PI / divide;
    auto angle = 0.0f;
    auto halfHeight = height * 0.5f;

    v.Normal.x = sin(angle);
    v.Normal.y = 0.0f;
    v.Normal.z = cos(angle);
    v.Position = v.Normal * radius;
    v.Position.y = -halfHeight;
    vertices.push_back(v);

    // 底面.
    for(auto i=0; i<divide; i++)
    {
        auto currIdx = prevIdx + 1;
        angle = stepAngle * ((i + 1) % divide);

        v.Normal.x = sin(angle);
        v.Normal.y = 0.0f;
        v.Normal.z = cos(angle);
        v.Position = v.Normal * radius;
        v.Position.y = -halfHeight;

        vertices.push_back(v);

        indices.push_back(0);
        indices.push_back(prevIdx);
        indices.push_back(currIdx);

        prevIdx = currIdx;
    }

    // 天面中心.
    auto centerIdx = UINT(vertices.size());
    v.Position = asdx::Vector3(0.0f, height * 0.5f, 0.0f);
    v.Normal   = asdx::Vector3(0.0f, 1.0f, 0.0f);
    vertices.push_back(v);
    prevIdx = UINT(vertices.size());

    v.Normal.x = sin(angle);
    v.Normal.y = 0.0f;
    v.Normal.z = cos(angle);
    v.Position = v.Normal * radius;
    v.Position.y = halfHeight;
    vertices.push_back(v);

    // 天面.
    for (auto i=0; i<divide; ++i)
    {
        auto currIdx = prevIdx + 1;
        angle = stepAngle * ((i + 1) % divide);

        v.Normal.x = sin(angle);
        v.Normal.y = 0.0f;
        v.Normal.z = cos(angle);
        v.Position = v.Normal * radius;
        v.Position.y = halfHeight;

        vertices.push_back(v);

        indices.push_back(centerIdx);
        indices.push_back(currIdx);
        indices.push_back(prevIdx);

        prevIdx = currIdx;
    }

    auto prevB = 1u;
    auto prevT = centerIdx + 1u;
    for(auto i=0; i<divide; ++i)
    {
        auto currB = prevB + 1;
        auto currT = prevT + 1;

        indices.push_back(currT);
        indices.push_back(prevB);
        indices.push_back(prevT);

        indices.push_back(currT);
        indices.push_back(currB);
        indices.push_back(prevB);

        prevB = currB;
        prevT = currT;
    }

    // 頂点バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(ShapeVertex) * vertices.size());
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = vertices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_VB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // インデックスバッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA res = {};
        res.pSysMem = indices.data();

        auto hr = pDevice->CreateBuffer(&desc, &res, m_IB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage      = D3D11_USAGE_DEFAULT;
        desc.ByteWidth  = UINT(sizeof(CbMesh));
        desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

        auto hr = pDevice->CreateBuffer(&desc, nullptr, m_CB.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateBuffer() Failed.");
            return false;
        }
    }

    m_Count = uint32_t(indices.size());

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void CylinderShape::Term()
{
    m_VB.Reset();
    m_IB.Reset();
    m_CB.Reset();
    m_Count = 0;
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void CylinderShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto pVB = m_VB.GetPtr();
    auto pCB = m_CB.GetPtr();
    auto pIB = m_IB.GetPtr();
    if (pVB == nullptr || pCB == nullptr || pIB == nullptr)
    { return; }

    UINT stride = sizeof(ShapeVertex);
    UINT offset = 0;

    CbMesh res;
    res.World = world;
    res.Color = color;

    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetConstantBuffers(1, 1, &pCB);
    pContext->DrawIndexed(m_Count, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// ArrowShape class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
ArrowShape::ArrowShape()
: m_OffsetCone(asdx::Matrix::CreateIdentity())
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
ArrowShape::~ArrowShape()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool ArrowShape::Init
(
    ID3D11Device*   pDevice,
    float           cylinderRadius,
    float           cylinderHeight,
    float           coneRadius,
    float           coneHeight,
    int             divide
)
{
    if (!m_Cylinder.Init(pDevice, cylinderRadius, cylinderHeight, divide))
    { return false; }

    if (!m_Cone.Init(pDevice, coneRadius, coneHeight, divide))
    { return false; }

    m_OffsetCone = asdx::Matrix::CreateTranslation(0.0f, cylinderHeight * 0.5f, 0.0f);

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void ArrowShape::Term()
{
    m_Cylinder.Term();
    m_Cone    .Term();
    m_OffsetCone = asdx::Matrix::CreateIdentity();
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void ArrowShape::Draw
(
    ID3D11DeviceContext*    pContext,
    const asdx::Matrix&     world,
    const asdx::Vector4&    color
)
{
    auto coneWorld = world * m_OffsetCone;
    m_Cone.Draw(pContext, coneWorld, color);

    m_Cylinder.Draw(pContext, world, color);
}

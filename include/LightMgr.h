#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <string>
#include <vector>
#include <d3d11.h>
#include <tinyxml2.h>
#include <asdxMath.h>
#include <asdxRef.h>
#include <asdxSkyBox.h>
#include <asdxConstantBuffer.h>


///////////////////////////////////////////////////////////////////////////////
// EditorLight structure
///////////////////////////////////////////////////////////////////////////////
class EditorLight
{
    friend class LightMgr;

public:
    std::string         Tag;
    asdx::Vector3       SunLightDir;
    float               SunLightIntensity;
    std::string         IBLPath;
    float               IBLIntensity;
    float               IBLRotation;

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    EditorLight();

    ~EditorLight();

    void Reset();

    //-------------------------------------------------------------------------
    //! @brief      シリアライズします.
    //-------------------------------------------------------------------------
    tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc);

    //-------------------------------------------------------------------------
    //! @brief      デシリアライズします.
    //-------------------------------------------------------------------------
    void Deserialize(tinyxml2::XMLElement* element);

    ID3D11ShaderResourceView* GetOriginal();

    ID3D11ShaderResourceView* GetDiffuseLD();

    ID3D11ShaderResourceView* GetSpecularLD();


private:
    asdx::RefPtr<ID3D11ShaderResourceView>  m_Original;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_DiffuseLD;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_SpecularLD;
};


///////////////////////////////////////////////////////////////////////////////
// LightMgr
///////////////////////////////////////////////////////////////////////////////
class LightMgr
{
public:
    static LightMgr& Instance();

    bool Init(const char* path);
    void Term();

    void Draw(ID3D11DeviceContext* pContext);

    ID3D11ShaderResourceView* GetEnvBRDF() const;
    ID3D11ShaderResourceView* GetDiffuseLD() const;
    ID3D11ShaderResourceView* GetSpecularLD() const;
    ID3D11Buffer* GetBuffer() const;

private:
    static LightMgr             s_Instance;
    std::vector<EditorLight>    m_Light;
    size_t                      m_CurrentIndex;

    asdx::SkyBox                            m_SkyBox;
    asdx::RefPtr<ID3D11ComputeShader*>      m_PrefilterCS;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_EnvBRDF;
    asdx::ConstantBuffer                    m_LightCB;

    void ConvolutionDiffuse(ID3D11DeviceContext* pContext);
    void ConvolutionSpecular(ID3D11DeviceContext* pContext);
};
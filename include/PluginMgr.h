//-----------------------------------------------------------------------------
// File : PluginMgr.h
// Desc : Plugin Manager.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include <d3d11.h>
#include <d3d11shader.h>
#include <tinyxml2.h>
#include <asdxRef.h>
#include <asdxEditParam.h>


///////////////////////////////////////////////////////////////////////////////
// UI_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum UI_TYPE
{
    UI_BOOL         = 0,
    UI_INT          = 1,
    UI_FLOAT        = 2,
    UI_FLOAT2       = 3,
    UI_FLOAT3       = 4,
    UI_FLOAT4       = 5,
    UI_COLOR3       = 6,
    UI_COLOR4       = 7,
    UI_BIT32        = 8,
    UI_TEXTURE1D    = 9,
    UI_TEXTURE2D    = 10,
    UI_TEXTURE3D    = 11,
};

///////////////////////////////////////////////////////////////////////////////
// DEAAULT_TEXTURE_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum DEFAULT_TEXTURE_TYPE
{
    DEFAULT_TEXTURE_WHITE       = 0,
    DEFAULT_TEXTURE_BLACK       = 1,
    DEFAULT_TEXTURE_NORMAL      = 2,
    DEFAULT_TEXTURE_ORM         = 3,
    DEFAULT_TEXTURE_VELCOITY    = 4,
};

struct UiDefShadowCast
{
    bool Editable;
    bool Default;
};

struct UiDefShadowReceive
{
    bool Editable;
    bool Default;
};

struct UiDefBlendState
{
    bool Editable;
    int  Default;
};

struct UiDefRasterizerState
{
    bool Editable;
    int  Default;
};

struct UiDefDepthState
{
    bool Editable;
    int  Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefBool structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefBool
{
    std::string     DisplayTag;
    std::string     Target;
    bool            Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefInt structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefInt
{
    std::string     DisplayTag;
    std::string     Target;
    int             Default;
    int             Step;
    int             Mini;
    int             Maxi;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefFloat structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefFloat
{
    std::string     DisplayTag;
    std::string     Target;
    float           Default;
    float           Step;
    float           Mini;
    float           Maxi;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefFloat2 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefFloat2
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector2   Default;
    float           Step;
    float           Mini;
    float           Maxi;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefFloat3 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefFloat3
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector3   Default;
    float           Step;
    float           Mini;
    float           Maxi;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefFloat4 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefFloat4
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector4  Default;
    float           Step;
    float           Mini;
    float           Maxi;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefColor3 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefColor3
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector3   Default;
    bool            Wheel;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefColor4 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefColor4
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector4   Default;
    bool            Wheel;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefBit32 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefBit32
{
    std::string     DisplayTag;
    std::string     Target;
    uint32_t        Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefTexture2D structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefTexture2D
{
    std::string             DisplayTag;
    std::string             Target;
    DEFAULT_TEXTURE_TYPE    Default;
};

tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefBool& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefInt& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat2& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat3& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefFloat4& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefColor3& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefColor4& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefBit32& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiDefTexture2D& value);

void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefBool>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefInt>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat2>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat3>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefFloat4>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefColor3>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefColor4>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefBit32>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiDefTexture2D>& value);


///////////////////////////////////////////////////////////////////////////////
// UiParam class
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class UiParam
{
public:
    std::string     Tag;
    T               Param;
    uint32_t        Offset;
};

///////////////////////////////////////////////////////////////////////////////
// UiImage class
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class UiImage
{
public:
    std::string             Tag;
    T                       Param;
    uint8_t                 Register;
};

using UiBool        = UiParam<asdx::EditBool>;
using UiInt         = UiParam<asdx::EditInt>;
using UiFloat       = UiParam<asdx::EditFloat>;
using UiFloat2      = UiParam<asdx::EditFloat2>;
using UiFloat3      = UiParam<asdx::EditFloat3>;
using UiFloat4      = UiParam<asdx::EditFloat4>;
using UiColor3      = UiParam<asdx::EditColor3>;
using UiColor4      = UiParam<asdx::EditColor4>;
using UiBit32       = UiParam<asdx::EditBit32>;
using UiTexture2D   = UiImage<asdx::EditTexture2D>;


tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiBool& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiInt& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat2& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat3& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiFloat4& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiColor3& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiColor4& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiBit32& value);
tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc, const UiTexture2D& value);

void Deserialize(tinyxml2::XMLElement* element, std::vector<UiBool>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiInt>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat2>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat3>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiFloat4>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiColor3>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiColor4>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiBit32>& value);
void Deserialize(tinyxml2::XMLElement* element, std::vector<UiTexture2D>& value);



///////////////////////////////////////////////////////////////////////////////
// MaterialInstance class
///////////////////////////////////////////////////////////////////////////////
class MaterialInstance
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    friend class PluginMaterial;
    friend class PluginShader;
    friend class PluginMgr;

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING*/

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      �V���A�����C�Y���܂�.
    //-------------------------------------------------------------------------
    tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc);

    //-------------------------------------------------------------------------
    //! @brief      �f�V���A���C�Y���܂�.
    //-------------------------------------------------------------------------
    void Deserialize(tinyxml2::XMLElement* element);

private:
    //=========================================================================
    // private variables.
    //========================================================================
    std::string                 m_MaterialName;
    asdx::EditBool              m_ShadowCast;
    asdx::EditBool              m_ShadowReceive;
    asdx::EditInt               m_BlendState;
    asdx::EditInt               m_RasterizerState;
    asdx::EditInt               m_DepthStencilState;
    std::vector<UiBool>         m_Bool;
    std::vector<UiInt>          m_Int;
    std::vector<UiFloat>        m_Float;
    std::vector<UiFloat2>       m_Float2;
    std::vector<UiFloat3>       m_Float3;
    std::vector<UiFloat4>       m_Float4;
    std::vector<UiColor3>       m_Color3;
    std::vector<UiColor4>       m_Color4;
    std::vector<UiBit32>        m_Bit32;
    std::vector<UiTexture2D>    m_Texture2D;

    //=========================================================================
    // private methods.
    //=========================================================================
    MaterialInstance();
    ~MaterialInstance();

    MaterialInstance(const MaterialInstance& value) = delete;
    MaterialInstance& operator = (const MaterialInstance& value) = delete;
};


///////////////////////////////////////////////////////////////////////////////
// PluginShader class
///////////////////////////////////////////////////////////////////////////////
class PluginShader
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    ///////////////////////////////////////////////////////////////////////////
    // MemberInfo structure
    ///////////////////////////////////////////////////////////////////////////
    struct MemberInfo
    {
        uint32_t    Offset  = 0;
        uint32_t    Size    = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    // BufferInfo structure
    ///////////////////////////////////////////////////////////////////////////
    struct BufferInfo
    {
        uint32_t                            BufferSize = 0;
        std::map<std::string, MemberInfo>   MemberTable;

        BufferInfo()
        { /* DO_NOTHING */ }

        ~BufferInfo()
        { MemberTable.clear(); }
    };

    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      �R���X�g���N�^�ł�.
    //-------------------------------------------------------------------------
    PluginShader();

    //-------------------------------------------------------------------------
    //! @brief      �f�X�g���N�^�ł�.
    //-------------------------------------------------------------------------
    ~PluginShader();

    //-------------------------------------------------------------------------
    //! @brief      ���[�h���܂�.
    //-------------------------------------------------------------------------
    bool Load(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      ����������s���܂�.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void Bind(ID3D11DeviceContext* pContext) const;

    //-------------------------------------------------------------------------
    //! @brief      �p�����[�^���X�V���܂�.
    //-------------------------------------------------------------------------
    void Update(ID3D11DeviceContext* pContext, const MaterialInstance* instance);

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_�̐ݒ���������܂�.
    //-------------------------------------------------------------------------
    void Unbind(ID3D11DeviceContext* pContext) const;

    //-------------------------------------------------------------------------
    //! @brief      �萔�o�b�t�@��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void SetCBV(
        ID3D11DeviceContext*    pContext,
        const std::string&      name,
        ID3D11Buffer*           pCB) const;

    //-------------------------------------------------------------------------
    //! @brief      �e�N�X�`����ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void SetSRV(
        ID3D11DeviceContext*        pContext,
        const std::string&          name,
        ID3D11ShaderResourceView*   pSRV) const;

    //-------------------------------------------------------------------------
    //! @brief      �����o�[�����������܂�.
    //-------------------------------------------------------------------------
    bool FindMemberInfo(
        const std::string&  bufferName,
        const std::string&  memberName,
        MemberInfo&         result) const;

    //-------------------------------------------------------------------------
    //! @brief      �e�N�X�`���X���b�g���������܂�.
    //-------------------------------------------------------------------------
    bool FindTextureSlot(const std::string& textureName, uint8_t& slot) const;


private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::RefPtr<ID3D11PixelShader>         m_PS;
    asdx::RefPtr<ID3D11Buffer>              m_EditableCB;
    std::map<std::string, uint8_t>          m_TableCBV;
    std::map<std::string, uint8_t>          m_TableSRV;
    std::map<std::string, uint8_t>          m_TableUAV;
    std::map<std::string, BufferInfo>       m_BufferTable;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};


///////////////////////////////////////////////////////////////////////////////
// PluginMaterial class
///////////////////////////////////////////////////////////////////////////////
class PluginMaterial
{
    //=========================================================================
    // friend of classes and methods.
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
    //! @brief      �R���X�g���N�^�ł�.
    //-------------------------------------------------------------------------
    PluginMaterial();

    //-------------------------------------------------------------------------
    //! @brief      �f�X�g���N�^�ł�.
    //-------------------------------------------------------------------------
    ~PluginMaterial();

    //-------------------------------------------------------------------------
    //! @brief      ���O���擾���܂�.
    //-------------------------------------------------------------------------
    const std::string& GetName() const;

    //-------------------------------------------------------------------------
    //! @brief      ���[�h���܂�.
    //-------------------------------------------------------------------------
    bool Load(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      �Z�[�u���܂�.
    //-------------------------------------------------------------------------
    bool Save(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      ����������s���܂�.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void Bind(ID3D11DeviceContext* pContext, const MaterialInstance* instance);

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_�ݒ���������܂�.
    //-------------------------------------------------------------------------
    void Unbind(ID3D11DeviceContext* pContext);

    //-------------------------------------------------------------------------
    //! @brief      �}�e���A����ҏW���܂�.
    //-------------------------------------------------------------------------
    void Edit(MaterialInstance* instance);

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_���擾���܂�.
    //-------------------------------------------------------------------------
    const PluginShader* GetShader() const;

    //-------------------------------------------------------------------------
    //! @brief      �f�t�H���g�l��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void Reset(MaterialInstance* instance);

    //-------------------------------------------------------------------------
    //! @brief      �e���v���[�g�𐶐����܂�.
    //-------------------------------------------------------------------------
    static PluginMaterial CreateTemplate();

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::string                     m_Name;
    std::string                     m_ShaderPath;
    PluginShader                    m_Shader;
    UiDefShadowCast                 m_ShadowCast;
    UiDefShadowReceive              m_ShadowReceive;
    UiDefBlendState                 m_BlendState;
    UiDefRasterizerState            m_RasterizerState;
    UiDefDepthState                 m_DepthState;
    std::vector<UiDefBool>          m_Bool;
    std::vector<UiDefInt>           m_Int;
    std::vector<UiDefFloat>         m_Float;
    std::vector<UiDefFloat2>        m_Float2;
    std::vector<UiDefFloat3>        m_Float3;
    std::vector<UiDefFloat4>        m_Float4;
    std::vector<UiDefColor3>        m_Color3;
    std::vector<UiDefColor4>        m_Color4;
    std::vector<UiDefBit32>         m_Bit32;
    std::vector<UiDefTexture2D>     m_Texture2D;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      �V���A���C�Y���܂�.
    //-------------------------------------------------------------------------
    tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument* doc);

    //-------------------------------------------------------------------------
    //! @brief      �f�V���A���C�Y���܂�.
    //-------------------------------------------------------------------------
    void Deserialize(tinyxml2::XMLDocument* doc);
};

///////////////////////////////////////////////////////////////////////////////
// PluginMgr class
///////////////////////////////////////////////////////////////////////////////
class PluginMgr
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
    //! @brief      �V���O���g���C���X�^���X���擾���܂�.
    //-------------------------------------------------------------------------
    static PluginMgr& Instance();

    //-------------------------------------------------------------------------
    //! @brief      ���[�h���܂�.
    //-------------------------------------------------------------------------
    bool Load();

    //-------------------------------------------------------------------------
    //! @brief      ����������s���܂�.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      �}�e���A�����������܂�.
    //-------------------------------------------------------------------------
    bool FindMaterial(const std::string& name, PluginMaterial** result);

    //-------------------------------------------------------------------------
    //! @brief      �}�e���A���C���X�^���X�𐶐����܂�.
    //-------------------------------------------------------------------------
    MaterialInstance* CreateInstance(const std::string& name);

    //-------------------------------------------------------------------------
    //! @brief      �}�e���A���C���X�^���X��j�����܂�.
    //-------------------------------------------------------------------------
    void DeleteInstance(MaterialInstance*& instance);

    //-------------------------------------------------------------------------
    //! @brief      �R���{�{�b�N�X��`�悵�܂�.
    //-------------------------------------------------------------------------
    const std::string& DrawCombo(const std::string& selected);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    static PluginMgr                        s_Instance;
    std::map<std::string, PluginMaterial*>  m_Materials;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
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
#include <asdxDisposer.h>
#include <ExportContext.h>


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
// DEFAULT_TEXTURE_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum DEFAULT_TEXTURE_TYPE
{
    DEFAULT_TEXTURE_CHECKER_BOARD   = 0,
    DEFAULT_TEXTURE_WHITE           = 1,
    DEFAULT_TEXTURE_BLACK           = 2,
    DEFAULT_TEXTURE_NORMAL          = 3,
    DEFAULT_TEXTURE_ORM             = 4,
    DEFAULT_TEXTURE_VELCOITY        = 5,

    DEFAULT_TEXTURE_COUNT
};

///////////////////////////////////////////////////////////////////////////////
// CONVERTER_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum CONVERTER_TYPE
{
    CONVERTER_NONE    = 0,    // �ϊ�����.
    CONVERTER_RADIAN  = 1,    // ���W�A���ɕϊ�.
    CONVERTER_DEGREE  = 2,    // �x�ɕϊ�.
};

///////////////////////////////////////////////////////////////////////////////
// UiDefShadowCast structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefShadowCast
{
    bool Editable;
    bool Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefShadowReceive structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefShadowReceive
{
    bool Editable;
    bool Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefBlendState structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefBlendState
{
    bool Editable;
    int  Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefRasterizerState structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefRasterizerState
{
    bool Editable;
    int  Default;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefDepthState structure
///////////////////////////////////////////////////////////////////////////////
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
    CONVERTER_TYPE  Converter;
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
    CONVERTER_TYPE  Converter;
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
    CONVERTER_TYPE  Converter;
};

///////////////////////////////////////////////////////////////////////////////
// UiDefFloat4 structure
///////////////////////////////////////////////////////////////////////////////
struct UiDefFloat4
{
    std::string     DisplayTag;
    std::string     Target;
    asdx::Vector4   Default;
    float           Step;
    float           Mini;
    float           Maxi;
    CONVERTER_TYPE  Converter;
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
    uint32_t        Offset    = 0;
    CONVERTER_TYPE  Converter = CONVERTER_NONE;
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
    DEFAULT_TEXTURE_TYPE    Default;
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


//-----------------------------------------------------------------------------
//! @brief      �G�N�X�|�[�^�[���Ăяo���C�G�N�X�|�[�g���������s���܂�.
//!
//! @param[in]      dllname     DLL�t�@�C���p�X.
//! @param[in]      context     �G�N�X�|�[�g�R���e�L�X�g.
//! @retval true    �G�N�X�|�[�g����.
//! @retval false   �G�N�X�|�[�g���s.
//-----------------------------------------------------------------------------
bool CallExporter(const char* dllname, const ExportContext* context);


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
    friend class EditorMaterial;

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

    //-------------------------------------------------------------------------
    //! @brief      �V���h�E���L���X�g���邩�ǂ���?
    //-------------------------------------------------------------------------
    bool CastShadow() const;

    //-------------------------------------------------------------------------
    //! @brief      �V���h�E�����V�[�u���邩�ǂ���?
    //-------------------------------------------------------------------------
    bool ReceiveShadow() const;

    //-------------------------------------------------------------------------
    //! @brief      ���X�^���C�U�[�X�e�[�g���擾���܂�.
    //-------------------------------------------------------------------------
    int GetRasterizerState() const;

    //-------------------------------------------------------------------------
    //! @brief      �u�����h�X�e�[�g���擾���܂�.
    //-------------------------------------------------------------------------
    int GetBlendState() const;

    //-------------------------------------------------------------------------
    //! @brief      �[�x�X�e�[�g���擾���܂�.
    //-------------------------------------------------------------------------
    int GetDepthState() const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::string                 m_MaterialName;
    asdx::EditBool              m_ShadowCast;
    asdx::EditBool              m_ShadowReceive;
    asdx::EditInt               m_BlendState;
    asdx::EditInt               m_RasterizerState;
    asdx::EditInt               m_DepthState;
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
    bool Load(const char* path, const char* entryPoint);

    //-------------------------------------------------------------------------
    //! @brief      �����[�h���܂�.
    //-------------------------------------------------------------------------
    bool Reload();

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
    std::string                             m_Path;
    std::string                             m_EntryPoint;

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
    friend class PluginMgr;
    friend class EditorMaterial;

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
    //! @brief      �V�F�[�_�������[�h���܂�.
    //-------------------------------------------------------------------------
    bool ReloadShader();

    //-------------------------------------------------------------------------
    //! @brief      ����������s���܂�.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void Bind(ID3D11DeviceContext* pContext, const MaterialInstance* instance, bool lightingPass = true);

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_�ݒ���������܂�.
    //-------------------------------------------------------------------------
    void Unbind(ID3D11DeviceContext* pContext, bool lightingPass = true);

    //-------------------------------------------------------------------------
    //! @brief      �}�e���A����ҏW���܂�.
    //-------------------------------------------------------------------------
    void Edit(MaterialInstance* instance);

    //-------------------------------------------------------------------------
    //! @brief      ���C�e�B���O�V�F�[�_���擾���܂�.
    //-------------------------------------------------------------------------
    const PluginShader* GetLightingShader() const;

    //-------------------------------------------------------------------------
    //! @brief      �V���h�E�C���O�V�F�[�_���擾���܂�.
    //-------------------------------------------------------------------------
    const PluginShader* GetShadowingShader() const;

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
    PluginShader                    m_LightingShader;
    PluginShader                    m_ShadowingShader;
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
    //! @brief      �V�F�[�_�������[�h���܂�.
    //-------------------------------------------------------------------------
    void ReloadShader();

    //-------------------------------------------------------------------------
    //! @brief      �G�N�X�|�[�^�[�������[�h���܂�.
    //-------------------------------------------------------------------------
    void ReloadExporter();

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
    const std::string& DrawTypeCombo(const std::string& selected);

    //-------------------------------------------------------------------------
    //! @brief      �^�C�v�t�B���^�R���{�{�b�N�X��`�悵�܂�.
    //-------------------------------------------------------------------------
    const std::string& DrawFilterCombo(const std::string& selected);

    //-------------------------------------------------------------------------
    //! @brief      �G�N�X�|�[�^�[�R���{�{�b�N�X��`�悵�܂�.
    //-------------------------------------------------------------------------
    const std::string& DrawExporterCombo(const std::string& selected);

    //-------------------------------------------------------------------------
    //! @brief      �f�t�H���g�e�N�X�`�����擾���܂�.
    //-------------------------------------------------------------------------
    ID3D11ShaderResourceView* GetDefaultSRV(DEFAULT_TEXTURE_TYPE type) const;

    //-------------------------------------------------------------------------
    //! @brief      �V�F�[�_��j�����܂�.
    //-------------------------------------------------------------------------
    void DisposeShader(ID3D11PixelShader*& pItem);

    //-------------------------------------------------------------------------
    //! @brief      �o�b�t�@��j�����܂�.
    //-------------------------------------------------------------------------
    void DisposeBuffer(ID3D11Buffer*& pItem);

    //-------------------------------------------------------------------------
    //! @brief      �����^�C�~���O��ݒ肵�܂�.
    //-------------------------------------------------------------------------
    void Sync();

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    static PluginMgr                        s_Instance;
    std::map<std::string, PluginMaterial*>  m_Materials;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_DefaultSRV[DEFAULT_TEXTURE_COUNT];
    asdx::Disposer<ID3D11PixelShader>       m_ShaderDisposer;
    asdx::Disposer<ID3D11Buffer>            m_BufferDisposer;
    std::vector<std::string>                m_Exporters;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
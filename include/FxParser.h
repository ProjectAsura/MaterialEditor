//-----------------------------------------------------------------------------
// File : FxParser.h
// Desc : Shader Effect File Parser Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Tokenizer.h"
#include <vector>
#include <map>


namespace asura {

///////////////////////////////////////////////////////////////////////////////
// SHADER_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum SHADER_TYPE
{
    SHADER_TYPE_VERTEX = 0,         //!< 頂点シェーダ.
    SHADER_TYPE_DOMAIN,             //!< ドメインシェーダ.
    SHADER_TYPE_GEOMETRY,           //!< ジオメトリシェーダ.
    SHADER_TYPE_HULL,               //!< ハルシェーダ.
    SHADER_TYPE_PIXEL,              //!< ピクセルシェーダ.
    SHADER_TYPE_COMPUTE,            //!< コンピュートシェーダ.
    SHADER_TYPE_AMPLIFICATION,      //!< 増幅シェーダ.
    SHADER_TYPE_MESH,               //!< メッシュシェーダ.
};

///////////////////////////////////////////////////////////////////////////////
// POLYGON_MODE enum
///////////////////////////////////////////////////////////////////////////////
enum POLYGON_MODE
{
    POLYGON_MODE_WIREFRAME = 0,     //!< ワイヤーフレーム.
    POLYGON_MODE_SOLID,             //!< ポリゴン.
};

///////////////////////////////////////////////////////////////////////////////
// BLEND_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum BLEND_TYPE
{
    BLEND_TYPE_ZERO,                //!< (0, 0, 0)
    BLEND_TYPE_ONE,                 //!< (1, 1, 1)
    BLEND_TYPE_SRC_COLOR,           //!< (src_r, src_g, src_b)
    BLEND_TYPE_INV_SRC_COLOR,       //!< (1-src_r, 1-src_g, 1-src_b).
    BLEND_TYPE_SRC_ALPHA,           //!< src_a
    BLEND_TYPE_INV_SRC_ALPHA,       //!< 1-src_a
    BLEND_TYPE_DST_ALPHA,           //!< dst_a
    BLEND_TYPE_INV_DST_ALPHA,       //!< 1-dst_a
    BLEND_TYPE_DST_COLOR,           //!< (dst_r, dst_r, dst_b)
    BLEND_TYPE_INV_DST_COLOR,       //!< (1-dst_r, 1-dst_g, 1-dst_b)
};

///////////////////////////////////////////////////////////////////////////////
// FILTER_MODE enum
//////////////////////////////////////////////////////////////////////////////
enum FILTER_MODE
{
    FILTER_MODE_NEAREST,
    FILTER_MODE_LINEAR
};

///////////////////////////////////////////////////////////////////////////////
// MIPMAP_MODE enum
///////////////////////////////////////////////////////////////////////////////
enum MIPMAP_MODE
{
    MIPMAP_MODE_NEAREST,
    MIPMAP_MODE_LINEAR,
    MIPMAP_MODE_NONE,
};

///////////////////////////////////////////////////////////////////////////////
// ADDRESS_MODE enum
///////////////////////////////////////////////////////////////////////////////
enum ADDRESS_MODE
{
    ADDRESS_MODE_WRAP,
    ADDRESS_MODE_CLAMP,
    ADDRESS_MODE_MIRROR,
    ADDRESS_MODE_BORDER,
};

///////////////////////////////////////////////////////////////////////////////
// BORDER_COLOR enum
///////////////////////////////////////////////////////////////////////////////
enum BORDER_COLOR
{
    BORDER_COLOR_TRANSPARENT_BLACK  = 0,    // (0, 0, 0, 0).
    BORDER_COLOR_OPAQUE_BLACK       = 1,    // (0, 0, 0, 1).
    BORDER_COLOR_OPAQUE_WHITE       = 2,    // (1, 1, 1, 1).
};

///////////////////////////////////////////////////////////////////////////////
// CULL_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum CULL_TYPE
{
    CULL_TYPE_NONE,
    CULL_TYPE_FRONT,
    CULL_TYPE_BACK,
};

///////////////////////////////////////////////////////////////////////////////
// COMPARE_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum COMPARE_TYPE
{
    COMPARE_TYPE_NEVER,
    COMPARE_TYPE_LESS,
    COMPARE_TYPE_EQUAL,
    COMPARE_TYPE_LEQUAL,
    COMPARE_TYPE_GREATER,
    COMPARE_TYPE_NEQUAL,
    COMPARE_TYPE_GEQUAL,
    COMPARE_TYPE_ALWAYS,
};

///////////////////////////////////////////////////////////////////////////////
// STENCIL_OP_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum STENCIL_OP_TYPE
{
    STENCIL_OP_KEEP,
    STENCIL_OP_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR,
};

///////////////////////////////////////////////////////////////////////////////
// DEPTH_WRITE_MASK enum
///////////////////////////////////////////////////////////////////////////////
enum DEPTH_WRITE_MASK
{
    DEPTH_WRITE_MASK_ZERO,
    DEPTH_WRITE_MASK_ALL,
};

///////////////////////////////////////////////////////////////////////////////
// BLEND_OP_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum BLEND_OP_TYPE
{
    BLEND_OP_TYPE_ADD,
    BLEND_OP_TYPE_SUB,
    BLEND_OP_TYPE_REV_SUB,
    BLEND_OP_TYPE_MIN,
    BLEND_OP_TYPE_MAX,
};

///////////////////////////////////////////////////////////////////////////////
// MEMBER_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum MEMBER_TYPE
{
    MEMBER_TYPE_UNKNOWN,
    MEMBER_TYPE_BOOL,
    MEMBER_TYPE_BOOL1x2,
    MEMBER_TYPE_BOOL1x3,
    MEMBER_TYPE_BOOL1x4,
    MEMBER_TYPE_BOOL2,
    MEMBER_TYPE_BOOL2x1,
    MEMBER_TYPE_BOOL2x2,
    MEMBER_TYPE_BOOL2x3,
    MEMBER_TYPE_BOOL2x4,
    MEMBER_TYPE_BOOL3,
    MEMBER_TYPE_BOOL3x1,
    MEMBER_TYPE_BOOL3x2,
    MEMBER_TYPE_BOOL3x3,
    MEMBER_TYPE_BOOL3x4,
    MEMBER_TYPE_BOOL4,
    MEMBER_TYPE_BOOL4x1,
    MEMBER_TYPE_BOOL4x2,
    MEMBER_TYPE_BOOL4x3,
    MEMBER_TYPE_BOOL4x4,
    MEMBER_TYPE_INT,
    MEMBER_TYPE_INT1x2,
    MEMBER_TYPE_INT1x3,
    MEMBER_TYPE_INT1x4,
    MEMBER_TYPE_INT2,
    MEMBER_TYPE_INT2x1,
    MEMBER_TYPE_INT2x2,
    MEMBER_TYPE_INT2x3,
    MEMBER_TYPE_INT2x4,
    MEMBER_TYPE_INT3,
    MEMBER_TYPE_INT3x1,
    MEMBER_TYPE_INT3x2,
    MEMBER_TYPE_INT3x3,
    MEMBER_TYPE_INT3x4,
    MEMBER_TYPE_INT4,
    MEMBER_TYPE_INT4x1,
    MEMBER_TYPE_INT4x2,
    MEMBER_TYPE_INT4x3,
    MEMBER_TYPE_INT4x4,
    MEMBER_TYPE_UINT,
    MEMBER_TYPE_UINT1x2,
    MEMBER_TYPE_UINT1x3,
    MEMBER_TYPE_UINT1x4,
    MEMBER_TYPE_UINT2,
    MEMBER_TYPE_UINT2x1,
    MEMBER_TYPE_UINT2x2,
    MEMBER_TYPE_UINT2x3,
    MEMBER_TYPE_UINT2x4,
    MEMBER_TYPE_UINT3,
    MEMBER_TYPE_UINT3x1,
    MEMBER_TYPE_UINT3x2,
    MEMBER_TYPE_UINT3x3,
    MEMBER_TYPE_UINT3x4,
    MEMBER_TYPE_UINT4,
    MEMBER_TYPE_UINT4x1,
    MEMBER_TYPE_UINT4x2,
    MEMBER_TYPE_UINT4x3,
    MEMBER_TYPE_UINT4x4,
    MEMBER_TYPE_DOUBLE,
    MEMBER_TYPE_DOUBLE1x2,
    MEMBER_TYPE_DOUBLE1x3,
    MEMBER_TYPE_DOUBLE1x4,
    MEMBER_TYPE_DOUBLE2,
    MEMBER_TYPE_DOUBLE2x1,
    MEMBER_TYPE_DOUBLE2x2,
    MEMBER_TYPE_DOUBLE2x3,
    MEMBER_TYPE_DOUBLE2x4,
    MEMBER_TYPE_DOUBLE3,
    MEMBER_TYPE_DOUBLE3x1,
    MEMBER_TYPE_DOUBLE3x2,
    MEMBER_TYPE_DOUBLE3x3,
    MEMBER_TYPE_DOUBLE3x4,
    MEMBER_TYPE_DOUBLE4,
    MEMBER_TYPE_DOUBLE4x1,
    MEMBER_TYPE_DOUBLE4x2,
    MEMBER_TYPE_DOUBLE4x3,
    MEMBER_TYPE_DOUBLE4x4,
    MEMBER_TYPE_FLOAT,
    MEMBER_TYPE_FLOAT1x2,
    MEMBER_TYPE_FLOAT1x3,
    MEMBER_TYPE_FLOAT1x4,
    MEMBER_TYPE_FLOAT2,
    MEMBER_TYPE_FLOAT2x1,
    MEMBER_TYPE_FLOAT2x2,
    MEMBER_TYPE_FLOAT2x3,
    MEMBER_TYPE_FLOAT2x4,
    MEMBER_TYPE_FLOAT3,
    MEMBER_TYPE_FLOAT3x1,
    MEMBER_TYPE_FLOAT3x2,
    MEMBER_TYPE_FLOAT3x3,
    MEMBER_TYPE_FLOAT3x4,
    MEMBER_TYPE_FLOAT4,
    MEMBER_TYPE_FLOAT4x1,
    MEMBER_TYPE_FLOAT4x2,
    MEMBER_TYPE_FLOAT4x3,
    MEMBER_TYPE_FLOAT4x4,
    MEMBER_TYPE_STRUCT,
};

///////////////////////////////////////////////////////////////////////////////
// PROPERTY_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum PROPERTY_TYPE
{
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_INT,
    PROPERTY_TYPE_FLOAT,
    PROPERTY_TYPE_FLOAT2,
    PROPERTY_TYPE_FLOAT3,
    PROPERTY_TYPE_FLOAT4,
    PROPERTY_TYPE_COLOR3,
    PROPERTY_TYPE_COLOR4,
    PROPERTY_TYPE_TEXTURE1D,
    PROPERTY_TYPE_TEXTURE1D_ARRAY,
    PROPERTY_TYPE_TEXTURE2D,
    PROPERTY_TYPE_TEXTURE2D_ARRAY,
    PROPERTY_TYPE_TEXTURE3D,
    PROPERTY_TYPE_TEXTURECUBE,
    PROPERTY_TYPE_TEXTURECUBE_ARRAY
};

///////////////////////////////////////////////////////////////////////////////
// TYPE_MODIFIER enum
///////////////////////////////////////////////////////////////////////////////
enum TYPE_MODIFIER
{
    TYPE_MODIFIER_NONE          = 0,
    TYPE_MODIFIER_CONST         = 0x1 << 0,
    TYPE_MODIFIER_ROW_MAJOR     = 0x1 << 1,
    TYPE_MODIFIER_COLUMN_MAJOR  = 0x1 << 2,
};

///////////////////////////////////////////////////////////////////////////////
// RESOURCE_TYPE
///////////////////////////////////////////////////////////////////////////////
enum RESOURCE_TYPE
{
    RESOURCE_TYPE_TEXTURE1D,
    RESOURCE_TYPE_TEXTURE1DARRAY,
    RESOURCE_TYPE_TEXTURE2D,
    RESOURCE_TYPE_TEXTURE2DARRAY,
    RESOURCE_TYPE_TEXTURE2DMS,
    RESOURCE_TYPE_TEXTURE2DMSARRAY,
    RESOURCE_TYPE_TEXTURE3D,
    RESOURCE_TYPE_TEXTURECUBE,
    RESOURCE_TYPE_TEXTURECUBEARRAY,
    RESOURCE_TYPE_BUFFER,
    RESOURCE_TYPE_STRUCTURED_BUFFER,
    RESOURCE_TYPE_BYTEADDRESS_BUFFER,
    RESOURCE_TYPE_RWTEXTURE1D,
    RESOURCE_TYPE_RWTEXTURE1DARRAY,
    RESOURCE_TYPE_RWTEXTURE2D,
    RESOURCE_TYPE_RWTEXTURE2DARRAY,
    RESOURCE_TYPE_RWTEXTURE3D,
    RESOURCE_TYPE_RWBUFFER,
    RESOURCE_TYPE_RWSTRUCTURED_BUFFER,
    RESOURCE_TYPE_RWBYTEADDRESS_BUFFER,
    RESOURCE_TYPE_SAMPLER_STATE,
    RESOURCE_TYPE_SAMPLER_COMPRISON_STATE,
};

///////////////////////////////////////////////////////////////////////////////
// Shader 
///////////////////////////////////////////////////////////////////////////////
struct Shader
{
    SHADER_TYPE                 Type;           //!< シェーダタイプです.
    std::string                 EntryPoint;     //!< エントリーポイント名です.
    std::string                 Profile;        //!< シェーダプロファイルです.
    std::vector<std::string>    Arguments;      //!< 引数です.
};

///////////////////////////////////////////////////////////////////////////////
// RasterizerState
///////////////////////////////////////////////////////////////////////////////
struct RasterizerState
{
    POLYGON_MODE  PolygonMode                   = POLYGON_MODE_SOLID;   //!< ポリゴンモード.
    CULL_TYPE     CullMode                      = CULL_TYPE_NONE;       //!< カリングタイプ.
    bool          FrontCCW                      = true;                 //!< 反時計回りを前面にするかどうか.
    uint32_t      DepthBias                     = 0;                    //!< 深度バイアス.
    float         DepthBiasClamp                = 0.0f;                 //!< 深度バイアスのクランプ値.
    float         SlopeScaledDepthBias          = 0.0f;                 //!< 傾斜スケール深度バイアス.
    bool          DepthClipEnable               = false;                //!< 深度クリップを有効化するかどうか.
    bool          EnableConservativeRaster      = false;                //!< コンサバティブラスタライゼーションを有効化するかどうか.
};

///////////////////////////////////////////////////////////////////////////////
// DepthStencilState
///////////////////////////////////////////////////////////////////////////////
struct DepthStencilState
{
    bool                DepthEnable                   = true;                   //!< 深度テストを有効化するかどうか.
    DEPTH_WRITE_MASK    DepthWriteMask                = DEPTH_WRITE_MASK_ALL;   //!< 深度書き込みマスク.
    COMPARE_TYPE        DepthFunc                     = COMPARE_TYPE_LESS;      //!< 深度比較関数.
    bool                StencilEnable                 = false;                  //!< ステンシルテストを有効化するかどうか.
    uint8_t             StencilReadMask               = 0xff;                   //!< ステンシル読み取りマスク.
    uint8_t             StencilWriteMask              = 0xff;                   //!< ステンシル書き込みマスク.
    STENCIL_OP_TYPE     FrontFaceStencilFail          = STENCIL_OP_KEEP;
    STENCIL_OP_TYPE     FrontFaceStencilDepthFail     = STENCIL_OP_KEEP;
    STENCIL_OP_TYPE     FrontFaceStencilPass          = STENCIL_OP_KEEP;
    COMPARE_TYPE        FrontFaceStencilFunc          = COMPARE_TYPE_ALWAYS;
    STENCIL_OP_TYPE     BackFaceStencilFail           = STENCIL_OP_KEEP;
    STENCIL_OP_TYPE     BackFaceStencilDepthFail      = STENCIL_OP_KEEP;
    STENCIL_OP_TYPE     BackFaceStencilPass           = STENCIL_OP_KEEP;
    COMPARE_TYPE        BackFaceStencilFunc           = COMPARE_TYPE_ALWAYS;
};

///////////////////////////////////////////////////////////////////////////////
// BlendState
///////////////////////////////////////////////////////////////////////////////
struct BlendState
{
    bool            AlphaToCoverageEnable   = false;
    bool            BlendEnable             = false;
    BLEND_TYPE      SrcBlend                = BLEND_TYPE_ONE;
    BLEND_TYPE      DstBlend                = BLEND_TYPE_ZERO;
    BLEND_OP_TYPE   BlendOp                 = BLEND_OP_TYPE_ADD;
    BLEND_TYPE      SrcBlendAlpha           = BLEND_TYPE_ONE;
    BLEND_TYPE      DstBlendAlpha           = BLEND_TYPE_ZERO;
    BLEND_OP_TYPE   BlendOpAlpha            = BLEND_OP_TYPE_ADD;
    uint8_t         RenderTargetWriteMask   = 0xff;
};

///////////////////////////////////////////////////////////////////////////////
// Member
///////////////////////////////////////////////////////////////////////////////
struct Member
{
    std::string     Name;                   //!< メンバー名です.
    MEMBER_TYPE     Type;                   //!< データ型です.
    TYPE_MODIFIER   Modifier;               //!< 修飾子です.
    uint32_t        PackOffset;             //!< パックオフセットです.
};

///////////////////////////////////////////////////////////////////////////////
// ConstantBuffer
///////////////////////////////////////////////////////////////////////////////
struct ConstantBuffer
{
    std::string             Name;           //!< 定数バッファ名です.
    uint32_t                Register;       //!< レジスタ番号です.
    std::vector<Member>     Members;        //!< メンバーです.
};

///////////////////////////////////////////////////////////////////////////////
// Structure
///////////////////////////////////////////////////////////////////////////////
struct Structure
{
    std::string             Name;           //!< 構造体名です.
    std::vector<Member>     Members;        //!< メンバー変数です.
};

///////////////////////////////////////////////////////////////////////////////
// ValueProperty 
///////////////////////////////////////////////////////////////////////////////
struct ValueProperty
{
    std::string         Name;               //!< 変数名です.
    std::string         DisplayTag;         //!< UI表示名です.
    PROPERTY_TYPE       Type;               //!< データ型です.
    uint32_t            Offset;             //!< 先頭からのオフセットです(バイト単位).
    float               Min;                //!< 最小値です.
    float               Max;                //!< 最大値です.
    float               Step;               //!< 値を増やす量です.
    std::string         DefaultValue0;      //!< 要素0のデフォルト値です.
    std::string         DefaultValue1;      //!< 要素1のデフォルト値です.
    std::string         DefaultValue2;      //!< 要素2のデフォルト値です.
    std::string         DefaultValue3;      //!< 要素3のデフォルト値です.
};

///////////////////////////////////////////////////////////////////////////////
// TextureProperty
///////////////////////////////////////////////////////////////////////////////
struct TextureProperty
{
    std::string         Name;               //!< 変数名です.
    std::string         DisplayTag;         //!< UI表示名です.
    PROPERTY_TYPE       Type;               //!< データ型です.
    bool                EnableSRGB;         //!< sRGBを有効にする場合は true を指定.
    std::string         DefaultValue;       //!< デフォルト値(文字列の解釈は使用者に委ねられます).
};

///////////////////////////////////////////////////////////////////////////////
// Properties
///////////////////////////////////////////////////////////////////////////////
struct Properties
{
    uint32_t                        BufferSize; //!< バッファサイズです.
    std::vector<ValueProperty>      Values;     //!< 値です.
    std::vector<TextureProperty>    Textures;   //!< テクスチャです.
};

///////////////////////////////////////////////////////////////////////////////
// Resource
///////////////////////////////////////////////////////////////////////////////
struct Resource
{
    std::string         Name;               //!< リソース名です.   
    RESOURCE_TYPE       ResourceType;       //!< リソースタイプです. 
    MEMBER_TYPE         DataType;           //!< データ型です.
    uint32_t            Register;           //!< レジスタ番号です.
};

///////////////////////////////////////////////////////////////////////////////
// Pass
///////////////////////////////////////////////////////////////////////////////
struct Pass
{
    std::string                 Name;               //!< パス名です.
    std::vector<Shader>         Shaders;            //!< シェーダデータです.
    std::string                 RasterizerState;    //!< ラスタライザーステートです.
    std::string                 DepthStencilState;  //!< 深度ステンシルステートです.
    std::string                 BlendState;         //!< ブレンドステートです.
};

///////////////////////////////////////////////////////////////////////////////
// Technique
///////////////////////////////////////////////////////////////////////////////
struct Technique
{
    std::string         Name;   //!< テクニック名です.
    std::vector<Pass>   Pass;   //!< パスデータです.
};

// 文字列に変換.
const char* ToString(SHADER_TYPE value);
const char* ToString(POLYGON_MODE mode);
const char* ToString(CULL_TYPE type);
const char* ToString(BLEND_TYPE type);
const char* ToString(FILTER_MODE type);
const char* ToString(MIPMAP_MODE type);
const char* ToString(ADDRESS_MODE type);
const char* ToString(BORDER_COLOR type);
const char* ToString(COMPARE_TYPE type);
const char* ToString(STENCIL_OP_TYPE type);
const char* ToString(DEPTH_WRITE_MASK type);
const char* ToString(BLEND_OP_TYPE type);

// 文字列から変換.
POLYGON_MODE        ParsePolygonMode    (const char* value);
BLEND_TYPE          ParseBlendType      (const char* value);
FILTER_MODE         ParseFilterMode     (const char* value);
MIPMAP_MODE         ParseMipmapMode     (const char* value);
ADDRESS_MODE        ParseAddressMode    (const char* value);
BORDER_COLOR        ParseBorderColor    (const char* value);
CULL_TYPE           ParseCullType       (const char* value);
COMPARE_TYPE        ParseCompareType    (const char* value);
STENCIL_OP_TYPE     ParseStencilOpType  (const char* value);
DEPTH_WRITE_MASK    ParseDepthWriteMask (const char* value);
BLEND_OP_TYPE       ParseBlendOpType    (const char* value);

///////////////////////////////////////////////////////////////////////////////
// FxParser class
///////////////////////////////////////////////////////////////////////////////
class FxParser
{
    //========================================================================
    // list of friend classes and methods.
    //========================================================================
    /* NOTHING */

public:
    //========================================================================
    // public variables.
    //========================================================================
    /* NOTHING */

    //========================================================================
    // public methods.
    //========================================================================

    //------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //------------------------------------------------------------------------
    FxParser ();

    //------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //------------------------------------------------------------------------
    ~FxParser();

    //------------------------------------------------------------------------
    //! @brief      解析結果をクリアします.
    //------------------------------------------------------------------------
    void Clear();

    //------------------------------------------------------------------------
    //! @brief      解析処理を行ないます.
    //! 
    //! @param[in]      filename        ファイル名
    //! @retval true    解析に成功.
    //! @retval false   解析に失敗.
    //------------------------------------------------------------------------
    bool Parse(const char* filename);

    //------------------------------------------------------------------------
    //! @brief      ソースコードを取得します.
    //! 
    //! @return     ソースコードを返却します.
    //------------------------------------------------------------------------
    const char* GetSourceCode() const;

    //------------------------------------------------------------------------
    //! @brief      ソースコードサイズを取得します.
    //! 
    //! @return     ソースコードサイズを返却します.
    //------------------------------------------------------------------------
    size_t GetSourceCodeSize() const;

    //------------------------------------------------------------------------
    //! @brief      ブレンドステートを取得します.
    //! 
    //! @return     ブレンドステートを返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, BlendState>& GetBlendStates() const;

    //------------------------------------------------------------------------
    //! @brief      ラスタライザーステートを取得します.
    //! 
    //! @return     ラスタライザーステートを返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, RasterizerState>& GetRasterizerStates() const;

    //------------------------------------------------------------------------
    //! @brief      深度ステンシルステートを取得します.
    //! 
    //! @return     深度ステンシルステートを返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, DepthStencilState>& GetDepthStencilStates() const;

    //------------------------------------------------------------------------
    //! @brief      定数バッファを取得します.
    //! 
    //! @return     定数バッファを返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, ConstantBuffer>& GetConstantBuffers() const;

    //------------------------------------------------------------------------
    //! @brief      構造体を取得します.
    //! 
    //! @return     構造体を返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, Structure>& GetStructures() const;

    //------------------------------------------------------------------------
    //! @brief      リソースを取得します.
    //! 
    //! @return     リソースを返却します.
    //------------------------------------------------------------------------
    const std::map<std::string, Resource>& GetResources() const;

    //------------------------------------------------------------------------
    //! @brief      テクニックを取得します.
    //! 
    //! @return     テクニックを返却します.
    //------------------------------------------------------------------------
    const std::vector<Technique>& GetTechniques() const;

    //------------------------------------------------------------------------
    //! @brief      プロパティを取得します.
    //! 
    //! @return     プロパティを返却します.
    //------------------------------------------------------------------------
    const Properties& GetProperties() const;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Info structure
    ///////////////////////////////////////////////////////////////////////////
    struct Info
    {
        std::string     IncludeFile;    //!< インクルード文
        std::string     FindPath;       //!< 解決済みファイルパス.
        std::string     Code;           //!< 該当ファイル.
    };

    //========================================================================
    // private variables.
    //========================================================================
    Tokenizer                                   m_Tokenizer;
    std::vector<Technique>                      m_Technieues;
    std::map<std::string, Shader>               m_Shaders;
    std::map<std::string, std::string>          m_Defines;
    std::map<std::string, BlendState>           m_BlendStates;
    std::map<std::string, RasterizerState>      m_RasterizerStates;
    std::map<std::string, DepthStencilState>    m_DepthStencilStates;
    std::map<std::string, ConstantBuffer>       m_ConstantBuffers;
    std::map<std::string, Structure>            m_Structures;
    std::map<std::string, Resource>             m_Resources;
    Properties                                  m_Properties;
    std::string                                 m_SourceCode;
    int                                         m_ShaderCounter;
    std::vector<std::string>                    m_DirPaths;
    std::vector<Info>                           m_Includes;
    std::string                                 m_Expanded;

    //========================================================================
    // private methods.
    //========================================================================
    bool Load(const char* filename);
    void ParseShader();
    void ParsePass(Technique& technique);
    void ParseTechnique();
    void ParseBlendState();
    void ParseRasterizerState();
    void ParseDepthStencilState();
    void ParsePreprocessor();
    void ParseConstantBuffer();
    void ParseConstantBufferMember(MEMBER_TYPE type, ConstantBuffer& buffer, TYPE_MODIFIER& modifier);
    void ParseStruct();
    void ParseProperties();
    void ParseStructMember(MEMBER_TYPE type, Structure& structure, TYPE_MODIFIER& modifier);
    void ParseResource();
    void ParseResourceDetail(RESOURCE_TYPE type);
    void ParseTextureProperty(PROPERTY_TYPE type);
    SHADER_TYPE GetShaderType();

    bool CorrectIncludes(const char* filename);
    bool Expand(std::string& inout);
};

} // namespace asura
//-----------------------------------------------------------------------------
// File : EditorMaterial.h
// Desc : Material For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <asdxEditParam.h>
#include <asdxConstantBuffer.h>
#include <EditorShader.h>


class EditorMaterial
{
public:
    uint32_t            ShaderHash;
    asdx::EditBool      Visble;
    asdx::EditBool      CastShadow;
    asdx::EditBool      ReceiveShadow;
    asdx::EditBool      Dither;
    asdx::EditInt       DisplayFace;
    asdx::EditInt       State;
    asdx::EditFloat2    UVScale0;
    asdx::EditFloat2    UVScale1;
    asdx::EditFloat2    UVScale2;
    asdx::EditFloat2    UVScale3;
    asdx::EditFloat2    UVRotate0;
    asdx::EditFloat2    UVRotate1;
    asdx::EditFloat2    UVRotate2;
    asdx::EditFloat2    UVRotate3;
    asdx::EditFloat2    UVScroll0;
    asdx::EditFloat2    UVScroll1;
    asdx::EditFloat2    UVScroll2;
    asdx::EditFloat2    UVScroll3;
    asdx::EditFloat     AlphaThreshold0;
    asdx::EditFloat     AlphaThreshold1;
    asdx::EditFloat     AlphaThreshold2;
    asdx::EditFloat     AlphaThreshold3;
    asdx::EditColor4    Color0;
    asdx::EditColor4    Color1;
    asdx::EditColor4    Color2;
    asdx::EditColor4    Color3;
    asdx::EditFloat4    ColorScale0;
    asdx::EditFloat4    ColorScale1;
    asdx::EditFloat4    ColorScale2;
    asdx::EditFloat4    ColorScale3;
    asdx::EditBool      CustomBool_0;
    asdx::EditBool      CustomBool_1;
    asdx::EditBool      CustomBool_2;
    asdx::EditBool      CustomBool_3;
    asdx::EditBool      CustomBool_4;
    asdx::EditBool      CustomBool_5;
    asdx::EditBool      CustomBool_6;
    asdx::EditBool      CustomBool_7;
    asdx::EditFloat     CustomFloat_0;
    asdx::EditFloat     CustomFloat_1;
    asdx::EditFloat     CustomFloat_2;
    asdx::EditFloat     CustomFloat_3;
    asdx::EditFloat     CustomFloat_4;
    asdx::EditFloat     CustomFloat_5;
    asdx::EditFloat     CustomFloat_6;
    asdx::EditFloat     CustomFloat_7;
    asdx::EditFloat2    CustomFloat2_0;
    asdx::EditFloat2    CustomFloat2_1;
    asdx::EditFloat2    CustomFloat2_2;
    asdx::EditFloat2    CustomFloat2_3;
    asdx::EditFloat2    CustomFloat2_4;
    asdx::EditFloat2    CustomFloat2_5;
    asdx::EditFloat2    CustomFloat2_6;
    asdx::EditFloat2    CustomFloat2_7;
    asdx::EditFloat3    CustomFloat3_0;
    asdx::EditFloat3    CustomFloat3_1;
    asdx::EditFloat3    CustomFloat3_2;
    asdx::EditFloat3    CustomFloat3_3;
    asdx::EditFloat3    CustomFloat3_4;
    asdx::EditFloat3    CustomFloat3_5;
    asdx::EditFloat3    CustomFloat3_6;
    asdx::EditFloat3    CustomFloat3_7;
    asdx::EditFloat4    CustomFloat4_0;
    asdx::EditFloat4    CustomFloat4_1;
    asdx::EditFloat4    CustomFloat4_2;
    asdx::EditFloat4    CustomFloat4_3;
    asdx::EditFloat4    CustomFloat4_4;
    asdx::EditFloat4    CustomFloat4_5;
    asdx::EditFloat4    CustomFloat4_6;
    asdx::EditFloat4    CustomFloat4_7;

    asdx::EditTexture   BaseColorMap0;
    asdx::EditTexture   BaseColorMap1;
    asdx::EditTexture   BaseColorMap2;
    asdx::EditTexture   BaseColorMap3;
    asdx::EditTexture   ORMMap0;
    asdx::EditTexture   ORMMap1;
    asdx::EditTexture   ORMMap2;
    asdx::EditTexture   ORMMap3;
    asdx::EditTexture   NormalMap0;
    asdx::EditTexture   NormalMap1;
    asdx::EditTexture   NormalMap2;
    asdx::EditTexture   NormalMap3;
    asdx::EditTexture   OpacityMap0;
    asdx::EditTexture   OpacityMap1;
    asdx::EditTexture   OpacityMap2;
    asdx::EditTexture   OpacityMap3;
    asdx::EditTexture   MaskMap0;
    asdx::EditTexture   MaskMap1;
    asdx::EditTexture   MaskMap2;
    asdx::EditTexture   MaskMap3;
    asdx::EditTexture   EmissiveMap0;
    asdx::EditTexture   EmissiveMap1;
    asdx::EditTexture   EmissiveMap2;
    asdx::EditTexture   EmissiveMap3;
    asdx::EditTexture   CurvatureMap;
    asdx::EditTexture   DistortionMap;
    asdx::EditTexture   FlowMap;
    asdx::EditTexture   CustomMap0;
    asdx::EditTexture   CustomMap1;
    asdx::EditTexture   CustomMap2;
    asdx::EditTexture   CustomMap3;
    asdx::EditTexture   CustomMap4;
    asdx::EditTexture   CustomMap5;
    asdx::EditTexture   CustomMap6;
    asdx::EditTexture   CustomMap7;

    EditorMaterial();
    ~EditorMaterial();

    bool Init(const char* path);
    void Term();
    void Update();
    void Bind(ID3D12GraphicsCommandList* pCmd);

    uint32_t GetHash() const;

private:
    std::string             m_Name;
    uint32_t                m_Hash;
    asdx::ConstantBuffer    m_CB;
    asdx::ConstantBuffer    m_UserCB;
};


void DrawParam(EditorMaterial& material, const EditorShader& shader, const char* defaultPath);
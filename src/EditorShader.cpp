//-----------------------------------------------------------------------------
// File : EditorShader.cpp
// Desc : Shader For Editor.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#include <EditorShader.h>
#include <dxcapi.h>


EditorShader::EditorShader()
: m_Hash(0)
{
}

EditorShader::~EditorShader()
{

}

bool EditorShader::Init(const char* path)
{
    return true;
}


const ShaderSetting& EditorShader::GetSetting() const
{ return m_Setting; }

D3D12_SHADER_BYTECODE EditorShader::GetShader() const
{
    D3D12_SHADER_BYTECODE result;
    result.BytecodeLength = m_Blob->GetBufferSize();
    result.pShaderBytecode = m_Blob->GetBufferPointer();
    return result;
}

uint32_t EditorShader::GetHash() const
{ return m_Hash; }

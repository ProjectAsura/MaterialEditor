//-----------------------------------------------------------------------------
// File : App.cpp
// Desc : Material Editor Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>


App::App()
: asdx::Application(L"MaterailEditor", 1920, 1080, nullptr, nullptr, nullptr)
{
}

App::~App()
{
}

bool App::OnInit()
{

    return true;
}

void App::OnTerm()
{
}

void App::OnFrameRender(asdx::FrameEventArgs& args)
{
}

void App::OnResize(const asdx::ResizeEventArgs& args)
{
}

void App::OnKey(const asdx::KeyEventArgs& args)
{
}

void App::OnMouse(const asdx::MouseEventArgs& args)
{
}

void App::OnTyping(uint32_t keyCode)
{
}

void App::OnDrop(const wchar_t** dropFiles, uint32_t fileCount)
{
}

bool App::ExportMesh(const char* path)
{
    return false;
}

bool App::ExportMaterial(const char* path)
{
    return false;
}
#include "Game.h"
#if !D3D_VERSION
#include "CNCDDraw.h"
#include "Texture.h"

HINSTANCE hGetProcIDDLL;

typedef D3DPRESENT_PARAMETERS*(__stdcall *getD3dp)();
typedef LPDIRECT3D9*(__stdcall *getD3d)();
typedef LPDIRECT3DDEVICE9*(__stdcall *getD3dDev)();
typedef IDirectDrawImpl**(__stdcall *ddraw_ptr)();
typedef RECT*(__stdcall *getWindowRect)();
typedef void(__stdcall *setPoptbCallback)(poptb_callback ptr);
typedef ccdraw_renderer(__stdcall *getRenderer)();
typedef poptb_callback(__stdcall *fullscreen)();

LPDIRECT3DDEVICE9*          poptb_d3d_device;
IDirectDrawImpl**           poptb_ddraw_ptr;
RECT*                       poptb_window_rect;
ccdraw_renderer             poptb_directx_renderer;
ccdraw_renderer             poptb_opengl_renderer;
poptb_callback              poptb_getFullscreen;

// Entry point for PopTB Draw Thread
void _stdcall draw_callback()
{
    bgui::directx_render();
}

void _stdcall directx_init()
{
    bgui::init();
}

void _stdcall directx_deinit()
{
    TextureManager->DeleteAll();
    bgui::deinit();
}

void setup_cnc_ddraw()
{
    hGetProcIDDLL = LoadLibrary(L"ddraw.dll");

    if (!hGetProcIDDLL) 
        Pop3Debug::fatalError_NoReport("Could not load DirectX Library");

    auto getD3dDev_func = (getD3dDev)GetProcAddress(hGetProcIDDLL, "getD3dDev");
    if (!getD3dDev_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find getD3dDev");
    } else   poptb_d3d_device = getD3dDev_func();

    auto getDDraw_func = (ddraw_ptr)GetProcAddress(hGetProcIDDLL, "getDDraw");
    if (!getDDraw_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find getDDraw");
    } else  poptb_ddraw_ptr = getDDraw_func();

    auto getWindowRect_func = (getWindowRect)GetProcAddress(hGetProcIDDLL, "getWindowRect");
    if (!getWindowRect_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find getWindowRect_func");
    }
    else poptb_window_rect = getWindowRect_func();

    auto poptb_getOpenGLRenderer_func = (getRenderer)GetProcAddress(hGetProcIDDLL, "poptb_getOpenGLRenderer");
    if (!poptb_getOpenGLRenderer_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find poptb_getOpenGLRenderer");
    }
    else poptb_opengl_renderer = poptb_getOpenGLRenderer_func();

    auto poptb_getDirectXRenderer_func = (getRenderer)GetProcAddress(hGetProcIDDLL, "poptb_getDirectXRenderer");
    if (!poptb_getDirectXRenderer_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find poptb_getDirectXRenderer");
    }
    else poptb_directx_renderer = poptb_getDirectXRenderer_func();

    auto poptb_getFullscreen_func = (fullscreen)GetProcAddress(hGetProcIDDLL, "poptb_getFullscreen");
    if (!poptb_getFullscreen_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find poptb_getFullscreen_func");
    }
    else poptb_getFullscreen = poptb_getFullscreen_func();
}

void init_callbacks()
{
    auto setPoptbCallback_func = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "setPoptbCallback");
    if (!setPoptbCallback_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find setPoptbCallback");
    } else setPoptbCallback_func(&draw_callback);

    auto setPoptbDx9Init_func = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "setPoptbDx9Init");
    if (!setPoptbDx9Init_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find setPoptbDx9Init");
    }
    else setPoptbDx9Init_func(&directx_init);

    auto setPoptbDx9Deinit_func = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "setPoptbDx9Deinit");
    if (!setPoptbDx9Deinit_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find setPoptbDx9Deinit");
    }
    else setPoptbDx9Deinit_func(&directx_deinit);
}
#endif
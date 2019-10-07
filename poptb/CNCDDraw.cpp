#include "Game.h"
#if !D3D_VERSION
#include "CNCDDraw.h"

HINSTANCE hGetProcIDDLL;

typedef D3DPRESENT_PARAMETERS*(__stdcall *getD3dp)();
typedef LPDIRECT3D9*(__stdcall *getD3d)();
typedef LPDIRECT3DDEVICE9*(__stdcall *getD3dDev)();
typedef IDirectDrawImpl**(__stdcall *ddraw_ptr)();
typedef RECT*(__stdcall *getWindowRect)();
typedef void(__stdcall *setPoptbCallback)(poptb_callback ptr);
typedef ccdraw_renderer(__stdcall *getRenderer)();

D3DPRESENT_PARAMETERS*      poptb_d3d_params;
LPDIRECT3D9*                poptb_d3d;
LPDIRECT3DDEVICE9*          poptb_d3d_device;
IDirectDrawImpl**           poptb_ddraw_ptr;
RECT*                       poptb_window_rect;
poptb_callback              poptb_device_lost;
ccdraw_renderer             poptb_directx_renderer;
ccdraw_renderer             poptb_opengl_renderer;

// Entry point for PopTB Draw Thread
void _stdcall draw_callback()
{
    bgui::directx_render();
}

void _stdcall device_lost()
{
    bgui::reset_render_engine();
    Pop3Debug::trace("Device lost!");
}

void setup_cnc_ddraw()
{
    hGetProcIDDLL = LoadLibrary(L"ddraw.dll");

    if (!hGetProcIDDLL) 
        Pop3Debug::fatalError_NoReport("Could not load DirectX Library");

    auto getD3dp_func = (getD3dp)GetProcAddress(hGetProcIDDLL, "getD3dp");
    if (!getD3dp_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find getD3dp");
    } else poptb_d3d_params = getD3dp_func();

    auto getD3d_func = (getD3d)GetProcAddress(hGetProcIDDLL, "getD3d");
    if (!getD3d_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find getD3d");
    } else poptb_d3d = getD3d_func();

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
}

void init_callbacks()
{
    auto setPoptbDeviceLost_func = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "setPoptbDeviceLost");
    if (!setPoptbDeviceLost_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find setPoptbDeviceLost_func");
    }
    else setPoptbDeviceLost_func(&device_lost);

    auto setPoptbCallback_func = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "setPoptbCallback");
    if (!setPoptbCallback_func)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find setPoptbCallback");
    } else setPoptbCallback_func(&draw_callback);

    auto poptb_device_lost = (setPoptbCallback)GetProcAddress(hGetProcIDDLL, "poptb_DeviceLost");
    if (!poptb_device_lost)
    {
        Pop3Debug::fatalError_NoReport("CNC-Draw -- Could not find poptb_DeviceLost");
    }
}
#endif
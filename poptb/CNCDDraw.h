#pragma once
#if !D3D_VERSION
#include <d3d9.h>
#include <windows.h>

struct IDirectDrawImpl;

struct IDirectDrawImplVtbl
{
    HRESULT(__stdcall *QueryInterface) (IDirectDrawImpl *, const IID* const riid, LPVOID * ppvObj);
    ULONG(__stdcall *AddRef) (IDirectDrawImpl *);
    ULONG(__stdcall *Release) (IDirectDrawImpl *);

    HRESULT(__stdcall *Compact)(IDirectDrawImpl *);
    HRESULT(__stdcall *CreateClipper)(IDirectDrawImpl *, DWORD, LPDIRECTDRAWCLIPPER *, IUnknown *);
    HRESULT(__stdcall *CreatePalette)(IDirectDrawImpl *, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
    HRESULT(__stdcall *CreateSurface)(IDirectDrawImpl *, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE *, IUnknown *);
    HRESULT(__stdcall *DuplicateSurface)(IDirectDrawImpl *, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *);
    HRESULT(__stdcall *EnumDisplayModes)(IDirectDrawImpl *, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
    HRESULT(__stdcall *EnumSurfaces)(IDirectDrawImpl *, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
    HRESULT(__stdcall *FlipToGDISurface)(IDirectDrawImpl *);
    HRESULT(__stdcall *GetCaps)(IDirectDrawImpl *, LPDDCAPS, LPDDCAPS);
    HRESULT(__stdcall *GetDisplayMode)(IDirectDrawImpl *, LPDDSURFACEDESC);
    HRESULT(__stdcall *GetFourCCCodes)(IDirectDrawImpl *, LPDWORD, LPDWORD);
    HRESULT(__stdcall *GetGDISurface)(IDirectDrawImpl *, LPDIRECTDRAWSURFACE *);
    HRESULT(__stdcall *GetMonitorFrequency)(IDirectDrawImpl *, LPDWORD);
    HRESULT(__stdcall *GetScanLine)(IDirectDrawImpl *, LPDWORD);
    HRESULT(__stdcall *GetVerticalBlankStatus)(IDirectDrawImpl *, LPBOOL);
    HRESULT(__stdcall *Initialize)(IDirectDrawImpl *, GUID *);
    HRESULT(__stdcall *RestoreDisplayMode)(IDirectDrawImpl *);
    HRESULT(__stdcall *SetCooperativeLevel)(IDirectDrawImpl *, HWND, DWORD);
    union
    {
        HRESULT(__stdcall *SetDisplayMode1)(IDirectDrawImpl *, DWORD, DWORD, DWORD);
        HRESULT(__stdcall *SetDisplayMode2)(IDirectDrawImpl *, DWORD, DWORD, DWORD, DWORD, DWORD);
    };

    HRESULT(__stdcall *WaitForVerticalBlank)(IDirectDrawImpl *, DWORD, HANDLE);
    HRESULT(__stdcall *GetAvailableVidMem)(IDirectDrawImpl *, LPDDSCAPS, LPDWORD, LPDWORD);
};

typedef struct SpeedLimiter
{
    DWORD ticklength;
    LONGLONG tickLengthNs;
    HANDLE hTimer;
    LARGE_INTEGER dueTime;
    BOOL useBltOrFlip;
} SpeedLimiter;

typedef struct IDirectDrawImpl
{
    struct IDirectDrawImplVtbl *lpVtbl;

    ULONG Ref;

    DWORD width;
    DWORD height;
    DWORD bpp;
    BOOL windowed;
    BOOL border;
    BOOL boxing;
    DEVMODEA mode;
    struct IDirectDrawSurfaceImpl *primary;
    char title[128];
    HMODULE real_dll;

    /* real export from system32\ddraw.dll */
    HRESULT(WINAPI *DirectDrawCreate)(GUID FAR*, LPDIRECTDRAW FAR*, IUnknown FAR*);
    CRITICAL_SECTION cs;

    struct
    {
        int maxfps;
        int width;
        int height;
        int bpp;

        HDC hDC;
        int *tex;

        HANDLE thread;
        BOOL run;
        HANDLE ev;
        HANDLE sem;
        DEVMODEA mode;
        struct { int width; int height; int x; int y; } viewport;

        LONG paletteUpdated;
        LONG surfaceUpdated;

        float scaleW;
        float scaleH;
        float unScaleW;
        float unScaleH;
    } render;

    HWND hWnd;
    LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);
    struct { float x; float y; } cursor;
    BOOL locked;
    BOOL adjmouse;
    BOOL devmode;
    BOOL vsync;
    BOOL vhack;
    BOOL isredalert;
    BOOL iscnc1;
    LONG incutscene;
    DWORD(WINAPI *renderer)(void);
    BOOL fullscreen;
    BOOL maintas;
    BOOL noactivateapp;
    BOOL handlemouse;
    char shader[MAX_PATH];
    BOOL wine;
    BOOL altenter;
    BOOL hidecursor;
    BOOL accurateTimers;
    BOOL resizable;
    BOOL bnetActive;
    BOOL bnetWasFullscreen;
    BOOL bnetWasUpscaled;
    RECT bnetWinRect;
    POINT bnetPos;
    SpeedLimiter ticksLimiter;
    SpeedLimiter flipLimiter;
    SpeedLimiter fpsLimiter;

} IDirectDrawImpl;

enum class Renderers
{
    NONE,
    DIRECTX9,
    OPENGL
};

typedef void(__stdcall *poptb_callback)(void);
typedef DWORD(__stdcall *poptb_renderer)(void);
typedef DWORD(WINAPI *ccdraw_renderer)(void);

extern LPDIRECT3DDEVICE9*       poptb_d3d_device;

extern IDirectDrawImpl**        poptb_ddraw_ptr;
extern RECT*                    poptb_window_rect;
extern poptb_callback           poptb_device_lost;
extern ccdraw_renderer          poptb_directx_renderer;
extern ccdraw_renderer          poptb_opengl_renderer;

inline Renderers identify_poptb_renderer()
{
    if (poptb_ddraw_ptr && *poptb_ddraw_ptr)
    {
        if ((*poptb_ddraw_ptr)->renderer == poptb_directx_renderer)
            return Renderers::DIRECTX9;

        if ((*poptb_ddraw_ptr)->renderer == poptb_opengl_renderer)
            return Renderers::OPENGL;
    }
    return Renderers::NONE;
}

extern void setup_cnc_ddraw();
extern void init_callbacks();
#endif
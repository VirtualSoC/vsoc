#ifndef __egl_window_h_
#define __egl_window_h_
//#include <windows.h>
//#include "wglext.h"
#ifdef __APPLE__

#endif

#ifdef __linux__
typedef void (*EGLproc)(void);

typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void *EGLConfig;
typedef void *EGLContext;
typedef void *EGLDisplay;
typedef void *EGLSurface;


#define EGL_NONE 0x3038 /* Attrib list terminator */

#define EGL_CONTEXT_MAJOR_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define EGL_WIDTH 0x3057
#define EGL_HEIGHT 0x3056

#define EGL_ALPHA_SIZE 0x3021
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026

#if defined(__cplusplus)
#define EGL_CAST(type, value) (static_cast<type>(value))
#else
#define EGL_CAST(type, value) ((type)(value))
#endif

#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)

// EGL function pointer typedefs
typedef EGLBoolean (*PFN_eglGetConfigAttrib)(EGLDisplay, EGLConfig, EGLint, EGLint *);
typedef EGLBoolean (*PFN_eglGetConfigs)(EGLDisplay, EGLConfig *, EGLint, EGLint *);
typedef EGLint (*PFN_eglGetError)(void);
typedef EGLBoolean (*PFN_eglInitialize)(EGLDisplay, EGLint *, EGLint *);
typedef EGLBoolean (*PFN_eglTerminate)(EGLDisplay);
typedef EGLBoolean (*PFN_eglBindAPI)(EGLenum);

typedef EGLContext (*PFN_eglCreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);

typedef EGLBoolean (*PFN_eglChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
typedef EGLSurface (*PFN_eglCreatePbufferSurface)(EGLDisplay, EGLConfig, const EGLint *);
typedef EGLBoolean (*PFN_eglDestroySurface)(EGLDisplay, EGLSurface);
typedef EGLBoolean (*PFN_eglDestroyContext)(EGLDisplay, EGLContext);
typedef EGLBoolean (*PFN_eglMakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
typedef EGLBoolean (*PFN_eglSwapInterval)(EGLDisplay, EGLint);

#endif

#ifdef _WIN32
#include <windows.h>
#include "wglext.h"
typedef void (*WGLproc)(void);

typedef BOOL(WINAPI *PFN_wglSwapInterval)(int);
typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribiv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribfv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
typedef BOOL(WINAPI *PFN_wglChoosePixelFormat)(HDC, const int *piAttribList, const FLOAT *pfAttribList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);





typedef void (*WGLproc)(void);

typedef BOOL(WINAPI *PFN_wglSwapInterval)(int);
typedef HGLRC(WINAPI *PFN_wglCreateContextAttribs)(HDC, HGLRC, const int *);

typedef HPBUFFERARB(WINAPI *PFN_wglCreatePbuffer)(HDC hdc, int iPixelFormat, int iWidth, int iHeight, const int* piAttribList);
typedef HDC(WINAPI *PFN_wglGetPbufferDC)(HPBUFFERARB hPbuffer);
typedef int(WINAPI *PFN_wglReleasePbufferDC)(HPBUFFERARB hPbuffer, HDC hdc);
typedef BOOL(WINAPI *PFN_wglDestroyPbuffer)(HPBUFFERARB hPbuffer);

typedef HGLRC(WINAPI *PFN_wglCreateContext)(HDC);
typedef BOOL(WINAPI *PFN_wglDeleteContext)(HGLRC);
typedef PROC(WINAPI *PFN_wglGetProcAddress)(LPCSTR);
typedef HDC(WINAPI *PFN_wglGetCurrentDC)(void);
typedef HGLRC(WINAPI *PFN_wglGetCurrentContext)(void);
typedef BOOL(WINAPI *PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL(WINAPI *PFN_wglShareLists)(HGLRC, HGLRC);

typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribiv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribfv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
typedef BOOL(WINAPI *PFN_wglChoosePixelFormat)(HDC, const int *piAttribList, const FLOAT *pfAttribList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
#endif

void egl_init(void *dpy, void *father_context);

#ifdef __WIN32
void *egl_createContext(int context_flags);
#else
void *egl_createContext();
#endif

/**
 * Attempt to make current the context specified.
 * return 0 on success, or a platform-dependent code on error.
*/
int egl_makeCurrent(void *context);
void egl_destroyContext(void *context);

#endif
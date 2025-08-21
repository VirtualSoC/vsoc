#ifndef __egl_window_h_
#define __egl_window_h_

void egl_init(void *dpy, void *father_context);
void *egl_createContext(int context_flags);

/**
 * Attempt to make current the context specified.
 * return EGL_TRUE on success, or a platform-dependent code on error.
*/
int egl_makeCurrent(void *context);
void egl_destroyContext(void *context);

#endif
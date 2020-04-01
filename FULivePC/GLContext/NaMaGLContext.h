#ifndef _NAMA_GL_CONTEXT_
#define _NAMA_GL_CONTEXT_

void * NaMaGetCurrentGLCtx();
void * NaMaCreateGLCtx(void * hWindow);
int NaMaMakeAsCurrentCtx(void *  hCtx);

#endif

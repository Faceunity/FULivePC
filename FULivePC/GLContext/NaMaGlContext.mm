#import <Foundation/Foundation.h>
#include "NaMaGLContext.h"
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/CGLRenderers.h>
#import <AppKit/AppKit.h>
#include "glfw3.h"
#include "glfw3native.h"

void * NaMaGetCurrentGLCtx()
{
    NSOpenGLContext * ctx = [NSOpenGLContext currentContext];
    
    return (__bridge_retained void*)ctx;
}

void * NaMaCreateGLCtx(void * hWindow)
{
    if (!hWindow) {
        return NULL;
    }
  
    NSWindow * pNsWindow = (__bridge NSWindow*)hWindow;
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:(const NSOpenGLPixelFormatAttribute[]){NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAColorSize,24,
        NSOpenGLPFAAlphaSize,8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFASampleBuffers,1,
        NSOpenGLPFASamples,4,
        0}];

    NSOpenGLContext * glContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    //[glContext setView:[pNsWindow contentView]];  //No Need Bind View
    
    return (__bridge_retained void*) glContext;
}

int NaMaMakeAsCurrentCtx(void *  hCtx)
{
    int iRet = 0;

    NSOpenGLContext * glContext = (__bridge  NSOpenGLContext *)(hCtx);
    [glContext makeCurrentContext];
    
    GLenum err = glGetError();
    
    return iRet;
}

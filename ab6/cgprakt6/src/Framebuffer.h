//
//  Framebuffer.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 03.12.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__Framebuffer__
#define __RealtimeRending__Framebuffer__

#include <iostream>

#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif

class Texture;

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();
    
    bool create(bool CreateDepthTarget=true, unsigned int Width=0, unsigned int Height=0);
    bool attachColorTarget( const Texture& Tex);
    bool detachColorTarget();
    const Texture* getAttachedTexture() { return m_pTexture; }
    
    void activate();
    void deactivate();
    
    unsigned int width() const { return m_Width; }
    unsigned int height() const { return m_Height; }
protected:
    GLuint m_FramebufferID;
    GLuint m_DepthTargetID;
    const Texture* m_pTexture;
    unsigned int m_Width;
    unsigned int m_Height;
};

#endif /* defined(__RealtimeRending__Framebuffer__) */

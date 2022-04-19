//
//  Texture.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 24.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "Texture.h"
#include "rgbimage.h"
#include "color.h"
#include <assert.h>
#include <stdint.h>
#include <exception>
#include <algorithm>
#include "FreeImage.h"

Texture* Texture::pDefaultTex = NULL;
Texture::SharedTexMap Texture::SharedTextures;

Texture* Texture::defaultTex()
{
    if(pDefaultTex)
        return pDefaultTex;
    
    unsigned char* data = new unsigned char[4*4*4];
    assert(data);
    std::memset(data, 255, 4*4*4);
    pDefaultTex = new Texture(4, 4, data);
    delete [] data;
    
    return pDefaultTex;
}

const Texture* Texture::LoadShared(const char* Filename)
{
    std::string path = Filename;
    
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);
    
    SharedTexMap::iterator it = SharedTextures.find( path);
    
    if(it != SharedTextures.end())
    {
        it->second.RefCount++;
        return it->second.pTex;
    }
    
    Texture* pTex = new Texture();
    if(!pTex->load(Filename) )
    {
        delete pTex;
        std::cout << "WARNING: Texture " << Filename << " not loaded (not found).\n";
        return NULL;
    }
    
    TexEntry TE;
    TE.pTex = pTex;
    TE.RefCount = 1;
    SharedTextures[path] = TE;
    
    return pTex;
}

void Texture::ReleaseShared( const Texture* pTex )
{
    for( SharedTexMap::iterator it=SharedTextures.begin(); it!=SharedTextures.end(); ++it)
    {
        if(it->second.pTex == pTex)
        {
            it->second.RefCount--;
            if(it->second.RefCount<=0)
            {
                delete it->second.pTex;
                SharedTextures.erase(it);
            }
            return;
        }
    }
}



Texture::Texture() : m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0)
{
    
}

Texture::Texture(unsigned int width, unsigned int height, unsigned char* data): m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0)
{
    bool Result = create(width, height, data);
    if(!Result)
        throw std::exception();
    
}
Texture::Texture(const char* Filename ): m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0)
{
    bool Result = load(Filename);
    if(!Result)
        throw std::exception();
}


Texture::~Texture()
{
    if(m_pImage)
        delete m_pImage;
    m_pImage = NULL;
}

bool Texture::isValid() const
{
    return m_TextureID > 0;
}

bool Texture::load( const char* Filename)
{
    FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileType(Filename, 0);
    if(ImageFormat == FIF_UNKNOWN)
        ImageFormat = FreeImage_GetFIFFromFilename(Filename);
    
    if(ImageFormat == FIF_UNKNOWN)
    {
        std::cout << "Warning: Unkown texture format: " << Filename << std::endl;
        return false;
    }
    
    FIBITMAP* pBitmap = FreeImage_Load( ImageFormat, Filename);
    
    if(pBitmap==NULL)
    {
        std::cout << "Warning: Unable to open texture image " << Filename << std::endl;
        return false;
    }
    
    FREE_IMAGE_TYPE Type = FreeImage_GetImageType(pBitmap);
    assert(Type==FIT_BITMAP);
    
    unsigned int Width = FreeImage_GetWidth(pBitmap);
    unsigned int Height = FreeImage_GetHeight(pBitmap);
    unsigned int bpp = FreeImage_GetBPP(pBitmap);
    assert(bpp==32||bpp==16||bpp==24);
    
    unsigned char* data = new unsigned char[Width* Height*4];
    unsigned char* dataPtr = data-1;
    
    if(data==NULL)
    {
        FreeImage_Unload(pBitmap);
        return false;
    }

    
    RGBQUAD c;
    for( unsigned int i=0; i<Height; ++i)
        for( unsigned int j=0; j<Width; ++j)
        {
            FreeImage_GetPixelColor(pBitmap, j, Height-i-1, &c);
            *(++dataPtr) = c.rgbRed;
            *(++dataPtr) = c.rgbGreen;
            *(++dataPtr) = c.rgbBlue;
            *(++dataPtr) = c.rgbReserved;
        }
    
    FreeImage_Unload(pBitmap);

    if( m_pImage )
        delete m_pImage;
    
    m_pImage = createImage(data, Width, Height);
    
    glGenTextures(1, &m_TextureID);
    
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture(GL_TEXTURE_2D, 0);
    
    delete [] data;
    return true;
}

bool Texture::create( unsigned int width, unsigned int height, unsigned char* data)
{
    if( m_pImage )
        delete m_pImage;
    
    m_pImage = createImage(data, width, height);
    
    glGenTextures(1, &m_TextureID);
    
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return true;
}

RGBImage* Texture::createImage( unsigned char* Data, unsigned int width, unsigned int height )
{
    // create CPU accessible image
    RGBImage* pImage = new RGBImage(width, height);
    assert(pImage);
    for( unsigned int i=0; i<height; i++)
		for (unsigned int j = 0; j<width; j++)
        {
            Color c( (float)*(Data)/255.0f, (float)*(Data+1)/255.0f, (float)*(Data+2)/255.0f);
            pImage->setPixelColor(j, i, c);
            Data+=4;
        }
    return pImage;
}

void Texture::activate(int slot) const
{
    if(m_TextureID==0 || slot < 0 || slot > 7 )
        return;
    
    CurrentTextureUnit = slot;

    glActiveTexture(GL_TEXTURE0 + CurrentTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::deactivate() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    if(CurrentTextureUnit>0)
        glActiveTexture(GL_TEXTURE0 + CurrentTextureUnit-1);
    CurrentTextureUnit=0;
    
}

const RGBImage* Texture::getRGBImage() const
{
    return m_pImage;
}

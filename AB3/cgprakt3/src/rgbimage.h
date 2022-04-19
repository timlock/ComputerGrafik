#ifndef __SimpleRayTracer__rgbimage__
#define __SimpleRayTracer__rgbimage__

#include <iostream>
#include <stdio.h>
#include <sstream>
class Color;

class RGBImage
{
public:
    RGBImage( unsigned int Width, unsigned Height);
    ~RGBImage();
    void setPixelColor( unsigned int x, unsigned int y, const Color& c);
    const Color& getPixelColor( unsigned int x, unsigned int y) const;
    bool saveToDisk( const char* Filename);
    unsigned int width() const;
    unsigned int height() const;

    int fileCount = 0;
    
    static unsigned char convertColorChannel( float f);
protected:
    Color* m_Image;
    unsigned int m_Height;
    unsigned int m_Width;
    
    
};

#endif /* defined(__SimpleRayTracer__rgbimage__) */

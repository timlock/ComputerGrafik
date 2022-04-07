#include "rgbimage.h"
#include "color.h"
#include "assert.h"

RGBImage::RGBImage( unsigned int Width, unsigned int Height)
{
	m_Width = Width;
	m_Height = Height;
	m_Image = new Color[m_Height*m_Width];
}

RGBImage::~RGBImage()
{
	delete[] m_Image;
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c)
{
	m_Image[x + y * m_Width] = c;

}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const
{
	return m_Image[x + y * m_Width];
}

unsigned int RGBImage::width() const
{
	return m_Width;
}
unsigned int RGBImage::height() const
{
	return m_Height;
}

unsigned char RGBImage::convertColorChannel( float v)
{
	if (v > 1.0f) return 1.0f;
	if (v < 0.0f) return 0.0f;
	return v * 255.0f;
}


bool RGBImage::saveToDisk( const char* Filename)
{
	FILE* file = nullptr;
	file = fopen(Filename, "wb");
	if (!file) 
	{
		std::cerr << "Datei konnte nicht geoeffnet werden" << std::endl;
		return false;
	}
	char* bfTypeChars = "BM";
	uint16_t abfType = (bfTypeChars[1] << 8) + bfTypeChars[0];
	uint16_t bfType[2] = { 'B','M' };
	fwrite(&abfType, 1, 2, file);
	uint32_t bfSize[1] = { 800 * 600 * 3 + 54 };
	fwrite(bfSize, 4, 1, file);
	uint32_t bfReserved[1] = { 0 };
	fwrite(bfReserved, 4, 1, file);
	uint32_t bfOffBits[1] = { 54 };
	fwrite(bfOffBits, 4, 1, file);
	uint32_t biSize[1] = { 40 };
	fwrite(biSize, 4, 1, file);
	int32_t biWidth[1] = { m_Width };
	fwrite(biWidth, 4, 1, file);
	int32_t biHeight[1] = { m_Height};
	fwrite(biHeight, 4, 1, file);
	uint16_t biPlanes[1] = { 1 };
	fwrite(biPlanes, 2, 1, file);
	uint16_t biBitCount[1] = { 24 };
	fwrite(biBitCount, 2, 1, file);
	uint32_t biCompression[1] = { 0 };
	fwrite(biCompression, 4, 1, file); //biCompression
	uint32_t biSizeImage[1] = { 0 };
	fwrite(biSizeImage, 4, 1, file); //biSizeImage
	int32_t biXPelsPerMeter[1] = { 0 };
	fwrite(biXPelsPerMeter, 4, 1, file); //biXPelsPerMeter
	int32_t biYPelsPerMeter[1] = { 0 };
	fwrite(biYPelsPerMeter, 4, 1, file); //biYPelsPerMeter
	uint32_t biClrUsed[1] = { 0 };
	fwrite(biClrUsed, 4, 1, file);
	uint32_t biClrImportant[1] = { 0 };
	fwrite(biClrImportant, 4, 1, file); //biClrImportant
	for (int y = m_Height - 1; y >= 0; y--) {
		for (int x = 0; x < m_Width; x++) {
			uint8_t r = convertColorChannel(m_Image[x + y * m_Width].R);
			uint8_t g = convertColorChannel(m_Image[x + y * m_Width].G);
			uint8_t b = convertColorChannel(m_Image[x + y * m_Width].B);
			uint8_t pixel[3] = { b,g,r };
			//fwrite(pixel, 1, 3, file);
			fwrite(&b, 1, 1, file);
			fwrite(&g, 1, 1, file);
			fwrite(&r, 1, 1, file);
		}
	}
	if (fclose(file) != 0)
	{
		std::cerr << "Datei konnte nicht geschlossen werden" << std::endl;
	};
	return true;
}

#include "color.h"
#include <assert.h>

Color::Color()
{
	R = 0.0f;
	G = 0.0f;
	B = 0.0f;
}

Color::Color( float r, float g, float b)
{
	if (r > 1.0f) r = 1.0f;
	R = r;
	if (g > 1.0f) g = 1.0f;
	G = g;
	if (b > 1.0f) b = 1.0f;
	B = b;
}

Color Color::operator*(const Color& c) const
{
	return Color(R * c.R, G * c.G, B * c.B);
}

Color Color::operator*(const float Factor) const
{
	return Color(R * Factor, G * Factor, B * Factor);
}

Color Color::operator+(const Color& c) const
{
	return Color(R + c.R, G + c.G, B + c.B);
}

Color& Color::operator+=(const Color& c)
{
	R += c.R;
	G += c.G;
	B += c.B;
	return *this; 
}
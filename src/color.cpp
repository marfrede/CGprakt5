#include "color.h"
#include <assert.h>

Color::Color()
{
}

Color::Color(float r, float g, float b)
{
	this->R = r;
	this->G = g;
	this->B = b;
}

Color Color::operator*(const Color& c) const
{
	return Color(
		this->R * c.R,
		this->G * c.G,
		this->B * c.B
	);
}

Color Color::operator*(const float Factor) const
{
	std::cout << this->R << ", " << this->G << ", " << this->B << std::endl;
	return Color(
		this->R * Factor,
		this->G * Factor,
		this->B * Factor
	);
}

Color Color::operator+(const Color& c) const
{
	return Color(
		this->R + c.R,
		this->G + c.G,
		this->B + c.B
	);
}

Color& Color::operator+=(const Color& c)
{
	return Color(
		this->R += c.R,
		this->G += c.G,
		this->B += c.B
	);
}
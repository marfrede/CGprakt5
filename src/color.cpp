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
	this->R += c.R;
	this->G += c.G;
	this->B += c.B;
	return *this;
}

std::ostream& operator<<(std::ostream& strm, const Color& C) {
	return strm << "{"
		<< "R: " << C.R << ", G:" << C.G << ", B:" << C.B
		<< "} ";
}

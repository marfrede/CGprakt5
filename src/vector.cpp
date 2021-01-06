#include "vector.h"
#include <assert.h>
#include <math.h>
//namespace std

Vector::Vector(float x, float y, float z)
{
	this->X = x;
	this->Y = y;
	this->Z = z;
}

Vector::Vector()
{
}

float Vector::dot(const Vector& v) const
{
	return (
		this->X * v.X +
		this->Y * v.Y +
		this->Z * v.Z
		);
}

Vector Vector::cross(const Vector& v) const
{
	return Vector(
		this->Y * v.Z - this->Z * v.Y,
		this->Z * v.X - this->X * v.Z,
		this->X * v.Y - this->Y * v.X
	);
}

Vector Vector::operator+(const Vector& v) const
{
	return Vector(
		this->X + v.X,
		this->Y + v.Y,
		this->Z + v.Z
	);
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(
		this->X - v.X,
		this->Y - v.Y,
		this->Z - v.Z
	);
}

Vector Vector::operator*(float c) const
{
	return Vector(
		this->X * c,
		this->Y * c,
		this->Z * c
	);
}

Vector Vector::operator-() const
{
	return Vector(
		this->X * -1,
		this->Y * -1,
		this->Z * -1
	);
}

Vector& Vector::operator+=(const Vector& v)
{
	this->X += v.X;
	this->Y += v.Y;
	this->Z += v.Z;
	return *this;
}

bool Vector::operator==(const Vector& v) const
{
	return
		this->X == v.X &&
		this->Y == v.Y &&
		this->Z == v.Z;
}

bool Vector::operator!=(const Vector& v) const
{
	return !this->operator==(v);
}

std::ostream& operator<<(std::ostream& strm, const Vector& V) {
	return strm << "{"
		<< V.X << "," << V.Y << "," << V.Z
		<< "} ";
}

Vector& Vector::normalize()
{
	return *this = (*this) * (1.0f / this->length());
}

float Vector::length() const
{
	return sqrtf(this->lengthSquared());
}

float Vector::lengthSquared() const
{
	return (
		powf(this->X, 2) +
		powf(this->Y, 2) +
		powf(this->Z, 2)
		);
}

/** get on the plane (normal) mirrored vector */
Vector Vector::reflection(const Vector& normal) const
{
	return *this - normal * (2 * (this->dot(normal)));
}

/**
* @param d Directional Vector
* @param a Corner a of Triangle
* @param b Corner a of Triangle
* @param c Corner a of Triangle
* @param s (out): factor of length of d till impact
*/
bool Vector::triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	// Vektor d muss eine L�nge haben, um �berhaupt irgendetwas schneiden zu k�nnen
	if (d.lengthSquared() == 0) {
		// liegt *this in der Ebene? dann true??
		return false;
	}

	// Normalvektor n der Ebene ABC berechnen nach Formel aus der VL
	// n ist der Normalvektor, der Ebene die sich aus ABC ergibt.
	// dabei ist die Ebene jedoch unendlich und ist nicht durch die Eckpunkte ABC begrenzt.
	Vector n = ((b - a).cross(c - a)).normalize();

	// aus der expliziten Ebenengleichung [f(Punkt p) = (Vektor n * Punkt p) - Strecke df = 0] folgt:
	// f�r jeden Punkt p, der in der Ebene aus ABC liegt, gilt: (n * p) - df = 0
	// Vektoren a,b und c sind Teil der Ebene, sodass gelten muss (n * a) - df = 0 [und auch (n * b) - d = (n * c) - df = 0]
	// df ist gesucht: df = (n * a) entspricht df = n.dot(a)
	float df = n.dot(a);

	// die Strahlengleichung [Punkt p = Punkt o + Faktor s * Vektor d], mit Punkt o = Ursprung des Vektors d, gibt an, dass 
	// ein Vektor d einen Punkt p, bei passender Skalierung s trifft.
	// gesucht ist, ob der gegebene Vektor d mit Ursprung *this einen beliebigen Punkt der Ebene ABC trifft.
	// das bedeutet dass ein Punkt p, der auf dem Vektor liegt, auch in der Ebene liegen muss
	// daraus folgt 0 = Schnittpunkt p * Normalvektor n - float df, mit Schnittpunkt p = o + sd 
	// p = o + s*d in die Strahlengleichung einsetzen.
	// 0 = n * (o + s*d) - df
	// 0 = (n * o) + (n * s*d) - df, wenn Vektor d NICHT parallel zur Ebene
	if ((n.dot(d)) == 0) {
		return false;
	}
	// -(n * s*d) = (n * o) - df
	// (n * s*d) = df - (n * o)
	// s (n * d) = df - (n * o)
	// s = (df - (n * o)) / (n * d), mit o als *this gegeben
	s = (df - n.dot(*this)) / n.dot(d);

	if (s <= 0) {
		return false;
	}

	// damit schneidet Vektor d nach Skalierung mit s die Ebene in Punkt p
	Vector p = *this + d * s;

	// offen ist, ob Vektor d die Ebene innerhalb oder au�erhalb des Deiecks schneidet
	// dazu l�sst sich die Fl�che des Dreiecks ABC mit der Fl�che der Teildreiecke ABP, ACP und BCP vergleichen.
	// liegt der Punkt p au�erhalb des Dreiecks ABC ist die Fl�che der Teildreiecke addiert gr��er als Fl�che ABC
	if (Vector::triangleArea(a, b, p) + Vector::triangleArea(a, c, p) + Vector::triangleArea(b, c, p) > Vector::triangleArea(a, b, c) + 0.0001f) {
		return false;
	}
	return true;
}

float Vector::triangleArea(Vector u, Vector v, Vector w)
{
	return ((v - u).cross(w - u)).length() / 2;
}


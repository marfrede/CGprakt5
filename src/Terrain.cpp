#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"

using namespace std;

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2) : Size(10, 1, 10)
{
	if (HeightMap && DetailMap1 && DetailMap2)
	{
		bool loaded = load(HeightMap, DetailMap1, DetailMap2);
		if (!loaded)
			throw std::exception();
	}
}

Terrain::~Terrain()
{

}

bool Terrain::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2)
{
	if (!HeightTex.load(HeightMap))
		return false;
	if (!DetailTex[0].load(DetailMap1))
		return false;
	if (!DetailTex[1].load(DetailMap2))
		return false;

	// TODO: add code for creating terrain model
	VB.begin();
	IB.begin();
	int index = 0;
	Texture texture(HeightMap);
	const RGBImage* img = texture.getRGBImage();
	RGBImage sobel(img->width(), img->height());
	RGBImage::SobelFilter(sobel, *img, 10.0f);
	/*sobel.saveToDisk("D:\\\\frede\\\\Documents\\\\SCHOOL\\\\HS OSNA\\\\7. Semester WiSe21\\\\CG\\\\cgprakt5\\\\assets\\\\mixmapExec.bmp");
	cout << "finished sobel" << endl;*/
	this->MixTex.create(sobel);

	//cout << "w,h: " << img->width() << ", " << img->height() << endl;
	for (int x = 0; x < img->width(); x++) {
		for (int y = 0; y < img->height(); y++) {
			// Texturkoordinate
			float u, v = 0.0f;
			u = (float)x / img->width();
			v = (float)y / img->height();
			VB.addTexcoord0(u, v); // mixmap

			// Farbe (Höhe)
			Color color = img->getPixelColor(x, y);

			// Berechnung Vertex
			//cout << endl;
			//if (y == 0) cout << endl << endl;
			//cout << "bmp (" << x << "," << y << "): " << color.G << endl;
			Vector pos(
				x - (float)(img->width() / 2),
				color.G, // only looking at color.g assuming r=g=b in grayscale image
				y - (float)(img->height() / 2)
			);
			//cout << "pos " << pos << endl;

			// Berechnung Normale
			Vector normal = this->avgNormal(img, pos);
			if (normal.Y < 0.99) {
				cout << "normal: " << normal << endl;
			}
			VB.addNormal(normal); // calc normals before scaling

			// !!Skalierung muss im shader passieren!!
			pos.X *= 10 / (float)img->width();
			pos.Z *= 10 / (float)img->height();
			pos.Y *= 1.0;
			// !!Skalierung muss im shader passieren!!

			// Hinzufügen Vertex
			VB.addVertex(pos.X, pos.Y, pos.Z);

			// Berechnung Indizes
			if (x != 0) { // Indizes erst ab Spalte 1 (nicht 0!) festlegen, da Spalte 1 sozusagen nur ein Strich
				if (y == 0) { // Erster Vertex von Spalte hat nur ein Dreieck
					IB.addIndex(index);
					IB.addIndex(index - img->height());
					IB.addIndex(index + 1);
				}
				else if (y != img->height() - 1) { // Alle anderen Vertizes von Spalte haben an sich zwei Dreiecke
					IB.addIndex(index);
					IB.addIndex(index - img->height() - 1);
					IB.addIndex(index - img->height());

					IB.addIndex(index);
					IB.addIndex(index - img->height());
					IB.addIndex(index + 1);
				}
				else if (y == img->height() - 1) { // Letzter Vertex von Spalte hat von sich ausgehend nur ein Dreieck (zweites Dreieck wird von Vertex davor gezogen)
					IB.addIndex(index);
					IB.addIndex(index - img->height());
					IB.addIndex(index - img->height() - 1);
				}
			}
			index++;
		}
	}
	VB.end();
	IB.end();
	return true;
}

void Terrain::shader(BaseShader* shader, bool deleteOnDestruction)
{
	BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
	applyShaderParameter();
	BaseModel::draw(Cam);

	VB.activate();
	IB.activate();
	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
	IB.deactivate();
	VB.deactivate();
}

void Terrain::applyShaderParameter()
{
	TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
	if (!Shader)
		return;

	Shader->mixTex(&MixTex);
	for (int i = 0; i < 2; i++)
		Shader->detailTex(i, &DetailTex[i]);
	Shader->scaling(Size);

	// TODO: add additional parameters if needed..
}

Vector Terrain::avgNormal(const RGBImage* img, const Vector pos) {
#define VecXZ(x, a, b, z) Vector(x, img->getPixelColor(a,b).G, z); cout << "inside(0, 2000): " << "a: " << a << ", b: " << b;

	Vector vLB(0, 0, 0), v0B(0, 0, 0), vR0(0, 0, 0), vRA(0, 0, 0), v0A(0, 0, 0), vL0(0, 0, 0); // 8er neighbours verteces => L-Left, R-Right, A-Above, B-Below. (but only 6 of them relevant bc of triangles)
	bool e0A = false, e0B = false, eL0 = false, eR0 = false; // neighbours verteces exist?
	Vector nA(0, 0, 0), nB(0, 0, 0), nC(0, 0, 0), nD(0, 0, 0), nE(0, 0, 0), nF(0, 0, 0); // normals of neighbour triangle areas (0 if not existing)
	int wH = (int)img->width() / 2, hH = (int)img->height() / 2; // half image width and height (to check overflow = neighbour exist?)

	// get neighbours
	if (pos.Z + 1.0f < hH) {
		e0A = true;
		v0A = VecXZ(pos.X, pos.X + wH, pos.Z + hH + 1, pos.Z + 1.0f);
	}
	if (pos.Z - 1 >= hH * -1) {
		e0B = true;
		v0B = VecXZ(pos.X, pos.X + wH, pos.Z + hH - 1, pos.Z - 1);
	}
	if (pos.X - 1 >= wH * -1) {
		eL0 = true;
		vL0 = VecXZ(pos.X - 1, pos.X + wH - 1, pos.Z + hH, pos.Z);
	}
	if (pos.X + 1 < wH) {
		eR0 = true;
		vR0 = VecXZ(pos.X + 1, pos.X + wH + 1, pos.Z + hH, pos.Z);
	}
	if (eL0 && e0B) {
		vLB = VecXZ(pos.X - 1, pos.X + wH - 1, pos.Z + hH - 1, pos.Z - 1);
	}
	if (eR0 && e0A) {
		vRA = VecXZ(pos.X + 1, pos.X + wH + 1, pos.Z + hH + 1, pos.Z + 1);
	}
	//get normals
	if (eL0 && e0B) {
		nA = (pos - vLB).cross(pos - v0B);
		nF = (pos - vL0).cross(pos - vLB);
	}
	if (e0B && eR0) {
		nB = (pos - v0B).cross(pos - vR0);
	}
	if (eR0 && e0A) {
		nC = (pos - vR0).cross(pos - vRA);
		nD = (pos - vRA).cross(pos - v0A);
	}
	if (e0A && eL0) {
		nE = (pos - v0A).cross(pos - vL0);
	}
	Vector avgNormal = nA + nB + nC + nD + nE + nF;
	avgNormal = avgNormal * (1.0f / avgNormal.length());
	if (v0A.Y != 0 || v0B.Y != 0 || vL0.Y != 0 || vR0.Y != 0 || vLB.Y != 0 || vRA.Y != 0) {
		cout << "v0A: " << v0A << endl;
		cout << "v0B: " << v0B << endl;
		cout << "vL0: " << vL0 << endl;
		cout << "vR0: " << vR0 << endl;
		cout << "vRA: " << vRA << endl;
		cout << "vLB: " << vLB << endl;
		cout << endl;
		cout << "nA: " << nA << endl;
		cout << "nF: " << nF << endl;
		cout << "nB: " << nB << endl;
		cout << "nC: " << nC << endl;
		cout << "nD: " << nD << endl;
		cout << "nE: " << nE << endl;
		cout << "avgNormal: " << avgNormal << endl << endl << endl;
	}
	return -avgNormal;
}

void Terrain::control(double diffMouseX, double diffMouseY) {
	Size.X -= diffMouseX * 0.001;
	Size.Y += diffMouseY * 0.001;
	Size.Z -= diffMouseX * 0.001;
	// cout << diffMouseX << endl;
}

#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include "Matrix.h"

RGBImage::RGBImage(unsigned int Width, unsigned int Height)
{
	this->m_Width = Width;
	this->m_Height = Height;
	this->m_Image = new Color[this->m_Width * this->m_Height];
}

RGBImage::~RGBImage()
{
	delete this->m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c)
{
	this->m_Image[y * this->m_Width + x] = c;
	return;
}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const
{
	return this->m_Image[y * this->m_Width + x];
}

unsigned int RGBImage::width() const
{
	return this->m_Width;
}
unsigned int RGBImage::height() const
{
	return this->m_Height;
}

unsigned char RGBImage::convertColorChannel(float v)
{
	v = v > 1.0f ? 1.0f : v; // max 1
	v = v < 0.0f ? 0.0f : v; // min 0
	return v * 255;
};

RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) {
	float k[3][3] = {
		{1.0f, 0.0f, -1.0f},
		{2.0f, 0.0f, -2.0f},
		{1.0f, 0.0f, -1.0f}
	};
	float kT[3][3] = {
		{1.0f, 2.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{-1.0f, -2.0f, -1.0f}
	};
	for (int x = 0; x < src.m_Width; x++) {
		for (int y = 0; y < src.m_Height; y++) {
			Color u(0.0f, 0.0f, 0.0f);
			Color v(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < 3; j++) {
				for (int i = 0; i < 3; i++) {
					u += src.getPixelColor(x - i - 1, y - j - 1) * k[i][j];
					u += src.getPixelColor(x - i - 1, y - j - 1) * kT[i][j];
				}
			}
			//Color s = (u * u) + (v * v);
			Color s(0, 0, 0);
			s.R = sqrtf(s.R);
			s.G = sqrtf(s.G);
			s.B = sqrtf(s.B);
			s = s * factor;
			dst.setPixelColor(x, y, s);
		}
	}
	return dst;
}

struct Pixel {
	uint8_t blue = 255;
	uint8_t green = 255;
	uint8_t red = 0;
} pixel;

struct BmpInfoHeader {
	uint32_t sizeOfThisHeader = sizeof(BmpInfoHeader);
	int32_t width = 0; // wird dynamisch gesetzt
	int32_t height = 0; // wird dynamisch gesetzt
	uint16_t numberOfColorPlanes = 1; // immer 1
	uint16_t colorDepth = 24; //Farbtiefe
	uint32_t compressionMethod = 0; //bei BMP 0
	uint32_t rawBitmapDataSize = 0; // generally ignored
	int32_t horizontalResolution = 0; // darf 0 sein
	int32_t verticalResolution = 0; // darf 0 sein
	uint32_t colorTableEntries = 0;
	uint32_t importantColors = 0;
};

struct BmpHeader {
	uint16_t bitmapSignatureByte = 0x4d42; // ASCII-Code f�r BM
	uint32_t sizeOfBitmapFile = 0; // wird dynamisch gesetzt
	uint32_t reservedBytes = 0;
	uint32_t pixelDataOffset = 54;
};

bool RGBImage::saveToDisk(const char* Filename)
{
	FILE* imageFile = fopen(Filename, "wb");
	if (imageFile == NULL) {
		std::cout << "could not open file. filename: >" << Filename << "<" << std::endl;
		return false;
	}

	BmpHeader bmpHeader;
	BmpInfoHeader bmpInfoHeader;
	bmpInfoHeader.width = width();
	bmpInfoHeader.height = height();
	bmpHeader.sizeOfBitmapFile = sizeof(bmpHeader) + sizeof(BmpInfoHeader) + 3 * width() * height();

	// write values from bmpHeader one by one, to avoid padding bytes
	// (bc its header size is 14 bytes and if written at once there are 2 padding bytes making it to 16 bytes large)
	fwrite(&bmpHeader.bitmapSignatureByte, sizeof(bmpHeader.bitmapSignatureByte), 1, imageFile);
	fwrite(&bmpHeader.sizeOfBitmapFile, sizeof(bmpHeader.sizeOfBitmapFile), 1, imageFile);
	fwrite(&bmpHeader.reservedBytes, sizeof(bmpHeader.reservedBytes), 1, imageFile);
	fwrite(&bmpHeader.pixelDataOffset, sizeof(bmpHeader.pixelDataOffset), 1, imageFile);

	fwrite(&bmpInfoHeader, sizeof(BmpInfoHeader), 1, imageFile);

	for (int i = height() - 1; i >= 0; i--) {
		for (int j = 0; j < width(); j++) {
			Color tmp = getPixelColor(j, i);
			unsigned char r = convertColorChannel(tmp.R);
			unsigned char g = convertColorChannel(tmp.G);
			unsigned char b = convertColorChannel(tmp.B);
			fwrite(&b, sizeof(unsigned char), 1, imageFile);
			fwrite(&g, sizeof(unsigned char), 1, imageFile);
			fwrite(&r, sizeof(unsigned char), 1, imageFile);
		}
	}

	fclose(imageFile);

	return true;
}

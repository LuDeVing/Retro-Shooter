#ifndef MISC_CLASS_H
#define MISC_CLASS_H

#include "olcPixelGameEngine.h"
#include "stb_image.h"

class Texture {

	unsigned char* imageData = NULL;

public:

	int imageWidth, imageHeight, channels;

	Texture();
	Texture(const char* filePath);

	olc::Pixel getPixel(int x, int y);

};

#endif

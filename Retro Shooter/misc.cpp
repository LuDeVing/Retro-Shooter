#include "misc.h"

Texture::Texture(const char* filePath) {
	imageData = stbi_load(filePath, &imageWidth, &imageHeight, &channels, 0);	
}

olc::Pixel Texture::getPixel(int x, int y) {

	if (x >= this->imageWidth || x < 0 || y >= this->imageHeight || y < 0)
		return olc::Pixel();

	int pixelLocation = (imageWidth * y + x) * channels;

	return olc::Pixel(
		imageData[pixelLocation + 0],
		imageData[pixelLocation + 1],
		imageData[pixelLocation + 2]
	);


}
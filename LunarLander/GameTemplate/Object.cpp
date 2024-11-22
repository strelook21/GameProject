#include "Object.h"
#include <vector>
#include <fstream>
#include <cstdint>

#pragma pack(push, 1) // Ensure no padding is added by the compiler
struct BMPHeader {
	uint16_t bfType;      // BMP file type
	uint32_t bfSize;      // Size of the file
	uint16_t bfReserved1; // Reserved; must be 0
	uint16_t bfReserved2; // Reserved; must be 0
	uint32_t bfOffBits;   // Offset to start of pixel data
};

struct DIBHeader {
	uint32_t biSize;          // Size of this header
	int32_t biWidth;          // Width of bitmap in pixels
	int32_t biHeight;         // Height of bitmap in pixels
	uint16_t biPlanes;        // Number of color planes
	uint16_t biBitCount;      // Bits per pixel
	uint32_t biCompression;   // Compression type
	uint32_t biSizeImage;     // Image size (can be 0 for uncompressed)
	int32_t biXPelsPerMeter;   // Horizontal resolution (pixels per meter)
	int32_t biYPelsPerMeter;   // Vertical resolution (pixels per meter)
	uint32_t biClrUsed;       // Number of colors in palette
	uint32_t biClrImportant;   // Important colors
};
#pragma pack(pop)

GameObject::GameObject() {
	id = obj_cnt;
	obj_cnt++;
}
GameObject::GameObject(const GameObject& cpy) {
	id = cpy.id;
	origin = cpy.origin;
	speed = cpy.speed;
	gravity = cpy.gravity;
	direction = cpy.direction;
	gravity_direction = cpy.gravity_direction;
}
GameObject::GameObject(const std::pair<double, double>& _origin) {
	id = obj_cnt;
	obj_cnt++;
	origin = _origin;
}
GameObject::~GameObject() {
	obj_cnt--;
}

VectorObject::VectorObject(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices) {
	origin = _origin;
	vertices = _vertices;
}

SpritedObject::SpritedObject(const std::pair<double, double>& _origin, const std::string& _sprite_name) {
	id = obj_cnt;
	obj_cnt++;
	origin = _origin;
	
	std::fstream file("Sprites/" + _sprite_name, std::ios::binary);

	if (!file.is_open())
		exit;

	BMPHeader bmpHeader;
	DIBHeader dibHeader;

	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

	if (bmpHeader.bfType != 0x4D42)
		exit;

	std::vector<std::vector<uint32_t>> _sprite(dibHeader.biHeight, std::vector<uint32_t>(dibHeader.biWidth));
	sprite = _sprite;

	file.seekg(bmpHeader.bfOffBits, std::ios::beg);
	
	for (int i = dibHeader.biHeight - 1; i >= 0; i--)
	{
		for (int j = 0; j < dibHeader.biWidth; j++)
		{
			unsigned char bgr[3];
			file.read(reinterpret_cast<char*>(bgr), sizeof(bgr));
			sprite[i][j] = (bgr[2] << 16) | (bgr[1] << 8) | bgr[0];
		}
		file.ignore(((dibHeader.biWidth * 3 + 3) & (~3)) - dibHeader.biWidth * 3 * sizeof (unsigned char));
	}
}

unsigned int instance_create_vector(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices) {
	return 1;
}

unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name) {
	return 1;
}

unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name, const std::pair<int, int>& _offset) {
	return 1;
}
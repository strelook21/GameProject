#include "Object.h"
#include "Engine.h"
#include <vector>
#include <fstream>
#include <cstdint>
#include <memory>
#include <algorithm>

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

#pragma region General
GameObject::GameObject() {
}
GameObject::GameObject(const GameObject& cpy) {
	id = cpy.id;
	origin = cpy.origin;
	speed = cpy.speed;
	gravity = cpy.gravity;
	direction = cpy.direction;
	//gravity_direction = cpy.gravity_direction;
}
GameObject::GameObject(const std::pair<double, double>& _origin) {
	origin = _origin;
}

uint32_t GameObject::Blend(uint32_t source, uint32_t dest) {

	uint8_t src_argb[4] = { ((source >> 24) & 0xFF), ((source >> 16) & 0xFF), ((source >> 8) & 0xFF), (source & 0xFF) };
	uint8_t dest_argb[4] = { ((dest >> 24) & 0xFF), ((dest >> 16) & 0xFF), ((dest >> 8) & 0xFF), (dest & 0xFF) };
	uint8_t blend[4];
	float ratio = std::clamp(Alpha(), 0.0, 1.0) * static_cast<float>(src_argb[0]) / 255;
	for (int i = 1; i < 4; i++)
		blend[i] = static_cast<uint8_t>(src_argb[i] * ratio + dest_argb[i] * (1 - ratio));
	blend[0] = std::min(0xFF, src_argb[0] + dest_argb[0]);
	return (blend[0] << 24) |
			(blend[1] << 16) |
			(blend[2] << 8) |
			blend[3];

}
#pragma endregion General

#pragma region Vector
VectorObject::VectorObject(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices, bool ui) {
	type = VECTOR;
	origin = _origin;
	vertices = _vertices;
}
void VectorObject::Draw() {

}
#pragma endregion Vector

#pragma region Sprited
SpritedObject::SpritedObject(const std::pair<double, double>& _origin, const std::string& _sprite_name, const std::pair<int, int>& _offset, bool ui, CType _ctype, double _v1, double _v2) {
	type = SPRITED;
	origin = _origin;
	if (ui)
		ui_cnt++;
	else
		obj_cnt++;
	id = (ui) ? ui_cnt : obj_cnt;
	offset = _offset;
	sprite_name = _sprite_name;
	if (_sprite_name == "BLANK")
	{
		sprite = std::vector<std::vector<uint32_t>>(SCREEN_HEIGHT, std::vector<uint32_t>(SCREEN_WIDTH, 0xFF000000));
		return;
	}
	collider_type = _ctype;
	if (_ctype == CIRCLE)
		collider = new CircleCollider(_v1);
	if (_ctype == RECTANGLE)
		collider = new RectangleCollider({ _v1, _v2 });

	std::ifstream file("Sprites\\" + _sprite_name, std::ios::binary);

	if (!file.is_open())
		schedule_quit_game();

	BMPHeader bmpHeader;
	DIBHeader dibHeader;

	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

 	if (bmpHeader.bfType != 0x4D42)
		schedule_quit_game();

	std::vector<std::vector<uint32_t>> _sprite(dibHeader.biHeight, std::vector<uint32_t>(dibHeader.biWidth));
	sprite = _sprite;


	file.seekg(bmpHeader.bfOffBits, std::ios::beg);
	
	for (int i = dibHeader.biHeight - 1; i >= 0; i--)
	{
		for (int j = 0; j < dibHeader.biWidth; j++)
		{
			unsigned char bgra[4];
			file.read(reinterpret_cast<char*>(bgra), sizeof(bgra));
			sprite[i][j] = (bgra[3] << 24) | (bgra[2] << 16) | (bgra[1] << 8) | bgra[0];
		}
	} 
}

void SpritedObject::ChangeSprite(const std::string& _sprite_name) {
	sprite_name = _sprite_name;
	std::ifstream file("Sprites\\" + _sprite_name, std::ios::binary);

	if (!file.is_open())
		schedule_quit_game();

	BMPHeader bmpHeader;
	DIBHeader dibHeader;

	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

	if (bmpHeader.bfType != 0x4D42)
		schedule_quit_game();

	std::vector<std::vector<uint32_t>> _sprite(dibHeader.biHeight, std::vector<uint32_t>(dibHeader.biWidth));
	sprite = _sprite;


	uint32_t size = dibHeader.biBitCount;

	file.seekg(bmpHeader.bfOffBits, std::ios::beg);

	for (int i = dibHeader.biHeight - 1; i >= 0; i--)
	{
		for (int j = 0; j < dibHeader.biWidth; j++)
		{
			unsigned char bgra[4];
			file.read(reinterpret_cast<char*>(bgra), sizeof(bgra));
			sprite[i][j] = (bgra[3] << 24) | (bgra[2] << 16) | (bgra[1] << 8) | bgra[0];
		}
	}
}

void SpritedObject::Draw() {
	for (int j = 0; j < sprite.size(); j++)
	{
		for (int i = 0; i < sprite[0].size(); i++)
		{
			int x = i + std::floor(X()) - OffsetX();
			int y = j + std::floor(Y()) - OffsetY();
			if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) || (y >= SCREEN_HEIGHT))
				continue;
			buffer[y][x] = Blend(sprite[j][i], buffer[y][x]);
		}
	}
}


bool SpritedObject::CollisionCheck(const GameObject& other) {
 	if (ColliderType() == NONE || other.ColliderType() == NONE)
		return false;
	if (ColliderType() == CIRCLE && other.ColliderType() == CIRCLE)
	{
		double distance2 = (X() - other.X()) * (X() - other.X()) + (Y() - other.Y()) * (Y() - other.Y());
		double diameter = Collider()->Radius() + other.Collider()->Radius();
		return distance2 <= diameter * diameter;
	}
	if (ColliderType() == RECTANGLE && other.ColliderType() == RECTANGLE)
	{
		if (X() + Collider()->Width() < other.X() ||
			other.X() + other.Collider()->Width() < X() ||
			Y() + Collider()->Height() < other.Y() ||
			other.Y() + other.Collider()->Height() < Y())
			return true;
		else
			return false;
	}
	if (ColliderType() == CIRCLE)
	{
		const double x = X();
		const double y = Y();
		const double rX = other.X() - other.OffsetX();
		const double rY = other.Y() - other.OffsetY();
		const double radius = Collider()->Radius();
		const double width = other.Collider()->Width();
		const double height = other.Collider()->Height();
		double closest_x = std::max(rX, std::min (x, rX + width));
		double closest_y = std::max(rY, std::min (y, rY + height));
		double distance2 = (x - closest_x) * (x - closest_x) + (y - closest_y) * (y - closest_y);
		return distance2 <= radius * radius;
	}
	else if (other.ColliderType() == CIRCLE)
	{
		const double x = other.X();
		const double y = other.Y();
		const double rX = X() - OffsetX();
		const double rY = Y() - OffsetY();
		const double radius = other.Collider()->Radius();
		const double width = Collider()->Width();
		const double height = Collider()->Height();
		double closest_x = std::max(rX, std::min(x, rX + width));
		double closest_y = std::max(rY, std::min(y, rY + height));
		double distance2 = (x - closest_x) * (x - closest_x) + (y - closest_y) * (y - closest_y);
		return distance2 <= radius * radius;
	}
	return false;
}
#pragma endregion Sprited

#pragma region Instantiation
unsigned int instance_create_vector(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices) {
	objectList.push_back(std::make_shared<VectorObject>(_origin, _vertices));
	return obj_cnt;
}

unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name, const bool _ui, const std::pair<int, int>& _offset, 
									const CType _ctype, const double _v1, const double _v2) {
	if (_ui)
		uiList.push_back(std::make_shared<SpritedObject>(_origin, _sprite_name, _offset, _ui, _ctype, _v1, _v2));
	else
		objectList.push_back(std::make_shared<SpritedObject>(_origin, _sprite_name, _offset, _ui, _ctype, _v1, _v2));
	return (_ui) ? ui_cnt : obj_cnt;
}

void instance_clear() {
	objectList.clear();
	uiList.clear();
	obj_cnt = 0;
	ui_cnt = 0;
}

void background_set(std::string _background) {
	if (_background == "NULL")
	{
		memset(background, 0xFF000000, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
		return;
	}
	std::ifstream file("Sprites\\" + _background, std::ios::binary);

	if (!file.is_open())
		schedule_quit_game();

	BMPHeader bmpHeader;
	DIBHeader dibHeader;

	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

	if (bmpHeader.bfType != 0x4D42)
		schedule_quit_game();

	uint32_t size = dibHeader.biBitCount;

	file.seekg(bmpHeader.bfOffBits, std::ios::beg);

	for (int i = dibHeader.biHeight - 1; i >= 0; i--)
	{
		for (int j = 0; j < dibHeader.biWidth; j++)
		{
			unsigned char bgra[4];
			file.read(reinterpret_cast<char*>(bgra), sizeof(bgra));
			background[i][j] = (bgra[3] << 24) | (bgra[2] << 16) | (bgra[1] << 8) | bgra[0];
		}
	}
}

#pragma endregion Instantiation
#pragma once
#include "Engine.h"
#include <vector>
#include <utility>
#include <string>
#include <memory>
#include <cmath>

# define M_PI           3.14159265358979323846

#pragma region Colliders
enum CType {
	NONE,
	CIRCLE,
	RECTANGLE
};

class Collider {
	protected:
		CType type = NONE;
	public:
		virtual ~Collider() {}
		virtual double Radius() const { return 0; }
		virtual double Height() const { return 0; }
		virtual double Width() const { return 0; }
};

class CircleCollider : public Collider {
	private:
		double radius = 0;
	public:
		CircleCollider(double _radius) : radius(_radius) {};
		double Radius() const { return radius; }
};

class RectangleCollider : public Collider {
	private:
		std::pair<double, double> size;
	public:
		RectangleCollider(std::pair<double, double> _size) : size(_size) {};
		double Height() const { return size.first; }
		double Width() const { return size.second; }
};
#pragma endregion Colliders

#pragma region Objects
enum OType {
	UNSPRITED,
	SPRITED,
	VECTOR
};

class GameObject {
	protected:
		int id = -1;
		std::pair<double, double> origin = {0, 0};
		std::pair<int, int> offset = { 0, 0 };
		double speed = 0;
		double gravity = 0;
		double direction = 0;
		//double gravity_direction = -1 * (M_PI) / 2;
		double alpha = 1;
		OType type = UNSPRITED;
		uint32_t Blend(uint32_t c1, uint32_t c2);
		CType collider_type = NONE;
		Collider* collider = nullptr;
	public:
		bool active = true;
		GameObject();
		GameObject(const GameObject& cpy);
		GameObject(const std::pair<double, double>& _origin);
		virtual ~GameObject() {}

		double X() const { return origin.first; }
		double Y() const { return origin.second; }
		double Speed() const { return speed; }
		double Gravity() const { return gravity; }
		double Direction() const { return direction; }
		//double GravityDirection() const { return gravity_direction;}
		CType ColliderType() const { return collider_type; }
		Collider* Collider() const { return collider; }
		int OffsetX() const { return offset.first; }
		int OffsetY() const { return offset.second; }
		void SetOffset(std::pair<int, int> _offset) { offset = _offset; }
		double Alpha() const { return alpha; }
		OType Type() const { return type; }
		void MoveTo(double _x, double _y) { origin = { _x, _y }; }
		void SetSpeed(double _speed) { speed = _speed; }
		void SetGravity(double _gravity) { gravity = _gravity; }
		void SetDirection(double _direction) { direction = _direction; }
		//void SetGravityDirection(double _gravity_direction) { gravity_direction = _gravity_direction; }
		void SetAlpha(double _alpha) { alpha = _alpha; }
		virtual std::string SpriteName() const { return "N/A"; }
		virtual void ChangeSprite(const std::string& _sprite_name) {}
		virtual void Draw() {}
		virtual bool CollisionCheck (const GameObject& other) { return true; }

};

class VectorObject : public GameObject {
	private:
		std::vector<std::pair<double, double>> vertices;
	public:
		VectorObject(const std::pair<double, double>&  _origin, const std::vector <std::pair<double, double>>& _vertices, bool ui = false);
		void Draw();
};

class SpritedObject : public GameObject {
	private:
		std::string sprite_name;
		std::vector<std::vector<uint32_t>> sprite = {};
	public:
		SpritedObject(const std::pair<double, double>& _origin, const std::string& _sprite_name = "BLANK", const std::pair<int, int>& _offset = { 0, 0 }, bool ui = false,
																CType _ctype = NONE, double _v1 = 0, double _v2 = 0);
		std::string SpriteName() const{ return sprite_name; }
		void ChangeSprite(const std::string& _sprite_name);
		void Draw();
		bool CollisionCheck (const GameObject& other);
};

#pragma endregion Objects


enum Rooms {
	STARTUP,
	TITLE,
	GAME,
	VICTORY
};

extern std::vector<std::shared_ptr<GameObject>> objectList;
extern unsigned int obj_cnt;
extern std::vector<std::shared_ptr<GameObject>> uiList;
extern unsigned int ui_cnt;
extern uint32_t background[SCREEN_HEIGHT][SCREEN_WIDTH];

unsigned int instance_create_vector(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices);
unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name, const bool ui = false, const std::pair<int, int>& _offset = {0, 0},
									const CType _ctype = NONE, const double _v1 = 0.0, const double _v2 = 0.0);
void instance_clear();
void background_set(std::string _background);
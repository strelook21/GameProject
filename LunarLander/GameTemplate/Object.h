#pragma once
#include <vector>
#include <utility>
#include <string>

extern unsigned int obj_cnt;

class GameObject {
	protected:
		int id = -1;
		std::pair<double, double> origin = {0, 0};
		double speed = 0;
		double gravity = 0;
		double direction = 0;
		double gravity_direction = 0;
	public:
		bool active = true;
		GameObject();
		GameObject(const GameObject& cpy);
		GameObject(const std::pair<double, double>& _origin);
		~GameObject();

		double X() const { return origin.first; }
		double Y() const { return origin.second; }
		double Speed() const { return speed; }
		double Gravity() const { return gravity; }
		double Direction() const { return direction; }
		double GravityDirection() const { return gravity_direction; }
		void SetX(double _x) { origin.first = _x; }
		void SetY(double _y) { origin.second = _y; }
		void SetSpeed(double _speed) { speed = _speed; }
		void SetGravity(double _gravity) { gravity = _gravity; }
		void SetDirection(double _direction) { direction = _direction; }
		void SetGravityDirection(double _gravity_direction) { gravity_direction = _gravity_direction; }


};

class VectorObject : public GameObject {
	private:
		std::vector<std::pair<double, double>> vertices;
	public:
		VectorObject(const std::pair<double, double>&  _origin, const std::vector <std::pair<double, double>>& _vertices);
};

class SpritedObject : public GameObject {
	private:
		std::pair<int, int> offset = {0, 0};
		std::vector<std::vector<uint32_t>> sprite;
	public:
		SpritedObject(const std::pair<double, double>& _origin, const std::string& _sprite_name);
};

extern std::vector<GameObject> objectList;

unsigned int instance_create_vector(const std::pair<double, double>& _origin, const std::vector <std::pair<double, double>>& _vertices);
unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name);
unsigned int instance_create_sprited(const std::pair<double, double>& _origin, const std::string& _sprite_name, const std::pair<int, int>& _offset);
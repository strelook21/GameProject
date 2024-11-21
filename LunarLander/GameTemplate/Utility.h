#pragma once
#include <vector>
#include <utility>
#include <string>
//struct Point {
//	double x;
//	double y;
//};

class GameObject {
	protected:
		int id;
		std::pair<double, double> origin;
	public:
		//struct Point origin;
		double speed;
		double gravity;
		double direction;
		double gravity_direction;
};

class VectorObject : public GameObject {
	private:
		std::vector<std::pair<double, double>> vertices;
		//struct Point* vertices;
	public:
		VectorObject(std::pair<double, double> _origin, std::vector <std::pair<double, double>> _vertices) {
			origin = _origin;
			vertices = _vertices;
		}
};

class SpritedObject : public GameObject {
	private:
		std::string sprite;
	public:
		SpritedObject(double _x, double _y, std::string _sprite) : sprite(_sprite) {};
};
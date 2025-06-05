#pragma once
#include <iostream>
#include <vector>
#include <string>


struct AirPort
{
	double x; // 经度
	double y; // 纬度
};

struct Task
{
	double x;
	double y;
	int id;
	int UAVIndex;
	bool conduct = false;
	int Urgency_level = 0;
	std::vector<std::string> used_uav_type = { "A", "B" };

	string name;
	double SAMPLE_Z1;


};

struct UAV
{
	double x;
	double y;
	int id;
	std::string airport;
	std::string start_time;
	time_t startTimeValue;
	Task DoingTask;
	std::string type;
	bool working = false;
	string uav_name = "";
	double start_height = 0.0;
	double end_height = 0.0;
	double cruise_height = 0.0;

	vector<vector<double>> wayPoints; // 途经点经纬坐标集合，每个vector<double>存储了途经点的经度度及高程信息
	double speed = -1;				  // 无人机的巡航速度信息

	void logInfo() {
		std::cout << "UAV [" << uav_name << "]" << endl;
		std::cout << "\tStart: " << x << " " << y << endl;
		std::cout << "\tTarget: " << DoingTask.x << " " << DoingTask.y << endl;
		std::cout << "\tStart time: " << start_time << endl;
	}

};

struct Poplation
{
	std::vector<int> gene;
	double f;
};

/*建筑物障碍-长方体*/
struct buildings
{
	double xmax;
	double xmin;
	double ymax;
	double ymin;
	double zmax;
	double zmin;
};

/*信号干扰障碍-球体*/
struct sphereSignal
{
	double x;
	double y;
	double z;
	double r;
};

/*定义三维点结构体*/
struct Point3D {
	double x;
	double y;
	double z;
	int parent_index = NULL;
	Point3D() { x = 0; y = 0; z = 0; };
	Point3D(double x, double y, double z) : x(x), y(y), z(z) {}
	// 减法运算符重载
	Point3D operator-(const Point3D& other) const {
		return Point3D(x - other.x, y - other.y, z - other.z);
	}
	// 乘法运算符重载（标量乘法）
	Point3D operator*(double scalar) const {
		return Point3D(x * scalar, y * scalar, z * scalar);
	}
	// 加法运算符重载
	Point3D operator+(const Point3D& other) const {
		return Point3D(x + other.x, y + other.y, z + other.z);
	}
};
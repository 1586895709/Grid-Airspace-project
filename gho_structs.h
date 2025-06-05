#pragma once
#include <iostream>
#include <vector>
#include <string>


struct AirPort
{
	double x; // ����
	double y; // γ��
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

	vector<vector<double>> wayPoints; // ;���㾭γ���꼯�ϣ�ÿ��vector<double>�洢��;����ľ��ȶȼ��߳���Ϣ
	double speed = -1;				  // ���˻���Ѳ���ٶ���Ϣ

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

/*�������ϰ�-������*/
struct buildings
{
	double xmax;
	double xmin;
	double ymax;
	double ymin;
	double zmax;
	double zmin;
};

/*�źŸ����ϰ�-����*/
struct sphereSignal
{
	double x;
	double y;
	double z;
	double r;
};

/*������ά��ṹ��*/
struct Point3D {
	double x;
	double y;
	double z;
	int parent_index = NULL;
	Point3D() { x = 0; y = 0; z = 0; };
	Point3D(double x, double y, double z) : x(x), y(y), z(z) {}
	// �������������
	Point3D operator-(const Point3D& other) const {
		return Point3D(x - other.x, y - other.y, z - other.z);
	}
	// �˷���������أ������˷���
	Point3D operator*(double scalar) const {
		return Point3D(x * scalar, y * scalar, z * scalar);
	}
	// �ӷ����������
	Point3D operator+(const Point3D& other) const {
		return Point3D(x + other.x, y + other.y, z + other.z);
	}
};
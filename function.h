/*
* =========================
* һЩ���õĺ�������
* ʱ�䣺2024��3��27��
* by: gho
* =========================
* Function 1:  json JSONVar = read_json_file("a.json")  // ��ȡjson�ļ�
* Function 2:  save_json_file("output.json", v1)        // ����json�ļ�
* Function 3:  print_vector(v1)                         // ��ӡvector
* Function 4:  random_int(0,10)						    // �����������
* Function 5:  random_double(0.0,10.0)				    // �������С��
* Function 6:  vector v = getUnion(v1, v2)              // ������vector����
* Function 7:  vector v = getIntersection(v1, v2)       // ������vector����
* Function 8:  vector v = getDifference(v1, v2)         // ������vector�
*/

#pragma once
#include <random>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <iostream>
#include <algorithm> 
#include "knlohmannjson.hpp"
using json_of_nlohmann = nlohmann::json;
using namespace std;

/*
* ������������ʱ�䣺
* clock_t start_time,end_time;
* start_time = clock();
* end_time = clock();
* double use_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
*/

/*json�ļ�����-��ȡjson�ļ�*/
json_of_nlohmann read_json_file(std::string filename);

/*json�ļ�����-����json�ļ�*/
template <typename T>
void save_json_file(std::string filename, const std::vector<T>& v);

/*���vector�ļ�*/
template <typename T>
void print_vector(std::vector<T>& v);

/*���������*/
int random_int(const int lb, const int ub);
double random_double(const double lb, const double ub);

/*vector���ϵĲ���*/
template <typename T>
std::vector<T> getUnion(std::vector<T> a, std::vector<T> b);
template <typename T>
std::vector<T> getIntersection(std::vector<T> a, std::vector<T> b);
template <typename T>
std::vector<T> getDifference(std::vector<T> a, std::vector<T> b);

template<typename T>
inline void save_json_file(std::string filename, const std::vector<T>& v)
{
	std::ofstream file(filename);
	json_of_nlohmann json_var;
	json_var["vector"] = v;
	file << json_var;
	file.close();
	std::cout << "JSON file saved success��" << std::endl;
};

template<typename T>
inline void print_vector(std::vector<T>& v)
{
	for (int i = 0; i < v.size(); i++)
	{
		std::cout << v[i] << " ";
	}
	std::cout << std::endl;
};

template<typename T>
inline std::vector<T> getUnion(std::vector<T>& a, std::vector<T>& b)
{
	std::vector<T> result;
	std::sort(a.begin(), a.end());
	std::sort(b.begin(), b.end());
	std::set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(result, result.begin()));
	return result;
};

template<typename T>
inline std::vector<T> getIntersection(std::vector<T>& a, std::vector<T>& b)
{
	std::vector<T> result;
	std::sort(a.begin(), a.end());
	std::sort(b.begin(), b.end());
	std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(result, result.begin()));
	return result;
};

template<typename T>
inline std::vector<T> getDifference(std::vector<T> a, std::vector<T> b)
{
	std::vector<T> result;
	std::sort(a.begin(), a.end());
	std::sort(b.begin(), b.end());
	std::set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(result, result.begin()));
	return result;
};

time_t gho_timeString2Sec(std::string timeString);

std::string gho_timeSec2String(time_t timeStamp);

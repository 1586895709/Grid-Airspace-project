/*
* =========================
* 一些常用的函数工具
* 时间：2024年3月27日
* by: gho
* =========================
* Function 1:  json JSONVar = read_json_file("a.json")  // 读取json文件
* Function 2:  save_json_file("output.json", v1)        // 保存json文件
* Function 3:  print_vector(v1)                         // 打印vector
* Function 4:  random_int(0,10)						    // 产生随机整数
* Function 5:  random_double(0.0,10.0)				    // 产生随机小数
* Function 6:  vector v = getUnion(v1, v2)              // 求两个vector并集
* Function 7:  vector v = getIntersection(v1, v2)       // 求两个vector交集
* Function 8:  vector v = getDifference(v1, v2)         // 求两个vector差集
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
* 计算代码的运行时间：
* clock_t start_time,end_time;
* start_time = clock();
* end_time = clock();
* double use_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
*/

/*json文件操作-读取json文件*/
json_of_nlohmann read_json_file(std::string filename);

/*json文件操作-保存json文件*/
template <typename T>
void save_json_file(std::string filename, const std::vector<T>& v);

/*输出vector文件*/
template <typename T>
void print_vector(std::vector<T>& v);

/*产生随机数*/
int random_int(const int lb, const int ub);
double random_double(const double lb, const double ub);

/*vector集合的操作*/
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
	std::cout << "JSON file saved success！" << std::endl;
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

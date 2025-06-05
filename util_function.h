#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include "knlohmannjson.hpp"
#include "Eigen/Dense"

// LOG日志方法宏定义
#define LOG_DEFAULT_WIDTH 16
#define LOGI std::cout << __FUNCTION__ << "(" << __LINE__ << "): " << std::setw(LOG_DEFAULT_WIDTH)

using std::vector;
using std::endl;

typedef Eigen::Vector3i CoorIJK;
typedef Eigen::Vector3d CoorXYZ;
typedef unsigned long long uint64;
typedef unsigned long uint32;
typedef std::vector<time_t> TimeInterval;

// 定义 96 位无符号整数类型，明确指定命名空间
typedef boost::multiprecision::number<
	boost::multiprecision::cpp_int_backend<
	96, 96,
	boost::multiprecision::unsigned_magnitude,
	boost::multiprecision::unchecked,
	void
	>
> uint96;


typedef uint64 GridCode;
//typedef boost::multiprecision::int128_t GridCode;

// 自定义CoorXYZ比较函数对象
struct CoorXYZComparator {
	bool operator()(const CoorXYZ& a, const CoorXYZ& b) const {
		// 按字典序比较向量元素
		for (int i = 0; i < 3; ++i) {
			if (a[i] < b[i]) return true;
			if (a[i] > b[i]) return false;
		}
		return false;
	}
};

// 用于存储网格信息的结构体
struct GridInfor
{
	GridCode code; // 存储对应的网格编码

	bool isObs = false; // 是否为障碍物
	std::set<time_t> occupy_set; // 占用信息（用于存储动态障碍物的相关信息
	std::map<std::string, std::set<time_t>> occupyIdTimeSet; // 占用信息（用于存储动态障碍物的相关信息
	std::set<std::vector<time_t>> occupy_interval_set; // 占用时间区间信息
	std::map<std::string, std::set<std::vector<time_t>>> occupyIntervalSetIdMap; // 占用时间区间信息

	double velFactor = 1; // 速度影响因子（0~1） (v = v0 * velFactor)

	std::map<TimeInterval, double> timeVelFactorMap; // 速度影响因子时变映射图
};
typedef std::unordered_map<GridCode, GridInfor> CodeMap; // 构建 编码--网格属性 的键值对集合
typedef std::shared_ptr<std::unordered_map<GridCode, GridInfor>> CodeMapPtr; // 构建 编码--网格属性 的键值对集合

const double RADIUS = 6371393.0; // 地球半径（单位：米）

CoorXYZ latLonToMercator(CoorXYZ inputGeopt);

CoorXYZ mercatorToLatLon(CoorXYZ inputMercatorPt);

/**
 * @brief 计算p1->p2->p3过程中的折角
 * @param p1 起始点
 * @param p2 中间点
 * @param p3 终点
 * @return 以度为单位的夹角
 */
double calAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3);

/**
 * @brief 计算p1->p2->p3过程中水平方向的折角
 * @param p1 起始点
 * @param p2 中间点
 * @param p3 终点
 * @return 以度为单位的夹角
 */
double calHorizonAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3);

/**
 * @brief 数据去重
 * @tparam T 数据类型
 * @param inputCodes 输入数据集合
 */
template <typename T>
void codeUnique(std::vector<T>& inputCodes) {
	sort(inputCodes.begin(), inputCodes.end());
	auto iter = unique(inputCodes.begin(), inputCodes.end());
	inputCodes.erase(iter, inputCodes.end());
}

template <typename T>
T calAverageValue(std::vector<T>& inputValues) {
	T sum = 0.0;
	for (int i = 0; i < inputValues.size(); i++) {
		sum += inputValues[i];
	}

	return sum / inputValues.size();
}

bool judgeInLine(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3);
bool judgeInLine(Eigen::Vector3i p1, Eigen::Vector3i p2, Eigen::Vector3i p3);

void stringSplit(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters = " ");

void readJsonFile(std::string jsonFileName, nlohmann::json& jsonValue);

time_t timeString2Sec(std::string timeString);

std::string timeSec2String(time_t timeStamp);

/**
 * @brief 判断两个时间区间是否相交
 * @param interval1 区间1信息
 * @param interval2 区间2信息
 * @param safeInterval 安全间隔
 * @return
 */
bool isIntervalIntersection(TimeInterval interval1, TimeInterval interval2, double safeInterval = 0.0);

/**
 * @brief 判断输入整数二进制表示下末尾连续1的个数
 * @param n 输入整数
 * @return
 */
template <typename T>
int countConsecutiveOnes(T n)
{
	int count = 0;
	while ((n & 1) == 1) {
		count++;
		n = n >> 1;
	}
	return count;
}

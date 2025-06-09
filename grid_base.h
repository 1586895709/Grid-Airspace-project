#pragma once
#ifndef GRID_BASE_H
#define GRID_BASE_H

#include "Eigen/Dense"
#include <iostream>
#include <iomanip>
#include <map>
#include "util_function.h"

const GridCode L[6] = { 0xFFFFFFFF00000000, 0xFFFF0000, 0xFF00, 0xF0, 0xC, 0x2 };
const GridCode R[6] = { 0x00000000FFFFFFFF, 0x0000FFFF, 0x00FF, 0x0F, 0x3, 0x1 };
const int NN[6] = { 32, 16, 8, 4, 2, 1 };

// 坐标类型
enum class CoorType {
	Default,
	CartesianCS_3D,			// 笛卡尔坐标系（三维）
	GeoElevationCS			// 地理坐标系（三维）
};

// 网格层级
struct Level {
	int x;
	int y;
	int z;

	// 构造函数，实现从 int 到 Level 的默认转换
	Level(int value = 0) : x(value), y(value), z(value) {
	}

	// 构造函数，允许用户自定义层级信息
	Level(int xVal, int yVal, int zVal) : x(xVal), y(yVal), z(zVal) {
	}

	// 重载输出运算符，方便打印结构体信息
	friend std::ostream& operator<<(std::ostream& os, const Level& level) {
		os << "Level(x: " << level.x << ", y: " << level.y << ", z: " << level.z << ")";
		return os;
	}

	// 定义 operator<
	bool operator<(const Level& other) const {
		if (this->x != other.x) return this->x < other.x;
		else if (this->y != other.y) return this->y < other.y;
		else return this->z < other.z;
	}

	// 定义 operator>
	bool operator>(const Level& other) const {
		if (this->x != other.x) return this->x > other.x;
		else if (this->y != other.y) return this->y > other.y;
		else return this->z > other.z;
	}

	// 定义 operator>=
	bool operator>=(const Level& other) const {
		if (this->x != other.x) return this->x >= other.x;
		else if (this->y != other.y) return this->y >= other.y;
		else return this->z >= other.z;
	}
};

class GridBase;
typedef std::shared_ptr<GridBase> GridBasePtr; //std::shared_ptr<GridBase>的别名GridBasePtr

class GridBase {
protected:
	CoorXYZ minBorder;
	CoorXYZ maxBorder;
	CoorType coorType;
	std::map<Level, CoorXYZ> gridLenMap;

public:

	/**
	 * @brief 获取网格化类的坐标类型
	 * @return
	 */
	CoorType getCoorType() {
		return this->coorType;
	}

	void setCoorType(CoorType inputCoorType) {
		this->coorType = inputCoorType;
	}

	/**
	 * @brief 初始化网格框架边界范围
	 * @param minBorder_ 最小范围边界
	 * @param maxBorder_ 最大范围边界
	 * @param coorType_ 边界范围类型
	 */
	void initBorderRange(CoorXYZ& minBorder_, CoorXYZ& maxBorder_, CoorType coorType_ = CoorType::Default);

	/**
	 * @brief 判断点是否在范围内
	 * @param inputPt 输入点
	 * @return 如果点在范围内，返回false，如果在范围外， 返回true
	 */
	bool isOutBorder(const CoorXYZ& inputPt);

	/**
	 * @brief 计算给定层级下网格坐标的范围
	 * @param level 坐标层级
	 * @param minIJK 网格坐标最小值
	 * @param maxIJK 网格坐标最大值
	 */
	void getCoorRange(Level level, CoorIJK& minIJK, CoorIJK& maxIJK);

	/**
	 * @brief 网格坐标转网格编码（批量转换）
	 * @param inputIJKs 输入网格坐标集合
	 * @param level 网格层级
	 * @return
	 */
	std::vector<GridCode> toCodesFromIJKs(std::vector<CoorIJK>& inputIJKs, Level level);

	/**
	 * @brief 计算两个点之间的欧式距离，如果坐标系为三维之间坐标系，则直接计算，如果为地理坐标系，则转换为墨卡托坐标系后计算
	 * @param pt1 输入坐标点1
	 * @param pt2 输入坐标点2
	 * @return 坐标点之间的距离
	 */
	double calDis(CoorXYZ pt1, CoorXYZ pt2);


public:

	virtual CoorIJK toIJKFromXYZ(const CoorXYZ& xyz, Level level) = 0;
	virtual CoorXYZ toXYZFromIJK(const CoorIJK& ijk, Level level) = 0;

	virtual GridCode toCodeFromIJK(const CoorIJK& ijk, Level level) = 0;
	virtual CoorIJK toIJKFromCode(GridCode code) = 0;

	virtual GridCode toCodeFromXYZ(const CoorXYZ& xyz, Level level) = 0;
	virtual CoorXYZ toXYZFromCode(GridCode code) = 0;

	virtual std::vector<CoorXYZ> calCornerPtsFromCode(GridCode code) = 0;

	virtual Level getLevelFromCode(GridCode code) = 0;

	virtual GridCode findDadCode(GridCode inputCode, int inputFatherLevel) = 0;

	virtual vector<GridCode> getChildCodes(GridCode inputCode, Level inputSonLevel) = 0;

	virtual std::vector<CoorIJK> calNeighbors(CoorIJK inputCoor, int neighborDis = 1, bool isManhattanDis = false) = 0;

	virtual void calOccupiedInfo(
		CoorXYZ origin, CoorXYZ target, double startTime, double endTime,
		Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap = false) = 0;

	virtual void getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend = false) = 0;

	virtual void getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange) = 0;

	/**
	 * @brief 获取网格边长
	 * @param level 输入网格层级
	 * @return 各维度网格边长
	 */
	virtual CoorXYZ getGridLen(Level level) = 0;

protected:
	int getMidN(GridCode& midCode, int level);


};

#endif //GRID_BASE_H
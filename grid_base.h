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

// ��������
enum class CoorType {
	Default,
	CartesianCS_3D,			// �ѿ�������ϵ����ά��
	GeoElevationCS			// ��������ϵ����ά��
};

// ����㼶
struct Level {
	int x;
	int y;
	int z;

	// ���캯����ʵ�ִ� int �� Level ��Ĭ��ת��
	Level(int value = 0) : x(value), y(value), z(value) {
	}

	// ���캯���������û��Զ���㼶��Ϣ
	Level(int xVal, int yVal, int zVal) : x(xVal), y(yVal), z(zVal) {
	}

	// �������������������ӡ�ṹ����Ϣ
	friend std::ostream& operator<<(std::ostream& os, const Level& level) {
		os << "Level(x: " << level.x << ", y: " << level.y << ", z: " << level.z << ")";
		return os;
	}

	// ���� operator<
	bool operator<(const Level& other) const {
		if (this->x != other.x) return this->x < other.x;
		else if (this->y != other.y) return this->y < other.y;
		else return this->z < other.z;
	}

	// ���� operator>
	bool operator>(const Level& other) const {
		if (this->x != other.x) return this->x > other.x;
		else if (this->y != other.y) return this->y > other.y;
		else return this->z > other.z;
	}

	// ���� operator>=
	bool operator>=(const Level& other) const {
		if (this->x != other.x) return this->x >= other.x;
		else if (this->y != other.y) return this->y >= other.y;
		else return this->z >= other.z;
	}
};

class GridBase;
typedef std::shared_ptr<GridBase> GridBasePtr; //std::shared_ptr<GridBase>�ı���GridBasePtr

class GridBase {
protected:
	CoorXYZ minBorder;
	CoorXYZ maxBorder;
	CoorType coorType;
	std::map<Level, CoorXYZ> gridLenMap;

public:

	/**
	 * @brief ��ȡ���������������
	 * @return
	 */
	CoorType getCoorType() {
		return this->coorType;
	}

	void setCoorType(CoorType inputCoorType) {
		this->coorType = inputCoorType;
	}

	/**
	 * @brief ��ʼ�������ܱ߽緶Χ
	 * @param minBorder_ ��С��Χ�߽�
	 * @param maxBorder_ ���Χ�߽�
	 * @param coorType_ �߽緶Χ����
	 */
	void initBorderRange(CoorXYZ& minBorder_, CoorXYZ& maxBorder_, CoorType coorType_ = CoorType::Default);

	/**
	 * @brief �жϵ��Ƿ��ڷ�Χ��
	 * @param inputPt �����
	 * @return ������ڷ�Χ�ڣ�����false������ڷ�Χ�⣬ ����true
	 */
	bool isOutBorder(const CoorXYZ& inputPt);

	/**
	 * @brief ��������㼶����������ķ�Χ
	 * @param level ����㼶
	 * @param minIJK ����������Сֵ
	 * @param maxIJK �����������ֵ
	 */
	void getCoorRange(Level level, CoorIJK& minIJK, CoorIJK& maxIJK);

	/**
	 * @brief ��������ת������루����ת����
	 * @param inputIJKs �����������꼯��
	 * @param level ����㼶
	 * @return
	 */
	std::vector<GridCode> toCodesFromIJKs(std::vector<CoorIJK>& inputIJKs, Level level);

	/**
	 * @brief ����������֮���ŷʽ���룬�������ϵΪ��ά֮������ϵ����ֱ�Ӽ��㣬���Ϊ��������ϵ����ת��Ϊī��������ϵ�����
	 * @param pt1 ���������1
	 * @param pt2 ���������2
	 * @return �����֮��ľ���
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
	 * @brief ��ȡ����߳�
	 * @param level ��������㼶
	 * @return ��ά������߳�
	 */
	virtual CoorXYZ getGridLen(Level level) = 0;

protected:
	int getMidN(GridCode& midCode, int level);


};

#endif //GRID_BASE_H
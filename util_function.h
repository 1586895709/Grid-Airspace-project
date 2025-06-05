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

// LOG��־�����궨��
#define LOG_DEFAULT_WIDTH 16
#define LOGI std::cout << __FUNCTION__ << "(" << __LINE__ << "): " << std::setw(LOG_DEFAULT_WIDTH)

using std::vector;
using std::endl;

typedef Eigen::Vector3i CoorIJK;
typedef Eigen::Vector3d CoorXYZ;
typedef unsigned long long uint64;
typedef unsigned long uint32;
typedef std::vector<time_t> TimeInterval;

// ���� 96 λ�޷����������ͣ���ȷָ�������ռ�
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

// �Զ���CoorXYZ�ȽϺ�������
struct CoorXYZComparator {
	bool operator()(const CoorXYZ& a, const CoorXYZ& b) const {
		// ���ֵ���Ƚ�����Ԫ��
		for (int i = 0; i < 3; ++i) {
			if (a[i] < b[i]) return true;
			if (a[i] > b[i]) return false;
		}
		return false;
	}
};

// ���ڴ洢������Ϣ�Ľṹ��
struct GridInfor
{
	GridCode code; // �洢��Ӧ���������

	bool isObs = false; // �Ƿ�Ϊ�ϰ���
	std::set<time_t> occupy_set; // ռ����Ϣ�����ڴ洢��̬�ϰ���������Ϣ
	std::map<std::string, std::set<time_t>> occupyIdTimeSet; // ռ����Ϣ�����ڴ洢��̬�ϰ���������Ϣ
	std::set<std::vector<time_t>> occupy_interval_set; // ռ��ʱ��������Ϣ
	std::map<std::string, std::set<std::vector<time_t>>> occupyIntervalSetIdMap; // ռ��ʱ��������Ϣ

	double velFactor = 1; // �ٶ�Ӱ�����ӣ�0~1�� (v = v0 * velFactor)

	std::map<TimeInterval, double> timeVelFactorMap; // �ٶ�Ӱ������ʱ��ӳ��ͼ
};
typedef std::unordered_map<GridCode, GridInfor> CodeMap; // ���� ����--�������� �ļ�ֵ�Լ���
typedef std::shared_ptr<std::unordered_map<GridCode, GridInfor>> CodeMapPtr; // ���� ����--�������� �ļ�ֵ�Լ���

const double RADIUS = 6371393.0; // ����뾶����λ���ף�

CoorXYZ latLonToMercator(CoorXYZ inputGeopt);

CoorXYZ mercatorToLatLon(CoorXYZ inputMercatorPt);

/**
 * @brief ����p1->p2->p3�����е��۽�
 * @param p1 ��ʼ��
 * @param p2 �м��
 * @param p3 �յ�
 * @return �Զ�Ϊ��λ�ļн�
 */
double calAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3);

/**
 * @brief ����p1->p2->p3������ˮƽ������۽�
 * @param p1 ��ʼ��
 * @param p2 �м��
 * @param p3 �յ�
 * @return �Զ�Ϊ��λ�ļн�
 */
double calHorizonAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3);

/**
 * @brief ����ȥ��
 * @tparam T ��������
 * @param inputCodes �������ݼ���
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
 * @brief �ж�����ʱ�������Ƿ��ཻ
 * @param interval1 ����1��Ϣ
 * @param interval2 ����2��Ϣ
 * @param safeInterval ��ȫ���
 * @return
 */
bool isIntervalIntersection(TimeInterval interval1, TimeInterval interval2, double safeInterval = 0.0);

/**
 * @brief �ж��������������Ʊ�ʾ��ĩβ����1�ĸ���
 * @param n ��������
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

#pragma once

#include <vector>
#include <map>
#include "grid_base.h"

class GRID_3D : public GridBase
{
public:
	void initBorderRange(CoorXYZ centerPt, CoorXYZ gridLen, int gridLayer = 21);

	// �ӿں���ʵ��
public:
	GRID_3D(CoorXYZ minBorder_, CoorXYZ maxBorder_, CoorType coorType_ = CoorType::CartesianCS_3D);
	GRID_3D() {};
	~GRID_3D();

	CoorIJK toIJKFromXYZ(const CoorXYZ& xyz, Level level) override;
	CoorXYZ toXYZFromIJK(const CoorIJK& ijk, Level level) override;

	GridCode toCodeFromIJK(const CoorIJK& ijk, Level level) override;
	CoorIJK toIJKFromCode(GridCode code) override;

	GridCode toCodeFromXYZ(const CoorXYZ& xyz, Level level) override;
	CoorXYZ toXYZFromCode(GridCode code) override;

	Level getLevelFromCode(GridCode code) override;

	/**
	 * @brief ����������������Ӧ����ı߽���Ϣ
	 * @param code �������
	 * @return �߽�ǵ㼯��
	 * (Xmin, Ymin, Zmin) (Xmin, Ymin, Zmax) (Xmin, Ymax, Zmin) (Xmin, Ymax, Zmax)
	 * (Xmax, Ymin, Zmin) (Xmax, Ymin, Zmax) (Xmax, Ymax, Zmin) (Xmax, Ymax, Zmax)
	 */
	std::vector<CoorXYZ> calCornerPtsFromCode(GridCode code) override;

	GridCode findDadCode(GridCode inputCode, int inputFatherLevel = -1) override;

	vector<GridCode> getChildCodes(GridCode inputCode, Level inputSonLevel) override;

	std::vector<CoorIJK> calNeighbors(CoorIJK inputCoor, int neighborDis = 1, bool isManhattanDis = false) override;

	/**
	* @brief ����ֱ�ߵ��˶���Ϣ����㡢�յ㡢��ֹʱ�䣩����ʱ������
	* @param origin �������
	* @param target �յ�����
	* @param startTime ��ʼʱ��
	* @param endTime ����ʱ��
	* @param gridLayer ����㼶��Ӱ��ռ�����
	* @param outputMap ���ڴ��ʱ�����񻯽����Ϣ
	* @param isClearMap �Ƿ��ʼ��outputMap, Ĭ�ϲ���ʼ��
	 */
	void calOccupiedInfo(
		CoorXYZ origin, CoorXYZ target, double startTime, double endTime,
		Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap = false) override;

	/**
	 * @brief ��Ԫ�ص����񻯣��õ��������ļ���
	 * @param p1 �˵�1������
	 * @param p2 �˵�2������
	 * @param layer ���񻯲㼶
	 * @param codes ������뼯��
	 * @param isAppend �Ƿ���������뼯������ӱ��룬Ĭ��Ϊ�񣬼��Ƚ����뼯�����
	 */
	void getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend = false) override;

	/**
	 * @brief ������������Ӧ������Χ
	 * @param inputCode �������
	 * @param minRange ��С��Χ
	 * @param maxRange ���Χ
	 */
	void getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange) override;

	/**
	 * @brief ��ȡ����߳�
	 * @param level ��������㼶
	 * @return ��ά������߳�
	 */
	CoorXYZ getGridLen(Level level) override;

	// ��Ϣ���
public:

	/**
	 * @brief ��ȡ�߽緶Χ
	 * @param borderMin
	 * @param borderMax
	 */
	void getBorderRange(CoorXYZ& borderMin, CoorXYZ& borderMax);

	/**
	 * @brief ��ȡ��������
	 * @return
	 */
	CoorType getCoorType() { return coorType; }

	// �������񻯺���
public:
	/**
	 * @brief ���㵥���������ϵ��ཻ����
	 * @param origin
	 * @param direction
	 * @param vmin
	 * @param vmax
	 * @param tmin
	 * @param tmax
	 */
	void calculateIntersection(double origin, double direction, double vmin, double vmax, double& tmin, double& tmax);

	/**
	 * @brief ����������������Χ�У�AABB���ཻ�ĺ��������ص��������߷���Ľ������tmin
	 * @param origin ���ߵ����
	 * @param direction ���ߵķ�������
	 * @param vmin ��Χ�е���С��������
	 * @param vmax ��Χ�е���󶥵�����
	 * @return
	 */
	double rayBoxIntersection(CoorXYZ origin, CoorXYZ direction, CoorXYZ vmin, CoorXYZ vmax);

	/**
	 * @brief
	 * @param origin
	 * @param end
	 * @param minBound
	 * @param maxBound
	 * @param nx
	 * @param ny
	 * @param nz
	 * @param ijk_list
	 * @return
	 */
	void amanatidesWooAlgorithm(
		CoorXYZ origin, CoorXYZ end,
		CoorXYZ minBound, CoorXYZ maxBound,
		int nx, int ny, int nz, std::vector<CoorIJK>& ijk_list);
};


#pragma once

#include <vector>
#include <map>
#include "grid_base.h"

class GRID_3D : public GridBase
{
public:
	void initBorderRange(CoorXYZ centerPt, CoorXYZ gridLen, int gridLayer = 21);

	// 接口函数实现
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
	 * @brief 计算所给网格编码对应网格的边界信息
	 * @param code 输入编码
	 * @return 边界角点集合
	 * (Xmin, Ymin, Zmin) (Xmin, Ymin, Zmax) (Xmin, Ymax, Zmin) (Xmin, Ymax, Zmax)
	 * (Xmax, Ymin, Zmin) (Xmax, Ymin, Zmax) (Xmax, Ymax, Zmin) (Xmax, Ymax, Zmax)
	 */
	std::vector<CoorXYZ> calCornerPtsFromCode(GridCode code) override;

	GridCode findDadCode(GridCode inputCode, int inputFatherLevel = -1) override;

	vector<GridCode> getChildCodes(GridCode inputCode, Level inputSonLevel) override;

	std::vector<CoorIJK> calNeighbors(CoorIJK inputCoor, int neighborDis = 1, bool isManhattanDis = false) override;

	/**
	* @brief 根据直线的运动信息（起点、终点、起止时间）对其时空网格化
	* @param origin 起点坐标
	* @param target 终点坐标
	* @param startTime 起始时间
	* @param endTime 到达时间
	* @param gridLayer 网格层级，影响空间网格化
	* @param outputMap 用于存放时空网格化结果信息
	* @param isClearMap 是否初始化outputMap, 默认不初始化
	 */
	void calOccupiedInfo(
		CoorXYZ origin, CoorXYZ target, double startTime, double endTime,
		Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap = false) override;

	/**
	 * @brief 线元素的网格化，得到网格编码的集合
	 * @param p1 端点1的坐标
	 * @param p2 端点2的坐标
	 * @param layer 网格化层级
	 * @param codes 网格编码集合
	 * @param isAppend 是否在网格编码集合上添加编码，默认为否，即先将编码集合清空
	 */
	void getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend = false) override;

	/**
	 * @brief 计算输入编码对应的网格范围
	 * @param inputCode 输入编码
	 * @param minRange 最小范围
	 * @param maxRange 最大范围
	 */
	void getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange) override;

	/**
	 * @brief 获取网格边长
	 * @param level 输入网格层级
	 * @return 各维度网格边长
	 */
	CoorXYZ getGridLen(Level level) override;

	// 信息输出
public:

	/**
	 * @brief 获取边界范围
	 * @param borderMin
	 * @param borderMax
	 */
	void getBorderRange(CoorXYZ& borderMin, CoorXYZ& borderMax);

	/**
	 * @brief 获取坐标类型
	 * @return
	 */
	CoorType getCoorType() { return coorType; }

	// 对象网格化函数
public:
	/**
	 * @brief 计算单个坐标轴上的相交距离
	 * @param origin
	 * @param direction
	 * @param vmin
	 * @param vmax
	 * @param tmin
	 * @param tmax
	 */
	void calculateIntersection(double origin, double direction, double vmin, double vmax, double& tmin, double& tmax);

	/**
	 * @brief 计算射线与轴对齐包围盒（AABB）相交的函数，返回的是沿射线方向的交点参数tmin
	 * @param origin 射线的起点
	 * @param direction 射线的方向向量
	 * @param vmin 包围盒的最小顶点坐标
	 * @param vmax 包围盒的最大顶点坐标
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


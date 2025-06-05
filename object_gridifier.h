// 点线面体对象网格化类
#pragma once
#include "util_function.h"
#include "grid_base.h"
#include "vtk_visualization.h"
#include "geojson_reader.h"

using std::vector;

class Object_Gridifier
{
protected:
	GridBasePtr pGridObjectPtr;

private:
	// 水平反向填充函数
	vector<CoorIJK> reverseScanLineSeedFill_XY(vector<CoorIJK>& inputEdgeCoors);

	int fillLineRight(int seedIdx, vector<bool>& sceneData);
	int fillLineLeft(int seedIdx, vector<bool>& sceneData);

	void searchLineNewSeed(
		vector<CoorIJK>& stk, vector<bool>& sceneData,
		CoorIJK sceneRangeIJK, CoorIJK sceneOriginIJK, int xLeft, int xRight, int J);

	// 网格化结果：编码集合
public:
	// 构造函数，接受一个 GridBase 的智能指针
	Object_Gridifier(GridBasePtr gridBase) : pGridObjectPtr(gridBase) {}

	// 将单个点离散化为网格单元
	GridCode discretizePoint(CoorXYZ point, Level level);
	std::vector<GridCode> discretizePoint(std::vector<CoorXYZ>& points, Level level);

	// 将一条线段离散化为一系列网格单元
	std::vector<GridCode> discretizeLine(CoorXYZ start, CoorXYZ end, Level level);

	// 将一个平面多边形表面离散化为网格单元（XY平面上的二维多边形）
	std::vector<GridCode> discretizeSurface_XY(std::vector<CoorXYZ>& vertices, Level level);

	/**
	 * @brief  将一个三维三角面进行网格化，得到网格编码集合
	 * @param inputPts 输入坐标点集合（元素个数应为3）
	 * @param level 网格化层级
	 * @param uniqueFlag 是否对编码进行去重
	 * @return
	 */
	std::vector<GridCode> discretizeTriangularSurface(vector<CoorXYZ>& inputPts, Level level, bool uniqueFlag = true);

	/**
	 * @brief 将一个三维多边形平面进行网格化，得到网格编码集合
	 * @param inputPolygonPts 三维多边形平面（凸多边形）
	 * @param level 网格层级
	 * @param uniqueFlag 是否对编码进行去重
	 * @return
	 */
	std::vector<GridCode> discretizePolygon(vector<CoorXYZ>& inputPolygonPts, Level level, bool uniqueFlag = true);

	/**
	 * @brief 将一个多面集合离散化为网格单元
	 * @param faces 多面集合
	 * @param level 网格层级
	 * @param uniqueFlag 是否对编码进行去重
	 * @return
	 */
	std::vector<GridCode> discretizeVolume(std::vector<std::vector<CoorXYZ>>& faces, Level level, bool uniqueFlag = true);

	/**
	 * @brief 对柱体进行网格化
	 * @param bottomPts 地面坐标集合
	 * @param height 柱体高度
	 * @param level 网格层级
	 * @return
	 */
	std::vector<GridCode> discretizeCylinder(std::vector<CoorXYZ> bottomPts, double height, Level level);

	// 网格化结果：网格坐标集合
public:
	// 将一个三维三角面进行网格化，得到网格坐标集合
	// 网格坐标集合中可能存在重复元素
	std::vector<CoorIJK> discretizeTriangularSurfaceToCoors(vector<CoorXYZ> inputPts, Level level);

};

class CylinderObjectGeojsonGridifier : Object_Gridifier, GeoJSON_NS::GeoJSON_Reader {
private:
	// 对象Id标识
	vector<std::string> objectId;
	// ====== 基础几何信息 ======
	vector<vector<CoorXYZ>> bottomBorderPts; // 底面边界点序列
	vector<double> minHeight;
	vector<double> maxHeight;

	vector<time_t> beginTime;
	vector<time_t> endTime;

	void parseJsonValue(GeoJSON_NS::json& inputFeature);
public:
	CylinderObjectGeojsonGridifier(GridBasePtr gridBase) : Object_Gridifier(gridBase) {}

	void readAndParseGeojson(std::string filename);

	vector<GridCode> gridifierObject(Level level);

	map<TimeInterval, vector<GridCode>> gridifierObject_SpatioTemporal(Level level);
	map<string, map<TimeInterval, vector<GridCode>>> gridifierObject_SpatioTemporalWithId(Level level);
};
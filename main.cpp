#include <iostream>
#include <random>
#include <iomanip>
#include <variant>
#include "geojson_reader.h"
#include "grid_3d.h"
#include "object_gridifier.h"
#include "vtk_visualization.h"
#include "PlanningMap.h"
#include "timer.h"

using namespace std;

#if 1
int main()
{
	string fileName = "../MapData/negaStringxiaojie.geojson";
	string positiveAirspaceFileName = "../MapData/air_corridor.geojson";

	//// 构建网格框架对象
	CoorXYZ minBorder(0, 0, 0);
	CoorXYZ maxBorder(180, 90, 1e7);
	GridBasePtr gridObjectPtr = std::make_shared<GRID_3D>(
		minBorder, maxBorder, CoorType::GeoElevationCS);

	Level mapLevel = 12;

	CoorXYZ mapMinBorder(113, 21, 0);
	CoorXYZ mapMaxBorder(123, 29, 12500);

	// 构建规划地图对象
	PlanningMapPtr planningMapPtr = std::make_shared<PlanningMap>(
		gridObjectPtr, mapMinBorder, mapMaxBorder, mapLevel);
	planningMapPtr->setMapType(PlanningMapType::BinaryMap);
	//planningMapPtr->setMapType(PlanningMapType::NonUniformMap);
	{
		// 构建柱体类网格化对象
		CylinderObjectGeojsonGridifier tempCylinderObject = CylinderObjectGeojsonGridifier(gridObjectPtr);
		tempCylinderObject.readAndParseGeojson(fileName); // 读取geosjon数据

		// 数据网格化
		map<TimeInterval, vector<GridCode>> tempTimeSpaceCodes
			= tempCylinderObject.gridifierObject_SpatioTemporal(mapLevel);
		//planningMapPtr->updateMapWithDynamicObs(tempTimeSpaceCodes);
		planningMapPtr->updateMapWithDynamicObsWithId(tempTimeSpaceCodes, "test");






		map<string, map<TimeInterval, vector<GridCode>>> tempTimeSpaceCodesWithId
			= tempCylinderObject.gridifierObject_SpatioTemporalWithId(mapLevel);

		for (auto& tempPair : tempTimeSpaceCodesWithId) {
			planningMapPtr->updateMapWithDynamicObsWithId(tempPair.second, tempPair.first);
		}

		// 数据网格化（仅得到编码）
		vector<GridCode> tempCodes123 = tempCylinderObject.gridifierObject(mapLevel);
		planningMapPtr->updateMapWithStaticObs(tempCodes123);
		VTK_Visualization::outputVTKformatXYZ(tempCodes123, gridObjectPtr, "../Result/sceneCodes.vtk", false);
	}

	{
		// 正向空域网格化构建

		CylinderObjectGeojsonGridifier tempCylinderObject = CylinderObjectGeojsonGridifier(gridObjectPtr);
		tempCylinderObject.readAndParseGeojson(positiveAirspaceFileName); // 读取geosjon数据

		// 数据网格化
		map<TimeInterval, vector<GridCode>> tempTimeSpaceCodes
			= tempCylinderObject.gridifierObject_SpatioTemporal(mapLevel);

		//planningMapPtr->updatePositiveArea(tempTimeSpaceCodes, 10000);


		// 数据网格化（仅得到编码）
		vector<GridCode> tempCodes123 = tempCylinderObject.gridifierObject(mapLevel);
		VTK_Visualization::outputVTKformatXYZ(tempCodes123, gridObjectPtr, "../Result/corridorCodes.vtk", false);
	}
}
#endif
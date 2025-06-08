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

	//// ���������ܶ���
	CoorXYZ minBorder(0, 0, 0);
	CoorXYZ maxBorder(180, 90, 1e7);
	GridBasePtr gridObjectPtr = std::make_shared<GRID_3D>(
		minBorder, maxBorder, CoorType::GeoElevationCS);

	Level mapLevel = 12;

	CoorXYZ mapMinBorder(113, 21, 0);
	CoorXYZ mapMaxBorder(123, 29, 12500);

	// �����滮��ͼ����
	PlanningMapPtr planningMapPtr = std::make_shared<PlanningMap>(
		gridObjectPtr, mapMinBorder, mapMaxBorder, mapLevel);
	planningMapPtr->setMapType(PlanningMapType::BinaryMap);
	//planningMapPtr->setMapType(PlanningMapType::NonUniformMap);
	{
		// �������������񻯶���
		CylinderObjectGeojsonGridifier tempCylinderObject = CylinderObjectGeojsonGridifier(gridObjectPtr);
		tempCylinderObject.readAndParseGeojson(fileName); // ��ȡgeosjon����

		// ��������
		map<TimeInterval, vector<GridCode>> tempTimeSpaceCodes
			= tempCylinderObject.gridifierObject_SpatioTemporal(mapLevel);
		//planningMapPtr->updateMapWithDynamicObs(tempTimeSpaceCodes);
		planningMapPtr->updateMapWithDynamicObsWithId(tempTimeSpaceCodes, "test");






		map<string, map<TimeInterval, vector<GridCode>>> tempTimeSpaceCodesWithId
			= tempCylinderObject.gridifierObject_SpatioTemporalWithId(mapLevel);

		for (auto& tempPair : tempTimeSpaceCodesWithId) {
			planningMapPtr->updateMapWithDynamicObsWithId(tempPair.second, tempPair.first);
		}

		// �������񻯣����õ����룩
		vector<GridCode> tempCodes123 = tempCylinderObject.gridifierObject(mapLevel);
		planningMapPtr->updateMapWithStaticObs(tempCodes123);
		VTK_Visualization::outputVTKformatXYZ(tempCodes123, gridObjectPtr, "../Result/sceneCodes.vtk", false);
	}

	{
		// ����������񻯹���

		CylinderObjectGeojsonGridifier tempCylinderObject = CylinderObjectGeojsonGridifier(gridObjectPtr);
		tempCylinderObject.readAndParseGeojson(positiveAirspaceFileName); // ��ȡgeosjon����

		// ��������
		map<TimeInterval, vector<GridCode>> tempTimeSpaceCodes
			= tempCylinderObject.gridifierObject_SpatioTemporal(mapLevel);

		//planningMapPtr->updatePositiveArea(tempTimeSpaceCodes, 10000);


		// �������񻯣����õ����룩
		vector<GridCode> tempCodes123 = tempCylinderObject.gridifierObject(mapLevel);
		VTK_Visualization::outputVTKformatXYZ(tempCodes123, gridObjectPtr, "../Result/corridorCodes.vtk", false);
	}
}
#endif
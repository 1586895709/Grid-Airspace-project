// �����������������
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
	// ˮƽ������亯��
	vector<CoorIJK> reverseScanLineSeedFill_XY(vector<CoorIJK>& inputEdgeCoors);

	int fillLineRight(int seedIdx, vector<bool>& sceneData);
	int fillLineLeft(int seedIdx, vector<bool>& sceneData);

	void searchLineNewSeed(
		vector<CoorIJK>& stk, vector<bool>& sceneData,
		CoorIJK sceneRangeIJK, CoorIJK sceneOriginIJK, int xLeft, int xRight, int J);

	// ���񻯽�������뼯��
public:
	// ���캯��������һ�� GridBase ������ָ��
	Object_Gridifier(GridBasePtr gridBase) : pGridObjectPtr(gridBase) {}

	// ����������ɢ��Ϊ����Ԫ
	GridCode discretizePoint(CoorXYZ point, Level level);
	std::vector<GridCode> discretizePoint(std::vector<CoorXYZ>& points, Level level);

	// ��һ���߶���ɢ��Ϊһϵ������Ԫ
	std::vector<GridCode> discretizeLine(CoorXYZ start, CoorXYZ end, Level level);

	// ��һ��ƽ�����α�����ɢ��Ϊ����Ԫ��XYƽ���ϵĶ�ά����Σ�
	std::vector<GridCode> discretizeSurface_XY(std::vector<CoorXYZ>& vertices, Level level);

	/**
	 * @brief  ��һ����ά������������񻯣��õ�������뼯��
	 * @param inputPts ��������㼯�ϣ�Ԫ�ظ���ӦΪ3��
	 * @param level ���񻯲㼶
	 * @param uniqueFlag �Ƿ�Ա������ȥ��
	 * @return
	 */
	std::vector<GridCode> discretizeTriangularSurface(vector<CoorXYZ>& inputPts, Level level, bool uniqueFlag = true);

	/**
	 * @brief ��һ����ά�����ƽ��������񻯣��õ�������뼯��
	 * @param inputPolygonPts ��ά�����ƽ�棨͹����Σ�
	 * @param level ����㼶
	 * @param uniqueFlag �Ƿ�Ա������ȥ��
	 * @return
	 */
	std::vector<GridCode> discretizePolygon(vector<CoorXYZ>& inputPolygonPts, Level level, bool uniqueFlag = true);

	/**
	 * @brief ��һ�����漯����ɢ��Ϊ����Ԫ
	 * @param faces ���漯��
	 * @param level ����㼶
	 * @param uniqueFlag �Ƿ�Ա������ȥ��
	 * @return
	 */
	std::vector<GridCode> discretizeVolume(std::vector<std::vector<CoorXYZ>>& faces, Level level, bool uniqueFlag = true);

	/**
	 * @brief �������������
	 * @param bottomPts �������꼯��
	 * @param height ����߶�
	 * @param level ����㼶
	 * @return
	 */
	std::vector<GridCode> discretizeCylinder(std::vector<CoorXYZ> bottomPts, double height, Level level);

	// ���񻯽�����������꼯��
public:
	// ��һ����ά������������񻯣��õ��������꼯��
	// �������꼯���п��ܴ����ظ�Ԫ��
	std::vector<CoorIJK> discretizeTriangularSurfaceToCoors(vector<CoorXYZ> inputPts, Level level);

};

class CylinderObjectGeojsonGridifier : Object_Gridifier, GeoJSON_NS::GeoJSON_Reader {
private:
	// ����Id��ʶ
	vector<std::string> objectId;
	// ====== ����������Ϣ ======
	vector<vector<CoorXYZ>> bottomBorderPts; // ����߽������
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
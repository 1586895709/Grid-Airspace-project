#include "object_gridifier.h"

vector<CoorIJK> Object_Gridifier::reverseScanLineSeedFill_XY(vector<CoorIJK>& inputEdgeCoors)
{
	if (inputEdgeCoors.size() == 0) {
		LOGI << "The input size of edge coordinates is 0!" << std::endl;
	}

	// ������������������Сֵ
	CoorIJK minIJK = inputEdgeCoors[0];
	CoorIJK maxIJK = inputEdgeCoors[0];
	for (int i = 0; i < inputEdgeCoors.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (inputEdgeCoors[i][j] < minIJK[j]) { minIJK[j] = inputEdgeCoors[i][j]; }
			if (inputEdgeCoors[i][j] > maxIJK[j]) { maxIJK[j] = inputEdgeCoors[i][j]; }
		}
	}

	// ������Χ�пռ�
	CoorIJK sceneRangeIJK = maxIJK - minIJK + CoorIJK(5, 5, 0);
	CoorIJK sceneOriginIJK = minIJK - CoorIJK(2, 2, 0);

	int sceneGridNum = sceneRangeIJK[0] * sceneRangeIJK[1];
	vector<bool> sceneData(sceneGridNum, false);

	for (int i = 0; i < sceneGridNum; i++) {
		int tempI = i % sceneRangeIJK[0];
		int tempJ = i / sceneRangeIJK[0];

		if (tempI == 0 || tempI == sceneRangeIJK[0] - 1 || tempJ == 0 || tempJ == sceneRangeIJK[1] - 1) {
			sceneData[i] = true;
		}
	}

	// ������α߽�����Ӧ�ĳ�����ʼ��Ϊ 1
	for (int i = 0; i < inputEdgeCoors.size(); i++) {
		CoorIJK tempBias = inputEdgeCoors[i] - sceneOriginIJK;
		int idx = tempBias[1] * sceneRangeIJK[0] + tempBias[0];
		sceneData[idx] = true;
	}

	// ��ά����ɨ��������㷨
	vector<CoorIJK> Stk;
	CoorIJK StartPoint = sceneOriginIJK + CoorIJK(1, 1, 1);

	// ��һ����������ӵ�
	Stk.push_back(StartPoint);

	int loopIdx = 0;
	while (!Stk.empty())
	{
		// �ڶ��� ��ȡ��ǰ���ӵ�
		CoorIJK SeedPoint = Stk[Stk.size() - 1];
		Stk.pop_back();
		CoorIJK tempBias = SeedPoint - sceneOriginIJK;
		int tempIdx = tempBias[1] * sceneRangeIJK[0] + tempBias[0];
		sceneData[tempIdx] = true;

		// ������ ���������
		int count = fillLineRight(tempIdx, sceneData);
		int xRight = tempBias[0] + count - 1;
		count = fillLineLeft(tempIdx - 1, sceneData);
		int xLeft = tempBias[0] - count;

		// ���Ĳ� ������������ɨ����
		searchLineNewSeed(Stk, sceneData, sceneRangeIJK, sceneOriginIJK, xLeft, xRight, tempBias[1] - 1);
		searchLineNewSeed(Stk, sceneData, sceneRangeIJK, sceneOriginIJK, xLeft, xRight, tempBias[1] + 1);

		//{
		//	loopIdx++;
		//	vector<CoorIJK> tempIJKs;
		//	for (int i = 0; i < sceneData.size(); i++)
		//	{
		//		if (sceneData[i] == true)
		//		{
		//			//CoorIJK tempBias(i % sceneRangeIJK[0] + sceneOriginIJK[0], i / sceneRangeIJK[0] + sceneOriginIJK[1], 0);
		//			CoorIJK tempBias(i % sceneRangeIJK[0], i / sceneRangeIJK[0], 0);
		//			tempIJKs.push_back(tempBias);
		//		}
		//	}
		//	VTK_Visualization::outputVTKformatIJK(tempIJKs, "./result/testCoors" + to_string(loopIdx) + ".vtk");
		//}
	}


	// �������
	vector<CoorIJK> result = inputEdgeCoors;

	for (int i = 0; i < sceneData.size(); i++)
	{
		if (sceneData[i] == false)
		{
			//CoorIJK tempBias(i % sceneRangeIJK[0] + sceneOriginIJK[0], i / sceneRangeIJK[0] + sceneOriginIJK[1], 0);
			CoorIJK tempBias(i % sceneRangeIJK[0], i / sceneRangeIJK[0], 0);
			result.push_back(tempBias + sceneOriginIJK);
		}
	}

	return result;
}

int Object_Gridifier::fillLineRight(int seedIdx, vector<bool>& sceneData)
{
	int count = 1;
	while ((sceneData[seedIdx + count] != true))
	{
		sceneData[seedIdx + count] = true;
		count++;
	}

	return count;
}

int Object_Gridifier::fillLineLeft(int seedIdx, vector<bool>& sceneData)
{
	int count = 0;
	while ((sceneData[seedIdx - count] != true))
	{
		sceneData[seedIdx - count] = true;
		count++;
	}

	return count;
}

void Object_Gridifier::searchLineNewSeed(vector<CoorIJK>& stk, vector<bool>& sceneData, CoorIJK sceneRangeIJK, CoorIJK sceneOriginIJK, int xLeft, int xRight, int J)
{
	if (J == -1 || J == sceneRangeIJK[1]) return;

	int xt = xLeft;
	while (xt <= xRight) {
		// �ҵ���һ�� Mode Ϊ 0 ��Ԫ��
		while (xt <= xRight && sceneData[J * sceneRangeIJK[0] + xt] == true) {
			xt++;
		}
		if (xt > xRight) break;

		// �ҵ������� Mode Ϊ 0 ��Ԫ��
		int start = xt;
		while (xt <= xRight && sceneData[J * sceneRangeIJK[0] + xt] == false) {
			xt++;
		}

		// ���������� Mode Ϊ 0 ��Ԫ��
		int end = xt - 1;
		sceneData[J * sceneRangeIJK[0] + end] = 1;
		stk.push_back(CoorIJK(end, J, 0) + sceneOriginIJK);
	}
}

GridCode Object_Gridifier::discretizePoint(CoorXYZ point, Level level)
{
	return this->pGridObjectPtr->toCodeFromXYZ(point, level);
}

std::vector<GridCode> Object_Gridifier::discretizePoint(std::vector<CoorXYZ>& points, Level level)
{
	vector<GridCode> result(points.size());

	for (int i = 0; i < points.size(); i++) {
		result[i] = this->pGridObjectPtr->toCodeFromXYZ(points[i], level);
	}

	// ����ȥ��
	codeUnique(result);
	return result;
}

std::vector<GridCode> Object_Gridifier::discretizeLine(CoorXYZ start, CoorXYZ end, Level level)
{
	vector<GridCode> result;
	this->pGridObjectPtr->getCodesOfLine(start, end, level, result);
	return result;
}

std::vector<GridCode> Object_Gridifier::discretizeSurface_XY(std::vector<CoorXYZ>& vertices, Level level)
{
	vertices.push_back(vertices[0]); // ��֤�߽����β���

	// ����ƽ�����α߽����񻯱��뼯��
	vector<GridCode> edgeCodes;
	for (int i = 0; i + 1 < vertices.size(); i++) {
		vector<GridCode> tempLineCodes;
		this->pGridObjectPtr->getCodesOfLine(vertices[i], vertices[i + 1], level, tempLineCodes);
		edgeCodes.insert(edgeCodes.end(), tempLineCodes.begin(), tempLineCodes.end());
	}
	codeUnique(edgeCodes);

	vector<CoorIJK> edgeCoors(edgeCodes.size()); // ƽ�����������������꼯��
	for (int i = 0; i < edgeCoors.size(); i++) {
		edgeCoors[i] = this->pGridObjectPtr->toIJKFromCode(edgeCodes[i]);
	}

	//VTK_Visualization::outputVTKformatIJK(edgeCoors, "./result/tempCoors.vtk");

	// ˮƽ���: ����ɨ�����㷨
	vector<CoorIJK> bottomSurfaceIJK = reverseScanLineSeedFill_XY(edgeCoors);

	vector<GridCode> result;
	for (int i = 0; i < bottomSurfaceIJK.size(); i++)
	{
		result.push_back(this->pGridObjectPtr->toCodeFromIJK(bottomSurfaceIJK[i], level));
	}

	codeUnique(result);
	return result;
}

std::vector<GridCode> Object_Gridifier::discretizeTriangularSurface(vector<CoorXYZ>& inputPts, Level level, bool uniqueFlag /*= true*/)
{
	vector<CoorIJK> tempCoors = this->discretizeTriangularSurfaceToCoors(inputPts, level);
	vector<GridCode> tempCodes = this->pGridObjectPtr->toCodesFromIJKs(tempCoors, level);
	if (uniqueFlag) { codeUnique(tempCodes); }
	return tempCodes;
}

std::vector<GridCode> Object_Gridifier::discretizePolygon(vector<CoorXYZ>& inputPolygonPts, Level level, bool uniqueFlag /*= true*/)
{
	vector<GridCode> result;
	if (inputPolygonPts.size() < 3) {
		LOGI << "[ERROR] The size of input polygon border points is < 3!" << std::endl;
		return result;
	}

	for (int i = 0; i < inputPolygonPts.size() - 2; i++) {
		vector<CoorXYZ> tempPts;
		tempPts.push_back(inputPolygonPts[i]);
		tempPts.push_back(inputPolygonPts[i + 1]);
		tempPts.push_back(inputPolygonPts[inputPolygonPts.size() - 1]);

		vector<GridCode> tempResult = this->discretizeTriangularSurface(tempPts, level, uniqueFlag);
		result.insert(result.end(), tempResult.begin(), tempResult.end());
	}
	codeUnique(result);

	return result;
}

std::vector<GridCode> Object_Gridifier::discretizeVolume(std::vector<std::vector<CoorXYZ>>& faces, Level level, bool uniqueFlag /*= true*/)
{
	vector<GridCode> result;
	for (int i = 0; i < faces.size(); i++) {
		vector<GridCode> tempFaceCodes = discretizePolygon(faces[i], level, uniqueFlag);
		result.insert(result.end(), tempFaceCodes.begin(), tempFaceCodes.end());
	}
	return result;
}

std::vector<GridCode> Object_Gridifier::discretizeCylinder(std::vector<CoorXYZ> bottomPts, double height, Level level)
{
	if (bottomPts.size() == 0) {
		LOGI << "The size of input bottom points is 0!" << std::endl;
		return vector<GridCode>();
	}

	// 1 ����ƽ������
	vector<GridCode> bottomCodes = discretizeSurface_XY(bottomPts, level);

	// 2 ����߶ȷ���������С����
	int bottomHeightCoor = this->pGridObjectPtr->toIJKFromXYZ(bottomPts[0], level)[2];
	int ceilHeightCoor = this->pGridObjectPtr->toIJKFromXYZ(bottomPts[0] + CoorXYZ(0, 0, height), level)[2];

	// 3 �߶ȷ������
	vector<GridCode> result(bottomCodes);
	for (int i = 0; i < bottomCodes.size(); i++) {
		CoorIJK tempIJK = this->pGridObjectPtr->toIJKFromCode(bottomCodes[i]);
		for (int tempK = bottomHeightCoor + 1; tempK <= ceilHeightCoor; tempK++) {
			tempIJK[2] = tempK;
			GridCode tempCode = this->pGridObjectPtr->toCodeFromIJK(tempIJK, level);
			result.push_back(tempCode);
		}
	}

	return result;
}

std::vector<CoorIJK> Object_Gridifier::discretizeTriangularSurfaceToCoors(vector<CoorXYZ> inputPts, Level level)
{
	// TODO: ����������񻯵Ĳ����ǲ������߽��Ͽ��ܴ�����©��
	// ����������
	if (inputPts.size() < 3) {
		LOGI << "[ERROR] The size of input points is less than 3!" << std::endl;
		return vector<CoorIJK>();
	}
	else if (inputPts.size() > 3) {
		LOGI << "The size of input points is more than 3!" << std::endl;
		inputPts.resize(3);
	}

	// ������������������
	CoorXYZ vecAB = inputPts[1] - inputPts[0];
	CoorXYZ vecAC = inputPts[2] - inputPts[0];
	CoorXYZ vecBC = inputPts[2] - inputPts[1];

	CoorXYZ gridLen = this->pGridObjectPtr->getGridLen(level);

	// �ȷֶ�������
	int num = 0; // �ڸ���ά�ȷ������ҵ�����ʷִ���
	for (int i = 0; i < 3; i++) {
		num = std::max(num, int(ceil(abs(vecAB[i] / gridLen[i]))));
		num = std::max(num, int(ceil(abs(vecAC[i] / gridLen[i]))));
		num = std::max(num, int(ceil(abs(vecBC[i] / gridLen[i]))));
	}

	// ����ƽ������
	CoorXYZ translationVecAB = vecAB / num;
	CoorXYZ translationVecBC = vecBC / num;

	// �������������
	vector<CoorIJK> result;
	CoorXYZ p, q;
	p = inputPts[0];
	for (int i = 0; i < num + 1; i++)
	{
		q = p;
		result.push_back(this->pGridObjectPtr->toIJKFromXYZ(q, level));
		for (int j = 0; j < i; j++)
		{
			q += translationVecBC;

			result.push_back(this->pGridObjectPtr->toIJKFromXYZ(q, level));
		}
		p += translationVecAB;
	}

	return result;
}

void CylinderObjectGeojsonGridifier::parseJsonValue(GeoJSON_NS::json& inputFeature)
{
	// ������Ϣ����
	GeoJSON_NS::json propertiesValue = inputFeature["properties"];
	this->minHeight.push_back(getOrDefault(propertiesValue, "MinHeight", 0.0));
	this->maxHeight.push_back(getOrDefault(propertiesValue, "MaxHeight", 0.0));
	this->beginTime.push_back(getStringConverted(propertiesValue, "BeginTime", (time_t)0, timeString2Sec));
	this->endTime.push_back(getStringConverted(propertiesValue, "EndTime", (time_t)0, timeString2Sec));

	this->objectId.push_back(getOrDefault(propertiesValue, "name", std::string("null")));

	string geometryType = inputFeature["geometry"]["type"];
	if (geometryType == "Polygon") {
		GeoJSON_NS::Polygon tempPolygon = this->parsePolygon(inputFeature["geometry"]["coordinates"]);
		if (tempPolygon.rings.size() > 1) {
			LOGI << "[WARNING] The input [Polygon] feature has more than 1 ring!" << std::endl;
		}
		this->bottomBorderPts.push_back(tempPolygon.rings[0].coordinates);
	}
	else if (geometryType == "MultiPolygon") {
		GeoJSON_NS::MultiPolygon tempMultiPolygon = this->parseMultiPolygon(inputFeature["geometry"]["coordinates"]);
		if (tempMultiPolygon.size() > 1) {
			LOGI << "[WARNING] The input [MultiPolygon] feature has more than 1 polygon!" << std::endl;
		}
		this->bottomBorderPts.push_back(tempMultiPolygon[0].rings[0].coordinates);
	}
	else {
		LOGI << "[ERROR] The input type is wrong!" << std::endl;
		return;
	}
}

void CylinderObjectGeojsonGridifier::readAndParseGeojson(std::string filename)
{
	GeoJSON_NS::json geojsonValue = readJsonFile(filename);

	if (!geojsonValue.contains("features")) {
		LOGI << "[ERROR] The input GeoJson file is wrong!" << std::endl;
		return;
	}

	for (auto& feature : geojsonValue["features"]) {
		this->parseJsonValue(feature);
	}
}

vector<GridCode> CylinderObjectGeojsonGridifier::gridifierObject(Level level)
{
	vector<GridCode> result;
	for (int i = 0; i < this->bottomBorderPts.size(); i++) {
		for (auto& tempPt : this->bottomBorderPts[i]) {
			tempPt[2] = this->minHeight[i];
		}

		//VTK_Visualization::outputVTKformatXYZ(this->bottomBorderPts[i], this->pGridObjectPtr, "./result/pts.vtk");

		vector<GridCode> tempCodes = this->discretizeCylinder(this->bottomBorderPts[i], this->maxHeight[i] - this->minHeight[i], level);
		result.insert(result.end(), tempCodes.begin(), tempCodes.end());
	}
	codeUnique(result);
	return result;
}

std::map<TimeInterval, vector<GridCode>> CylinderObjectGeojsonGridifier::gridifierObject_SpatioTemporal(Level level)
{
	std::map<TimeInterval, vector<GridCode>> result;
	for (int i = 0; i < this->bottomBorderPts.size(); i++) {
		for (auto& tempPt : this->bottomBorderPts[i]) {
			tempPt[2] = this->minHeight[i];
		}
		TimeInterval tempInterval = { this->beginTime[i], this->endTime[i] };
		vector<GridCode> tempResult = this->discretizeCylinder(this->bottomBorderPts[i], this->maxHeight[i] - this->minHeight[i], level);
		auto& tempValueVector = result[tempInterval];
		tempValueVector.insert(tempValueVector.end(), tempResult.begin(), tempResult.end());
		codeUnique(tempValueVector);
		//result[tempInterval] = this->discretizeCylinder(this->bottomBorderPts[i], this->maxHeight[i] - this->minHeight[i], level);
	}

	return result;
}

std::map<std::string, std::map<TimeInterval, vector<GridCode>>> CylinderObjectGeojsonGridifier::gridifierObject_SpatioTemporalWithId(Level level)
{
	std::map<std::string, std::map<TimeInterval, vector<GridCode>>> result;
	for (int i = 0; i < this->bottomBorderPts.size(); i++) {
		for (auto& tempPt : this->bottomBorderPts[i]) {
			tempPt[2] = this->minHeight[i];
		}
		TimeInterval tempInterval = { this->beginTime[i], this->endTime[i] };
		vector<GridCode> tempResult = this->discretizeCylinder(this->bottomBorderPts[i], this->maxHeight[i] - this->minHeight[i], level);
		std::map<TimeInterval, vector<GridCode>> tempMap;
		tempMap[tempInterval] = tempResult;
		result[this->objectId[i]] = tempMap;
	}

	return result;
}

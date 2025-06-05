#include "object_gridifier.h"

vector<CoorIJK> Object_Gridifier::reverseScanLineSeedFill_XY(vector<CoorIJK>& inputEdgeCoors)
{
	if (inputEdgeCoors.size() == 0) {
		LOGI << "The input size of edge coordinates is 0!" << std::endl;
	}

	// 计算网格坐标的最大最小值
	CoorIJK minIJK = inputEdgeCoors[0];
	CoorIJK maxIJK = inputEdgeCoors[0];
	for (int i = 0; i < inputEdgeCoors.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (inputEdgeCoors[i][j] < minIJK[j]) { minIJK[j] = inputEdgeCoors[i][j]; }
			if (inputEdgeCoors[i][j] > maxIJK[j]) { maxIJK[j] = inputEdgeCoors[i][j]; }
		}
	}

	// 构建包围盒空间
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

	// 将多边形边界所对应的场景初始化为 1
	for (int i = 0; i < inputEdgeCoors.size(); i++) {
		CoorIJK tempBias = inputEdgeCoors[i] - sceneOriginIJK;
		int idx = tempBias[1] * sceneRangeIJK[0] + tempBias[0];
		sceneData[idx] = true;
	}

	// 二维种子扫描线填充算法
	vector<CoorIJK> Stk;
	CoorIJK StartPoint = sceneOriginIJK + CoorIJK(1, 1, 1);

	// 第一步，添加种子点
	Stk.push_back(StartPoint);

	int loopIdx = 0;
	while (!Stk.empty())
	{
		// 第二步 提取当前种子点
		CoorIJK SeedPoint = Stk[Stk.size() - 1];
		Stk.pop_back();
		CoorIJK tempBias = SeedPoint - sceneOriginIJK;
		int tempIdx = tempBias[1] * sceneRangeIJK[0] + tempBias[0];
		sceneData[tempIdx] = true;

		// 第三步 向左右填充
		int count = fillLineRight(tempIdx, sceneData);
		int xRight = tempBias[0] + count - 1;
		count = fillLineLeft(tempIdx - 1, sceneData);
		int xLeft = tempBias[0] - count;

		// 第四步 处理相邻两条扫描线
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


	// 反向填充
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
		// 找到第一个 Mode 为 0 的元素
		while (xt <= xRight && sceneData[J * sceneRangeIJK[0] + xt] == true) {
			xt++;
		}
		if (xt > xRight) break;

		// 找到连续的 Mode 为 0 的元素
		int start = xt;
		while (xt <= xRight && sceneData[J * sceneRangeIJK[0] + xt] == false) {
			xt++;
		}

		// 处理连续的 Mode 为 0 的元素
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

	// 编码去重
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
	vertices.push_back(vertices[0]); // 保证边界点首尾相接

	// 计算平面多边形边界网格化编码集合
	vector<GridCode> edgeCodes;
	for (int i = 0; i + 1 < vertices.size(); i++) {
		vector<GridCode> tempLineCodes;
		this->pGridObjectPtr->getCodesOfLine(vertices[i], vertices[i + 1], level, tempLineCodes);
		edgeCodes.insert(edgeCodes.end(), tempLineCodes.begin(), tempLineCodes.end());
	}
	codeUnique(edgeCodes);

	vector<CoorIJK> edgeCoors(edgeCodes.size()); // 平面多边形网格化网格坐标集合
	for (int i = 0; i < edgeCoors.size(); i++) {
		edgeCoors[i] = this->pGridObjectPtr->toIJKFromCode(edgeCodes[i]);
	}

	//VTK_Visualization::outputVTKformatIJK(edgeCoors, "./result/tempCoors.vtk");

	// 水平填充: 种子扫描线算法
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

	// 1 底面平面网格化
	vector<GridCode> bottomCodes = discretizeSurface_XY(bottomPts, level);

	// 2 计算高度方向的最大最小网格
	int bottomHeightCoor = this->pGridObjectPtr->toIJKFromXYZ(bottomPts[0], level)[2];
	int ceilHeightCoor = this->pGridObjectPtr->toIJKFromXYZ(bottomPts[0] + CoorXYZ(0, 0, height), level)[2];

	// 3 高度方向填充
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
	// TODO: 这个函数网格化的策略是采样，边界上可能存在遗漏！
	// 输入参数检测
	if (inputPts.size() < 3) {
		LOGI << "[ERROR] The size of input points is less than 3!" << std::endl;
		return vector<CoorIJK>();
	}
	else if (inputPts.size() > 3) {
		LOGI << "The size of input points is more than 3!" << std::endl;
		inputPts.resize(3);
	}

	// 三角形三边向量计算
	CoorXYZ vecAB = inputPts[1] - inputPts[0];
	CoorXYZ vecAC = inputPts[2] - inputPts[0];
	CoorXYZ vecBC = inputPts[2] - inputPts[1];

	CoorXYZ gridLen = this->pGridObjectPtr->getGridLen(level);

	// 等分段数计算
	int num = 0; // 在各个维度方向上找到最大剖分次数
	for (int i = 0; i < 3; i++) {
		num = std::max(num, int(ceil(abs(vecAB[i] / gridLen[i]))));
		num = std::max(num, int(ceil(abs(vecAC[i] / gridLen[i]))));
		num = std::max(num, int(ceil(abs(vecBC[i] / gridLen[i]))));
	}

	// 计算平移向量
	CoorXYZ translationVecAB = vecAB / num;
	CoorXYZ translationVecBC = vecBC / num;

	// 顶点采样与网格化
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
	// 属性信息解析
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

#include "VTK_Visualization.h"

void VTK_Visualization::outputVTKformatIJK(std::vector<CoorIJK>& inputIJKs, string filename)
{
	vector<double> cellAttributes;
	outputVTKformatIJK(inputIJKs, cellAttributes, filename);
}

void VTK_Visualization::outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, string filename)
{
	vector<double> cellAttributes;
	outputVTKLatLng(inputPts, cellAttributes, filename);
}


void VTK_Visualization::outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, vector<double>& cellAttributes, string filename)
{
	ofstream fileout = ofstream(filename);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << filename << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int cellNum = inputPts.size();
	int ptsNum = 0;
	for (int i = 0; i < inputPts.size(); i++) {
		ptsNum += inputPts[i].size();
	}

	fileout << "POINTS " << ptsNum << " double" << endl;
	for (int i = 0; i < cellNum; i++)
	{
		for (int j = 0; j < inputPts[i].size(); j++)
		{
			fileout << setprecision(10) <<
				inputPts[i][j][0] << " " <<
				inputPts[i][j][1] << " " <<
				inputPts[i][j][2] << endl;
		}
	}
	fileout << endl;

	// 输出单元声明
	fileout << "CELLS " << cellNum << " " << cellNum + ptsNum << endl;
	int ptsOffset = 0;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << inputPts[i].size();
		for (int j = 0; j < inputPts[i].size(); j++)
		{
			fileout << " " << j + ptsOffset;
		}
		ptsOffset += inputPts[i].size();
		fileout << endl;
	}
	fileout << endl;

	// 输出单元类型
	fileout << "CELL_TYPES " << cellNum << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 7 << endl;
	}

	// 输出单元属性
	if (inputPts.size() == cellAttributes.size())
	{
		fileout << "CELL_DATA " << cellNum << endl;
		fileout << "SCALARS scalars float 1" << endl;
		fileout << "LOOKUP_TABLE CellColors" << endl;

		for (int i = 0; i < cellAttributes.size(); i++)
		{
			fileout << cellAttributes[i] << endl;
		}
	}

	cout << "[" + filename + "] 文件输出成功！" << endl;
}


void VTK_Visualization::outputVTKLatLng(std::vector<CoorXYZ>& inputPts, string filename)
{
	vector<vector<CoorXYZ>> tempPts;
	tempPts.push_back(inputPts);
	outputVTKLatLng(tempPts, filename);
}

void VTK_Visualization::outputVTKLatLngPts(std::vector<CoorXYZ> inputPts, string filename)
{
	ofstream fileout = ofstream(filename);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << filename << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int cellNum = inputPts.size();

	fileout << "POINTS " << cellNum << " double" << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << setprecision(10) <<
			inputPts[i][0] << " " <<
			inputPts[i][1] << " " <<
			inputPts[i][2] << endl;

	}
	fileout << endl;

	// 输出单元声明
	fileout << "CELLS " << cellNum << " " << cellNum * 2 << endl;
	int ptsOffset = 0;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 1 << " " << i << endl;
	}

	// 输出单元类型
	fileout << "CELL_TYPES " << cellNum << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 1 << endl;
	}

	cout << "[" + filename + "] 文件输出成功！" << endl;
}

void VTK_Visualization::outputVTKLatLngMercator(std::vector<std::vector<CoorXYZ>>& inputPts, string filename)
{
	ofstream fileout = ofstream(filename);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << filename << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int cellNum = inputPts.size();
	int ptsNum = 0;
	for (int i = 0; i < inputPts.size(); i++) {
		ptsNum += inputPts[i].size();
	}

	fileout << "POINTS " << ptsNum << " double" << endl;
	for (int i = 0; i < cellNum; i++)
	{
		for (int j = 0; j < inputPts[i].size(); j++)
		{
			CoorXYZ inputXYZMercator = latLonToMercator(inputPts[i][j]);
			fileout << setprecision(10) <<
				inputXYZMercator[0] << " " <<
				inputXYZMercator[1] << " " <<
				inputXYZMercator[2] << endl;
		}
	}
	fileout << endl;

	// 输出单元声明
	fileout << "CELLS " << cellNum << " " << cellNum + ptsNum << endl;
	int ptsOffset = 0;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << inputPts[i].size();
		for (int j = 0; j < inputPts[i].size(); j++)
		{
			fileout << " " << j + ptsOffset;
		}
		ptsOffset += inputPts[i].size();
		fileout << endl;
	}
	fileout << endl;

	// 输出单元类型
	fileout << "CELL_TYPES " << cellNum << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 7 << endl;
	}

	cout << "[" + filename + "] 文件输出成功！" << endl;
}

void VTK_Visualization::outputVTKformatXYZ(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, bool is2D /*= true*/)
{
	vector<double> codeAttributes;
	outputVTKformatXYZ(inputCodes, gridObjectPtr, fileName, codeAttributes, is2D);
}

void VTK_Visualization::outputVTKformatLine(std::vector<CoorXYZ> inputPts, GridBasePtr gridObjectPtr, string fileName, bool is2D /*= true*/)
{
	vector<vector<CoorXYZ>> inputPtsList = { inputPts };
	outputVTKformatLine(inputPtsList, gridObjectPtr, fileName, is2D);
}

void VTK_Visualization::outputVTKformatLine(std::vector<std::vector<CoorXYZ>> inputPtsList, GridBasePtr gridObjectPtr, string fileName, bool is2D /*= true*/)
{
	ofstream fileout = ofstream(fileName);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << fileName << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int ptsNum = 0;
	int lineNum = inputPtsList.size();
	for (int i = 0; i < inputPtsList.size(); i++) {
		ptsNum += inputPtsList[i].size();
	}

	fileout << "POINTS " << ptsNum << " double" << endl;
	for (int i = 0; i < lineNum; i++) {
		auto& inputPts = inputPtsList[i];
		for (int j = 0; j < inputPts.size(); j++)
		{
			if (gridObjectPtr->getCoorType() == CoorType::GeoElevationCS) {
				inputPts[j] = latLonToMercator(inputPts[j]);
			}

			if (is2D) {
				inputPts[j][2] = 0;
			}

			fileout << setprecision(10) <<
				inputPts[j][0] << " " <<
				inputPts[j][1] << " " <<
				inputPts[j][2] << endl;

		}
		fileout << endl;
	}

	// 输出单元声明
	fileout << "CELLS " << lineNum << " " << ptsNum + lineNum << endl;
	int ptsOffset = 0;
	for (int i = 0; i < inputPtsList.size(); i++) {
		fileout << inputPtsList[i].size();
		for (int j = 0; j < inputPtsList[i].size(); j++) {
			fileout << " " << j + ptsOffset;
		}
		ptsOffset += inputPtsList.size();
		fileout << endl;
	}

	// 输出单元类型
	fileout << "CELL_TYPES " << lineNum << endl;
	for (int i = 0; i < lineNum; i++) {
		fileout << 4 << endl;
	}

	cout << "[" + fileName + "] 文件输出成功！" << endl;
}

void VTK_Visualization::outputVTKformatLine(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, bool is2D /*= true*/)
{
	vector<CoorXYZ> inputPts(inputCodes.size());
	for (int i = 0; i < inputCodes.size(); i++) {
		inputPts[i] = gridObjectPtr->toXYZFromCode(inputCodes[i]);
	}
	outputVTKformatLine(inputPts, gridObjectPtr, fileName, is2D);
}

void VTK_Visualization::outputVTKformatXYZ(std::vector<CoorXYZ> inputPts, GridBasePtr gridObjectPtr, string fileName, bool is2D /*= true*/)
{
	if (gridObjectPtr->getCoorType() == CoorType::GeoElevationCS) {
		for (int i = 0; i < inputPts.size(); i++) {
			inputPts[i] = latLonToMercator(inputPts[i]);
		}
	}

	outputVTKLatLngPts(inputPts, fileName);
}

void VTK_Visualization::outputVTKformatMap(CodeMap& inputMap, GridBasePtr gridObjectPtr, string filename, bool is2D /*= true*/)
{
	vector<GridCode> inputCodes;
	vector<double> codeAttributes;
	for (auto& tempPair : inputMap) {
		inputCodes.push_back(tempPair.first);
		codeAttributes.push_back(tempPair.second.velFactor);
	}
	outputVTKformatXYZ(inputCodes, gridObjectPtr, filename, codeAttributes, is2D);
}

void VTK_Visualization::outputVTKGrid(vector<CoorXYZ>& minBorders, vector<CoorXYZ>& maxBorders, vector<double>& cellAttributes, string fileName)
{
	if (minBorders.size() != maxBorders.size()) {
		LOGI << "The size of input border is not equal!" << endl;
	}

	ofstream fileout = ofstream(fileName);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << fileName << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int cellNum = minBorders.size();

	// 输出格点信息
	bool t_I[8] = { 0,1,0,1,0,1,0,1 };
	bool t_J[8] = { 0,0,1,1,0,0,1,1 };
	bool t_K[8] = { 0,0,0,0,1,1,1,1 };

	fileout << "POINTS " << cellNum * 8 << " double" << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << minBorders[i][0] << " " << minBorders[i][1] << " " << minBorders[i][2] << endl; // 0
		fileout << maxBorders[i][0] << " " << minBorders[i][1] << " " << minBorders[i][2] << endl; // 1
		fileout << minBorders[i][0] << " " << maxBorders[i][1] << " " << minBorders[i][2] << endl; // 2
		fileout << maxBorders[i][0] << " " << maxBorders[i][1] << " " << minBorders[i][2] << endl; // 3
		fileout << minBorders[i][0] << " " << minBorders[i][1] << " " << maxBorders[i][2] << endl; // 4
		fileout << maxBorders[i][0] << " " << minBorders[i][1] << " " << maxBorders[i][2] << endl; // 5
		fileout << minBorders[i][0] << " " << maxBorders[i][1] << " " << maxBorders[i][2] << endl; // 6
		fileout << maxBorders[i][0] << " " << maxBorders[i][1] << " " << maxBorders[i][2] << endl; // 7
	}
	fileout << endl;

	// 输出单元声明
	fileout << "CELLS " << cellNum << " " << cellNum * 9 << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 8;
		for (int j = 0; j < 8; j++)
		{
			fileout << " " << j + i * 8;
		}
		fileout << endl;
	}
	fileout << endl;

	// 输出单元类型
	fileout << "CELL_TYPES " << cellNum << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 11 << endl;
	}

	// 输出单元属性
	if (cellAttributes.size() == cellNum)
	{
		fileout << "CELL_DATA " << cellNum << endl;
		fileout << "SCALARS scalars float 1" << endl;
		fileout << "LOOKUP_TABLE CellColors" << endl;

		for (int i = 0; i < cellAttributes.size(); i++)
		{
			fileout << cellAttributes[i] << endl;
		}
	}

	fileout.close();

	cout << "[" + fileName + "] 文件输出成功！" << endl;
}

void VTK_Visualization::outputVTKformatXYZ(
	std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, vector<double>& codeAttributes, bool is2D /*= true*/)
{
	if (!is2D) {
		vector<CoorXYZ> codesMinBorder(inputCodes.size());
		vector<CoorXYZ> codesMaxBorder(inputCodes.size());
		for (int i = 0; i < inputCodes.size(); i++) {
			gridObjectPtr->getRangeOfCode(inputCodes[i], codesMinBorder[i], codesMaxBorder[i]);
			if (gridObjectPtr->getCoorType() == CoorType::GeoElevationCS) {
				codesMinBorder[i] = latLonToMercator(codesMinBorder[i]);
				codesMaxBorder[i] = latLonToMercator(codesMaxBorder[i]);
			}
		}

		outputVTKGrid(codesMinBorder, codesMaxBorder, codeAttributes, fileName);
		return;
	}

	std::vector<std::vector<CoorXYZ>> inputBorders(inputCodes.size());
	for (int i = 0; i < inputCodes.size(); i++) {
		std::vector<CoorXYZ> tempCornerPts = gridObjectPtr->calCornerPtsFromCode(inputCodes[i]);
		if (tempCornerPts.size() == 8) tempCornerPts.resize(4);

		if (gridObjectPtr->getCoorType() == CoorType::GeoElevationCS) {
			for (auto& tempPt : tempCornerPts) {
				tempPt = latLonToMercator(tempPt);
			}
		}

		inputBorders[i] = tempCornerPts;
	}

	outputVTKLatLng(inputBorders, codeAttributes, fileName);
}

void VTK_Visualization::outputVTKformatIJK(std::vector<CoorIJK>& inputIJKs, vector<double>& cellAttributes, string filename)
{
	ofstream fileout = ofstream(filename);

	// 输出文件头信息
	fileout << "# vtk DataFile Version 3.0" << endl;
	fileout << filename << endl;
	fileout << "ASCII" << endl;
	fileout << "DATASET UNSTRUCTURED_GRID" << endl;

	int cellNum = inputIJKs.size();

	// 输出格点信息
	bool t_I[8] = { 0,1,0,1,0,1,0,1 };
	bool t_J[8] = { 0,0,1,1,0,0,1,1 };
	bool t_K[8] = { 0,0,0,0,1,1,1,1 };

	fileout << "POINTS " << cellNum * 8 << " double" << endl;
	for (int i = 0; i < cellNum; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			fileout <<
				(inputIJKs[i][0] + t_I[j]) << " " <<
				(inputIJKs[i][1] + t_J[j]) << " " <<
				(inputIJKs[i][2] + t_K[j]) << endl;
		}
	}
	fileout << endl;

	// 输出单元声明
	fileout << "CELLS " << cellNum << " " << cellNum * 9 << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 8;
		for (int j = 0; j < 8; j++)
		{
			fileout << " " << j + i * 8;
		}
		fileout << endl;
	}
	fileout << endl;

	// 输出单元类型
	fileout << "CELL_TYPES " << cellNum << endl;
	for (int i = 0; i < cellNum; i++)
	{
		fileout << 11 << endl;
	}

	// 输出单元属性
	if (cellAttributes.size() == inputIJKs.size())
	{
		fileout << "CELL_DATA " << cellNum << endl;
		fileout << "SCALARS scalars float 1" << endl;
		fileout << "LOOKUP_TABLE CellColors" << endl;

		for (int i = 0; i < cellAttributes.size(); i++)
		{
			fileout << cellAttributes[i] << endl;
		}
	}

	fileout.close();

	cout << "[" + filename + "] 文件输出成功！" << endl;
}


void VTK_Visualization::outputVTKformatIJK(CoorIJK& inputIJK, string filename)
{
	vector<CoorIJK> tempIJK = { inputIJK };
	return outputVTKformatIJK(tempIJK, filename);
}

void VTK_Visualization::outputGeojsonGrid(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName)
{
	std::vector<std::vector<CoorXYZ>> inputBorders(inputCodes.size());
	for (int i = 0; i < inputCodes.size(); i++) {
		std::vector<CoorXYZ> tempCornerPts = gridObjectPtr->calCornerPtsFromCode(inputCodes[i]);
		if (tempCornerPts.size() == 8) tempCornerPts.resize(4);
		inputBorders[i] = tempCornerPts;
	}

	if (gridObjectPtr->getCoorType() == CoorType::CartesianCS_3D) {
		for (int i = 0; i < inputBorders.size(); i++) {
			for (int j = 0; j < inputBorders[i].size(); j++) {
				inputBorders[i][j] = mercatorToLatLon(inputBorders[i][j]);
			}
		}
	}

	vector<double> cellAttributes;
	outputGeojsonPolygon(inputBorders, cellAttributes, fileName);
}

void VTK_Visualization::outputGeojsonLine(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName)
{
	std::vector<CoorXYZ> inputPts(inputCodes.size());
	for (int i = 0; i < inputCodes.size(); i++) {
		inputPts[i] = gridObjectPtr->toXYZFromCode(inputCodes[i]);
	}

	if (gridObjectPtr->getCoorType() == CoorType::CartesianCS_3D) {
		for (int i = 0; i < inputPts.size(); i++) {
			inputPts[i] = mercatorToLatLon(inputPts[i]);
		}
	}

	vector<double> cellAttributes;
	outputGeojsonLine(inputPts, cellAttributes, fileName);
}

void VTK_Visualization::outputGeojsonPolygon(std::vector<std::vector<CoorXYZ>>& inputPts, std::vector<double>& cellAttributes, const std::string& filename)
{
	json featureCollection;
	featureCollection["type"] = "FeatureCollection";
	json features = json::array();

	for (size_t i = 0; i < inputPts.size(); ++i) {
		json feature;
		feature["type"] = "Feature";

		json geometry;
		geometry["type"] = "Polygon";
		json coordinates = json::array();
		json ring = json::array();

		for (const auto& point : inputPts[i]) {
			ring.push_back(point);
		}
		coordinates.push_back(ring);
		geometry["coordinates"] = coordinates;
		feature["geometry"] = geometry;

		json properties;
		if (i < cellAttributes.size()) {
			properties["scalar"] = cellAttributes[i];
		}
		feature["properties"] = properties;

		features.push_back(feature);
	}

	featureCollection["features"] = features;

	std::ofstream fileout(filename);
	if (fileout.is_open()) {
		fileout << std::setw(4) << featureCollection;
		fileout.close();
		std::cout << "[" + filename + "] 文件输出成功！" << std::endl;
	}
	else {
		std::cerr << "无法打开文件: " << filename << std::endl;
	}
}

void VTK_Visualization::outputGeojsonLine(std::vector<CoorXYZ>& inputPts, std::vector<double>& cellAttributes, const std::string& filename)
{
	json featureCollection;
	featureCollection["type"] = "FeatureCollection";
	json features = json::array();

	json feature;
	feature["type"] = "Feature";

	json geometry;
	geometry["type"] = "LineString";
	json coordinates = json::array();

	for (const auto& point : inputPts) {
		coordinates.push_back(point);
	}
	geometry["coordinates"] = coordinates;
	feature["geometry"] = geometry;

	json properties;
	feature["properties"] = properties;

	features.push_back(feature);
	featureCollection["features"] = features;

	std::ofstream fileout(filename);
	if (fileout.is_open()) {
		fileout << std::setw(4) << featureCollection;
		fileout.close();
		std::cout << "[" + filename + "] 文件输出成功！" << std::endl;
	}
	else {
		std::cerr << "无法打开文件: " << filename << std::endl;
	}
}
#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "grid_base.h"
#include "util_function.h"

using namespace std;

namespace VTK_Visualization
{
	void outputVTKformatIJK(CoorIJK& inputIJK, string filename);
	void outputVTKformatIJK(std::vector<CoorIJK>& inputIJKs, vector<double>& cellAttributes, string filename);

	void outputVTKformatIJK(std::vector<CoorIJK>& inputIJKs, string filename);

	// �Զ�����������VTK�ļ�������������任������δأ�
	void outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, string filename);
	void outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, vector<double>& cellAttributes, string filename);
	// �Զ�����������VTK�ļ�������������任����������Σ�
	void outputVTKLatLng(std::vector<CoorXYZ>& inputPts, string filename);

	// �Ե��������VTK�ļ�������������任
	void outputVTKLatLngPts(std::vector<CoorXYZ> inputPts, string filename);

	// ��������任
	void outputVTKLatLngMercator(std::vector<std::vector<CoorXYZ>>& inputPts, string filename);
}

namespace VTK_Visualization
{
	void outputVTKformatXYZ(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, bool is2D = true);
	void outputVTKformatXYZ(std::vector<CoorXYZ> inputPts, GridBasePtr gridObjectPtr, string fileName, bool is2D = true);
	void outputVTKformatXYZ(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, vector<double>& codeAttributes, bool is2D = true);

	void outputVTKformatLine(std::vector<CoorXYZ> inputPts, GridBasePtr gridObjectPtr, string fileName, bool is2D = true);
	void outputVTKformatLine(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName, bool is2D = true);
	void outputVTKformatLine(std::vector<std::vector<CoorXYZ>> inputPtsList, GridBasePtr gridObjectPtr, string fileName, bool is2D = true);


	void outputVTKformatMap(CodeMap& inputMap, GridBasePtr gridObjectPtr, string filename, bool is2D = true);

	/**
	 * @brief ����ÿ������������С��Χ���������ӻ����
	 * @param minBorders ��С��Χ����
	 * @param maxBorders ���Χ����
	 * @param fileName ����ļ�·��
	 */
	void outputVTKGrid(vector<CoorXYZ>& minBorders, vector<CoorXYZ>& maxBorders, vector<double>& cellAttributes, string fileName);
}

namespace VTK_Visualization
{
	using json = nlohmann::json;
	void outputGeojsonGrid(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName);
	void outputGeojsonLine(std::vector<GridCode> inputCodes, GridBasePtr gridObjectPtr, string fileName);

	void outputGeojsonPolygon(std::vector<std::vector<CoorXYZ>>& inputPts, std::vector<double>& cellAttributes, const std::string& filename);
	void outputGeojsonLine(std::vector<CoorXYZ>& inputPts, std::vector<double>& cellAttributes, const std::string& filename);
}
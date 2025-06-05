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

	// 以多边形类型输出VTK文件，不进行坐标变换（多边形簇）
	void outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, string filename);
	void outputVTKLatLng(std::vector<std::vector<CoorXYZ>>& inputPts, vector<double>& cellAttributes, string filename);
	// 以多边形类型输出VTK文件，不进行坐标变换（单个多边形）
	void outputVTKLatLng(std::vector<CoorXYZ>& inputPts, string filename);

	// 以点类型输出VTK文件，不进行坐标变换
	void outputVTKLatLngPts(std::vector<CoorXYZ> inputPts, string filename);

	// 进行坐标变换
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
	 * @brief 根据每个网格的最大最小范围输出网格可视化结果
	 * @param minBorders 最小范围集合
	 * @param maxBorders 最大范围集合
	 * @param fileName 输出文件路径
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
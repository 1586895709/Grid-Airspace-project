#include "grid_3d.h"

GRID_3D::~GRID_3D() {

}

CoorIJK GRID_3D::toIJKFromXYZ(const CoorXYZ& xyz, Level level) {
	// Check if the point is out of border
	if (this->isOutBorder(xyz)) {
		LOGI << "The point is out of border!" << std::endl;
		CoorXYZ tempXYZ = xyz;
		tempXYZ[0] = std::min(std::max(tempXYZ[0], minBorder[0]), maxBorder[0]);
		tempXYZ[1] = std::min(std::max(tempXYZ[1], minBorder[1]), maxBorder[1]);
		tempXYZ[2] = std::min(std::max(tempXYZ[2], minBorder[2]), maxBorder[2]);
		return toIJKFromXYZ(tempXYZ, level);
	}

	// Calculate the IJK
	CoorIJK ijk;
	CoorXYZ gridLen = this->getGridLen(level);
	ijk[0] = (int)((xyz[0] - minBorder[0]) / gridLen[0]);
	ijk[1] = (int)((xyz[1] - minBorder[1]) / gridLen[1]);
	ijk[2] = (int)((xyz[2] - minBorder[2]) / gridLen[2]);

	// Check if the IJK is out of border
	int layerMaxValue = (int)(1 << level.x);
	ijk[0] = std::min(std::max(ijk[0], 0), layerMaxValue - 1);
	ijk[1] = std::min(std::max(ijk[1], 0), layerMaxValue - 1);
	ijk[2] = std::min(std::max(ijk[2], 0), layerMaxValue - 1);

	return ijk;
}

CoorXYZ GRID_3D::toXYZFromIJK(const CoorIJK& ijk, Level level) {
	// Check if the IJK is out of border
	int layerMaxValue = (int)(1 << level.x);
	if (ijk[0] < 0 || ijk[0] >= layerMaxValue
		|| ijk[1] < 0 || ijk[1] >= layerMaxValue
		|| ijk[2] < 0 || ijk[2] >= layerMaxValue) {
		LOGI << "The IJK is out of border!" << std::endl;
		CoorIJK tempIJK = ijk;
		tempIJK[0] = std::min(std::max(tempIJK[0], 0), layerMaxValue - 1);
		tempIJK[1] = std::min(std::max(tempIJK[1], 0), layerMaxValue - 1);
		tempIJK[2] = std::min(std::max(tempIJK[2], 0), layerMaxValue - 1);
		return toXYZFromIJK(tempIJK, level);
	}

	// Calculate the XYZ
	CoorXYZ gridLen = this->getGridLen(level);
	CoorXYZ xyz;
	xyz[0] = (ijk[0] + 0.5) * gridLen[0] + minBorder[0];
	xyz[1] = (ijk[1] + 0.5) * gridLen[1] + minBorder[1];
	xyz[2] = (ijk[2] + 0.5) * gridLen[2] + minBorder[2];
	return xyz;
}

GridCode GRID_3D::toCodeFromIJK(const CoorIJK& ijk, Level level) {
	GridCode z = 0;

	//交叉取位，得到编码值，取位的方式Z-Y-X三个方向依次进行
	for (int i = 0; i < 21; i++)
	{
		z |= ((ijk[0]) & (GridCode(1)) << i) << (i << 1)
			| ((ijk[1]) & (GridCode(1)) << i) << ((i << 1) + 1)
			| ((ijk[2]) & (GridCode(1)) << i) << ((i << 1) + 2);
	}

	GridCode levelFlag = (1ull << (63 - 3 * level.x)) - 1;
	z = (z << (64 - 3 * level.x)) + levelFlag;

	return z;
}

CoorIJK GRID_3D::toIJKFromCode(GridCode code) {
	//计算层级
	Level N = getLevelFromCode(code);

	code = code >> (64 - 3 * N.x);

	GridCode m = 0;
	GridCode n = 0;
	GridCode p = 0;

	//位运算计算得到i,j,k的值
	//牵涉到乘除运算，能够修改以提高效率
	for (int i = 0; i < 63; i = i + 3)
	{
		m |= (code & (GridCode(1)) << i) >> (i * 2 / 3);
		n |= (code & (GridCode(1)) << (i + 1)) >> (i * 2 / 3 + 1);
		p |= (code & (GridCode(1)) << (i + 2)) >> (i * 2 / 3 + 2);
	}
	return CoorIJK(int(m), int(n), int(p));
}

GridCode GRID_3D::toCodeFromXYZ(const CoorXYZ& xyz, Level level)
{
	return this->toCodeFromIJK(this->toIJKFromXYZ(xyz, level), level);
}

CoorXYZ GRID_3D::toXYZFromCode(GridCode code)
{
	return this->toXYZFromIJK(this->toIJKFromCode(code), this->getLevelFromCode(code));
}

Level GRID_3D::getLevelFromCode(GridCode code) {

	//// 输入的是偶数，就是21层
	//if (!(code & 1))
	//{
	//	return 21;
	//}

	//GridCode mid = (code - 1) ^ (code + 1);
	//int N = 0;
	//for (int i = 0; i < 6; i++)
	//{
	//	N = N + (this->getMidN(mid, i));
	//}

	////返回层级;使用模数进行除法运算，相当于：N/3
	////return ((GridCode)N * 0xAAAAAAAB) >> 33;
	//return int(N / 3);

	return 21 - int(countConsecutiveOnes(code) / 3);
}

std::vector<CoorXYZ> GRID_3D::calCornerPtsFromCode(GridCode code)
{
	int codeLevel = this->getLevelFromCode(code).x;
	CoorXYZ centerPt = this->toXYZFromCode(code);
	CoorXYZ halfGridLen = 0.5 * this->getGridLen(codeLevel);

	std::vector<CoorXYZ> result(8);
	result[0] = centerPt + CoorXYZ(-halfGridLen.x(), -halfGridLen.y(), -halfGridLen.z());
	result[1] = centerPt + CoorXYZ(-halfGridLen.x(), +halfGridLen.y(), -halfGridLen.z());
	result[2] = centerPt + CoorXYZ(+halfGridLen.x(), +halfGridLen.y(), -halfGridLen.z());
	result[3] = centerPt + CoorXYZ(+halfGridLen.x(), -halfGridLen.y(), -halfGridLen.z());
	result[4] = centerPt + CoorXYZ(-halfGridLen.x(), -halfGridLen.y(), halfGridLen.z());
	result[5] = centerPt + CoorXYZ(-halfGridLen.x(), +halfGridLen.y(), halfGridLen.z());
	result[6] = centerPt + CoorXYZ(+halfGridLen.x(), +halfGridLen.y(), halfGridLen.z());
	result[7] = centerPt + CoorXYZ(+halfGridLen.x(), -halfGridLen.y(), halfGridLen.z());

	return result;
}

GridCode GRID_3D::findDadCode(GridCode inputCode, int inputFatherLevel /*= -1*/)
{
	if (inputFatherLevel < 0) {
		LOGI << "The input father level is < 0!";
		return 0;
	}
	else {
		// 寻找N1层级网格
		GridCode DetaT;
		DetaT = (GridCode)1 << (63 - (inputFatherLevel << 1) - inputFatherLevel);

		// 寻找InputCode的N级父单元
		GridCode mid;
		mid = (inputCode >> (64 - (inputFatherLevel << 1) - inputFatherLevel)) << (64 - (inputFatherLevel << 1) - inputFatherLevel);

		// 其本质和计算第N层的父单元是一致的。
		return mid + DetaT - 1;
	}
}

vector<GridCode> GRID_3D::getChildCodes(GridCode inputCode, Level inputSonLevel)
{
	Level fatherLevel = getLevelFromCode(inputCode);
	if (fatherLevel >= inputSonLevel) {
		LOGI << "The input son level is smaller or equal than father level!" << endl;
		return vector<GridCode>{inputCode};
	}

	int diff = inputSonLevel.x - fatherLevel.x;
	int sonCodesNum = pow(8, diff);

	GridCode sonLevelFlag = ((GridCode)1 << (63 - 3 * inputSonLevel.x)) - 1;
	int fatherLevelFlagLen = 64 - 3 * fatherLevel.x;
	GridCode baseCode = (inputCode >> fatherLevelFlagLen << fatherLevelFlagLen) + sonLevelFlag;

	vector<GridCode> result;
	for (int i = 0; i < sonCodesNum; i++) {
		result.push_back(baseCode);
		baseCode += sonLevelFlag;
	}
	return result;
}

std::vector<CoorIJK> GRID_3D::calNeighbors(CoorIJK inputCoor, int neighborDis /*= 1*/, bool isManhattanDis /*= false*/)
{
	std::vector<CoorIJK> result;
	for (int i = -neighborDis; i <= neighborDis; i++) {
		int tempManhattanDisI = abs(i);
		for (int j = -neighborDis; j <= neighborDis; j++) {
			int tempManhattanDisIJ = tempManhattanDisI + abs(j);
			if (isManhattanDis && tempManhattanDisIJ > neighborDis) continue;

			for (int k = -neighborDis; k <= neighborDis; k++) {
				if (i == 0 && j == 0 && k == 0) continue;
				int tempManhattanDisIJK = tempManhattanDisIJ + abs(k);
				if (isManhattanDis && tempManhattanDisIJK > neighborDis) continue;

				result.push_back(inputCoor + CoorIJK(i, j, k));
			}
		}
	}
	return result;
}

CoorXYZ GRID_3D::getGridLen(Level level) {
	auto tempFindResult = this->gridLenMap.find(level);
	if (tempFindResult != this->gridLenMap.end()) {
		return tempFindResult->second;
	}

	CoorXYZ gridLen;
	gridLen = (maxBorder - minBorder) / (1 << level.x);

	this->gridLenMap[level] = gridLen;
	return gridLen;
}

void GRID_3D::getBorderRange(CoorXYZ& borderMin, CoorXYZ& borderMax)
{
	borderMin = this->minBorder;
	borderMax = this->maxBorder;
}

void GRID_3D::calculateIntersection(double origin, double direction, double vmin, double vmax, double& tmin, double& tmax)
{
	if (fabs(direction) < 1e-6) {
		// 处理方向分量为 0 的情况
		if (origin < vmin || origin > vmax) {
			tmin = -1;
			tmax = -1;
		}
		return;
	}

	if (direction >= 0) {
		tmin = (vmin - origin) / direction;
		tmax = (vmax - origin) / direction;
	}
	else {
		tmin = (vmax - origin) / direction;
		tmax = (vmin - origin) / direction;
	}
}

double GRID_3D::rayBoxIntersection(CoorXYZ origin, CoorXYZ direction, CoorXYZ vmin, CoorXYZ vmax)
{
	double txmin, txmax, tymin, tymax, tzmin, tzmax;

	// 计算 X 轴上的相交距离
	calculateIntersection(origin[0], direction[0], vmin[0], vmax[0], txmin, txmax);
	if (txmin == -1) return -1;

	// 计算 Y 轴上的相交距离
	calculateIntersection(origin[1], direction[1], vmin[1], vmax[1], tymin, tymax);
	if (tymin == -1) return -1;

	// 检查X和Y轴的t范围是否有重叠。如果tmin > tymax或者tymin > tmax，说明没有交集，返回-1
	// 如果没有交集，说明射线与网格范围没有交集
	if ((txmin > tymax) || (tymin > txmax)) {
		return -1;
	}

	txmin = std::max(txmin, tymin);
	txmax = std::min(txmax, tymax);

	// 计算 Z 轴上的相交距离
	calculateIntersection(origin[2], direction[2], vmin[2], vmax[2], tzmin, tzmax);
	if (tzmin == -1) return -1;

	if ((txmin > tzmax) || (tzmin > txmax)) {
		return -1;
	}

	txmin = std::max(txmin, tzmin);
	txmax = std::min(txmax, tzmax);

	return txmin;
}

void GRID_3D::amanatidesWooAlgorithm(CoorXYZ origin, CoorXYZ end, CoorXYZ minBound, CoorXYZ maxBound, int nx, int ny, int nz, std::vector<CoorIJK>& ijk_list)
{
	CoorXYZ direction = end - origin;

	double tmin = rayBoxIntersection(origin, direction, minBound, maxBound);
	if (tmin < 0)
	{
		tmin = 0;
	}
	CoorXYZ start = origin + tmin * direction;
	CoorXYZ boxSize = maxBound - minBound;

	int x = floor(((start[0] - minBound[0]) / boxSize[0]) * nx) + 1;
	int y = floor(((start[1] - minBound[1]) / boxSize[1]) * ny) + 1;
	int z = floor(((start[2] - minBound[2]) / boxSize[2]) * nz) + 1;

	if (x == (nx + 1))
		x = x - 1;
	if (y == (ny + 1))
		y = y - 1;
	if (z == (nz + 1))
		z = z - 1;

	double tVoxelX, tVoxelY, tVoxelZ;
	int stepX, stepY, stepZ;
	if (direction[0] >= 0)
	{
		tVoxelX = double(x) / double(nx);
		stepX = 1;
	}
	else
	{
		tVoxelX = double(x - 1) / double(nx);
		stepX = -1;
	}
	if (direction[1] >= 0)
	{
		tVoxelY = double(y) / double(ny);
		stepY = 1;
	}
	else
	{
		tVoxelY = double(y - 1) / double(ny);
		stepY = -1;
	}
	if (direction[2] >= 0)
	{
		tVoxelZ = double(z) / double(nz);
		stepZ = 1;
	}
	else
	{
		tVoxelZ = double(z - 1) / double(nz);
		stepZ = -1;
	}

	double voxelMaxX, voxelMaxY, voxelMaxZ;
	voxelMaxX = minBound[0] + tVoxelX * boxSize[0];
	voxelMaxY = minBound[1] + tVoxelY * boxSize[1];
	voxelMaxZ = minBound[2] + tVoxelZ * boxSize[2];

	double tMaxX, tMaxY, tMaxZ;
	tMaxX = tmin + (voxelMaxX - start[0]) / direction[0];
	tMaxY = tmin + (voxelMaxY - start[1]) / direction[1];
	tMaxZ = tmin + (voxelMaxZ - start[2]) / direction[2];


	double voxelSizeX = boxSize[0] / nx;
	double voxelSizeY = boxSize[1] / ny;
	double voxelSizeZ = boxSize[2] / nz;

	double tDeltaX = voxelSizeX / abs(direction[0]);
	double tDeltaY = voxelSizeY / abs(direction[1]);
	double tDeltaZ = voxelSizeZ / abs(direction[2]);

	while ((x <= nx) && (x >= 1) && (y <= ny) && (y >= 1) && (z <= nz) && (z >= 1))
	{
		//将网格坐标存入数组
		CoorIJK tt(x, y, z);
		ijk_list.push_back(tt);

		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				x = x + stepX;
				tMaxX = tMaxX + tDeltaX;
			}
			else
			{
				z = z + stepZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				y = y + stepY;
				tMaxY = tMaxY + tDeltaY;
			}
			else
			{
				z = z + stepZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		}
	}
}

void GRID_3D::getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend /*= false*/)
{
	if (isOutBorder(p1) || isOutBorder(p2))
	{
		LOGI << "the data is out of boder!" << std::endl;

		return;
	}

	if (!isAppend)	codes.clear();

	//起点终点检查是否相同
	if (p1 == p2)
	{
		codes.push_back(this->toCodeFromXYZ(p1, layer));
	}

	// 考虑平行于坐标轴的直线
	// 情况1 平行于Y轴
	if (p1[0] == p2[0] && p1[2] == p2[2])
	{
		// 对Y坐标换顺序，保证转换为IJK坐标时，p2点的J坐标更大
		if (p1[1] > p2[1])
		{
			std::swap(p1[1], p2[1]);
		}
		CoorIJK temp_IJK1 = this->toIJKFromXYZ(p1, layer);
		CoorIJK temp_IJK2 = this->toIJKFromXYZ(p2, layer);
		for (int i = 0; i <= temp_IJK2[1] - temp_IJK1[1]; i++)
		{
			CoorIJK temp_IJK = temp_IJK1;
			temp_IJK[1] += i;
			codes.push_back(this->toCodeFromIJK(temp_IJK, layer));
		}
		return;
	}

	// 情况2 平行于Z轴
	else if (p1[0] == p2[0] && p1[1] == p2[1])
	{
		// 对Y坐标换顺序，保证转换为IJK坐标时，p2点的J坐标更大
		if (p1[2] > p2[2])
		{
			std::swap(p1[2], p2[2]);
		}
		CoorIJK temp_IJK1 = this->toIJKFromXYZ(p1, layer);
		CoorIJK temp_IJK2 = this->toIJKFromXYZ(p2, layer);
		for (int i = 0; i <= temp_IJK2[2] - temp_IJK1[2]; i++)
		{
			CoorIJK temp_IJK = temp_IJK1;
			temp_IJK[2] += i;
			codes.push_back(this->toCodeFromIJK(temp_IJK, layer));
		}
		return;
	}

	// 情况3 平行于X轴
	else if (p1[1] == p2[1] && p1[2] == p2[2])
	{
		// 对Y坐标换顺序，保证转换为IJK坐标时，p2点的J坐标更大
		if (p1[0] > p2[0])
		{
			std::swap(p1[0], p2[0]);
		}
		CoorIJK temp_IJK1 = this->toIJKFromXYZ(p1, layer);
		CoorIJK temp_IJK2 = this->toIJKFromXYZ(p2, layer);
		for (int i = 0; i <= temp_IJK2[0] - temp_IJK1[0]; i++)
		{
			CoorIJK temp_IJK = temp_IJK1;
			temp_IJK[0] += i;
			codes.push_back(this->toCodeFromIJK(temp_IJK, layer));
		}
		return;
	}

	// 情况4 一般情况
	//将p1,p2转到最大层级当成double
	CoorIJK p1xyt_nmax = this->toIJKFromXYZ(p1, 21);
	CoorIJK p2xyt_nmax = this->toIJKFromXYZ(p2, 21);
	CoorXYZ origin, end;
	double deta = 1 << (21 - layer.x);
	origin = p1xyt_nmax.cast<double>() / deta;
	end = p2xyt_nmax.cast<double>() / deta;

	//定义boundbox
	CoorXYZ minBound, maxBound;
	minBound[0] = floor(std::min(origin[0], end[0]));
	minBound[1] = floor(std::min(origin[1], end[1]));
	minBound[2] = floor(std::min(origin[2], end[2]));
	maxBound[0] = ceil(std::max(origin[0], end[0]));
	maxBound[1] = ceil(std::max(origin[1], end[1]));
	maxBound[2] = ceil(std::max(origin[2], end[2]));

	//增加判断，防止外包盒为空
	if (maxBound[0] == minBound[0])
	{
		maxBound[0] = maxBound[0] + 1;
	}
	if (maxBound[1] == minBound[1])
	{
		maxBound[1] = maxBound[1] + 1;
	}
	if (maxBound[2] == minBound[2])
	{
		maxBound[2] = maxBound[2] + 1;
	}

	//根据两个xyt坐标计算[对应层级]网格化后的坐标串
	//amanatides算法(三维线栅格化)
	std::vector<CoorIJK> ijk_list;
	int nx, ny, nz;

	nx = maxBound[0] - minBound[0];
	ny = maxBound[1] - minBound[1];
	nz = maxBound[2] - minBound[2];

	amanatidesWooAlgorithm(origin, end, minBound, maxBound, nx, ny, nz, ijk_list);


	//坐标转换，加上起点的ijt坐标
	std::vector<CoorIJK> ijk_list_real;
	CoorIJK tt;
	for (int i = 0; i < ijk_list.size(); i++)
	{
		tt[0] = ijk_list[i][0] + floor(minBound[0]) - 1;
		tt[1] = ijk_list[i][1] + floor(minBound[1]) - 1;
		tt[2] = ijk_list[i][2] + floor(minBound[2]) - 1;
		ijk_list_real.push_back(tt);
		GridCode code = this->toCodeFromIJK(tt, layer);
		codes.push_back(code);
	}

	// 编码去重
	// 在追加模式下，一般会多次调用该函数，故不在函数中进行去重
	if (!isAppend)
	{
		codeUnique(codes);
	}
	return;
}

void GRID_3D::getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange)
{
	CoorXYZ minPt = this->toXYZFromCode(inputCode);
	CoorXYZ gridLen = this->getGridLen(this->getLevelFromCode(inputCode));
	minRange = minPt - gridLen * 0.5;
	maxRange = minPt + gridLen * 0.5;
}

void GRID_3D::calOccupiedInfo(CoorXYZ origin, CoorXYZ target, double startTime, double endTime, Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap /*= false*/)
{
	// 1 判断是否需要对outputMap初始化
	if (isClearMap) {
		outputMap.clear();
		isClearMap = false;
	}

	time_t startTimeInteger = startTime + 0.5 - 1e-6;
	time_t endTimeInteger = endTime + 0.5 - 1e-6;

	// 2 如果起点终点对应的时刻相同，那么对起点终点连线进行三维网格化
	if (startTimeInteger == endTimeInteger) {
		std::vector<GridCode> lineCodes;
		this->getCodesOfLine(origin, target, layer, lineCodes);

		for (int i = 0; i < lineCodes.size(); i++) {
			if (outputMap[lineCodes[i]].size() < 2) {
				outputMap[lineCodes[i]].push_back(startTimeInteger);
				outputMap[lineCodes[i]].push_back(endTimeInteger);
			}
			else {
				outputMap[lineCodes[i]][0] = std::min(outputMap[lineCodes[i]][0], startTimeInteger);
				outputMap[lineCodes[i]][1] = std::max(outputMap[lineCodes[i]][0], endTimeInteger);
			}
		}
	}
	// 3 如果时间不同
	else {
		// 对起点和终点网格化
		GridCode originCode = this->toCodeFromXYZ(origin, layer);
		GridCode targetCode = this->toCodeFromXYZ(target, layer);

		// 3.1 如果在相同的空间编码中，则占用为对应的时间段
		if (originCode == targetCode) {
			if (outputMap[originCode].size() < 2) {
				outputMap[originCode].push_back(startTimeInteger);
				outputMap[originCode].push_back(endTimeInteger);
			}
			else {
				outputMap[originCode][0] = std::min(outputMap[originCode][0], startTimeInteger);
				outputMap[originCode][1] = std::max(outputMap[originCode][0], endTimeInteger);
			}
			return;
		}
		// 3.2 如果在不同的空间编码中，则使用二分法
		else {
			Eigen::Vector3d midPt = 0.5 * (origin + target);
			double midTime = 0.5 * (startTime + endTime);
			calOccupiedInfo(origin, midPt, startTime, midTime, layer, outputMap, isClearMap);
			calOccupiedInfo(midPt, target, midTime, endTime, layer, outputMap, isClearMap);
		}
	}
}

void GRID_3D::initBorderRange(CoorXYZ centerPt, CoorXYZ gridLen, int gridLayer /*= 21*/)
{
	int gridNum = 1 << gridLayer;
	CoorXYZ borderLen = gridLen * gridNum;

	this->minBorder = centerPt - borderLen * 0.5;
	this->maxBorder = centerPt + borderLen * 0.5;

	this->gridLenMap.clear();
	this->gridLenMap.insert(std::make_pair(gridLayer, gridLen));
}

GRID_3D::GRID_3D(CoorXYZ minBorder_, CoorXYZ maxBorder_, CoorType coorType_ /*= CoorType::CartesianCS_3D*/) {
	this->minBorder = std::move(minBorder_);
	this->maxBorder = std::move(maxBorder_);
	this->coorType = coorType_;
}

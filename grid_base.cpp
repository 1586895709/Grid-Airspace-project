#include "grid_base.h"

void GridBase::initBorderRange(CoorXYZ& minBorder_, CoorXYZ& maxBorder_, CoorType coorType_ /*= CoorType::Default*/)
{
	// 1 如果坐标类型为Default或与网格框架坐标类型相同，则直接赋值
	if (coorType_ == CoorType::Default || coorType_ == this->coorType) {
		this->minBorder = minBorder_;
		this->maxBorder = maxBorder_;
	}
	// 2 如果网格框架类型为三维笛卡尔坐标系，输入坐标类型为地理坐标系，则进行经纬度-->墨卡托转换
	else if (this->coorType == CoorType::CartesianCS_3D && coorType_ == CoorType::GeoElevationCS) {
		this->minBorder = latLonToMercator(minBorder);
		this->maxBorder = latLonToMercator(maxBorder);
	}
	// 3 如果网格框架类型为地理坐标系，输入坐标类型为三维笛卡尔坐标系，则进行墨卡托-->经纬度转换
	else if (this->coorType == CoorType::GeoElevationCS && coorType_ == CoorType::CartesianCS_3D) {
		this->minBorder = mercatorToLatLon(minBorder);
		this->maxBorder = mercatorToLatLon(maxBorder);
	}
}

bool GridBase::isOutBorder(const CoorXYZ& inputPt)
{
	if (inputPt[0] >= this->minBorder[0] && inputPt[0] <= this->maxBorder[0]
		&& inputPt[1] >= this->minBorder[1] && inputPt[1] <= this->maxBorder[1]
		&& inputPt[2] >= this->minBorder[2] && inputPt[2] <= this->maxBorder[2]) {
		return false;
	}
	else {
		return true;
	}
}

void GridBase::getCoorRange(Level level, CoorIJK& minIJK, CoorIJK& maxIJK)
{
	minIJK = CoorIJK(0, 0, 0);
	maxIJK = CoorIJK(
		(1 << level.x) - 1,
		(1 << level.y) - 1,
		(1 << level.z) - 1);
}

std::vector<GridCode> GridBase::toCodesFromIJKs(std::vector<CoorIJK>& inputIJKs, Level level)
{
	std::vector<GridCode> result;
	for (int i = 0; i < inputIJKs.size(); i++) {
		result.push_back(this->toCodeFromIJK(inputIJKs[i], level));
	}
	return result;
}

double GridBase::calDis(CoorXYZ pt1, CoorXYZ pt2)
{
	if (this->coorType == CoorType::CartesianCS_3D) {
		return (pt1 - pt2).norm();
	}

	CoorXYZ pt1Mercator = latLonToMercator(pt1);
	CoorXYZ pt2Mercator = latLonToMercator(pt2);

	return (pt1Mercator - pt2Mercator).norm();
}

int GridBase::getMidN(GridCode& midCode, int level)
{
	GridCode mid0 = (midCode & L[level]) >> NN[level];
	if (mid0)
	{
		midCode = mid0;
		return 0;
	}
	midCode = midCode & R[level];
	return NN[level];
}

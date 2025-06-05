#include "PlanningMap.h"

std::map<time_t, std::vector<GridCode>> PlanningMap::dynamicMapConversation(std::map<GridCode, TimeInterval>& inputDynamicMap)
{
	std::map<time_t, std::vector<GridCode>> tempDynamicInfor;
	for (auto iter = inputDynamicMap.begin(); iter != inputDynamicMap.end(); iter++)
	{
		for (time_t tempTime = iter->second[0]; tempTime <= iter->second[1]; tempTime++) {
			tempDynamicInfor[tempTime].push_back(iter->first);
		}
	}
	return tempDynamicInfor;
}

std::map<GridCode, std::vector<time_t>> PlanningMap::dynamicMapConversation(std::map<time_t, std::vector<GridCode>>& inputDynamicMap)
{
	std::map<GridCode, std::vector<time_t>> result;
	for (auto tempDynamicObject : inputDynamicMap) {
		for (auto tempCode : tempDynamicObject.second) {
			result[tempCode].push_back(tempDynamicObject.first);
		}
	}
	return result;
}

std::map<GridCode, std::vector<TimeInterval>> PlanningMap::dynamicMapConversation(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap)
{
	// 1 重新组织数据结构
	std::map<GridCode, std::vector<TimeInterval>> tempMap;
	for (auto mapIter = inputDynamicObsMap.begin(); mapIter != inputDynamicObsMap.end(); mapIter++) {
		for (int j = 0; j < mapIter->second.size(); j++) {
			tempMap[mapIter->second[j]].push_back(mapIter->first);
		}
	}
	return tempMap;
}

void PlanningMap::setBoder(CoorXYZ _minBoder, CoorXYZ _maxBoder)
{
	this->minBorder = _minBoder;
	this->maxBorder = _maxBoder;
}

bool PlanningMap::isInBorder(CoorXYZ& inputPoint)
{
	if (lazyConstructFlag) {
		if (inputPoint[0] <= maxBorder[0] && inputPoint[0] >= minBorder[0]
			&& inputPoint[1] <= maxBorder[1] && inputPoint[1] >= minBorder[1]
			&& inputPoint[2] <= maxBorder[2] && inputPoint[2] >= minBorder[2]) {
			return true;
		}
		return false;
	}
	else {
		return isInBorder(this->pGridObjectPtr->toCodeFromXYZ(inputPoint, this->gridLevel));
	}
}

bool PlanningMap::isInBorder(CoorIJK& inputIJK)
{
	if (lazyConstructFlag) {
		if (inputIJK[0] <= minBorderIJK[0] && inputIJK[0] >= maxBorderIJK[0]
			&& inputIJK[1] <= minBorderIJK[1] && inputIJK[1] >= maxBorderIJK[1]
			&& inputIJK[2] <= minBorderIJK[2] && inputIJK[2] >= maxBorderIJK[2]) {
			return true;
		}
		return false;
	}
	else {
		return isInBorder(this->pGridObjectPtr->toCodeFromIJK(inputIJK, gridLevel));
	}

}

bool PlanningMap::isInBorder(GridCode inputCode)
{
	if (lazyConstructFlag) {
		CoorXYZ inputPt = this->pGridObjectPtr->toXYZFromCode(inputCode);
		return isInBorder(inputPt);
	}
	else {
		auto tempFindResult = this->codeMap.find(inputCode);
		if (tempFindResult == this->codeMap.end()) return false;
		return true;
	}
}

void PlanningMap::initialMap(Level mapLevel)
{
	minBorderIJK = this->pGridObjectPtr->toIJKFromXYZ(minBorder, this->gridLevel);
	maxBorderIJK = this->pGridObjectPtr->toIJKFromXYZ(maxBorder, this->gridLevel);

	LOGI << "地图初始化规模\n";
	LOGI << "起始网格坐标：" << minBorderIJK.transpose() << std::endl;
	LOGI << "终止网格坐标：" << maxBorderIJK.transpose() << std::endl;
	LOGI << "网格坐标长度：" << (maxBorderIJK - minBorderIJK).transpose() << std::endl;

	for (int i = minBorderIJK[0]; i <= maxBorderIJK[0]; i++) {
		for (int j = minBorderIJK[1]; j <= maxBorderIJK[1]; j++) {
			for (int k = minBorderIJK[2]; k <= maxBorderIJK[2]; k++) {
				GridCode tempCode = this->pGridObjectPtr->toCodeFromIJK(CoorIJK(i, j, k), this->gridLevel);
				GridInfor tempGridInfor;
				tempGridInfor.code = tempCode;
				tempGridInfor.velFactor = defaultVelFactor;
				this->codeMap.insert(std::make_pair(tempCode, tempGridInfor));
			}
		}
	}
}

void PlanningMap::constructMap(vector<GridCode>& mapCodes)
{
	this->lazyConstructFlag = false;
	for (int i = 0; i < mapCodes.size(); i++) {
		GridInfor tempGridInfor;
		tempGridInfor.code = mapCodes[i];
		tempGridInfor.isObs = false;
		tempGridInfor.velFactor = defaultVelFactor;
		this->codeMap.insert(std::make_pair(mapCodes[i], tempGridInfor));
	}
}

bool PlanningMap::updateMapWithStaticObs(std::vector<GridCode>& inputStaticObsCodes)
{
	for (int i = 0; i < inputStaticObsCodes.size(); i++) {
		GridCode tempCode = inputStaticObsCodes[i];
		if (isInBorder(tempCode)) { // 判断输入的网格编码是否在地图矢量范围内，如果不在不进行更新
			auto tempFindResult = this->findCodeMapElement(tempCode);
			tempFindResult->second.code = tempCode;
			tempFindResult->second.isObs = true;
		}
	}

	return true;
}

bool PlanningMap::updateMapWithDynamicObs(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap)
{
	// 1 重新组织数据结构
	std::map<GridCode, std::vector<TimeInterval>> tempMap = dynamicMapConversation(inputDynamicObsMap);

	// 2 对地图进行更新
	return updateMapWithDynamicObs(tempMap);
}

bool PlanningMap::updateMapWithDynamicObs(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap)
{
	// 对地图进行更新
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // 判断网格编码是否位于地图范围内
			auto tempFindResult = this->findCodeMapElement(tempMapIter->first);
			for (int i = 0; i < tempMapIter->second.size(); i++) {
				tempFindResult->second.occupy_interval_set.insert(tempMapIter->second[i]);
			}
		}
	}
	return true;
}

bool PlanningMap::updateMapWithDynamicObs(std::map<time_t, std::vector<GridCode>>& inputDynamicObsMap)
{
	// 对动态信息存储容器进行类型转换，转换后以位置变化为键
	std::map<GridCode, std::vector<time_t>> tempDynamicObsInfor = dynamicMapConversation(inputDynamicObsMap);

	// 遍历动态信息存储容器
	for (auto tempDynamicObject : tempDynamicObsInfor) {
		// 如果该对象不在地图范围内，则跳过更新
		if (!this->isInBorder(tempDynamicObject.first)) continue;

		// 获取对应编码的网格信息
		auto tempFindResult = this->findCodeMapElement(tempDynamicObject.first);
		for (auto tempTime : tempDynamicObject.second) {
			tempFindResult->second.occupy_set.insert(tempTime);
		}
	}

	return true;
}

bool PlanningMap::updateMapWithDynamicObs(std::map<GridCode, TimeInterval>& inputDynamicObsMap)
{
	// 对地图进行更新
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // 判断网格编码是否位于地图范围内
			auto tempFindResult = this->findCodeMapElement(tempMapIter->first);
			tempFindResult->second.occupy_interval_set.insert(tempMapIter->second);
		}
	}
	return true;
}

bool PlanningMap::updateMapWithDynamicObsWithId(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap, string objectId)
{
	// 1 重新组织数据结构
	std::map<GridCode, std::vector<TimeInterval>> tempMap = dynamicMapConversation(inputDynamicObsMap);

	// 2 对地图进行更新
	return updateMapWithDynamicObsWithId(tempMap, objectId);
}

bool PlanningMap::updateMapWithDynamicObsWithId(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap, string objectId)
{
	// 对地图进行更新
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // 判断网格编码是否位于地图范围内
			auto tempFindResult = this->findCodeMapElement(tempMapIter->first);
			for (int i = 0; i < tempMapIter->second.size(); i++) {
				tempFindResult->second.occupyIntervalSetIdMap[objectId].insert(tempMapIter->second[i]);
			}
		}
	}
	return true;
}

bool PlanningMap::updatePositiveArea(
	std::map<TimeInterval, std::vector<GridCode>>& inputPositiveAreaCodes, double positiveAreaVelFactor /*= 1*/)
{
	// 1 重新组织数据结构
	std::map<GridCode, std::vector<TimeInterval>> tempMap;
	for (auto mapIter = inputPositiveAreaCodes.begin(); mapIter != inputPositiveAreaCodes.end(); mapIter++) {
		for (int j = 0; j < mapIter->second.size(); j++) {
			tempMap[mapIter->second[j]].push_back(mapIter->first);
		}
	}

	// 2 对地图进行更新
	return updatePositiveArea(tempMap, positiveAreaVelFactor);
}

bool PlanningMap::updatePositiveArea(std::map<GridCode, std::vector<TimeInterval>>& inputPositiveAreaCodes, double positiveAreaVelFactor /*= 1*/)
{
	this->considerPositiveArea = true;

	// 对地图进行更新
	for (auto tempMapIter = inputPositiveAreaCodes.begin(); tempMapIter != inputPositiveAreaCodes.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // 判断网格编码是否位于地图范围内
			auto tempFindResult = this->findCodeMapElement(tempMapIter->first);
			for (int i = 0; i < tempMapIter->second.size(); i++) {
				tempFindResult->second.timeVelFactorMap.insert(make_pair(tempMapIter->second[i], positiveAreaVelFactor));
			}
		}
	}
	return true;
}

bool PlanningMap::isOccupied(GridCode inputCode, time_t queryTime)
{
	if (!isInBorder(inputCode)) return true; // 如果输入编码不在地图范围内，则直接返回true，认为在地图外也是被占用的

	auto tempFindResult = this->codeMap.find(inputCode);
	if (tempFindResult != this->codeMap.end()) {
		// 静态障碍物判断
		if (tempFindResult->second.isObs) return true;
		// 进行时间维度的判断
		// 1 离散时间点判断
		if (tempFindResult->second.occupy_set.count(queryTime)) return true;
		// 2 时间区间判断
		for (auto tempTimeInterval : tempFindResult->second.occupy_interval_set) {
			if (tempTimeInterval[0] <= queryTime && tempTimeInterval[1] >= queryTime) return true;
		}
	}

	return false;
}

bool PlanningMap::isOccupied(
	std::vector<std::pair<time_t, CoorXYZ>>& inputMotionInfor)
{
	for (int i = 0; i < inputMotionInfor.size(); i++) {
		GridCode tempCode = this->pGridObjectPtr->toCodeFromXYZ(inputMotionInfor[i].second, this->gridLevel);
		if (isOccupied(tempCode, inputMotionInfor[i].first)) return true;
	}
	return false;
}

bool PlanningMap::isOccupied(std::map<GridCode, TimeInterval>& dynamicResult)
{
	// 根据时空网格化结果信息判断是否与地图障碍存在冲突
	for (auto dynamicIter = dynamicResult.begin(); dynamicIter != dynamicResult.end(); dynamicIter++) {
		auto tempFindResult = this->codeMap.find(dynamicIter->first);
		if (tempFindResult != this->codeMap.end()) {
			// 静态障碍物判断
			if (tempFindResult->second.isObs || fabs(tempFindResult->second.velFactor) < 1e-6) return true;
			// 进行时间维度的判断
			// 1 离散时间点判断
			for (auto tempFindResultTime : tempFindResult->second.occupy_set) {
				if (tempFindResultTime >= dynamicIter->second[0] && tempFindResultTime <= dynamicIter->second[1]) {
					return true;
				}
			}
			// 2 时间区间判断
			for (auto tempTimeInterval : tempFindResult->second.occupy_interval_set) {
				if (tempTimeInterval[0] > dynamicIter->second[1] || tempTimeInterval[1] < dynamicIter->second[0]) continue;
				return true;
			}
		}
	}
	return false;
}

bool PlanningMap::isOccupied(std::map<GridCode, vector<double>>& dynamicResult)
{
	// 根据时空网格化结果信息判断是否与地图障碍存在冲突
	for (auto dynamicIter = dynamicResult.begin(); dynamicIter != dynamicResult.end(); dynamicIter++) {
		auto tempFindResult = this->codeMap.find(dynamicIter->first);
		if (tempFindResult != this->codeMap.end()) {
			// 静态障碍物判断
			if (tempFindResult->second.isObs || fabs(tempFindResult->second.velFactor) < 1e-6) {
				return true;
			}
			// 进行时间维度的判断
			// 1 离散时间点判断
			for (auto tempFindResultTime : tempFindResult->second.occupy_set) {
				if (tempFindResultTime >= dynamicIter->second[0] && tempFindResultTime <= dynamicIter->second[1]) {
					return true;
				}
			}
			// 2 时间区间判断
			for (auto tempTimeInterval : tempFindResult->second.occupy_interval_set) {
				if (tempTimeInterval[0] > dynamicIter->second[1] || tempTimeInterval[1] < dynamicIter->second[0]) continue;
				return true;
			}
		}
		else if (!this->isInBorder(dynamicIter->first)) {
			return true; // 在精细构建地图的过程中，如果在codeMap中找不到对应编码，则认为越界
		}
	}
	return false;
}

bool PlanningMap::isOccupied(std::map<time_t, set<GridCode>>& dynamicResult)
{
	for (auto dynamicIter = dynamicResult.begin(); dynamicIter != dynamicResult.end(); dynamicIter++) {
		for (auto tempCode : dynamicIter->second) {
			if (isOccupied(tempCode, dynamicIter->first)) return true;
		}
	}
	return false;
}

bool PlanningMap::isOccupied(GridCode code)
{
	auto tempFindResult = this->codeMap.find(code);
	if (tempFindResult != this->codeMap.end()) {
		if (tempFindResult->second.isObs) return true;
		if (fabs(tempFindResult->second.velFactor) < 1e-6) return true;
	}

	return false;
}

bool PlanningMap::isOccupied(vector<GridCode>& codes)
{
	for (int i = 0; i < codes.size(); i++) {
		if (isOccupied(codes[i])) return true;
	}
	return false;
}

bool PlanningMap::isOccupied(CoorXYZ inputPt, time_t queryTime)
{
	GridCode tempCode = this->pGridObjectPtr->toCodeFromXYZ(inputPt, this->gridLevel);
	return isOccupied(tempCode, queryTime);
}

bool PlanningMap::isAtBorder(GridCode inputCode, int border /*= 1*/)
{
	// 如果没有明确的地图编码集合数据，则认为输入编码一定不在边界上
	if (this->lazyConstructFlag == true) return false;

	// 计算水平方向外扩 border 的距离的所有网格编码
	CoorIJK inputCoor = this->pGridObjectPtr->toIJKFromCode(inputCode);
	vector<GridCode> neighborCodes;
	for (int i = -border; i <= border; i++) {
		for (int j = -border; j <= border; j++) {
			if (i == 0 && j == 0) continue;
			CoorIJK tempCoor = inputCoor;
			tempCoor[0] += i;
			tempCoor[1] += j;
			GridCode tempCode = this->pGridObjectPtr->toCodeFromIJK(tempCoor, gridLevel);
			neighborCodes.push_back(tempCode);
		}
	}

	// 邻域集合中如果有不在地图范围内的网格编码，则认为在边界上
	for (int i = 0; i < neighborCodes.size(); i++) {
		if (!this->isInBorder(neighborCodes[i])) return true;
	}

	return false;
}

void PlanningMap::outputStaticObs(std::string fileName, bool isLog /*= true*/, bool mercatorFlag /*= true*/)
{
	std::vector<GridCode> staticCodes;
	// 遍历地图，获取其中为静态障碍物的网格（isObs为true）
	for (auto tempMapObject : this->codeMap) {
		if (tempMapObject.second.isObs == true) {
			staticCodes.push_back(tempMapObject.second.code);
		}
	}

	// 网格化输出
	VTK_Visualization::outputVTKformatXYZ(staticCodes, pGridObjectPtr, fileName, false);
}

void PlanningMap::setMapType(PlanningMapType inputMapType)
{
	this->pPlanningMapType = inputMapType;
}

PlanningMapType PlanningMap::getMapType()
{
	return this->pPlanningMapType;
}

CodeMap::iterator PlanningMap::findCodeMapElement(GridCode inputCode)
{
	auto tempFindResult = this->codeMap.find(inputCode);
	if (tempFindResult != this->codeMap.end()) {
		return tempFindResult;
	}
	else {
		GridInfor tempGridInfor;
		tempGridInfor.code = inputCode;
		tempGridInfor.velFactor = defaultVelFactor;
		tempGridInfor.isObs = false;
		auto tempInsertResult = this->codeMap.insert(make_pair(inputCode, tempGridInfor));
		return tempInsertResult.first;
	}
}

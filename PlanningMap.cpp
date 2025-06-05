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
	// 1 ������֯���ݽṹ
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

	LOGI << "��ͼ��ʼ����ģ\n";
	LOGI << "��ʼ�������꣺" << minBorderIJK.transpose() << std::endl;
	LOGI << "��ֹ�������꣺" << maxBorderIJK.transpose() << std::endl;
	LOGI << "�������곤�ȣ�" << (maxBorderIJK - minBorderIJK).transpose() << std::endl;

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
		if (isInBorder(tempCode)) { // �ж��������������Ƿ��ڵ�ͼʸ����Χ�ڣ�������ڲ����и���
			auto tempFindResult = this->findCodeMapElement(tempCode);
			tempFindResult->second.code = tempCode;
			tempFindResult->second.isObs = true;
		}
	}

	return true;
}

bool PlanningMap::updateMapWithDynamicObs(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap)
{
	// 1 ������֯���ݽṹ
	std::map<GridCode, std::vector<TimeInterval>> tempMap = dynamicMapConversation(inputDynamicObsMap);

	// 2 �Ե�ͼ���и���
	return updateMapWithDynamicObs(tempMap);
}

bool PlanningMap::updateMapWithDynamicObs(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap)
{
	// �Ե�ͼ���и���
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // �ж���������Ƿ�λ�ڵ�ͼ��Χ��
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
	// �Զ�̬��Ϣ�洢������������ת����ת������λ�ñ仯Ϊ��
	std::map<GridCode, std::vector<time_t>> tempDynamicObsInfor = dynamicMapConversation(inputDynamicObsMap);

	// ������̬��Ϣ�洢����
	for (auto tempDynamicObject : tempDynamicObsInfor) {
		// ����ö����ڵ�ͼ��Χ�ڣ�����������
		if (!this->isInBorder(tempDynamicObject.first)) continue;

		// ��ȡ��Ӧ�����������Ϣ
		auto tempFindResult = this->findCodeMapElement(tempDynamicObject.first);
		for (auto tempTime : tempDynamicObject.second) {
			tempFindResult->second.occupy_set.insert(tempTime);
		}
	}

	return true;
}

bool PlanningMap::updateMapWithDynamicObs(std::map<GridCode, TimeInterval>& inputDynamicObsMap)
{
	// �Ե�ͼ���и���
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // �ж���������Ƿ�λ�ڵ�ͼ��Χ��
			auto tempFindResult = this->findCodeMapElement(tempMapIter->first);
			tempFindResult->second.occupy_interval_set.insert(tempMapIter->second);
		}
	}
	return true;
}

bool PlanningMap::updateMapWithDynamicObsWithId(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap, string objectId)
{
	// 1 ������֯���ݽṹ
	std::map<GridCode, std::vector<TimeInterval>> tempMap = dynamicMapConversation(inputDynamicObsMap);

	// 2 �Ե�ͼ���и���
	return updateMapWithDynamicObsWithId(tempMap, objectId);
}

bool PlanningMap::updateMapWithDynamicObsWithId(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap, string objectId)
{
	// �Ե�ͼ���и���
	for (auto tempMapIter = inputDynamicObsMap.begin(); tempMapIter != inputDynamicObsMap.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // �ж���������Ƿ�λ�ڵ�ͼ��Χ��
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
	// 1 ������֯���ݽṹ
	std::map<GridCode, std::vector<TimeInterval>> tempMap;
	for (auto mapIter = inputPositiveAreaCodes.begin(); mapIter != inputPositiveAreaCodes.end(); mapIter++) {
		for (int j = 0; j < mapIter->second.size(); j++) {
			tempMap[mapIter->second[j]].push_back(mapIter->first);
		}
	}

	// 2 �Ե�ͼ���и���
	return updatePositiveArea(tempMap, positiveAreaVelFactor);
}

bool PlanningMap::updatePositiveArea(std::map<GridCode, std::vector<TimeInterval>>& inputPositiveAreaCodes, double positiveAreaVelFactor /*= 1*/)
{
	this->considerPositiveArea = true;

	// �Ե�ͼ���и���
	for (auto tempMapIter = inputPositiveAreaCodes.begin(); tempMapIter != inputPositiveAreaCodes.end(); tempMapIter++) {
		if (isInBorder(tempMapIter->first)) { // �ж���������Ƿ�λ�ڵ�ͼ��Χ��
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
	if (!isInBorder(inputCode)) return true; // ���������벻�ڵ�ͼ��Χ�ڣ���ֱ�ӷ���true����Ϊ�ڵ�ͼ��Ҳ�Ǳ�ռ�õ�

	auto tempFindResult = this->codeMap.find(inputCode);
	if (tempFindResult != this->codeMap.end()) {
		// ��̬�ϰ����ж�
		if (tempFindResult->second.isObs) return true;
		// ����ʱ��ά�ȵ��ж�
		// 1 ��ɢʱ����ж�
		if (tempFindResult->second.occupy_set.count(queryTime)) return true;
		// 2 ʱ�������ж�
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
	// ����ʱ�����񻯽����Ϣ�ж��Ƿ����ͼ�ϰ����ڳ�ͻ
	for (auto dynamicIter = dynamicResult.begin(); dynamicIter != dynamicResult.end(); dynamicIter++) {
		auto tempFindResult = this->codeMap.find(dynamicIter->first);
		if (tempFindResult != this->codeMap.end()) {
			// ��̬�ϰ����ж�
			if (tempFindResult->second.isObs || fabs(tempFindResult->second.velFactor) < 1e-6) return true;
			// ����ʱ��ά�ȵ��ж�
			// 1 ��ɢʱ����ж�
			for (auto tempFindResultTime : tempFindResult->second.occupy_set) {
				if (tempFindResultTime >= dynamicIter->second[0] && tempFindResultTime <= dynamicIter->second[1]) {
					return true;
				}
			}
			// 2 ʱ�������ж�
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
	// ����ʱ�����񻯽����Ϣ�ж��Ƿ����ͼ�ϰ����ڳ�ͻ
	for (auto dynamicIter = dynamicResult.begin(); dynamicIter != dynamicResult.end(); dynamicIter++) {
		auto tempFindResult = this->codeMap.find(dynamicIter->first);
		if (tempFindResult != this->codeMap.end()) {
			// ��̬�ϰ����ж�
			if (tempFindResult->second.isObs || fabs(tempFindResult->second.velFactor) < 1e-6) {
				return true;
			}
			// ����ʱ��ά�ȵ��ж�
			// 1 ��ɢʱ����ж�
			for (auto tempFindResultTime : tempFindResult->second.occupy_set) {
				if (tempFindResultTime >= dynamicIter->second[0] && tempFindResultTime <= dynamicIter->second[1]) {
					return true;
				}
			}
			// 2 ʱ�������ж�
			for (auto tempTimeInterval : tempFindResult->second.occupy_interval_set) {
				if (tempTimeInterval[0] > dynamicIter->second[1] || tempTimeInterval[1] < dynamicIter->second[0]) continue;
				return true;
			}
		}
		else if (!this->isInBorder(dynamicIter->first)) {
			return true; // �ھ�ϸ������ͼ�Ĺ����У������codeMap���Ҳ�����Ӧ���룬����ΪԽ��
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
	// ���û����ȷ�ĵ�ͼ���뼯�����ݣ�����Ϊ�������һ�����ڱ߽���
	if (this->lazyConstructFlag == true) return false;

	// ����ˮƽ�������� border �ľ���������������
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

	// ���򼯺�������в��ڵ�ͼ��Χ�ڵ�������룬����Ϊ�ڱ߽���
	for (int i = 0; i < neighborCodes.size(); i++) {
		if (!this->isInBorder(neighborCodes[i])) return true;
	}

	return false;
}

void PlanningMap::outputStaticObs(std::string fileName, bool isLog /*= true*/, bool mercatorFlag /*= true*/)
{
	std::vector<GridCode> staticCodes;
	// ������ͼ����ȡ����Ϊ��̬�ϰ��������isObsΪtrue��
	for (auto tempMapObject : this->codeMap) {
		if (tempMapObject.second.isObs == true) {
			staticCodes.push_back(tempMapObject.second.code);
		}
	}

	// �������
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

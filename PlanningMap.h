#pragma once
#include <map>
#include <unordered_map>
#include <set>
#include "util_function.h"
#include "grid_base.h"
#include "vtk_visualization.h"

using std::vector;
using std::set;

class PlanningMap;
typedef std::shared_ptr<PlanningMap> PlanningMapPtr;

struct MultiLayerPlanningMap {
	PlanningMapPtr fineMapPtr;			// ϸ���ȹ滮��ͼָ��
	PlanningMapPtr coarseMapPtr;		// �����ȹ滮��ͼָ��
};
typedef std::shared_ptr<MultiLayerPlanningMap> MultiLayerPlanningMapPtr;

// ��������
enum class PlanningMapType {
	NonUniformMap,		// �Ǿ��������ͼ
	BinaryMap			// ��ֵ��ͼ
};

class PlanningMap
{
private:
	GridBasePtr pGridObjectPtr = NULL;	// ���񻯱������
	PlanningMapType pPlanningMapType;   // ��ͼ����ֵ����

	std::map<time_t, std::vector<GridCode>> dynamicMapConversation(std::map<GridCode, TimeInterval>& inputDynamicMap);
	std::map<GridCode, std::vector<time_t>> dynamicMapConversation(std::map<time_t, std::vector<GridCode>>& inputDynamicMap);
	std::map<GridCode, std::vector<TimeInterval>> dynamicMapConversation(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap);

public:
	bool lazyConstructFlag = true;
	double defaultVelFactor = 1;
	bool considerPositiveArea = false;

public:
	Level gridLevel;
	CoorXYZ minBorder;									// �߽���С��Χ��ʸ��ֵ
	CoorXYZ maxBorder;									// �߽����Χ��ʸ��ֵ

	CoorIJK minBorderIJK;								// �߽���С��Χ������ֵ
	CoorIJK maxBorderIJK;								// �߽����Χ������ֵ

	CodeMap codeMap;

public:
	PlanningMap() {};
	PlanningMap(GridBasePtr _gridObjectPtr, Level _level, CodeMap& _codeMap) {
		this->pGridObjectPtr = _gridObjectPtr;
		this->gridLevel = _level;
		this->lazyConstructFlag = false;

		this->codeMap = _codeMap;
	}
	PlanningMap(GridBasePtr _gridObjectPtr, CoorXYZ _minBorder, CoorXYZ _maxBorder,
		Level _level, bool _lazyConstructFlag = true
	) {
		this->pGridObjectPtr = _gridObjectPtr;
		this->maxBorder = _maxBorder;
		this->minBorder = _minBorder;
		this->gridLevel = _level;

		this->minBorderIJK = pGridObjectPtr->toIJKFromXYZ(this->minBorder, this->gridLevel);
		this->maxBorderIJK = pGridObjectPtr->toIJKFromXYZ(this->maxBorder, this->gridLevel);

		lazyConstructFlag = _lazyConstructFlag;
	}

	/**
	 * @brief ���õ�ͼ��Χ��Ϣ
	 * @param _minBorder ��ͼ��С��Χ
	 * @param _maxBorder ��ͼ���Χ
	 * @param _coorType ��Χ��������ͣ�Ĭ��Ϊ��ά�ѿ�������ϵ
	*/
	void setBoder(
		CoorXYZ _minBoder, CoorXYZ _maxBoder);

	/**
	 * @brief �ж�������Ƿ��ڵ�ͼ��Χ��
	 * @param inputPoint ��������Ϣ
	 * @return true���ڵ�ͼ��Χ�ڣ�false�����ڵ�ͼ��Χ��
	*/
	bool isInBorder(CoorXYZ& inputPoint);
	bool isInBorder(CoorIJK& inputIJK);
	bool isInBorder(GridCode inputCode);

	/**
	 * @brief ��ʼ��������ͼ���ڷ�Χ�ϴ�ʱ����ʱ�ϳ�
	 * @param gridLayer ����㼶
	*/
	void initialMap(Level mapLevel);

	/**
	 * @brief ���ݵ�ͼ������Ϣ��������ͼ����ͨ������lazyConstructFlagΪfalse�����
	*/
	void constructMap(vector<GridCode>& mapCodes);

	/**
	 * @brief ���ݾ�̬�ϰ�����Ϣ�Ե�ͼ���и���
	 * @param inputStaticObsCodes ��̬�ϰ����������
	 * @return ����״̬��Ϣ�����³ɹ�Ϊtrue������Ϊfalse
	*/
	bool updateMapWithStaticObs(std::vector<GridCode>& inputStaticObsCodes);

	/**
	 * @brief ���ݶ�̬�ϰ�����Ϣ�Ե�ͼ���и���
	 * @param inputDynamicObsCodes ��̬�ϰ��� {ʱ�����䡢������뼯��}
	 * @return ����״̬��Ϣ�����³ɹ�Ϊtrue������Ϊfalse
	*/
	bool updateMapWithDynamicObs(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<GridCode, TimeInterval>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<time_t, std::vector<GridCode>>& inputDynamicObsMap);

	bool updateMapWithDynamicObsWithId(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap, string objectId);
	bool updateMapWithDynamicObsWithId(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap, string objectId);


	/**
	 * @brief ���ݶ�̬���������Ϣ���и���
	 * @param inputPositiveAreaCodes ���������Ϣ
	 * @param positiveAreaVelFactor ������������Ӧ������ֵ
	 * @return
	 */
	bool updatePositiveArea(std::map<TimeInterval, std::vector<GridCode>>& inputPositiveAreaCodes, double positiveAreaVelFactor = 1);
	bool updatePositiveArea(std::map<GridCode, std::vector<TimeInterval>>& inputPositiveAreaCodes, double positiveAreaVelFactor = 1);

	/**
	 * @brief �ж�inputCode��Ӧ�������Ƿ���queryTimeʱ�̱�ռ��
	 * @param inputCode �������
	 * @param queryTime ��ѯʱ��
	 * @return
	*/
	bool isOccupied(GridCode inputCode, time_t queryTime);

	/**
	 * @brief �ж�inputPt��Ӧ�������Ƿ���queryTimeʱ�̱�ռ��
	 * @param inputPt ���������
	 * @param queryTime ��ѯʱ��
	 * @return
	 */
	bool isOccupied(CoorXYZ inputPt, time_t queryTime);

	/**
	 * @brief �жϸ����˶���Ϣ�������Ƿ����ͼ��������������ͻ
	 * @param inputMotionInfor �����˶���Ϣ����һϵ��{ʱ�̣�λ��}��Ԫ��������
	 * @return ��ͻ true; ����ͻ false
	*/
	bool isOccupied(std::vector<std::pair<time_t, CoorXYZ>>& inputMotionInfor);

	/**
	 * @brief �жϸ����˶���Ϣ�������Ƿ����ͼ��������������ͻ
	 * @param inputMotionInfor �����˶���Ϣ����һϵ��״̬���������
	 * @param gridLayer ��ͼ��Ϣ������㼶
	 * @return ��ͻ true; ����ͻ false
	*/
	bool isOccupied(std::map<GridCode, TimeInterval>& dynamicResult);
	bool isOccupied(std::map<GridCode, vector<double>>& dynamicResult);
	bool isOccupied(std::map<time_t, set<GridCode>>& dynamicResult);

	/**
	 * @brief �жϸ�����������Ƿ�Ϊ�ϰ������ͨ��ϵ���Ƿ����0
	 * @param code
	 * @return
	 */
	bool isOccupied(GridCode code);
	bool isOccupied(vector<GridCode>& codes);

	/**
	 * @brief �ж���������Ƿ��ڱ߽���
	 * @param inputCode �������
	 * @param border �߽��������
	 * @return
	 */
	bool isAtBorder(GridCode inputCode, int border = 1);

	/**
	 * @brief ����ͼ�����о�̬������ӻ����
	 * @param fileName ���ӻ�����ļ�·��
	 * @param isLog �Ƿ������־
	*/
	void outputStaticObs(std::string fileName, bool isLog = true, bool mercatorFlag = true);

	void setMapType(PlanningMapType inputMapType);

	PlanningMapType getMapType();

	CodeMap::iterator findCodeMapElement(GridCode inputCode);
};


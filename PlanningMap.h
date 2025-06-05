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
	PlanningMapPtr fineMapPtr;			// 细粒度规划地图指针
	PlanningMapPtr coarseMapPtr;		// 粗粒度规划地图指针
};
typedef std::shared_ptr<MultiLayerPlanningMap> MultiLayerPlanningMapPtr;

// 坐标类型
enum class PlanningMapType {
	NonUniformMap,		// 非均匀网格地图
	BinaryMap			// 二值地图
};

class PlanningMap
{
private:
	GridBasePtr pGridObjectPtr = NULL;	// 网格化编解码类
	PlanningMapType pPlanningMapType;   // 地图量化值类型

	std::map<time_t, std::vector<GridCode>> dynamicMapConversation(std::map<GridCode, TimeInterval>& inputDynamicMap);
	std::map<GridCode, std::vector<time_t>> dynamicMapConversation(std::map<time_t, std::vector<GridCode>>& inputDynamicMap);
	std::map<GridCode, std::vector<TimeInterval>> dynamicMapConversation(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap);

public:
	bool lazyConstructFlag = true;
	double defaultVelFactor = 1;
	bool considerPositiveArea = false;

public:
	Level gridLevel;
	CoorXYZ minBorder;									// 边界最小范围的矢量值
	CoorXYZ maxBorder;									// 边界最大范围的矢量值

	CoorIJK minBorderIJK;								// 边界最小范围的网格值
	CoorIJK maxBorderIJK;								// 边界最大范围的网格值

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
	 * @brief 设置地图范围信息
	 * @param _minBorder 地图最小范围
	 * @param _maxBorder 地图最大范围
	 * @param _coorType 范围坐标的类型，默认为三维笛卡尔坐标系
	*/
	void setBoder(
		CoorXYZ _minBoder, CoorXYZ _maxBoder);

	/**
	 * @brief 判断输入点是否在地图范围内
	 * @param inputPoint 输入点的信息
	 * @return true，在地图范围内；false，不在地图范围内
	*/
	bool isInBorder(CoorXYZ& inputPoint);
	bool isInBorder(CoorIJK& inputIJK);
	bool isInBorder(GridCode inputCode);

	/**
	 * @brief 初始化整个地图，在范围较大时，耗时较长
	 * @param gridLayer 网格层级
	*/
	void initialMap(Level mapLevel);

	/**
	 * @brief 根据地图编码信息，构建地图对象，通常用于lazyConstructFlag为false的情况
	*/
	void constructMap(vector<GridCode>& mapCodes);

	/**
	 * @brief 根据静态障碍物信息对地图进行更新
	 * @param inputStaticObsCodes 静态障碍物网格编码
	 * @return 更新状态信息，更新成功为true，否则为false
	*/
	bool updateMapWithStaticObs(std::vector<GridCode>& inputStaticObsCodes);

	/**
	 * @brief 根据动态障碍物信息对地图进行更新
	 * @param inputDynamicObsCodes 动态障碍物 {时间区间、网格编码集合}
	 * @return 更新状态信息，更新成功为true，否则为false
	*/
	bool updateMapWithDynamicObs(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<GridCode, TimeInterval>& inputDynamicObsMap);
	bool updateMapWithDynamicObs(std::map<time_t, std::vector<GridCode>>& inputDynamicObsMap);

	bool updateMapWithDynamicObsWithId(std::map<TimeInterval, std::vector<GridCode>>& inputDynamicObsMap, string objectId);
	bool updateMapWithDynamicObsWithId(std::map<GridCode, std::vector<TimeInterval>>& inputDynamicObsMap, string objectId);


	/**
	 * @brief 根据动态正向空域信息进行更新
	 * @param inputPositiveAreaCodes 正向空域信息
	 * @param positiveAreaVelFactor 正向空域区域对应的属性值
	 * @return
	 */
	bool updatePositiveArea(std::map<TimeInterval, std::vector<GridCode>>& inputPositiveAreaCodes, double positiveAreaVelFactor = 1);
	bool updatePositiveArea(std::map<GridCode, std::vector<TimeInterval>>& inputPositiveAreaCodes, double positiveAreaVelFactor = 1);

	/**
	 * @brief 判断inputCode对应的网格是否在queryTime时刻被占用
	 * @param inputCode 输入编码
	 * @param queryTime 查询时间
	 * @return
	*/
	bool isOccupied(GridCode inputCode, time_t queryTime);

	/**
	 * @brief 判断inputPt对应的网格是否在queryTime时刻被占用
	 * @param inputPt 输入坐标点
	 * @param queryTime 查询时间
	 * @return
	 */
	bool isOccupied(CoorXYZ inputPt, time_t queryTime);

	/**
	 * @brief 判断给定运动信息的物体是否与地图中其他对象发生冲突
	 * @param inputMotionInfor 物体运动信息，由一系列{时刻，位置}二元组来描述
	 * @return 冲突 true; 不冲突 false
	*/
	bool isOccupied(std::vector<std::pair<time_t, CoorXYZ>>& inputMotionInfor);

	/**
	 * @brief 判断给定运动信息的物体是否与地图中其他对象发生冲突
	 * @param inputMotionInfor 物体运动信息，由一系列状态点进行描述
	 * @param gridLayer 地图信息的网格层级
	 * @return 冲突 true; 不冲突 false
	*/
	bool isOccupied(std::map<GridCode, TimeInterval>& dynamicResult);
	bool isOccupied(std::map<GridCode, vector<double>>& dynamicResult);
	bool isOccupied(std::map<time_t, set<GridCode>>& dynamicResult);

	/**
	 * @brief 判断给定网格编码是否为障碍物或者通行系数是否等于0
	 * @param code
	 * @return
	 */
	bool isOccupied(GridCode code);
	bool isOccupied(vector<GridCode>& codes);

	/**
	 * @brief 判断输入编码是否在边界上
	 * @param inputCode 输入编码
	 * @param border 边界网格距离
	 * @return
	 */
	bool isAtBorder(GridCode inputCode, int border = 1);

	/**
	 * @brief 将地图中所有静态网格可视化输出
	 * @param fileName 可视化输出文件路径
	 * @param isLog 是否输出日志
	*/
	void outputStaticObs(std::string fileName, bool isLog = true, bool mercatorFlag = true);

	void setMapType(PlanningMapType inputMapType);

	PlanningMapType getMapType();

	CodeMap::iterator findCodeMapElement(GridCode inputCode);
};


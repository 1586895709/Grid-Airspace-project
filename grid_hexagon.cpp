#include "grid_hexagon.h"
namespace HexagonGridSystem {
	HexagonGridSystem::Hex hex_direction(int direction)
	{
		return hex_directions[direction];
	}

	HexagonGridSystem::Hex hex_neighbor(Hex hex, int direction)
	{
		return hex_add(hex, hex_direction(direction));
	}

	HexagonGridSystem::Hex hex_diagonal_neighbor(Hex hex, int direction)
	{
		return hex_add(hex, hex_diagonals[direction]);
	}

	int hex_length(Hex hex)
	{
		return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
	}

	int hex_distance(Hex a, Hex b)
	{
		return hex_length(hex_subtract(a, b));
	}

	HexagonGridSystem::Hex hex_round(FractionalHex h)
	{
		int qi = int(round(h.q));
		int ri = int(round(h.r));
		int si = int(round(h.s));
		double q_diff = abs(qi - h.q);
		double r_diff = abs(ri - h.r);
		double s_diff = abs(si - h.s);
		if (q_diff > r_diff && q_diff > s_diff)
		{
			qi = -ri - si;
		}
		else
			if (r_diff > s_diff)
			{
				ri = -qi - si;
			}
			else
			{
				si = -qi - ri;
			}
		return Hex(qi, ri, si);
	}

	HexagonGridSystem::FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t)
	{
		return FractionalHex(a.q * (1.0 - t) + b.q * t, a.r * (1.0 - t) + b.r * t, a.s * (1.0 - t) + b.s * t);
	}

	std::vector<HexagonGridSystem::Hex> hex_linedraw(Hex a, Hex b)
	{
		int N = hex_distance(a, b);
		FractionalHex a_nudge = FractionalHex(a.q + 1e-06, a.r + 1e-06, a.s - 2e-06);
		FractionalHex b_nudge = FractionalHex(b.q + 1e-06, b.r + 1e-06, b.s - 2e-06);
		std::vector<Hex> results = {};
		double step = 1.0 / std::max(N, 1);
		for (int i = 0; i <= N; i++)
		{
			results.push_back(hex_round(hex_lerp(a_nudge, b_nudge, step * i)));
		}
		return results;
	}

	HexagonGridSystem::OffsetCoord qoffset_from_cube(int offset, Hex h)
	{
		int col = h.q;
		int row = h.r + int((h.q + offset * (h.q & 1)) / 2);
		if (offset != EVEN && offset != ODD)
		{
			throw "offset must be EVEN (+1) or ODD (-1)";
		}
		return OffsetCoord(col, row);
	}

	HexagonGridSystem::Hex qoffset_to_cube(int offset, OffsetCoord h)
	{
		int q = h.col;
		int r = h.row - int((h.col + offset * (h.col & 1)) / 2);
		int s = -q - r;
		if (offset != EVEN && offset != ODD)
		{
			throw "offset must be EVEN (+1) or ODD (-1)";
		}
		return Hex(q, r, s);
	}

	HexagonGridSystem::OffsetCoord roffset_from_cube(int offset, Hex h)
	{
		int col = h.q + int((h.r + offset * (h.r & 1)) / 2);
		int row = h.r;
		if (offset != EVEN && offset != ODD)
		{
			throw "offset must be EVEN (+1) or ODD (-1)";
		}
		return OffsetCoord(col, row);
	}

	HexagonGridSystem::Hex roffset_to_cube(int offset, OffsetCoord h)
	{
		int q = h.col - int((h.row + offset * (h.row & 1)) / 2);
		int r = h.row;
		int s = -q - r;
		if (offset != EVEN && offset != ODD)
		{
			throw "offset must be EVEN (+1) or ODD (-1)";
		}
		return Hex(q, r, s);
	}

	HexagonGridSystem::DoubledCoord qdoubled_from_cube(Hex h)
	{
		int col = h.q;
		int row = 2 * h.r + h.q;
		return DoubledCoord(col, row);
	}

	HexagonGridSystem::Hex qdoubled_to_cube(DoubledCoord h)
	{
		int q = h.col;
		int r = int((h.row - h.col) / 2);
		int s = -q - r;
		return Hex(q, r, s);
	}

	HexagonGridSystem::DoubledCoord rdoubled_from_cube(Hex h)
	{
		int col = 2 * h.q + h.r;
		int row = h.r;
		return DoubledCoord(col, row);
	}

	HexagonGridSystem::Hex rdoubled_to_cube(DoubledCoord h)
	{
		int q = int((h.col - h.row) / 2);
		int r = h.row;
		int s = -q - r;
		return Hex(q, r, s);
	}

	HexagonGridSystem::Hex hex_rotate_left(Hex a)
	{
		return Hex(-a.s, -a.q, -a.r);
	}

	HexagonGridSystem::Hex hex_rotate_right(Hex a)
	{
		return Hex(-a.r, -a.s, -a.q);
	}

	HexagonGridSystem::Hex hex_scale(Hex a, int k)
	{
		return Hex(a.q * k, a.r * k, a.s * k);
	}

	HexagonGridSystem::Hex hex_subtract(Hex a, Hex b)
	{
		return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
	}

	HexagonGridSystem::Hex hex_add(Hex a, Hex b)
	{
		return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
	}

	double lonToWebmercator(double lon) {
		return 6371393 * lon * M_PI / 180.0;
	}
	double latToWebmercator(double lat) {
		return 6371393 * log(tan(M_PI / 4.0 + lat * M_PI / 360.0));

	}


}

namespace HexagonGridSystem {
	std::vector<CoorXYZ> Grid_Hexagon::getCodeRange(GridCode code)
	{
		CoorIJK tempCoor = this->toIJKFromCode(code);
		Hex tempHex = toHexFromCoor(tempCoor);
		return polygon_corners(tempHex);
	}

	//判断两个编码是否是相邻网格 相邻是true 不相邻是false
	bool Grid_Hexagon::is_neighbor_2d_6(GridCode code1, GridCode code2)
	{
		std::vector<GridCode> neighbor = getNeighbors(code1);
		int size = neighbor.size();
		for (int i = 0; i < size; i++)
		{
			if (neighbor[i] == code2)
			{
				return true;//相邻
				break;
			}
		}
		return false;
	}

	std::vector<GridCode> Grid_Hexagon::getLines(GridCode startCode, GridCode targetCode)
	{
		CoorIJK startCoor = this->toIJKFromCode(startCode);
		CoorIJK targetCoor = this->toIJKFromCode(targetCode);

		Hex startHex = toHexFromCoor(startCoor);
		Hex targetHex = toHexFromCoor(targetCoor);

		std::vector<Hex> resultHexes = hex_linedraw(startHex, targetHex);
		std::vector<GridCode> result;
		for (int i = 0; i < resultHexes.size(); i++) {
			result.push_back(toCodeFromIJK(toCoorFromHex(resultHexes[i])));
		}
		return result;
	}

	std::vector<GridCode> Grid_Hexagon::getLines(CoorXYZ startXYZ, CoorXYZ targetXYZ, int level/*=N_MAX*/)
	{
		CoorIJK startCoor = this->toIJKFromXYZ(startXYZ);
		CoorIJK targetCoor = this->toIJKFromXYZ(targetXYZ);

		Hex startHex = toHexFromCoor(startCoor);
		Hex targetHex = toHexFromCoor(targetCoor);

		std::vector<Hex> resultHexes = hex_linedraw(startHex, targetHex);
		std::vector<GridCode> result;
		for (int i = 0; i < resultHexes.size(); i++) {
			result.push_back(toCodeFromIJK(toCoorFromHex(resultHexes[i])));
		}
		return result;
	}

	std::vector<GridCode> Grid_Hexagon::getNeighbors(GridCode inputCode)
	{
		CoorIJK inputCoor = this->toIJKFromCode(inputCode);
		std::vector<CoorIJK> neighborCoors = this->calNeighbors(inputCoor);
		std::vector<GridCode> neighborCodes;
		for (int i = 0; i < neighborCoors.size(); i++) {
			neighborCodes.push_back(this->toCodeFromIJK(neighborCoors[i]));
		}
		return neighborCodes;
	}

	CoorIJK Grid_Hexagon::toIJKFromXYZ(const CoorXYZ& xyz, Level level /*= N_MAX*/)
	{
		if (this->isOutBorder(xyz)) {
			std::cout << __FUNCTION__ << ": " << "输入的点越界!" << std::endl;
			return CoorIJK(0, 0);
		}
		// 1 将输入点转为六角坐标
		Hex tempHex = hex_round(pixel_to_hex(xyz));

		// 2 将坐标转为偏移坐标 offset coordinates
		OffsetCoord tempOffsetCoord = qoffset_from_cube(ODD, tempHex);

		return CoorIJK(tempOffsetCoord.col, tempOffsetCoord.row, 0);
	}

	CoorXYZ Grid_Hexagon::toXYZFromIJK(const CoorIJK& ijk, Level level)
	{
		OffsetCoord tempOffsetCoord(ijk[0], ijk[1]);

		Hex tempHex = qoffset_to_cube(ODD, tempOffsetCoord);

		return hex_to_pixel(tempHex);
	}

	GridCode Grid_Hexagon::toCodeFromIJK(const CoorIJK& ijk, Level level /*= N_MAX*/)
	{
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

	CoorIJK Grid_Hexagon::toIJKFromCode(GridCode code)
	{
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

	GridCode Grid_Hexagon::toCodeFromXYZ(const CoorXYZ& xyz, Level level)
	{
		return this->toCodeFromIJK(this->toIJKFromXYZ(xyz));
	}

	CoorXYZ Grid_Hexagon::toXYZFromCode(GridCode code)
	{
		return this->toXYZFromIJK(this->toIJKFromCode(code), this->getLevelFromCode(code));
	}

	std::vector<CoorXYZ> Grid_Hexagon::calCornerPtsFromCode(GridCode code)
	{
		CoorIJK inputIJK = this->toIJKFromCode(code);
		Hex inputHex = this->toHexFromCoor(inputIJK);
		return polygon_corners(inputHex);
	}

	Level Grid_Hexagon::getLevelFromCode(GridCode code)
	{
		return 21 - int(countConsecutiveOnes(code) / 3);

		// 输入的是偶数，就是21层
		if (!(code & 1))
		{
			return 21;
		}

		GridCode mid = (code - 1) ^ (code + 1);
		int N = 0;
		for (int i = 0; i < 6; i++)
		{
			N = N + this->getMidN(mid, i);
		}

		//返回层级;使用模数进行除法运算，相当于：N/3
		//return ((GridCode)N * 0xAAAAAAAB) >> 33;
		return int(N / 3);
	}

	GridCode Grid_Hexagon::findDadCode(GridCode inputCode, int inputFatherLevel/*= -1*/)
	{
		LOGI << "[Warning] The hexagon does not support multiple scales!" << endl;
		LOGI << "[Warning] The result only represent the parent unit of input code!" << endl;
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

	vector<GridCode> Grid_Hexagon::getChildCodes(GridCode inputCode, Level inputSonLevel)
	{
		Level fatherLevel = getLevelFromCode(inputCode);
		if (fatherLevel >= inputSonLevel) {
			LOGI << "The input son level is smaller or equal than father level!" << endl;
			return vector<GridCode>{inputCode};
		}
		if (inputSonLevel > 21) {
			LOGI << "The input son level is larger than the max level (21)!" << endl;
			inputSonLevel = 21;
		}

		int diff = inputSonLevel.x - fatherLevel.x;
		int sonCodesNum = pow(8, diff);

		GridCode sonLevelFlag = ((GridCode)1 << (63 - 3 * inputSonLevel.x)) - 1;
		int fatherLevelFlagLen = 64 - 3 * fatherLevel.x;
		GridCode baseCode = (inputCode >> fatherLevelFlagLen << fatherLevelFlagLen) + sonLevelFlag;

		vector<GridCode> result(sonCodesNum);
		for (int i = 0; i < sonCodesNum; i++) {
			result[i] = baseCode;
			baseCode += sonLevelFlag;
		}
		return result;
	}

	std::vector<CoorIJK> Grid_Hexagon::calNeighbors(CoorIJK inputCoor, int neighborDis /*= 1*/, bool isManhattanDis /*= false*/)
	{
		if (neighborDis != 1) {
			LOGI << "相关功能暂不支持！" << std::endl;
		}

		Hex tempHex = toHexFromCoor(inputCoor);

		std::vector<CoorIJK> neighbors;
		for (int i = 0; i < 6; i++) {
			Hex tempNeighbor = hex_neighbor(tempHex, i);
			neighbors.push_back((toCoorFromHex(tempNeighbor)));
		}
		return neighbors;
	}

	void Grid_Hexagon::calOccupiedInfo(CoorXYZ origin, CoorXYZ target, double startTime, double endTime, Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap /*= false*/)
	{
		//1 判断是否需要对outputMap初始化
		if (isClearMap)
		{
			outputMap.clear();
			isClearMap = false;
		}
		time_t startTimeInteger = startTime + 0.5 - 1e-6;
		time_t endTimeInteger = endTime + 0.5 - 1e-6;
		//2 如果起点终点对应的时刻相同，那么对起点终点连线进行三维网格化
		if (startTimeInteger == endTimeInteger)
		{
			std::vector<GridCode> lineCodes = this->getLines(origin, target, N_MAX);
			for (int i = 0; i < lineCodes.size(); i++)
			{
				if (outputMap[lineCodes[i]].size() < 2)
				{
					outputMap[lineCodes[i]].push_back(startTimeInteger);
					outputMap[lineCodes[i]].push_back(endTimeInteger);
				}
				else
				{
					outputMap[lineCodes[i]][0] = std::min(outputMap[lineCodes[i]][0], startTimeInteger);
					outputMap[lineCodes[i]][1] = std::max(outputMap[lineCodes[i]][0], endTimeInteger);
				}
			}
		}
		//3 如果时间不同
		else
		{
			//对起点和终点网格化
			GridCode origin_code = this->toCodeFromXYZ(origin, N_MAX);
			GridCode target_code = this->toCodeFromXYZ(target, N_MAX);

			//3.1 如果在相同的空间编码中，则占用为对应的时间段
			if (origin_code == target_code)
			{
				if (outputMap[origin_code].size() < 2)
				{
					outputMap[origin_code].push_back(startTimeInteger);
					outputMap[target_code].push_back(endTimeInteger);
				}
				else
				{
					outputMap[origin_code][0] = std::min(outputMap[origin_code][0], startTimeInteger);
					outputMap[origin_code][1] = std::max(outputMap[origin_code][0], endTimeInteger);
				}
				return;
			}
			//3.2 如果在不同的空间编码中 则用二分法
			else
			{
				Eigen::Vector3d midPt = 0.5 * (origin + target);
				double midTime = 0.5 * (startTime + endTime);
				calOccupiedInfo(origin, midPt, startTime, midTime, layer, outputMap, isClearMap);
				calOccupiedInfo(midPt, target, midTime, endTime, layer, outputMap, isClearMap);
			}
		}
	}

	void Grid_Hexagon::getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend /*= false*/)
	{
		if (!isAppend) codes.clear();

		CoorIJK startCoor = this->toIJKFromXYZ(p1);
		CoorIJK targetCoor = this->toIJKFromXYZ(p2);

		Hex startHex = toHexFromCoor(startCoor);
		Hex targetHex = toHexFromCoor(targetCoor);

		std::vector<Hex> resultHexes = hex_linedraw(startHex, targetHex);
		std::vector<GridCode> result;
		for (int i = 0; i < resultHexes.size(); i++) {
			codes.push_back(toCodeFromIJK(toCoorFromHex(resultHexes[i])));
		}
	}

	void Grid_Hexagon::getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange)
	{
		std::vector<CoorXYZ> cornerPts = calCornerPtsFromCode(inputCode);
		minRange = cornerPts[0];
		maxRange = cornerPts[0];

		for (int i = 1; i < cornerPts.size(); i++) {
			for (int j = 0; j < 3; j++) {
				minRange[j] = std::min(minRange[j], cornerPts[i][j]);
				maxRange[j] = std::max(maxRange[j], cornerPts[i][j]);
			}
		}
	}

	CoorXYZ Grid_Hexagon::getGridLen(Level level)
	{
		LOGI << "六边形网类型的网格长度计算不明确，请确认是否使用在使用六边形网格时，获取网格的边长！" << std::endl;
		return CoorXYZ();
	}

}
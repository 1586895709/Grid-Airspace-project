#pragma once
#include <vector>
#include <map>
#include "grid_base.h"

namespace HexagonGridSystem {

	//struct Coor
	//{
	//	int I;
	//	int J;
	//	Coor(int _I, int _J) : I(_I), J(_J) {}
	//};

	//struct Point
	//{
	//	double x;
	//	double y;
	//	Point(double x_, double y_) : x(x_), y(y_) {}
	//};

	struct Hex
	{
		const int q;
		const int r;
		const int s;
		Hex(int q_, int r_, int s_) : q(q_), r(r_), s(s_) {
			if (q + r + s != 0) throw "q + r + s must be 0";
		}
	};

	struct FractionalHex
	{
		const double q;
		const double r;
		const double s;
		FractionalHex(double q_, double r_, double s_) : q(q_), r(r_), s(s_) {
			if (round(q + r + s) != 0) throw "q + r + s must be 0";
		}
	};

	struct OffsetCoord
	{
		const int col;
		const int row;
		OffsetCoord(int col_, int row_) : col(col_), row(row_) {}
	};


	struct DoubledCoord
	{
		const int col;
		const int row;
		DoubledCoord(int col_, int row_) : col(col_), row(row_) {}
	};


	struct Orientation
	{
		double f0;
		double f1;
		double f2;
		double f3;
		double b0;
		double b1;
		double b2;
		double b3;
		double start_angle;
		Orientation(double f0_, double f1_, double f2_, double f3_, double b0_, double b1_, double b2_, double b3_, double start_angle_) : f0(f0_), f1(f1_), f2(f2_), f3(f3_), b0(b0_), b1(b1_), b2(b2_), b3(b3_), start_angle(start_angle_) {}
	};


	struct Layout
	{
		Orientation orientation;
		CoorXYZ size;
		CoorXYZ origin;
		Layout(Orientation orientation_, CoorXYZ size_, CoorXYZ origin_) : orientation(orientation_), size(size_), origin(origin_) {}
	};

	Hex hex_add(Hex a, Hex b);


	Hex hex_subtract(Hex a, Hex b);


	Hex hex_scale(Hex a, int k);


	Hex hex_rotate_left(Hex a);


	Hex hex_rotate_right(Hex a);

	const std::vector<Hex> hex_directions = { Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1), Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1) };
	Hex hex_direction(int direction);

	Hex hex_neighbor(Hex hex, int direction);


	const std::vector<Hex> hex_diagonals = { Hex(2, -1, -1), Hex(1, -2, 1), Hex(-1, -1, 2), Hex(-2, 1, 1), Hex(-1, 2, -1), Hex(1, 1, -2) };
	Hex hex_diagonal_neighbor(Hex hex, int direction);

	int hex_length(Hex hex);

	int hex_distance(Hex a, Hex b);

	Hex hex_round(FractionalHex h);

	FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t);


	std::vector<Hex> hex_linedraw(Hex a, Hex b);

	const int EVEN = 1;
	const int ODD = -1;
	OffsetCoord qoffset_from_cube(int offset, Hex h);

	Hex qoffset_to_cube(int offset, OffsetCoord h);

	OffsetCoord roffset_from_cube(int offset, Hex h);

	Hex roffset_to_cube(int offset, OffsetCoord h);

	DoubledCoord qdoubled_from_cube(Hex h);


	Hex qdoubled_to_cube(DoubledCoord h);


	DoubledCoord rdoubled_from_cube(Hex h);


	Hex rdoubled_to_cube(DoubledCoord h);

	const Orientation layout_pointy = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
	const Orientation layout_flat = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);
}

namespace HexagonGridSystem {
	const int N_MAX = 21;

	class Grid_Hexagon : public GridBase
	{
	private:
		Layout layout;

	private:
		CoorXYZ hex_to_pixel(Hex h)
		{
			Orientation M = layout.orientation;
			CoorXYZ size = layout.size;
			CoorXYZ origin = layout.origin;
			double x = (M.f0 * h.q + M.f1 * h.r) * size[0];
			double y = (M.f2 * h.q + M.f3 * h.r) * size[1];
			return CoorXYZ(x + origin[0], y + origin[1], 0);
		}

		FractionalHex pixel_to_hex(CoorXYZ p)
		{
			Orientation M = layout.orientation;
			CoorXYZ size = layout.size;
			CoorXYZ origin = layout.origin;
			CoorXYZ pt = CoorXYZ((p[0] - origin[0]) / size[0], (p[1] - origin[1]) / size[1], 0);
			double q = M.b0 * pt[0] + M.b1 * pt[1];
			double r = M.b2 * pt[0] + M.b3 * pt[1];
			return FractionalHex(q, r, -q - r);
		}

		CoorXYZ hex_corner_offset(int corner)
		{
			Orientation M = layout.orientation;
			CoorXYZ size = layout.size;
			double angle = 2.0 * M_PI * (M.start_angle - corner) / 6.0;
			return CoorXYZ(size[0] * cos(angle), size[1] * sin(angle), 0);
		}

		std::vector<CoorXYZ> polygon_corners(Hex h)
		{
			std::vector<CoorXYZ> corners = {};
			CoorXYZ center = hex_to_pixel(h);
			for (int i = 0; i < 6; i++)
			{
				CoorXYZ offset = hex_corner_offset(i);
				corners.push_back(center + offset);
			}
			return corners;
		}

		Hex toHexFromCoor(const CoorIJK& coor) {
			OffsetCoord tempOffset(coor[0], coor[1]);
			return qoffset_to_cube(ODD, tempOffset);
		}

		CoorIJK toCoorFromHex(const Hex& hex) {
			OffsetCoord tempOffset = qoffset_from_cube(ODD, hex);
			return CoorIJK(tempOffset.col, tempOffset.row, 0);
		}
		// 接口函数实现
	public:
		// 点转网格坐标
		CoorIJK toIJKFromXYZ(const CoorXYZ& xyz, Level level = N_MAX) override;
		CoorXYZ toXYZFromIJK(const CoorIJK& ijk, Level level) override;

		GridCode toCodeFromIJK(const CoorIJK& ijk, Level level = N_MAX) override;
		CoorIJK toIJKFromCode(GridCode code) override;

		GridCode toCodeFromXYZ(const CoorXYZ& xyz, Level level = N_MAX) override;
		CoorXYZ toXYZFromCode(GridCode code) override;

		std::vector<CoorXYZ> calCornerPtsFromCode(GridCode code) override;

		Level getLevelFromCode(GridCode code) override;

		GridCode findDadCode(GridCode inputCode, int inputFatherLevel = -1) override;

		vector<GridCode> getChildCodes(GridCode inputCode, Level inputSonLevel) override;

		std::vector<CoorIJK> calNeighbors(CoorIJK inputCoor, int neighborDis = 1, bool isManhattanDis = false) override;

		void calOccupiedInfo(
			CoorXYZ origin, CoorXYZ target, double startTime, double endTime,
			Level layer, std::map<GridCode, TimeInterval>& outputMap, bool isClearMap = false) override;

		void getCodesOfLine(CoorXYZ p1, CoorXYZ p2, Level layer, std::vector<GridCode>& codes, bool isAppend = false) override;

		void getRangeOfCode(GridCode inputCode, CoorXYZ& minRange, CoorXYZ& maxRange) override;

		/**
		 * @brief 获取网格边长
		 * @param level 输入网格层级
		 * @return 各维度网格边长
		 */
		CoorXYZ getGridLen(Level level) override;

	public:
		Grid_Hexagon(Layout _Layout) : layout(_Layout) {
			// 强制六边形为水平放置的
			layout.orientation = layout_flat;

			this->minBorder = CoorXYZ(_Layout.origin - _Layout.size);
			this->maxBorder = CoorXYZ(_Layout.origin + (pow(2, N_MAX) - 1) * 1.5 * layout.size + layout.size);

			this->minBorder[2] = 0;
			this->maxBorder[2] = 0;
		};


		// 根据输入编码求邻域
		std::vector<GridCode> getNeighbors(GridCode inputCode);

		// 直线网格化
		std::vector<GridCode> getLines(GridCode startCode, GridCode targetCode);
		std::vector<GridCode> getLines(CoorXYZ startXYZ, CoorXYZ targetXYZ, int level = N_MAX);

		// 由网格编码计算对应六角网格的覆盖范围
		std::vector<CoorXYZ> getCodeRange(GridCode code);

		//判断两个编码是否是相邻网格 相邻是true 不相邻是false
		bool is_neighbor_2d_6(GridCode code1, GridCode code2);
	};
}


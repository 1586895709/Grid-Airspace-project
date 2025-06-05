#pragma once
#include <fstream>
#include <vector>
#include <variant>
#include "knlohmannjson.hpp"
#include "util_function.h"

namespace GeoJSON_NS
{
	using json = nlohmann::json;

	// 几何类型定义
	struct Point {
		CoorXYZ coordinate;
	};

	struct LineString {
		std::vector<CoorXYZ> coordinates;
	};

	struct Polygon {
		std::vector<LineString> rings; // 第一个环是外轮廓，后续为孔洞
	};

	//using Point = CoorXYZ;
	//using LineString = std::vector<Point>;
	//using Polygon = std::vector<LineString>;

	using MultiPoint = std::vector<Point>;
	using MultiLineString = std::vector<LineString>;
	using MultiPolygon = std::vector<Polygon>;

	using Geometry = std::variant<
		Point,
		LineString,
		Polygon,
		MultiPoint,       // MultiPoint
		MultiLineString,  // MultiLineString
		MultiPolygon      // MultiPolygon
	>;

	// 特征属性（示例）
	using Properties = std::unordered_map<std::string, json>;

	// 完整特征定义
	struct Feature {
		Geometry geometry;
		Properties properties;
	};

	class GeoJSON_Reader
	{
	public:
		std::vector<Feature> read(const std::string& filename);

		/**
		 * @brief 读取并解析MultiPolygon格式的geojson文件
		 * @param filename geojson文件路径
		 * @param geometrys 各feature的几何信息
		 * @param properties 各feature的属性信息
		 */
		bool readAndParseMultiPolygon(
			const std::string& filename, std::vector<MultiPolygon>& geometrys, std::vector<Properties>& properties);

	protected:
		json readJsonFile(const std::string& filename);

		std::vector<Feature> parseFeatures(const json& geojson);

		Feature parseFeature(const json& featureJson);

		Geometry parseGeometry(const json& geometryJson);

		Point parsePoint(const json& coordJson);

		LineString parseLineString(const json& coordsJson);

		Polygon parsePolygon(const json& ringsJson);

		// Multi类型解析
		MultiPoint parseMultiPoint(const json& pointsJson);

		MultiLineString parseMultiLineString(const json& linesJson);

		MultiPolygon parseMultiPolygon(const json& polygonsJson);

		// 公共工具方法
		CoorXYZ parseCoordinate(const json& coordJson);

		Properties parseProperties(const json& propsJson);

		// 通用模板函数处理存在性检查和默认值
		template<typename T>
		T getOrDefault(const json& obj, const std::string& key, const T& defaultValue) {
			if (obj.contains(key)) {
				return obj[key].get<T>();
			}
			else {
				std::cerr << "Warning: Missing key '" << key << "', using default value." << std::endl;
				return defaultValue;
			}
		}

		// 处理需要字符串转换的特殊类型
		template<typename T>
		T getStringConverted(const json& obj, const std::string& key, const T& defaultValue,
			T(*converter)(std::string)) {
			if (obj.contains(key)) {
				const auto& value = obj[key];
				if (value.is_string()) {
					return converter(value.get<std::string>());
				}
				else {
					std::cerr << "Error: Key '" << key << "' is not a string." << std::endl;
					return defaultValue;
				}
			}
			else {
				std::cerr << "Warning: Missing key '" << key << "', using default value." << std::endl;
				return defaultValue;
			}
		}
	};
}

// 测试示例
/*

int main() {
	GeoJSONReader reader;
	try {
		auto features = reader.read("example.geojson");

		for (const auto& feature : features) {
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, Point>) {
					std::cout << "Point: (" << arg.coordinate.longitude
							  << ", " << arg.coordinate.latitude << ")\n";
				}
				else if constexpr (std::is_same_v<T, LineString>) {
					std::cout << "LineString with " << arg.coordinates.size()
							  << " points\n";
				}
				// 其他类型处理类似...

			}, feature.geometry);
		}
	} catch (const std::exception& e) {
		std::cerr << "错误: " << e.what() << std::endl;
	}
	return 0;
}

*/


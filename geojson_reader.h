#pragma once
#include <fstream>
#include <vector>
#include <variant>
#include "knlohmannjson.hpp"
#include "util_function.h"

namespace GeoJSON_NS
{
	using json = nlohmann::json;

	// �������Ͷ���
	struct Point {
		CoorXYZ coordinate;
	};

	struct LineString {
		std::vector<CoorXYZ> coordinates;
	};

	struct Polygon {
		std::vector<LineString> rings; // ��һ������������������Ϊ�׶�
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

	// �������ԣ�ʾ����
	using Properties = std::unordered_map<std::string, json>;

	// ������������
	struct Feature {
		Geometry geometry;
		Properties properties;
	};

	class GeoJSON_Reader
	{
	public:
		std::vector<Feature> read(const std::string& filename);

		/**
		 * @brief ��ȡ������MultiPolygon��ʽ��geojson�ļ�
		 * @param filename geojson�ļ�·��
		 * @param geometrys ��feature�ļ�����Ϣ
		 * @param properties ��feature��������Ϣ
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

		// Multi���ͽ���
		MultiPoint parseMultiPoint(const json& pointsJson);

		MultiLineString parseMultiLineString(const json& linesJson);

		MultiPolygon parseMultiPolygon(const json& polygonsJson);

		// �������߷���
		CoorXYZ parseCoordinate(const json& coordJson);

		Properties parseProperties(const json& propsJson);

		// ͨ��ģ�庯����������Լ���Ĭ��ֵ
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

		// ������Ҫ�ַ���ת������������
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

// ����ʾ��
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
				// �������ʹ�������...

			}, feature.geometry);
		}
	} catch (const std::exception& e) {
		std::cerr << "����: " << e.what() << std::endl;
	}
	return 0;
}

*/


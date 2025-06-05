#include "geojson_reader.h"

std::vector<GeoJSON_NS::Feature> GeoJSON_NS::GeoJSON_Reader::read(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("无法打开文件: " + filename);
	}

	json geojson;
	try {
		file >> geojson;
	}
	catch (const json::parse_error& e) {
		throw std::runtime_error("JSON解析错误: " + std::string(e.what()));
	}

	return parseFeatures(geojson);
}

bool GeoJSON_NS::GeoJSON_Reader::readAndParseMultiPolygon(
	const std::string& filename, std::vector<MultiPolygon>& geometrys, std::vector<Properties>& properties)
{
	json geojsonValue = readJsonFile(filename);

	if (!geojsonValue.contains("features")) return false;

	geometrys.clear();
	properties.clear();
	for (const auto& feature : geojsonValue["features"]) {
		//std::cout << feature << std::endl;
		std::string type = feature["geometry"]["type"];
		if (type != "MultiPolygon") {
			LOGI << "[ERROR] The type of input geojson is not [MultiPolygon]!" << std::endl;
			return false;
		}
		geometrys.push_back(parseMultiPolygon(feature["geometry"]["coordinates"]));
		properties.push_back(parseProperties(feature["properties"]));
	}

	return true;
}

GeoJSON_NS::json GeoJSON_NS::GeoJSON_Reader::readJsonFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("无法打开文件: " + filename);
	}

	json geojson;
	try {
		file >> geojson;
	}
	catch (const json::parse_error& e) {
		throw std::runtime_error("JSON解析错误: " + std::string(e.what()));
	}

	return geojson;
}

std::vector<GeoJSON_NS::Feature> GeoJSON_NS::GeoJSON_Reader::parseFeatures(const json& geojson)
{
	std::vector<Feature> features;

	if (geojson.contains("features")) {
		for (const auto& feature : geojson["features"]) {
			features.push_back(parseFeature(feature));
		}
	}
	else {
		features.push_back(parseFeature(geojson));
	}

	return features;
}

GeoJSON_NS::Feature GeoJSON_NS::GeoJSON_Reader::parseFeature(const json& featureJson)
{
	Feature feature;
	feature.geometry = parseGeometry(featureJson["geometry"]);
	feature.properties = parseProperties(featureJson["properties"]);
	return feature;
}

GeoJSON_NS::Geometry GeoJSON_NS::GeoJSON_Reader::parseGeometry(const json& geometryJson)
{
	const std::string type = geometryJson["type"];
	const auto& coordinates = geometryJson["coordinates"];

	if (type == "Point") return parsePoint(coordinates);
	if (type == "LineString") return parseLineString(coordinates);
	if (type == "Polygon") return parsePolygon(coordinates);
	if (type == "MultiPoint") return parseMultiPoint(coordinates);
	if (type == "MultiLineString") return parseMultiLineString(coordinates);
	if (type == "MultiPolygon") return parseMultiPolygon(coordinates);

	throw std::runtime_error("未知的几何类型: " + type);
}

GeoJSON_NS::Point GeoJSON_NS::GeoJSON_Reader::parsePoint(const json& coordJson)
{
	return { parseCoordinate(coordJson) };
}

GeoJSON_NS::LineString GeoJSON_NS::GeoJSON_Reader::parseLineString(const json& coordsJson)
{
	LineString line;
	for (const auto& coord : coordsJson) {
		line.coordinates.push_back(parseCoordinate(coord));
	}
	return line;
}

GeoJSON_NS::Polygon GeoJSON_NS::GeoJSON_Reader::parsePolygon(const json& ringsJson)
{
	Polygon polygon;
	for (const auto& ring : ringsJson) {
		polygon.rings.push_back(parseLineString(ring));
	}
	return polygon;
}

GeoJSON_NS::MultiPoint GeoJSON_NS::GeoJSON_Reader::parseMultiPoint(const json& pointsJson)
{
	std::vector<Point> points;
	for (const auto& coord : pointsJson) {
		points.push_back(parsePoint(coord));
	}
	return points;
}

GeoJSON_NS::MultiLineString GeoJSON_NS::GeoJSON_Reader::parseMultiLineString(const json& linesJson)
{
	std::vector<LineString> lines;
	for (const auto& line : linesJson) {
		lines.push_back(parseLineString(line));
	}
	return lines;
}

GeoJSON_NS::MultiPolygon GeoJSON_NS::GeoJSON_Reader::parseMultiPolygon(const json& polygonsJson)
{
	std::vector<Polygon> polygons;
	for (const auto& polygon : polygonsJson) {
		polygons.push_back(parsePolygon(polygon));
	}
	return polygons;
}

CoorXYZ GeoJSON_NS::GeoJSON_Reader::parseCoordinate(const json& coordJson)
{
	if (coordJson.size() < 2) throw std::runtime_error("无效坐标");
	return {
		coordJson[0].get<double>(),
		coordJson[1].get<double>(),
		coordJson.size() > 2 ? coordJson[2].get<double>() : 0.0
	};
}

GeoJSON_NS::Properties GeoJSON_NS::GeoJSON_Reader::parseProperties(const json& propsJson)
{
	Properties props;
	if (propsJson.is_object()) {
		for (auto& [key, value] : propsJson.items()) {
			props[key] = value;
		}
	}
	return props;
}
#include "gen_random_point.h"
#include "function.h"
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>

Polygon::Polygon(const std::vector<Point>& coords) : coordinates(coords) {}

bool Polygon::contains(const Point& p) const {
    bool inside = false;
    for (size_t i = 0, j = coordinates.size() - 1; i < coordinates.size(); j = i++) {
        if (((coordinates[i].y > p.y) != (coordinates[j].y > p.y)) &&
            (p.x < (coordinates[j].x - coordinates[i].x) * (p.y - coordinates[i].y) / (coordinates[j].y - coordinates[i].y) + coordinates[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

std::vector<Point> gen_random_points(const std::vector<Point>& polygon_coords, int num) {
    // 创建多边形对象
    Polygon polygon(polygon_coords);

    double lon_min = 180.0, lon_max = -180.0, lat_min = 90.0, lat_max = -90.0;

    for (const auto& point : polygon_coords) {
        lon_min = std::min(lon_min, point.x);
        lon_max = std::max(lon_max, point.x);
        lat_min = std::min(lat_min, point.y);
        lat_max = std::max(lat_max, point.y);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> lon_dist(lon_min, lon_max);
    std::uniform_real_distribution<double> lat_dist(lat_min, lat_max);

    int count = 0;
    std::vector<Point> random_points;
    while (random_points.size() < num) {
        double random_longitude = lon_dist(gen);
        double random_latitude = lat_dist(gen);
        Point random_point = { random_longitude, random_latitude };

        if (polygon.contains(random_point)) {
            random_points.push_back(random_point);
        }

        count++;
        if (count > num * 20) {
            std::cout << "Error BBox of polygon_coords" << std::endl;
            break;
        }
    }

    if (random_points.size() != num) {
        std::cout << "Attention: random points num: " << random_points.size() << std::endl;
    }

    return random_points;
}

std::vector<Point> read_polygon_txt(const std::string& file_path) {
    std::vector<Point> points;
    std::ifstream file(file_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token1, token2;
            double x, y;

            if (std::getline(ss, token1, ',') && std::getline(ss, token2, ',')) {
                x = std::stod(token1);
                y = std::stod(token2);
                points.push_back({ x, y });
            }
        }

        file.close();
    }
    else {
        std::cerr << "Error opening file: " << file_path << std::endl;
    }

    return points;
}


vector<Point> test_gen_random_point(string file_path, int num) 
{
    json_of_nlohmann task_border_json_file = read_json_file(file_path);
    std::vector<Point> polygon_coords;
    Point tmp;
    for (auto& ix : task_border_json_file["features"][0]["geometry"]["coordinates"][0])
    {
        tmp.x = ix[0];
        tmp.y = ix[1];
        polygon_coords.push_back(tmp);
    }
    std::vector<Point> points = gen_random_points(polygon_coords, num);
    return points;
}

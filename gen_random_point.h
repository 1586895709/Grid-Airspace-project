#pragma once

#include <vector>
#include <iostream>
using namespace std;

struct Point {
    double x;
    double y;
};

class Polygon {
public:
    Polygon(const std::vector<Point>& coords);
    bool contains(const Point& p) const;

private:
    std::vector<Point> coordinates;
};

std::vector<Point> gen_random_points(const std::vector<Point>& polygon_coords, int num = 10);

std::vector<Point> read_polygon_txt(const std::string& file_path);

//使用用例
vector<Point> test_gen_random_point(string file_path, int num);


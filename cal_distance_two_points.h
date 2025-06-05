#pragma once


#include <iostream>
#include <cmath>
#if defined(_WIN32)
#include <corecrt_math_defines.h>
#endif

const double EARTH_RADIUS = 6371000; // 地球半径，单位：米


double deg2rad(double deg)
{
	return deg * M_PI / 180;
}

double cal_distance_two_lonlat(double lon1, double lat1, double lon2, double lat2)
{
	double dlat = deg2rad(lat2 - lat1);
	double dlon = deg2rad(lon2 - lon1);
	double a = sin(dlat / 2) * sin(dlat / 2) +
		cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
		sin(dlon / 2) * sin(dlon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	return EARTH_RADIUS * c;
}

int test_cal_distance_two_points() {

	double distance = cal_distance_two_lonlat(112.1, 34.3, 113.1, 34.3);

	std::cout << "The distance between the two points is: " << distance << " meters" << std::endl;

	return 0;
}
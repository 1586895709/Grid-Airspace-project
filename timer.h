#pragma once

#include <iostream>
#include <chrono>

class Timer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
	std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
	bool is_running;

public:
	Timer() : is_running(false) {}

	// 开始计时
	void start() {
		start_time = std::chrono::high_resolution_clock::now();
		is_running = true;
	}

	// 停止计时
	void stop() {
		if (is_running) {
			end_time = std::chrono::high_resolution_clock::now();
			is_running = false;
		}
	}

	// 获取计时的时长（以毫秒为单位）
	double elapsedMilliseconds() const {
		if (is_running) {
			auto current_time = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<double, std::milli>(current_time - start_time).count();
		}
		return std::chrono::duration<double, std::milli>(end_time - start_time).count();
	}

	// 获取计时的时长（以秒为单位）
	double elapsedSeconds() const {
		return elapsedMilliseconds() / 1000.0;
	}
};
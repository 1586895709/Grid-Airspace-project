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

	// ��ʼ��ʱ
	void start() {
		start_time = std::chrono::high_resolution_clock::now();
		is_running = true;
	}

	// ֹͣ��ʱ
	void stop() {
		if (is_running) {
			end_time = std::chrono::high_resolution_clock::now();
			is_running = false;
		}
	}

	// ��ȡ��ʱ��ʱ�����Ժ���Ϊ��λ��
	double elapsedMilliseconds() const {
		if (is_running) {
			auto current_time = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<double, std::milli>(current_time - start_time).count();
		}
		return std::chrono::duration<double, std::milli>(end_time - start_time).count();
	}

	// ��ȡ��ʱ��ʱ��������Ϊ��λ��
	double elapsedSeconds() const {
		return elapsedMilliseconds() / 1000.0;
	}
};
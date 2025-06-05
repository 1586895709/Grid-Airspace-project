#pragma once
#include "function.h"
#include "gho_structs.h"
#include "gen_random_point.h"

class UAVTaskPlanning
{
public:
	UAVTaskPlanning();

	vector<UAV> uavs;
	vector<Task> all_tasks;
	
	void Update(const string a, const string b);

	/// <summary>
	/// 随机初始化模式
	/// </summary>
	/// <param name="airports_json_file_path">机场位置的geojson</param>
	/// <param name="tasks_range_file_path">任务范围(经纬度坐标txt文件)</param>
	/// <param name="min_num_uav">每个机场最少飞机数量</param>
	/// <param name="max_num_uav">每个机场最大飞机数量</param>
	/// <param name="num_of_tasks">任务数量n</param>
	/// <param name="min_num_task">每个任务点需要的最少无人机数量</param>
	/// <param name="max_num_task">每个任务点需要的最大无人机数量</param>
	void InitRandomMode(string airports_json_file_path, string tasks_range_file_path, int min_num_uav, int max_num_uav, int num_of_tasks, int min_num_task, int max_num_task);

	/// <summary>
	/// 传入JSON字符串初始化模式
	/// </summary>
	/// <param name="a">机场JSON字符串</param>
	/// <param name="b">任务JSON字符串</param>
	void InitSelectMode(json_of_nlohmann a, json_of_nlohmann b);

	json_of_nlohmann new_tasks;
	json_of_nlohmann tmp_output_json_file;
	json_of_nlohmann tmp_air_ports_json_file;
	json_of_nlohmann tmp_json_var_of_before_assassin_task;
	json_of_nlohmann get_airports_before_assassin(const string filename = "NULL");
	json_of_nlohmann get_airports_after_assassin(const string filename = "NULL");
	json_of_nlohmann get_tasks_before_assassin(const string filename = "NULL");
	json_of_nlohmann get_tasks_after_assassin(const string filename = "NULL");
	json_of_nlohmann get_lianxian(const string filename = "NULL");
	void run();
	void run(json_of_nlohmann appoint_info);
	vector<Point> get_tasks_positon();
	void print_uav_task_relation();
	void save_uav_init_goal_file(const string filename, string start_time);

	//初始化清空数据
	void ClearAll();
};
#include "uav_task_plan.h"
#include "cal_distance_two_points.h"
#include "gen_random_point.h"
//#include "LBHGridSystem.h"

string kAirPortsName[41] =
{ "0","1","2","3","4","5","6","7","8","9","10",
"11","12","13","14","15","16","17","18","19","20","21","22","23","24","25",
"26","27","28","29","30","31","32","33","34","35","36","37","38","39","40" };

string kUAVtype[3] = { "A","B","C" };

json_of_nlohmann UAVTaskPlanning::get_airports_before_assassin(const string filename/*=="NULL"*/)
{
	json_of_nlohmann air_ports_json_file_1 = tmp_air_ports_json_file;
	//初始化机场数据量-这里重新看一下uav列表，是为了解决上传模式和随机模式之间的切换，这里如果使用上传模式，可以免去这一步
	//for (auto& ix : air_ports_json_file_1["features"])
	//{
	//	ix["properties"]["num_a"] = 0;
	//	ix["properties"]["num_b"] = 0;
	//	ix["properties"]["num_c"] = 0;
	//	ix["properties"]["num_totle"] = 0;
	//}
	//for (auto& u : uavs)
	//{
	//	if (u.type == "A")
	//	{
	//		int a = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_a"];
	//		a++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_a"] = a;
	//		int b = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"];
	//		b++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
	//	}
	//	else if (u.type == "B")
	//	{
	//		int a = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_b"];
	//		a++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_b"] = a;
	//		int b = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"];
	//		b++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
	//	}
	//	else
	//	{
	//		int a = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_c"];
	//		a++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_c"] = a;
	//		int b = air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"];
	//		b++;
	//		air_ports_json_file_1["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
	//	}
	//}
	tmp_air_ports_json_file = air_ports_json_file_1;

	if (filename != "NULL")
	{
		ofstream outfile(filename);
		outfile << air_ports_json_file_1;
		outfile.close();
		std::cout << "任务分配前机场数据已导出" << endl;
	}
	return air_ports_json_file_1;
}

json_of_nlohmann UAVTaskPlanning::get_airports_after_assassin(const string filename)
{
	json_of_nlohmann air_ports_json_file_2 = tmp_air_ports_json_file;
	for (auto& a : air_ports_json_file_2["features"])
	{
		json_of_nlohmann lianxian;
		json_of_nlohmann target;
		bool flag = false;
		for (auto u : uavs)
		{
			string str_u_airport = a["properties"]["id"];
			if (u.airport == str_u_airport)
			{
				if (u.working) //在执行任务
				{
					lianxian.push_back(to_string(u.id));
					target.push_back(to_string(u.DoingTask.id));
					flag = true;

					//对应位置属性的飞机-1
					string type = u.type;
					if (type == "A")
					{
						int num = a["properties"]["num_a"];
						num--;
						a["properties"]["num_a"] = num;
						int num_t = a["properties"]["num_totle"];
						num_t--;
						a["properties"]["num_totle"] = num_t;
					}
					else if (type == "B")
					{
						int num = a["properties"]["num_b"];
						num--;
						a["properties"]["num_b"] = num;
						int num_t = a["properties"]["num_totle"];
						num_t--;
						a["properties"]["num_totle"] = num_t;
					}
					else if (type == "C")
					{
						int num = a["properties"]["num_c"];
						num--;
						a["properties"]["num_c"] = num;
						int num_t = a["properties"]["num_totle"];
						num_t--;
						a["properties"]["num_totle"] = num_t;
					}
				}
				else
				{
					//lianxian.push_back("NULL");
					//target.push_back("NULL");
				}
			}
		}
		if (!flag)
		{
			lianxian.push_back("NULL");
			target.push_back("NULL");
		}
		a["properties"]["lianxian"] = lianxian;
		a["properties"]["target"] = target;
	}

	if (filename != "NULL")
	{
		ofstream outfile1(filename);
		outfile1 << air_ports_json_file_2;
		outfile1.close();
		cout << "任务分配后机场数据已导出" << endl;
	}
	return air_ports_json_file_2;
}

json_of_nlohmann UAVTaskPlanning::get_tasks_before_assassin(const string filename)
{
	json_of_nlohmann output_json_file = tmp_output_json_file;
	json_of_nlohmann tmp = output_json_file["features"][0];
	output_json_file["features"].clear();
	output_json_file["crs"] = new_tasks["crs"];
	output_json_file["type"] = new_tasks["type"];
	output_json_file["name"] = new_tasks["name"];
	for (auto& single_task : all_tasks)
	{
		tmp["properties"]["need_uav"].clear();
		tmp["properties"]["airport"].clear();
		tmp["properties"]["lianxian"].clear();
		tmp["properties"]["airport"] = "NULL";
		tmp["properties"]["lianxian"] = "NULL";
		tmp["properties"]["path"] = "NULL";
		tmp["properties"]["name"] = single_task.name;
		tmp["properties"]["SAMPLE_Z1"] = single_task.SAMPLE_Z1;
		tmp["properties"]["id"] = single_task.id;
		tmp["properties"]["num_a"] = 0;
		tmp["properties"]["num_b"] = 0;
		tmp["properties"]["num_c"] = 0;
		tmp["properties"]["num_totle"] = 0;
		tmp["geometry"]["coordinates"][0] = single_task.x;
		tmp["geometry"]["coordinates"][1] = single_task.y;
		for (auto ix : single_task.used_uav_type)
		{
			tmp["properties"]["need_uav"].push_back(ix);
			if (ix == "A")
			{
				int a = tmp["properties"]["num_a"];
				a++;
				tmp["properties"]["num_a"] = a;
				int b = tmp["properties"]["num_totle"];
				b++;
				tmp["properties"]["num_totle"] = b;
			}
			else if (ix == "B")
			{
				int a = tmp["properties"]["num_b"];
				a++;
				tmp["properties"]["num_b"] = a;
				int b = tmp["properties"]["num_totle"];
				b++;
				tmp["properties"]["num_totle"] = b;
			}
			else if (ix == "C")
			{
				int a = tmp["properties"]["num_c"];
				a++;
				tmp["properties"]["num_c"] = a;
				int b = tmp["properties"]["num_totle"];
				b++;
				tmp["properties"]["num_totle"] = b;
			}
		}
		output_json_file["features"].push_back(tmp);
	}

	if (filename != "NULL")
	{
		std::ofstream file(filename);
		file << output_json_file;
		file.close();
		cout << "任务分配前目标点数据已导出" << endl;
	}
	tmp_json_var_of_before_assassin_task = output_json_file;
	return output_json_file;
}

json_of_nlohmann UAVTaskPlanning::get_tasks_after_assassin(const string filename/*=="NULL"*/)
{
	json_of_nlohmann output_task_after_assaign_json_file = tmp_json_var_of_before_assassin_task;
	for (auto& single_task : output_task_after_assaign_json_file["features"])
	{
		json_of_nlohmann lianxian;
		json_of_nlohmann airport;
		for (const auto& u : uavs)
		{
			if (u.DoingTask.id == single_task["properties"]["id"])
			{
				if (u.working)
				{
					lianxian.push_back(to_string(u.id));
					airport.push_back(u.airport);
				}
				else
				{
					lianxian.push_back("NULL");
					airport.push_back("NULL");
				}
			}
		}
		if (lianxian.size() == 0)
		{
			lianxian.push_back("NULL");
			airport.push_back("NULL");
		}
		single_task["properties"]["airport"] = airport;
		single_task["properties"]["lianxian"] = lianxian;
	}

	if (filename != "NULL")
	{
		std::ofstream file2(filename);
		file2 << output_task_after_assaign_json_file;
		file2.close();
		std::cout << "任务分配后目标点数据已导出" << endl;
	}
	return output_task_after_assaign_json_file;
}

json_of_nlohmann UAVTaskPlanning::get_lianxian(const string filename/*=="NULL"*/)
{
	json_of_nlohmann j;
	for (const auto& u : uavs)
	{
		int num_of_a = 0;
		int num_of_b = 0;
		int num_of_c = 0;
		if (u.working)
		{
			//计算与该条线重合的其他全部飞机的类型数据
			for (const auto& tmp_uav : uavs)
			{
				if (tmp_uav.working && tmp_uav.airport == u.airport && tmp_uav.DoingTask.id == u.DoingTask.id)
				{
					if (tmp_uav.type == "A") num_of_a++;
					else if (tmp_uav.type == "B") num_of_b++;
					else num_of_c++;
				}
			}
			j[u.uav_name]["type"] = u.type;
			j[u.uav_name]["A"] = num_of_a;
			j[u.uav_name]["B"] = num_of_b;
			j[u.uav_name]["C"] = num_of_c;
			j[u.uav_name]["Airport"]["id"] = u.airport;
			j[u.uav_name]["Airport"]["coordinates"]["x"] = u.x;
			j[u.uav_name]["Airport"]["coordinates"]["y"] = u.y;
			j[u.uav_name]["Task"]["id"] = u.DoingTask.id;
			j[u.uav_name]["Task"]["coordinates"]["x"] = u.DoingTask.x;
			j[u.uav_name]["Task"]["coordinates"]["y"] = u.DoingTask.y;
		}
	}
	if (filename != "NULL")
	{
		ofstream file(filename);
		file << j;
		file.close();
		cout << "连线数据保存成功" << endl;
	}
	return j;
}

void UAVTaskPlanning::run()
{
	/// 开始任务分配--合同网算法
	int uav_num = uavs.size();
	double dis = 0;
	for (auto& single_task : all_tasks)
	{
		for (auto& need_uav_type : single_task.used_uav_type)
		{
			std::vector<double> dis_list(uav_num, 1e+20);
			for (int i = 0; i < uavs.size(); i++)
			{
				if (uavs[i].type != need_uav_type) continue;
				if (uavs[i].working == true) continue;
				dis = cal_distance_two_lonlat(uavs[i].x, uavs[i].y, single_task.x, single_task.y);
				dis_list[i] = dis;
			}

			int index = min_element(dis_list.begin(), dis_list.end()) - dis_list.begin(); //把距离最近的无工作UAV执行该任务
			if (index == 0 && dis_list[index] == 1e+20) // 证明所有飞机都在飞
			{
				cout << "没有飞机空闲" << endl;
			}
			else {
				uavs[index].DoingTask = single_task;
				uavs[index].working = true;
				need_uav_type = " ";
				single_task.conduct = true;
			}
		}
	}
}

void UAVTaskPlanning::run(json_of_nlohmann appoint_info)
{
	vector<Task> tmp_all_task = all_tasks;
	for (auto& ix : appoint_info)
	{
		bool isassigned = false;
		for (auto& ta : tmp_all_task)
		{
			if (!(ta.id == (int)ix["task"]["id"])) continue;
			for (auto& used_uav : ta.used_uav_type)
			{
				if (!(used_uav == ix["type"]["name"])) continue;
				for (auto& suav : uavs)
				{
					if (suav.working == true) continue;
					if (suav.airport == ix["airport"]["id"] && suav.type == used_uav)
					{
						suav.DoingTask = ta;
						suav.working = true;
						suav.start_time = (string)ix["start_time"];
						suav.uav_name = ix["uav_name"];

						suav.start_height = ix["start_height"];
						suav.end_height = ix["end_height"];
						suav.cruise_height = ix["cruise_height"];

						if (!ix["way_point"].empty()) {
							suav.wayPoints.resize(ix["way_point"].size());
							for (int i = 0; i < suav.wayPoints.size(); i++) {
								if (ix["way_point"][i].size() == 2) ix["way_point"][i].push_back(suav.cruise_height);

								suav.wayPoints[i].push_back(ix["way_point"][i][0]);
								suav.wayPoints[i].push_back(ix["way_point"][i][1]);
								suav.wayPoints[i].push_back(ix["way_point"][i][2]);
							}
						}

						if (!ix["speed"].empty()) {
							suav.speed = ix["speed"];
						}

						auto it = find(ta.used_uav_type.begin(), ta.used_uav_type.end(), used_uav);
						ta.used_uav_type.erase(it);
						isassigned = true;
						break;
					}
				}
				if (isassigned) break;
			}
			if (isassigned) break;
		}
	}
}

// 构造函数功能--初始化机场和任务点信息
UAVTaskPlanning::UAVTaskPlanning()
{
	cout << "当前任务分配版本：v(2.0) 2024年8月30日 09:51:51" << endl;
}

void UAVTaskPlanning::Update(const string a, const string b)
{
	json_of_nlohmann new_air_ports = read_json_file(a); //读取机场数据
	tmp_output_json_file = new_air_ports;
	json_of_nlohmann new_tasks = read_json_file(b); //读取机场数据

	UAV tmp_u = uavs[0];
	Task tmp_t = all_tasks[0];
	int airport_index = 0;
	int uav_id = 0;
	uavs.clear();
	all_tasks.clear();

	vector<string> type_list1 = { "A", "B", "C" };
	vector<string> type_list2 = { "num_a", "num_b", "num_c" };

	for (auto airport : new_air_ports["features"])
	{
		//cout << airport["properties"]["num_a"] << endl;
		//cout << airport["properties"]["num_b"] << endl;
		//cout << airport["properties"]["num_c"] << endl;

		for (int i = 0; i < type_list1.size(); i++) {
			for (int temp_num = 0; temp_num < airport["properties"][type_list2[i]]; temp_num++) {
				tmp_u.airport = to_string(airport_index);
				tmp_u.id = uav_id++;
				tmp_u.type = type_list1[i];
				tmp_u.working = false;
				tmp_u.x = airport["geometry"]["coordinates"][0];
				tmp_u.y = airport["geometry"]["coordinates"][1];
				uavs.push_back(tmp_u);
			}
		}

		//int numA = 0;
		//int numB = 0;
		//int numC = 0;
		//for (numA; numA < airport["properties"]["num_a"]; numA++)
		//{
		//	tmp_u.airport = to_string(airport_index);
		//	tmp_u.id = uav_id++;
		//	tmp_u.type = "A";
		//	tmp_u.working = false;
		//	tmp_u.x = airport["geometry"]["coordinates"][0];
		//	tmp_u.y = airport["geometry"]["coordinates"][1];
		//	uavs.push_back(tmp_u);
		//}
		//for (numB; numB < airport["properties"]["num_b"]; numB++)
		//{
		//	tmp_u.airport = to_string(airport_index);
		//	tmp_u.id = uav_id++;
		//	tmp_u.type = "B";
		//	tmp_u.working = false;
		//	tmp_u.x = airport["geometry"]["coordinates"][0];
		//	tmp_u.y = airport["geometry"]["coordinates"][1];
		//	uavs.push_back(tmp_u);
		//}
		//for (numC; numC < airport["properties"]["num_c"]; numC++)
		//{
		//	tmp_u.airport = to_string(airport_index);
		//	tmp_u.id = uav_id++;
		//	tmp_u.type = "C";
		//	tmp_u.working = false;
		//	tmp_u.x = airport["geometry"]["coordinates"][0];
		//	tmp_u.y = airport["geometry"]["coordinates"][1];
		//	uavs.push_back(tmp_u);
		//}
		airport_index++;
	}



	int task_index = 0;
	for (auto Task : new_tasks["features"])
	{
		tmp_t.conduct = false;
		tmp_t.id = task_index++;
		tmp_t.UAVIndex = -1;
		tmp_t.Urgency_level = 2;
		tmp_t.x = Task["geometry"]["coordinates"][0];
		tmp_t.y = Task["geometry"]["coordinates"][1];


		tmp_t.used_uav_type.clear();
		int a = 0;
		int b = 0;
		int c = 0;
		for (a; a < Task["properties"]["num_a"]; a++)
		{
			tmp_t.used_uav_type.push_back("A");
		}
		for (b; b < Task["properties"]["num_b"]; b++)
		{
			tmp_t.used_uav_type.push_back("B");
		}
		for (c; c < Task["properties"]["num_c"]; c++)
		{
			tmp_t.used_uav_type.push_back("C");
		}
		all_tasks.push_back(tmp_t);
	}
}

void UAVTaskPlanning::InitRandomMode(string airports_json_file_path, string tasks_range_file_path, int min_num_uav, int max_num_uav, int num_of_tasks, int min_num_task, int max_num_task)
{
	/// 初始化机场信息
	json_of_nlohmann air_ports_json_file = read_json_file(airports_json_file_path); //读取机场数据
	UAV tmpu; // 用于存放临时生成的无人机信息
	int index_airport = 0; //机场id的索引
	int uav_id = 0; //无人机id的索引
	for (auto& single_airport : air_ports_json_file["features"])
	{
		single_airport["properties"]["num_a"] = 0; //a类飞机数量初始化
		single_airport["properties"]["num_b"] = 0; //b类飞机数量初始化
		single_airport["properties"]["num_c"] = 0; //c类飞机数量初始化
		single_airport["properties"]["num_totle"] = 0; //飞机数量总和初始化
		int UAV_num = random_int(min_num_uav, max_num_uav); //该机场飞机数量，随机设定
		for (int i = 0; i < UAV_num; i++) //循环产生飞机
		{
			tmpu.x = single_airport["geometry"]["coordinates"][0];
			tmpu.y = single_airport["geometry"]["coordinates"][1];
			tmpu.id = uav_id++;
			tmpu.airport = single_airport["properties"]["id"];
			int type_index = random_int(0, 2); //随机指定该飞机的类型
			tmpu.type = kUAVtype[type_index];
			uavs.push_back(tmpu);
		}
	}
	//把新生成的每个机场的飞机数量更新到json文档中
	for (auto& u : uavs)
	{
		if (u.type == "A")
		{
			int a = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_a"];
			a++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_a"] = a;
			int b = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"];
			b++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
		}
		else if (u.type == "B")
		{
			int a = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_b"];
			a++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_b"] = a;
			int b = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"];
			b++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
		}
		else
		{
			int a = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_c"];
			a++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_c"] = a;
			int b = air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"];
			b++;
			air_ports_json_file["features"][stoi(u.airport)]["properties"]["num_totle"] = b;
		}
	}
	tmp_air_ports_json_file = air_ports_json_file; //保存一份初始化后的机场对象数据

	//初始化任务点信息
	vector<Point> task_points = test_gen_random_point(tasks_range_file_path, num_of_tasks);
	Task tmpt;
	int task_id = 0;
	for (int i = 0; i < task_points.size(); i++)
	{
		tmpt.x = task_points[i].x;
		tmpt.y = task_points[i].y;
		tmpt.id = task_id++;
		// 每个任务点，会随机需要3,5架不同类型的无人机
		tmpt.used_uav_type.clear();
		for (int j = 0; j < random_int(min_num_task, max_num_task); j++)
		{
			int need_uav_type_index1 = random_int(0, 2);
			tmpt.used_uav_type.push_back(kUAVtype[need_uav_type_index1]);
		}
		all_tasks.push_back(tmpt);
	}
	///// 上传模式的代码
	//json_of_nlohmann new_air_ports = a; //载入机场
	//new_tasks = b; //载入任务

	////初始化
	//UAV tmp_u; Task tmp_t; int airport_index = 0; int uav_id = 0; int task_index = 0;
	//uavs.clear(); all_tasks.clear();
	//vector<string> type_list1 = { "A", "B", "C" };
	//vector<string> type_list2 = { "num_a", "num_b", "num_c" };

	////依次读取机场
	//for (auto& airport : new_air_ports["features"])
	//{
	//	for (int i = 0; i < type_list1.size(); i++) {
	//		for (int temp_num = 0; temp_num < airport["properties"][type_list2[i]]; temp_num++) {
	//			tmp_u.airport = airport["properties"]["id"];
	//			tmp_u.id = uav_id++;
	//			tmp_u.type = type_list1[i];
	//			tmp_u.working = false;
	//			tmp_u.x = airport["geometry"]["coordinates"][0];
	//			tmp_u.y = airport["geometry"]["coordinates"][1];
	//			uavs.push_back(tmp_u);
	//		}
	//	}
	//	airport_index++;
	//}
	//tmp_air_ports_json_file = new_air_ports;

	////依次读取任务
	//for (auto& Task : new_tasks["features"])
	//{
	//	tmp_t.conduct = false;
	//	tmp_t.id = Task["properties"]["id"];
	//	tmp_t.UAVIndex = -1;
	//	tmp_t.Urgency_level = 2;
	//	tmp_t.x = Task["geometry"]["coordinates"][0];
	//	tmp_t.y = Task["geometry"]["coordinates"][1];
	//	tmp_t.name = Task["properties"]["name"];
	//	tmp_t.SAMPLE_Z1 = Task["properties"]["SAMPLE_Z1"];
	//	tmp_t.used_uav_type.clear();
	//	int a(0), b(0), c(0);
	//	for (a; a < Task["properties"]["num_a"]; a++)
	//	{
	//		tmp_t.used_uav_type.push_back("A");
	//	}
	//	for (b; b < Task["properties"]["num_b"]; b++)
	//	{
	//		tmp_t.used_uav_type.push_back("B");
	//	}
	//	for (c; c < Task["properties"]["num_c"]; c++)
	//	{
	//		tmp_t.used_uav_type.push_back("C");
	//	}
	//	all_tasks.push_back(tmp_t);
	//}
}

void UAVTaskPlanning::InitSelectMode(json_of_nlohmann a, json_of_nlohmann b)
{
	json_of_nlohmann new_air_ports = a; //载入机场
	new_tasks = b; //载入任务

	//初始化
	UAV tmp_u; Task tmp_t; int airport_index = 0; int uav_id = 0; int task_index = 0;
	uavs.clear(); all_tasks.clear();
	vector<string> type_list1 = { "A", "B", "C" };
	vector<string> type_list2 = { "num_a", "num_b", "num_c" };

	//依次读取机场
	for (auto& airport : new_air_ports["features"])
	{
		for (int i = 0; i < type_list1.size(); i++) {
			for (int temp_num = 0; temp_num < airport["properties"][type_list2[i]]; temp_num++) {
				tmp_u.airport = airport["properties"]["id"];
				tmp_u.id = uav_id++;
				tmp_u.type = type_list1[i];
				tmp_u.working = false;
				tmp_u.x = airport["geometry"]["coordinates"][0];
				tmp_u.y = airport["geometry"]["coordinates"][1];
				uavs.push_back(tmp_u);
			}
		}
		airport_index++;
	}
	tmp_air_ports_json_file = new_air_ports;

	//依次读取任务
	for (auto& Task : new_tasks["features"])
	{
		tmp_t.conduct = false;
		tmp_t.id = Task["properties"]["id"];
		tmp_t.UAVIndex = -1;
		tmp_t.Urgency_level = 2;
		tmp_t.x = Task["geometry"]["coordinates"][0];
		tmp_t.y = Task["geometry"]["coordinates"][1];
		tmp_t.name = Task["properties"]["name"];
		tmp_t.SAMPLE_Z1 = Task["properties"]["SAMPLE_Z1"];
		tmp_t.used_uav_type.clear();
		int a(0), b(0), c(0);
		for (a; a < Task["properties"]["num_a"]; a++)
		{
			tmp_t.used_uav_type.push_back("A");
		}
		for (b; b < Task["properties"]["num_b"]; b++)
		{
			tmp_t.used_uav_type.push_back("B");
		}
		for (c; c < Task["properties"]["num_c"]; c++)
		{
			tmp_t.used_uav_type.push_back("C");
		}
		all_tasks.push_back(tmp_t);
	}
}

vector<Point> UAVTaskPlanning::get_tasks_positon()
{
	vector<Point> tasks_positon;
	Point tmp;
	for (const auto& single_task : all_tasks)
	{
		tmp.x = single_task.x;
		tmp.y = single_task.y;
		tasks_positon.push_back(tmp);
	}
	return tasks_positon;
}

void UAVTaskPlanning::print_uav_task_relation()
{
	int sum = 0;
	for (const auto& u : uavs)
	{
		if (u.working)
		{
			cout << "为" << to_string(u.id) << "号飞机" << "\t类型是：" << u.type << "\t机场为：" << u.airport << " 分配 " << to_string(u.DoingTask.id) << " 任务" << endl;
			sum++;
		}
	}
	if (sum != uavs.size())
	{
		cout << "共有" << to_string(sum) << "架飞机起飞执行任务，飞机未全部起飞，机场有剩余" << endl;
	}
	else
	{
		cout << "共有" << to_string(sum) << "架飞机起飞执行任务，飞机已经全部起飞" << endl;
	}
}

void UAVTaskPlanning::save_uav_init_goal_file(const string filename, string start_time)
{
	// 将郭浩规划的任务信息转化为我这边的路径信息
	map<double, map<double, int>> airport_uav_cnts;

	long long start_time_value = gho_timeString2Sec(start_time);

	ofstream uav_output_file(filename);
	vector<double> lon;
	vector<double> lat;
	for (auto u : uavs)
	{
		if (u.working)
		{
			uav_output_file << "[UAV]" << to_string(u.id) << endl;
			uav_output_file << to_string(0) << endl;

			//long long temp_start_time = start_time_value + 30 * airport_uav_cnts[u.x][u.y];
			long long temp_start_time = start_time_value;
			string temp_start_time_value = gho_timeSec2String(temp_start_time);

			double target_height = 3000;
			if (u.type == "A") target_height = 2000;
			else if (u.type == "B") target_height = 3000;
			else if (u.type == "C") target_height = 4000;
			uav_output_file << target_height << endl;

			uav_output_file << temp_start_time_value << endl;
			uav_output_file << to_string(u.x) << " " << to_string(u.y) << " " << "0" << endl;
			uav_output_file << to_string(u.DoingTask.x) << " " << to_string(u.DoingTask.y) << " " << "0" << endl;
			lon.push_back(u.x);
			lon.push_back(u.DoingTask.x);
			lat.push_back(u.y);
			lat.push_back(u.DoingTask.y);

			airport_uav_cnts[u.x][u.y]++;
		}
	}
	uav_output_file.close();
}

void UAVTaskPlanning::ClearAll()
{

	uavs.clear();
	all_tasks.clear();
	tmp_air_ports_json_file.clear();
	tmp_json_var_of_before_assassin_task.clear();
}

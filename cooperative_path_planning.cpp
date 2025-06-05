#include "cooperative_path_planning.h"

void CooperativePathPlanning::readInfo(string infoFolder)
{
	/// 指定模式用法 - 指定无人机执行特定任务
	json_of_nlohmann var_new_air_ports = read_json_file(infoFolder + "/airport.geojson");//读取机场数据
	json_of_nlohmann var_new_tasks = read_json_file(infoFolder + "/target.geojson");       //载入任务数据
	pUav.InitSelectMode(var_new_air_ports, var_new_tasks);                                                   //直接传入相关JSON数据
	json_of_nlohmann airports_before_assassin = pUav.get_airports_before_assassin(infoFolder + "/result/11.geojson"); //导出分配前的机场信息
	json_of_nlohmann tasks_before_assassin = pUav.get_tasks_before_assassin(infoFolder + "/result/22.geojson");       //导出分配前的任务信息
	json_of_nlohmann appoint_info = read_json_file(infoFolder + "/lianxian.geojson");
	pUav.run(appoint_info);
	pUav.print_uav_task_relation();
	json_of_nlohmann airports_after_assassin = pUav.get_airports_after_assassin(infoFolder + "/result/33.geojson");   //导出任务分配后的机场对象信息
	json_of_nlohmann tasks_after_assassin = pUav.get_tasks_after_assassin(infoFolder + "/result/44.geojson");         //导出任务分配后的任务点对象信息
	json_of_nlohmann lianxian = pUav.get_lianxian(infoFolder + "/result/lianxian.json");                              //导出任务分配后的目标连线信息，以及该条连线上有多少A类飞机、B类飞机、C类飞机等
}

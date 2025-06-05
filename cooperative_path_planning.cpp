#include "cooperative_path_planning.h"

void CooperativePathPlanning::readInfo(string infoFolder)
{
	/// ָ��ģʽ�÷� - ָ�����˻�ִ���ض�����
	json_of_nlohmann var_new_air_ports = read_json_file(infoFolder + "/airport.geojson");//��ȡ��������
	json_of_nlohmann var_new_tasks = read_json_file(infoFolder + "/target.geojson");       //������������
	pUav.InitSelectMode(var_new_air_ports, var_new_tasks);                                                   //ֱ�Ӵ������JSON����
	json_of_nlohmann airports_before_assassin = pUav.get_airports_before_assassin(infoFolder + "/result/11.geojson"); //��������ǰ�Ļ�����Ϣ
	json_of_nlohmann tasks_before_assassin = pUav.get_tasks_before_assassin(infoFolder + "/result/22.geojson");       //��������ǰ��������Ϣ
	json_of_nlohmann appoint_info = read_json_file(infoFolder + "/lianxian.geojson");
	pUav.run(appoint_info);
	pUav.print_uav_task_relation();
	json_of_nlohmann airports_after_assassin = pUav.get_airports_after_assassin(infoFolder + "/result/33.geojson");   //������������Ļ���������Ϣ
	json_of_nlohmann tasks_after_assassin = pUav.get_tasks_after_assassin(infoFolder + "/result/44.geojson");         //�����������������������Ϣ
	json_of_nlohmann lianxian = pUav.get_lianxian(infoFolder + "/result/lianxian.json");                              //�������������Ŀ��������Ϣ���Լ������������ж���A��ɻ���B��ɻ���C��ɻ���
}

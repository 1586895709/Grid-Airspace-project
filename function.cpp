#include "function.h"

json_of_nlohmann read_json_file(std::string filename)
{
	json_of_nlohmann JSONFile;
	std::ifstream file(filename);
	file >> JSONFile;
	file.close();
	return JSONFile;
}




int random_int(const int lb, const int ub)
{
	std::random_device seed;
	std::mt19937 RandomEngine(seed());
	std::uniform_int_distribution<int> random(lb, ub);
	return random(RandomEngine);
}

double random_double(const double lb, const double ub)
{
	std::random_device seed;
	std::mt19937 RandomEngine(seed());
	std::uniform_real_distribution<double> random(lb, ub);
	return random(RandomEngine);
}

time_t gho_timeString2Sec(std::string timeString)
{
	if (timeString.find(":") != std::string::npos) {
		// "2021/09/03 08:00:00"
		//timeString = "2021/09/03 08:00:00";
		//timeString = "2023-02-02T16:00:05Z"
		// �����ַ������Ǳ�׼��ʽ���ȳ�
		if (timeString.length() != 19)
		{
			timeString = timeString.substr(0, 19);
		}
		std::string year, month, day, hour, minute, second;
		year = timeString.substr(0, 4);
		month = timeString.substr(5, 2);
		day = timeString.substr(8, 2);
		hour = timeString.substr(11, 2);
		minute = timeString.substr(14, 2);
		second = timeString.substr(17, 2);

		tm temp_tm;
		temp_tm.tm_year = stoi(year) - 1900;
		temp_tm.tm_mon = stoi(month) - 1;
		temp_tm.tm_mday = stoi(day);
		temp_tm.tm_hour = stoi(hour);
		temp_tm.tm_min = stoi(minute);
		temp_tm.tm_sec = stoi(second);

		time_t temp_t = mktime(&temp_tm);
		return temp_t;
	}
	else {
		time_t temp_t = std::stoull(timeString);
		return temp_t;
	}
}


std::string gho_timeSec2String(time_t timeStamp)
{
	// ����һ�� tm �ṹ��������ڴ洢ʱ����Ϣ
	struct tm timeinfo;
	// ʹ�� localtime_s ������ʱ���ת��Ϊ����ʱ��
	if (localtime_s(&timeinfo, &timeStamp) != 0) {
		std::cout << "The time transformation fails!" << std::endl;
		// ���ת��ʧ�ܣ�����ʱ������ַ�����ʽ
		return std::to_string(timeStamp);
	}
	// Ϊ��ʽ������ַ������仺����
	char buffer[21];
	// ʹ�� strftime ������ʱ����Ϣ��ʽ��Ϊ�ַ���
	strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
	// ���ظ�ʽ������ַ���
	return std::string(buffer);
}

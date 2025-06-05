#include "util_function.h"

CoorXYZ latLonToMercator(CoorXYZ inputGeopt)
{
	CoorXYZ result = inputGeopt;
	result[0] = RADIUS * inputGeopt[0] * M_PI / 180.0;
	result[1] = RADIUS * log(tan(M_PI / 4.0 + inputGeopt[1] * M_PI / 360.0));
	return result;
}

CoorXYZ mercatorToLatLon(CoorXYZ inputMercatorPt)
{
	CoorXYZ result;
	result[0] = (inputMercatorPt[0] / RADIUS) * 180.0 / M_PI;
	result[1] = 360.0 * atan(exp(inputMercatorPt[1] / RADIUS)) / M_PI - 90.0;

	return result;
}

double calAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3)
{
	CoorXYZ l12 = p2 - p1;
	CoorXYZ l23 = p3 - p2;

	double l12Norm = l12.norm();
	double l23Norm = l23.norm();
	if (fabs(l12Norm) < 1e-6 || fabs(l23Norm) < 1e-6) {
		return 0.0;
	}

	double cosTheta = l12.dot(l23) / (l12Norm * l23Norm);
	cosTheta = std::max(-1.0, std::min(cosTheta, 1.0));
	return acos(cosTheta) / M_PI * 180.0;
}

double calHorizonAngle(CoorXYZ p1, CoorXYZ p2, CoorXYZ p3)
{
	p1[2] = 0;
	p2[2] = 0;
	p3[2] = 0;

	return calAngle(p1, p2, p3);
}

bool judgeInLine(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3) {
	double changleAngle = calAngle(p1, p2, p3);
	if (abs(changleAngle) < 1e-8) return true;
	return false;
}

bool judgeInLine(Eigen::Vector3i p1, Eigen::Vector3i p2, Eigen::Vector3i p3)
{
	// �������� AB �� AC
	Eigen::Vector3i AB = p2 - p1;
	Eigen::Vector3i AC = p3 - p1;

	// �������� AB �� AC �Ĳ��
	Eigen::Vector3i crossProduct = AB.cross(AC);

	// �жϲ���Ƿ�Ϊ������
	return crossProduct.isZero();
}

void stringSplit(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters /*= " "*/)
{
	std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
	std::string::size_type pos = s.find_first_of(delimiters, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}
}

void readJsonFile(std::string jsonFileName, nlohmann::json& jsonValue)
{
	std::ifstream jsonFile(jsonFileName);
	if (!jsonFile.is_open())
	{
		LOGI << "�޷���json�ļ�" << std::endl;
	}
	jsonFile >> jsonValue;
}

time_t timeString2Sec(std::string timeString)
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

std::string timeSec2String(time_t timeStamp) {
	// ����һ�� tm �ṹ��������ڴ洢ʱ����Ϣ
	struct tm timeinfo;
	// ʹ�� localtime_s ������ʱ���ת��Ϊ����ʱ��
	if (localtime_s(&timeinfo, &timeStamp) != 0) {
		LOGI << "The time transformation fails!" << std::endl;
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

bool isIntervalIntersection(TimeInterval interval1, TimeInterval interval2, double safeInterval /*= 0.0*/)
{
	if (interval1.size() != 2 || interval2.size() != 2)
	{
		LOGI << "������������" << std::endl;
		return true;
	}

	//if (fabs(interval1[0] - interval1[1]) < 1e-6 || fabs(interval2[0] - interval2[1]) < 1e-6) return false;

	if (interval1[0] > interval2[1] + safeInterval || interval1[1] + safeInterval < interval2[0]) return false;
	return true;
}

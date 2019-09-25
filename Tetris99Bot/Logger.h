#pragma once

#include <ctime>  
#include <iostream>  
#include <fstream>  
#include <direct.h>

using namespace std;

#ifndef __EASYLOG_PIPI_0813
#define __EASYLOG_PIPI_0813

class Loger
{
public:
	static void Log(std::string log) {
		try
		{
			std::ofstream ofs;
			time_t t = time(0); 
			struct tm now_time;
			char tmp[64]; 
			localtime_s(&now_time, &t);
			//strftime(tmp, sizeof(tmp), "[%Y-%m-%d %X]",);
			ofs.open("D:\\PipeLog.log", std::ofstream::app);

			ofs << tmp << " - ";
			ofs.write(log.c_str(), log.size());
			ofs << std::endl;
			ofs.close();
		}
		catch (...)
		{
		}
	}
};

#endif
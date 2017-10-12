#include "logger.h"
#include <string>
#include <cstdarg>
#include <iostream>

#define LOG_SIZE 1024
logger::logger()
	: m_level(LOG_LEVEL_DEBUG)
{
}

logger::~logger()
{
}

void logger::log(LOG_LEVEL level, const char* funname, int nline, const char* plog, ...)
{
	if (level > m_level || funname == NULL || plog == NULL)
	{
		return;
	}
	
	std::string strLog;
	try
	{
		char chLog[LOG_SIZE + 1] = { 0 };

		va_list ap;
		va_start(ap, plog);
		if (vsnprintf(chLog, LOG_SIZE - 1, plog, ap) > 0)
		{
			strLog = chLog;
		}
		va_end(ap);
	}
	catch (...)
	{
		strLog = "Exception on log";
	}
	std::cout << strLog << std::endl;
}
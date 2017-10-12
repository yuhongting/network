#pragma once
#include "singleton.hpp"

enum LOG_LEVEL
{
	LOG_LEVEL_NONE = 0,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
};

class logger : public singleton<logger>
{
public:
	logger();
	virtual ~logger();

	void log(LOG_LEVEL level, const char* funname, int nline, const char* plog, ...);

	int m_level;
};

#define LOG_INFO(str, ...)		logger::instance().log(LOG_LEVEL_INFO, __FUNCTION__, __LINE__, str, ##__VA_ARGS__)
#define LOG_DEBUG(str, ...)		logger::instance().log(LOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, str, ##__VA_ARGS__)


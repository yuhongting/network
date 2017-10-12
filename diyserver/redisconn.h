//redisconn.hpp
//created by yht 2017-08-27
//simple redis connection class based on a open source template class

#pragma once
#include "singleton.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include "redisclient/redissyncclient.h"

class redisconn :
	public singleton<redisconn>
{
public:
	redisconn(void);
	virtual ~redisconn(void);
	bool connect(std::string& addr,std::string& port);
	bool setvalue(std::string& key, char* value);
	bool getvalue(std::string& key,std::string& value);

	boost::asio::io_service      m_ios;
	redisclient::RedisSyncClient m_redis;
	std::string                  m_errmsg;
	std::string                  m_addr;
	std::string                  m_port;
};

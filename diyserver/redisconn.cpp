//redisconn.cpp
//created by yht 2017-08-27
//impl the redisconn class

#include "redisconn.h"
#include "logger.h"

redisconn::redisconn(void)
	: m_ios()
	, m_redis(m_ios)
	, m_addr("")
	, m_port("")
{
}

redisconn::~redisconn(void)
{
}

bool redisconn::connect( std::string& addr,std::string& port )
{
	m_addr = addr;
	m_port = addr;
	boost::asio::ip::address address = boost::asio::ip::address::from_string(addr);
	const unsigned short nport = atoi(port.c_str());
	if( !m_redis.connect(address, nport, m_errmsg) )
	{
		LOG_INFO("Can't connect to redis: %s", m_errmsg.c_str());
		return false;
	}
	LOG_INFO("Connect to redis host %s %s success", addr.c_str(), port.c_str());
	return true;
}

bool redisconn::setvalue(std::string& key, char* value)
{
	redisclient::RedisValue result;
	result = m_redis.command("SET", {key.c_str(),value});
	LOG_DEBUG("Resis Run:SET %s %s", key.c_str(),value);
	LOG_DEBUG("result: %s",result.toString().c_str());
	return true;
}

bool redisconn::getvalue(std::string& key, std::string& value)
{
	redisclient::RedisValue result;
	result = m_redis.command("GET", {key.c_str()});
	value = result.toString();
	LOG_DEBUG("Resis Run:GET %s", key.c_str());
	LOG_DEBUG("result: %s", result.toString().c_str());
	return true;
}

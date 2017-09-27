//redisconn.cpp
//created by yht 2017-09-26
//impl the redisconn class

#include "redisconn.h"

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
		std::cerr << "Can't connect to redis: " << m_errmsg << std::endl;
		return false;
	}
	/*redisclient::RedisValue result;
	result = m_redis.command("GET", {"fuck"});
	std::cout << "GET: " << result.toString() << "\n";*/
	return true;
}

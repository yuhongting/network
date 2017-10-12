//
// main.cpp
// copy from boost.asio http demo by yht 2017-08-27
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "redisconn.h"
#include "msgpack.hpp"
#include "logger.h"

int main(int argc, char* argv[])
{
	try
	{
		std::string addr = "127.0.0.1";
		std::string port = "6379";
		redisconn::instance().connect(addr, port);
		server s("127.0.0.1", "10008", 2);
		s.run();
	}
	catch (std::exception& e)
	{
		LOG_INFO(e.what());
	}
	getchar();

	return 0;
}

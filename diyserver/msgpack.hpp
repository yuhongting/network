//msgpack.hpp
//created by yht 2017-08-27
//simple message pack impl

#ifndef MSGPACK_HPP
#define MSGPACK_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "netpack.hpp"
#include "connection.hpp"
#include "logger.h"

struct msgpack
{
	msgpack(netpack* pack,connection_ptr& con)
	{
		npack = pack;
		conn = con;
	}
	virtual ~msgpack()
	{
		if (npack)
		{
			delete npack;
			npack = NULL;
		}
	}
	netpack* npack;
	connection_ptr conn;
};

#endif

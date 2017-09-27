//msgpack.hpp
//created by yht 2017-09-26
//simple message pack impl

#ifndef MSGPACK_HPP
#define MSGPACK_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "netpack.hpp"
#include "connection.hpp"

struct msgpack
{
	msgpack(netpack* pack,connection_ptr& con)
	{
		npack = pack;
		conn = con;
	}
	netpack* npack;
	connection_ptr conn;
};

#endif

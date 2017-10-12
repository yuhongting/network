#include "msgpack.hpp"
#include "diysector.h"
#include "redisconn.h"
#include "usermanager.h"
#include "logger.h"

diysector::diysector(void)
{
}

diysector::~diysector(void)
{
}

#define MAX_ELEMENT_LENGTH      32

int get_next_str(netpack* npack, std::string& str,int npos)
{
	str = "";
	char* pdata = npack->data();
	int nsize = npack->size();
	if (!pdata || nsize <= 0)
	{
		return -1;
	}
	char buf[MAX_ELEMENT_LENGTH] = { 0 };
	int endpos = npos + MAX_ELEMENT_LENGTH;
	for (int i = npos; i < nsize && i < endpos; i++)
	{
		if (pdata[i] == ' ' || i == nsize - 1)
		{
			int len = i - npos;
			if (i == nsize - 1) {
				len++;
			}
			memcpy(buf, pdata + npos, len);
			npos = i + 1;
			str = buf;
			return npos;
		}
	}
	return -1;
}

void send_msg(msgpack* pack,const char* str)
{
	if (pack->conn)
	{
		std::string value = str;
		netpack* npack = new netpack(value.length() + 1);
		npack->data(value);
		pack->conn->write(npack);
	}
}

int handle_get(msgpack* pack, int npos)
{
	std::string key = "";
	std::string value;
	npos = get_next_str(pack->npack, key, npos);
	if (npos > 0 && pack->conn)
	{
		key = pack->conn->getuser() + "|" + key;
		if (redisconn::instance().getvalue(key, value) && value.length())
		{
			send_msg(pack, "2 OK");
			netpack* npack = new netpack(value.length() + 1);
			npack->data(value);
			pack->conn->write(npack);
		}
		else
		{
			send_msg(pack, "1 none");
		}
	}
	return 0;
}

int handle_set(msgpack* pack,int npos)
{
	std::string key = "";
	npos = get_next_str(pack->npack, key, npos);
	if(npos > 0)
	{
		key = pack->conn->getuser() + "|" + key;
		if (redisconn::instance().setvalue(key, pack->npack->data() + npos))
		{
			send_msg(pack, "0 OK");
		}
	}
	return 0;
}

int handle_login(msgpack* pack,int npos)
{
	std::string user = "";
	if (get_next_str(pack->npack, user, npos) > 0)
	{
		usermanager::instance().login(user, pack->conn);
		send_msg(pack, "0 OK");
	}
	else
	{
		send_msg(pack, "1 login username");
	}
	return 0;
}

int handle_logout(msgpack* pack,int npos)
{
	if (pack->conn)
	{
		pack->conn->stop();
	}
	return 0;
}

int handle_undef(msgpack* pack, int npos)
{
	send_msg(pack, "1 undefined command");
	return 0;
}

bool check_login(msgpack* pack, int npos)
{
	if (pack && pack->conn && pack->conn->getuser().length() > 0)
	{
		return true;
	}
	send_msg(pack, "1 Please login first");
	return false;
}

void diysector::handle_msg( void* msg )
{
	msgpack* pack = (msgpack*)msg;
	std::string cmd = "";
	int npos = 0;
	npos = get_next_str(pack->npack, cmd, npos);
	if (npos > 0)
	{
		if (cmd != "login")
		{
			if (!check_login(pack, npos))
			{
				return;
			}
		}
		if (cmd == "login")
		{
			handle_login(pack, npos);
		}
		else if (cmd == "exit" 
			|| cmd == "end" 
			|| cmd == "quit" 
			|| cmd == "bye")
		{
			handle_logout(pack, npos);
		}
		else if (cmd == "get")
		{
			LOG_DEBUG("User: %s Command: get", pack->conn->getuser().c_str());
			handle_get(pack,npos);
		}
		else if (cmd == "set")
		{
			LOG_DEBUG("User: %s Command: set", pack->conn->getuser().c_str());
			handle_set(pack, npos);
		}
		else
		{
			LOG_DEBUG("User: %s Command: %s", pack->conn->getuser().c_str(),cmd.c_str());
			handle_undef(pack,npos);
		}
	}
}
#pragma once
#include "connection.hpp"
#include "singleton.hpp"
#include <map>

class usermanager : public singleton<usermanager>
{
public:
	usermanager();
	virtual ~usermanager();
	void login(std::string& user, connection_ptr& conn);
	void logout(std::string& user);
protected:
	std::map<std::string, connection_ptr> m_users;
};


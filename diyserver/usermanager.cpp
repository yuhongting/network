#include "usermanager.h"
#include "logger.h"

usermanager::usermanager()
{
}

usermanager::~usermanager()
{
}

void usermanager::login(std::string& user, connection_ptr& conn)
{
	std::map<std::string, connection_ptr>::iterator ite = m_users.find(user);
	if (ite != m_users.end())
	{
		m_users.erase(ite);
	}
	m_users[user] = conn;
	conn->setuser(user);
	LOG_INFO("User: %s login", user.c_str());
	LOG_INFO("Current User count: %d", m_users.size());
}

void usermanager::logout(std::string& user)
{
	std::map<std::string, connection_ptr>::iterator ite = m_users.find(user);
	if (ite != m_users.end())
	{
		m_users.erase(ite);
		LOG_INFO("User: %s logout", user.c_str());
		LOG_INFO("Current User count: %d", m_users.size());
	}
}
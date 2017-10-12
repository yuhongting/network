//connection.cpp
//created by yht 2017-08-27
//impl the connection class

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "diysector.h"
#include "msgpack.hpp"
#include "logger.h"
#include "usermanager.h"

connection::connection(boost::asio::io_service& io_service)
	: strand_(io_service)
	, socket_(io_service)
	, m_sending(false)
	, m_user("")
	, m_buffer(NULL)
	, m_data(NULL)
{
	LOG_DEBUG("connection construction");
}

connection::~connection()
{
	LOG_DEBUG("connection distruction");
}

boost::asio::ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::start()
{
	m_buffer = new netpack();
	socket_.async_read_some(boost::asio::buffer(m_buffer->data(),m_buffer->capacity()),
		strand_.wrap(
		boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));
}

void connection::stop()
{
	boost::system::error_code ignored_ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	usermanager::instance().logout(m_user);
	if (m_buffer)
	{
		delete m_buffer;
		m_buffer = NULL;
	}
	for (std::list<netpack*>::iterator ite = m_sendPool.begin(); 
	ite != m_sendPool.end(); ite++)
	{
		netpack* p = *ite;
		delete p;
	}
	m_sendPool.clear();
}

void connection::check_pool()
{
	if(!m_sending && m_sendPool.size())
	{
		m_sending = true;
		netpack* npack = m_sendPool.front();
		LOG_DEBUG("send pack: %s size: %d", npack->data(), npack->size());
		boost::asio::async_write(socket_,boost::asio::buffer(npack->data(),npack->size()),
			strand_.wrap(
			boost::bind(&connection::handle_write, shared_from_this(),
			boost::asio::placeholders::error)));
	}
}

void connection::write(netpack* npack)
{
	boost::mutex::scoped_lock l(m_mutexWrite);
	m_sendPool.push_back(npack);
	check_pool();
}

void connection::handle_read(const boost::system::error_code& e,
							 std::size_t bytes_transferred)
{
	if (!e)
	{
		m_buffer->m_size = bytes_transferred;
		msgpack* msg = new msgpack(m_buffer,shared_from_this());
		diysector::instance().push(msg);
		m_buffer = new netpack();
		{
			socket_.async_read_some(boost::asio::buffer(m_buffer->data(),m_buffer->capacity()),
				strand_.wrap(
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));
		}
	}
	else
	{
		boost::system::error_code ignored_ec;
		LOG_INFO(e.message().c_str());
		stop();
	}

	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const boost::system::error_code& e)
{
	if (!e)
	{
		// Initiate graceful connection closure.
		boost::mutex::scoped_lock l(m_mutexWrite);
		netpack* npack = m_sendPool.front();
		m_sendPool.pop_front();
		delete npack;
		m_sending = false;
		check_pool();
	}
	else
	{
		boost::system::error_code ignored_ec;
		LOG_INFO(e.message().c_str());
		stop();
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

std::string& connection::getuser()
{
	return m_user;
}

void connection::setuser(std::string& user)
{
	m_user = user;
}
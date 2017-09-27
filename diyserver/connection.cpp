//connection.cpp
//created by yht 2017-09-26
//impl the connection class

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "diysector.h"
#include "msgpack.hpp"

connection::connection(boost::asio::io_service& io_service)
					   : strand_(io_service)
					   , socket_(io_service)
					   , m_sending(false)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::start()
{
	buffer_ = new netpack();
	socket_.async_read_some(boost::asio::buffer(buffer_->data(),buffer_->capacity()),
		strand_.wrap(
		boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));
}

void connection::check_pool()
{
	if(!m_sending && m_sendPool.size())
	{
		m_sending = true;
		netpack* npack = m_sendPool.front();
		boost::asio::async_write(socket_,boost::asio::buffer(npack->data(),npack->size()),
			strand_.wrap(
			boost::bind(&connection::handle_write, shared_from_this(),
			boost::asio::placeholders::error)));
	}
}

void connection::write(netpack* npack)
{
	m_sendPool.push_back(npack);
	check_pool();
}

void connection::handle_read(const boost::system::error_code& e,
							 std::size_t bytes_transferred)
{
	if (!e)
	{
		buffer_->m_size = bytes_transferred;
		msgpack* msg = new msgpack(buffer_,shared_from_this());
		diysector::instance().push(msg);
		buffer_ = new netpack();
		{
			socket_.async_read_some(boost::asio::buffer(buffer_->data(),buffer_->capacity()),
				strand_.wrap(
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));
		}
	}
	else
	{
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
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
		netpack* npack = m_sendPool.front();
		m_sendPool.pop_front();
		delete npack;
		m_sending = false;
		check_pool();
	}
	else
	{
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

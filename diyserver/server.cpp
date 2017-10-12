//server.cpp
//created by yht 2017-08-27
//impl the server class

#include "server.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "diysector.h"
#include "logger.h"

server::server(const std::string& address, const std::string& port,
			   std::size_t thread_pool_size)
			   : thread_pool_size_(thread_pool_size),
			   signals_(io_service_),
			   acceptor_(io_service_)
{
	// Register to handle the signals that indicate when the server should exit.
	// It is safe to register for the same signal multiple times in a program,
	// provided all registration for the specified signal is made through Asio.
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#if defined(SIGQUIT)
	signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
	signals_.async_wait(boost::bind(&server::handle_stop, this));

	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver(io_service_);
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	start_accept();
}

void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for (std::size_t i = 0; i < thread_pool_size_; ++i)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, &io_service_)));
		threads.push_back(thread);
	}
	diysector::instance().start();
	diysector::instance().join();
	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();
}

void server::start_accept()
{
	connection_ptr conn(new connection(io_service_));
	acceptor_.async_accept(conn->socket(),
		boost::bind(&server::handle_accept, this,
		boost::asio::placeholders::error,conn));
}

void server::handle_accept(const boost::system::error_code& e,connection_ptr conn)
{
	if (!e)
	{
		LOG_DEBUG("Client %s connected",
			conn->socket().remote_endpoint().address().to_string().c_str());
		conn->start();
	}

	start_accept();
}

void server::handle_stop()
{
	io_service_.stop();
}

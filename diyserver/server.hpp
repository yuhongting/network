//server.hpp
//created by yht 2017-08-27
//actually server is a network manager

#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "singleton.hpp"

/// The top-level class of the HTTP server.
class server
	: private boost::noncopyable
{
public:
	/// Construct the server to listen on the specified TCP address and port, and
	/// serve up files from the given directory.
	explicit server(const std::string& address, const std::string& port,
		std::size_t thread_pool_size);

	/// Run the server's io_service loop.
	void run();

private:
	/// Initiate an asynchronous accept operation.
	void start_accept();

	/// Handle completion of an asynchronous accept operation.
	void handle_accept(const boost::system::error_code& e, connection_ptr conn);

	/// Handle a request to stop the server.
	void handle_stop();

	/// The number of threads that will call io_service::run().
	std::size_t thread_pool_size_;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;

	/// The signal_set is used to register for process termination notifications.
	boost::asio::signal_set signals_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;
};

#endif

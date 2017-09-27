//connection.hpp
//created by yht 2017-09-26
//Represents a single connection from a client.

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <list>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "netpack.hpp"

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();
  void write(netpack* npack);

private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
      std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  //Check send pool and send the front of the queue
  void check_pool();

  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  netpack* buffer_;
  bool     m_sending;
  std::list<netpack*> m_sendPool;
};

typedef boost::shared_ptr<connection> connection_ptr;


#endif //CONNECTION_HPP

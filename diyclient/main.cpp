//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "netpack.hpp"
#include <string>  
#include <fstream>  
#include <sstream>  

using boost::asio::ip::tcp;
using namespace std;
typedef std::deque<netpack> netpack_queue;

class chat_client
{
public:
	chat_client(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		: io_service_(io_service),
		socket_(io_service)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
			boost::bind(&chat_client::handle_connect, this,
				boost::asio::placeholders::error));
	}

	void write(const netpack& msg)
	{
		io_service_.post(boost::bind(&chat_client::do_write, this, msg));
	}

	void close()
	{
		io_service_.post(boost::bind(&chat_client::do_close, this));
	}

private:

	void handle_connect(const boost::system::error_code& error)
	{
		if (!error)
		{
			socket_.async_read_some(boost::asio::buffer(read_msg_.data(), read_msg_.capacity()),
				boost::bind(&chat_client::handle_read, this,
					boost::asio::placeholders::error));
		}
	}

	void handle_read(const boost::system::error_code& error)
	{
		if (!error)
		{
			cout << read_msg_.data() << endl;
			memset(read_msg_.data(), 0, read_msg_.capacity());
			socket_.async_read_some(boost::asio::buffer(read_msg_.data(), read_msg_.capacity()),
				boost::bind(&chat_client::handle_read, this,
					boost::asio::placeholders::error));
		}
		else
		{
			cout << error.message() << endl;
			do_close();
		}
	}

	void do_write(netpack msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(write_msgs_.front().data(),
					write_msgs_.front().size()),
				boost::bind(&chat_client::handle_write, this,
					boost::asio::placeholders::error));
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				boost::asio::async_write(socket_,
					boost::asio::buffer(write_msgs_.front().data(),
						write_msgs_.front().size()),
					boost::bind(&chat_client::handle_write, this,
						boost::asio::placeholders::error));
			}
		}
		else
		{
			cout << error.message() << endl;
			do_close();
		}
	}

	void do_close()
	{
		socket_.close();
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	netpack read_msg_;
	netpack_queue write_msgs_;
};

std::string loadfile(std::string& file)
{
	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string contents(buffer.str());
	return contents;
}

void send_file(chat_client& c,std::string& file)
{
	std::string cmd = "set "+ file + " ";
	std::string fdata = loadfile(file);
	if (!fdata.length())
	{
		std::cout << "loadfile error!\n";
		return;
	}
	int len = cmd.length()+fdata.length();
	netpack msg(len + 1);
	memcpy(msg.data(), cmd.data(), cmd.length());
	memcpy(msg.data()+cmd.length(), fdata.data(), fdata.length());
	msg.m_size = len;
	c.write(msg);
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "10008");
		tcp::resolver::iterator iterator = resolver.resolve(query);

		chat_client c(io_service, iterator);

		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

		char line[1024];
		char file[256];
		while (std::cin.getline(line, 1024))
		{
			using namespace std;
			string strFile;
			if (memcmp(line, "send", 4) == 0)
			{
				memset(file, 0, sizeof(file));
				strcpy(file, &line[5]);
				cout << file << endl;
				strFile = file;
				send_file(c, strFile);
				continue;
			}
			int len = strlen(line);
			netpack msg(len+1);
			memcpy(msg.data(), line, len);
			msg.m_size = len;
			c.write(msg);
		}

		c.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

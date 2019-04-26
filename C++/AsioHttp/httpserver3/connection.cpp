//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"

namespace http {
namespace server2 {
	int m = 0;
	bool bKeepAlive = false;
connection::connection(boost::asio::io_context& io_context,
    request_handler& handler)
  : socket_(io_context),
    request_handler_(handler)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
  socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
		SYSTEMTIME currentTime;
		::GetLocalTime(&currentTime);
		printf("[%.2d:%.2d:%.2d:%.2d]\r\n", currentTime.wHour, currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds);
		printf("connect %d handle is %d\r\n", m, this);
		printf("\r\n");
		m++;

		int nBodyLen = 0;
		std::vector<header> headers = request_.headers;
		std::vector<header>::iterator it = headers.begin();
		for (;it != headers.end();++it)
		{
			header h1 = *it;
			if (h1.name == "content-length")
			{
				nBodyLen = atoi(h1.value.c_str());
				break;
			}	

		}
		char* chBody = new char[nBodyLen];
		strncpy(chBody, buffer_.data() + bytes_transferred - nBodyLen, nBodyLen);
		std::string strBody = chBody;
		delete[] chBody;
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          boost::bind(&connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
	if (bKeepAlive)
	{
		start();
	}
	
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const boost::system::error_code& e)
{
	if (bKeepAlive)
	{
		return;
	}
	
  if (!e)
  {

	//标记是否长连接
	//等待异步读取请求
	//  start();
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

} // namespace server2
} // namespace http

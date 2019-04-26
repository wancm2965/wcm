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
#include "GlobalDef.h"
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include <string>
#include <algorithm>
using namespace std;
namespace http {
namespace server2 {
	int m = 0;
	
connection::connection(boost::asio::io_context& io_context,
    request_handler& handler)
  : socket_(io_context),
    request_handler_(handler)
{
	m_bKeepAlive = false;
	m_bKeepRead = false;
	m_strSaveUrl = "";
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
	/**/
  socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void connection::AsyncRead()
{
	boost::asio::async_read(socket_, boost::asio::buffer(buffer_), boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void connection::AsyncReply(const char* replymsg)
{
	reply reply1;
	reply1.status = reply::ok;
	reply1.content = replymsg;
	reply1.headers.resize(2);
	reply1.headers[0].name = "Content-Length";
	reply1.headers[0].value = boost::lexical_cast<std::string>(reply1.content.size());
	reply1.headers[1].name = "Content-Type";
	reply1.headers[1].value = mime_types::extension_to_type("");

	/*socket_.async_write_some(reply1.to_buffers(), boost::bind(&connection::handle_Async_write, shared_from_this(),
		boost::asio::placeholders::error));*/

	/*std::string strData = replymsg;
	socket_.async_write_some(boost::asio::buffer(strData), boost::bind(&connection::handle_Async_write, shared_from_this(),
		boost::asio::placeholders::error));*/
	boost::asio::async_write(socket_, reply1.to_buffers(),
		boost::bind(&connection::handle_Async_write, shared_from_this(),
			boost::asio::placeholders::error));
}

void connection::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
	  if (m_bKeepRead)
	  {
		  buffer_.data()[bytes_transferred] = '\0';
		  m_strData = m_strData + buffer_.data();
		  if (m_strData.length() == m_nBodyLen)
		  {
			  m_bKeepRead = false;
		  }
		  else
		  {
			  m_bKeepRead = true;
			  start();
			  return;
		  }

		  std::string strBody = m_strData.erase(0, m_strData.length() - m_nBodyLen);
		 
		  char *pReply = nullptr;
		  if (g_phttpserver)
		  {
			  g_phttpserver->HandleHttpReq((void*)this, m_strSaveUrl.c_str(), strBody.c_str(), &pReply);
		  }
		  
		  reply_.status = reply::ok;
		  if (pReply == nullptr)
		  {
			  reply_.content = "";
		  }
		  else
		  {
			  reply_.content = pReply;
		  }
		  /*std::string strData = pReply;
		  socket_.async_write_some(boost::asio::buffer(strData), boost::bind(&connection::handle_write, shared_from_this(),
			  boost::asio::placeholders::error));*/

		  reply_.headers.resize(2);
		  reply_.headers[0].name = "Content-Length";
		  reply_.headers[0].value = boost::lexical_cast<std::string>(reply_.content.size());
		  reply_.headers[1].name = "Content-Type";
		  reply_.headers[1].value = mime_types::extension_to_type("");
		  boost::asio::async_write(socket_, reply_.to_buffers(),
			  boost::bind(&connection::handle_write, shared_from_this(),
				  boost::asio::placeholders::error));
		  return;
	  }
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);
	
    if (result)
    {
		m_bKeepAlive = false;
		int nBodyLen = 0;
		std::string strUrl = request_handler_.decodeurl(request_);
		m_strSaveUrl = strUrl;
		std::vector<header> headers = request_.headers;
		std::vector<header>::iterator it = headers.begin();
		for (; it != headers.end(); ++it)
		{
			header h1 = *it;
			std::string strname = h1.name;
			transform(strname.begin(), strname.end(), strname.begin(), ::tolower);
			if (strname == "content-length")
			{
				nBodyLen = atoi(h1.value.c_str());
				m_nBodyLen = nBodyLen;
			}
			if (strname == "connection")
			{
				if (h1.value == "keep-alive")
				{
					m_bKeepAlive = true;
				}
			}
		}
		
		if (m_nBodyLen > bytes_transferred)
		{
			m_bKeepRead = true;
			start();
			return;

		}
		buffer_.data()[bytes_transferred] = '\0';
		m_strData = m_strData + buffer_.data();
		
		m_bKeepRead = false;
		std::string strBody = m_strData.erase(0, m_strData.length() - m_nBodyLen);
		
		
		char *pReply = nullptr;
		if (g_phttpserver)
		{
			g_phttpserver->HandleHttpReq((void*)this, m_strSaveUrl.c_str(), strBody.c_str(), &pReply);
		}		

		/*std::string strData = pReply;
		socket_.async_write_some(boost::asio::buffer(strData), boost::bind(&connection::handle_write, shared_from_this(),
			boost::asio::placeholders::error));*/
		//reply reply_;
		reply_.status = reply::ok;
		if (pReply == nullptr)
		{
			reply_.content = "";
		}
		else
		{
			reply_.content = pReply;
		}
		reply_.headers.resize(2);
		reply_.headers[0].name = "Content-Length";
		reply_.headers[0].value = boost::lexical_cast<std::string>(reply_.content.size());
		reply_.headers[1].name = "Content-Type";
		reply_.headers[1].value = mime_types::extension_to_type("");
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
	if (m_bKeepAlive)
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
	if (m_bKeepAlive)
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

void connection::handle_Async_write(const boost::system::error_code& e)
{

}

void connection::AnalysisParameters(const char* strParam)
{

}

} // namespace server2
} // namespace http

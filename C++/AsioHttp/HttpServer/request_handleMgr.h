#ifndef HTTP_SERVER2_REQUEST_HANDLER_MGR_HPP
#define HTTP_SERVER2_REQUEST_HANDLER_MGR_HPP

#include <boost/asio.hpp>
#include <list>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "request_handler.hpp"
namespace http {
	namespace server2 {

		class request_handlemgr {
		public:
			explicit request_handlemgr(std::size_t count,const std::string& doc_root);


			request_handler& get_request_handler();


		private:
			typedef boost::shared_ptr<request_handler> request_handler_ptr;
			std::vector<request_handler_ptr> request_handlers_;
			std::size_t next_handle_;
		};


	}

}


#endif
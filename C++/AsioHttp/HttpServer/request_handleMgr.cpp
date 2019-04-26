#include "server.hpp"
#include <stdexcept>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace http {
	namespace server2 {



		request_handlemgr::request_handlemgr(std::size_t count, const std::string& doc_root)
			:next_handle_(0)
		{
			if (count == 0)
				throw std::runtime_error("io_context_pool size is 0");


			for (std::size_t i = 0; i < count; ++i)
			{
				request_handler_ptr reqhandle(new request_handler(doc_root));
				request_handlers_.push_back(reqhandle);
			}
		}


		http::server2::request_handler& request_handlemgr::get_request_handler()
		{
			request_handler& reqhandle = *request_handlers_[next_handle_];
			++next_handle_;
			if (next_handle_ == request_handlers_.size())
				next_handle_ = 0;
			return reqhandle;
		}

	}
}
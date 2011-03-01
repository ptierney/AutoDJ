
#include <adj/net/adj_net_Request.h>

namespace adj {
namespace net {

Request::Request(const std::string& val) : value_(val) {

}

std::string Request::to_string() {
    return value_;
}

}
}

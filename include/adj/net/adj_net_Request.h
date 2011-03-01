
#pragma once

#include <string>

namespace adj {
namespace net {

// this should be something like /~patrick/votes/votes.cgi
// NOTE: the leading "/"

class Request {
public:
    Request(const std::string& val);

    std::string to_string();

private:
    std::string value_;

};

}
}
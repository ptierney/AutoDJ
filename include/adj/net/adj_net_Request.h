
#pragma once

#include <string>

namespace adj {
namespace net {

// this should be something like index.html
// or votes.php

class Request {
public:
    Request();

    std::string to_string();

private:
    std::string value_;

};

}
}
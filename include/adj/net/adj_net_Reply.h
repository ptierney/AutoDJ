
#pragma once

#include <string>

namespace adj {
namespace net {

class Reply {
public:
    Reply(const std::string& data);

private:
    std::string data_;

};

}
}
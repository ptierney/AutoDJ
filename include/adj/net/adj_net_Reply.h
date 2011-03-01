
#pragma once

#include <string>

namespace adj {
namespace net {

class Reply {
public:
    Reply(const std::string& data);

    const std::string& data() { return data_; }

private:
    std::string data_;
};

}
}
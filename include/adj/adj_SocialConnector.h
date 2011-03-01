
#pragma once

#include <string>

#include <adj/net/adj_net_Client.h>

namespace adj {

typedef std::shared_ptr<class net::Reply> ReplyPtr;

class SocialConnector : public net::ReceivableInterface {

public:
    SocialConnector();
    void init();
    void cleanup();

    void update();

    void test_connect();

    // called by the Client when it receives a reply
    void receive_reply(ReplyPtr);

    static SocialConnector& instance();

private:
    std::string server_address_;
    std::string server_base_;
    std::string query_path_;

    std::shared_ptr<net::Client> client_;

    static SocialConnector* instance_;

};

}
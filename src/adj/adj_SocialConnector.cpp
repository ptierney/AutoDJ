
/*

#include <cinder/app/App.h>

#include <adj/adj_SocialConnector.h>
#include <adj/net/adj_net_Client.h>
#include <adj/net/adj_net_Reply.h>
#include <adj/net/adj_net_Request.h>

namespace adj {

SocialConnector* SocialConnector::instance_ = NULL;

SocialConnector::SocialConnector() {
    server_address_ = "http://ptierney.com/~patrick/votes/votes.cgi";

    server_base_ = "ptierney.com";
    query_path_ = "/~patrick/votes/votes.cgi";

    //server_base_ = "google.com";
    //query_path_ = "/";
}

void SocialConnector::init() {
    client_ = std::shared_ptr<net::Client>(new net::Client(server_base_));
    client_->add_receivable(*this);
    
    test_connect();
}

void SocialConnector::update() {
    client_->update();
}

void SocialConnector::test_connect() {
    net::Request r(query_path_);
    client_->send_request(r); 
}

void SocialConnector::receive_reply(ReplyPtr reply) {
    ci::app::console() << reply->data() << std::endl;
}

SocialConnector& SocialConnector::instance() {
    if (instance_ == NULL) {
        instance_ = new SocialConnector();
        instance_->init();
    }

    return *instance_;
}

void SocialConnector::cleanup() {
    delete instance_;
}

}

*/
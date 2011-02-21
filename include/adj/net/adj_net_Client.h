
#pragma once

#include <deque>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#include <adj/net/adj_net_Request.h>

namespace adj {
namespace net {

typedef std::shared_ptr<boost::thread> ThreadPtr;

typedef std::shared_ptr<class Reply> ReplyPtr;

class ReceivableInterface {
public:
    virtual ~ReceivableInterface() { }

    virtual void receive_reply(ReplyPtr) = 0;
};

typedef std::shared_ptr<ReceivableInterface> ReceivableInterfacePtr;

class Client;

// method for spawning a new thread
class ClientQuery {
public:
    ClientQuery(Client&, const Request&);

    void operator()();

private:
    void connect();
    void send_request();
    void receive_reply();
    void socket_read();

    std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
    std::shared_ptr<boost::asio::ip::tcp::resolver::query> query_;
    std::shared_ptr<boost::asio::io_service> io_service_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    boost::asio::ip::tcp::resolver::iterator iterator_;
    std::shared_ptr<boost::asio::io_service> timer_io_;
    std::shared_ptr<boost::asio::deadline_timer> t_;

    Client& client_;
    Request request_;
    std::string address_;
    std::string port_;

    int connect_timeout_; // in seconds
    int reply_timeout_; // in seconds
    int receive_data_timeout_; // in seconds
};

class Client {
public:
    Client();
    Client(const std::string& address);
    virtual ~Client();

    void update();

    void register_reply(std::shared_ptr<Reply>);

    void send_request(const Request&);

    void set_address(const std::string& address) { address_ = address; }

    const std::string& address() { return address_; }
    int port() { return port_; }

private:
    void init();
    void prune_threads(); // deletes oldest thread
    void parse_replies();
    
    unsigned int max_threads_;
    std::deque<ThreadPtr> threads_;

    // when a response comes back, let these receivables know
    std::vector<ReceivableInterfacePtr> receivables_;

    std::string address_;

    int port_;

    std::deque<std::shared_ptr<Reply> > replies_;
    boost::mutex reply_mutex_;
};

}
}

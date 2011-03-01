
#include <boost/lexical_cast.hpp>
#include <boost/asio/buffer.hpp>

#include <cinder/app/App.h>

#include <adj/net/adj_net_Client.h>
#include <adj/net/adj_net_Reply.h>

namespace adj {
namespace net {

ClientQuery::ClientQuery(Client& c, const Request& r) 
    : client_(c), request_(r) {

    timer_io_ = std::shared_ptr<boost::asio::io_service>(
            new boost::asio::io_service());

    address_ = client_.address();
    port_ = boost::lexical_cast<std::string>(client_.port());

    connect_timeout_ = 10; // in seconds
    reply_timeout_ = 10; // in seconds
    receive_data_timeout_ = 10;
}

// BEGIN THREADED
void ClientQuery::operator()() {
    try {
        connect();
        send_request();
        receive_reply();
    } catch (std::runtime_error err) {
        ci::app::console() << err.what() << std::endl;
        return;
    }
}

// establish a connection with the server
void ClientQuery::connect() {
    try {
        io_service_ = std::shared_ptr<boost::asio::io_service>(
            new boost::asio::io_service());

        query_ = std::shared_ptr<boost::asio::ip::tcp::resolver::query>(
            new boost::asio::ip::tcp::resolver::query(
            boost::asio::ip::tcp::v4(), address_, port_));

        iterator_ = resolver_->resolve(*query_);
        socket_ = std::shared_ptr<boost::asio::ip::tcp::socket>(
            new boost::asio::ip::tcp::socket(*io_service_));
        socket_->connect(*iterator_);

    } catch (...) {
        throw (std::runtime_error("Could not connect to server"));
    }

    int t_counter = 0;

    for (;;) {
        if(socket_->is_open())
            break;

        if (t_counter > connect_timeout_)
            throw (std::runtime_error("Could not connect to server"));

        boost::asio::deadline_timer t(*timer_io_, boost::posix_time::seconds(1));
        t.wait();
        
        ++t_counter;
    }
}

// send data over HTTP to the server
void ClientQuery::send_request() {
    size_t ret = 0u;

    // get the text from whatever the reply wants
    // wrap that in the appropriate HTTP stuff
    // send it down the wire

    std::string request_string = request_.to_string();
    std::string write_string;

    // just write the request
    write_string += "GET " + request_string + "HTTP/1.1\r\n";
    write_string += "\r\n";

    try {
        ret = boost::asio::write(*socket_, 
            boost::asio::buffer(write_string));
    } catch (...) {
        throw (std::runtime_error("Could not write to server"));
    }

    // if you care about the ret value do something with it
}

// receive data over HTTP to the server
void ClientQuery::receive_reply() {
    int t_counter = 0;

   for (;;) {
		if (socket_->available() > 0u) {
			socket_read();
            break;
        }

        if (t_counter > reply_timeout_)
            throw (std::runtime_error("Reply timout"));

        boost::asio::deadline_timer t(*timer_io_, boost::posix_time::seconds(1));
        t.wait();

        ++t_counter;
    }
}

void ClientQuery::socket_read() {
    std::string buf_incoming;
    std::string buf;
    size_t amount_received;

    int t_counter = 0;

    for (;;) {

        do {
            buf_incoming.clear();
            amount_received = socket_->receive(boost::asio::buffer(buf_incoming));
            buf += buf_incoming;
        } while (amount_received != 0);

        ci::app::console() << buf_incoming << std::endl;

        if (buf.find("\r\n\r\n") != std::string::npos)
            break;

        if (t_counter > receive_data_timeout_)
            throw (std::runtime_error("Receive data timout"));

        boost::asio::deadline_timer t(*timer_io_, boost::posix_time::seconds(1));
        t.wait();

        ++t_counter;
    } 

    ci::app::console() << buf << std::endl;

    // TODO: FIX THIS; VERY GHETTO
    size_t json_start = buf.find("{");
    std::string content = buf.substr(json_start);

    std::shared_ptr<Reply> r(new Reply(content));
    client_.register_reply(r);
}


// END THREADED

Client::Client() {
    init();
}

Client::Client(const std::string& address) : address_(address) {
    init();
}

Client::~Client() {

}

void Client::init() {
    port_ = 80;
    max_threads_ = 1;
}

void Client::update() {
    parse_replies();
}

void Client::parse_replies() {
    boost::mutex::scoped_lock lock(reply_mutex_);

    if (replies_.empty())
        return;

    for (std::deque<std::shared_ptr<Reply> >::iterator it = replies_.begin();
        it != replies_.end(); ++it) {

        for (std::vector<ReceivableInterface*>::iterator rec_it = 
            receivables_.begin(); rec_it != receivables_.end(); ++rec_it) {

            (*rec_it)->receive_reply(*it);
        }
    }

    replies_.clear();
}

void Client::send_request(const Request& req) {
   ClientQuery query(*this, req);

   prune_threads();

   threads_.push_back(ThreadPtr(new boost::thread(query)));
}

void Client::prune_threads() {
    if (threads_.size() < max_threads_)
        return;

    threads_.front()->join();
    threads_.pop_front();
}

void Client::register_reply(std::shared_ptr<Reply> reply) {
    boost::mutex::scoped_lock lock(reply_mutex_);

    replies_.push_back(reply);
}

void Client::add_receivable(ReceivableInterface& r) {
    receivables_.push_back(&r);
}

}
}

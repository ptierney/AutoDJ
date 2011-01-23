
#include <set>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "json/reader.h"
#include "json/value.h"

#include "cinder/Url.h"

#include "adj/adj_VoteManager.h"
#include "adj/adj_User.h"
#include "adj/adj_GraphNodeFactory.h"

namespace adj {

VoteServerQuery::VoteServerQuery() {
    vote_server_ = VoteManager::instance().vote_server();
    known_ids_ = VoteManager::instance().known_ids();
}

// THIS RUNS IN A SEPARATE THREAD
void VoteServerQuery::operator()() {
    query_vote_server();
    VoteManager::instance().register_thread_finished();
}

void VoteServerQuery::query_vote_server() {
    std::string root;
    ci::IStreamUrlRef urlRef = ci::IStreamUrl::createRef(vote_server_);

    ci::Buffer buf = loadStreamBuffer(urlRef);
    unsigned char* j = (unsigned char*) buf.getData();
    // assign the char array to string using the size of the buffer array
    root.assign(j,j+buf.getDataSize());

    // root now contains the vote json
    Json::Reader reader;
    Json::Value votes;

    reader.parse(root, votes);

    parse_votes(votes);
}

void VoteServerQuery::parse_votes(Json::Value& val) {
    for (Json::Value::iterator it = val.begin(); it != val.end(); ++it) {
        VoteId id = (*it)["id"].asString();

        if (known_ids_.find(id) != known_ids_.end())
            continue;

        VoteManager::instance().register_new_vote(*it);
    }
}

VoteManager::VoteManager() {
    query_time_ = 5; // in seconds
    vote_server_ = "http://ptierney.com/~patrick/votes/votes.json";
    thread_finished_ = true;
}

void VoteManager::init() {
    last_query_time_ = boost::posix_time::second_clock::universal_time();
}

void VoteManager::update() {
    if (enough_time_elapsed())
        query_vote_server();

    parse_new_votes();
}

bool VoteManager::enough_time_elapsed() {
    boost::posix_time::time_duration diff = 
        boost::posix_time::second_clock::universal_time() - last_query_time_;

    return diff.total_seconds() > query_time_;
}

void VoteManager::query_vote_server() {
    last_query_time_ = boost::posix_time::second_clock::universal_time();

    // wait another query_time in length
    if (!thread_finished_)
        return;

    if (query_thread_.get() != NULL)
        query_thread_->join();

    thread_finished_ = false;

    VoteServerQuery query;

    query_thread_ = ThreadPtr(new boost::thread(query));
}

void VoteManager::parse_new_votes() {
    boost::mutex::scoped_lock lock(new_votes_mutex_);

    for (std::deque<Json::Value>::iterator it = new_votes_.begin();
        it != new_votes_.end(); ++it) {
        VoteId id = get_id_from_vote(*it);

        if (known_ids_.find(id) != known_ids_.end())
            continue;

        known_ids_.insert(id);

        parse_vote(*it);
    }
}

VoteId VoteManager::get_id_from_vote(Json::Value& val) {
    return val["id"].asString();
}

void VoteManager::parse_vote(Json::Value& val) {
    VotePtr vote(new Vote());
    vote->id = get_id_from_vote(val);
    vote->song_id = val["vote"].asInt();
    vote->user = UserFactory::instance().get_user_from_value(val["user"]);

    // Inform graph of new vote
    GraphNodeFactory::instance().update_graph_from_vote(vote);
}

void VoteManager::register_new_vote(Json::Value val) {
    boost::mutex::scoped_lock lock(new_votes_mutex_);

    new_votes_.push_back(val);
}

void VoteManager::register_thread_finished() {
    boost::mutex::scoped_lock lock(thread_finished_mutex_);

    thread_finished_ = true;
}

VoteManager* VoteManager::instance_ = NULL;

VoteManager& VoteManager::instance() {
    if (instance_ == NULL) {
        instance_ = new VoteManager();
        instance_->init();
    }

    return *instance_;
}

}

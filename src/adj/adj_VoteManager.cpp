
#include <set>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "json/reader.h"
#include "json/value.h"

#include "cinder/Url.h"

#include "adj/adj_VoteManager.h"

namespace adj {

VoteManager::VoteManager() {
    query_time_ = 5; // in seconds
    vote_server_ = "http://ptierney.com/~patrick/votes/votes.json";
}

void VoteManager::init() {
    last_query_time_ = boost::posix_time::second_clock::universal_time();
}

void VoteManager::update() {
    if (enough_time_elapsed())
        query_vote_server();
}

bool VoteManager::enough_time_elapsed() {
    boost::posix_time::time_duration diff = 
        boost::posix_time::second_clock::universal_time() - last_query_time_;

    return diff.total_seconds() > query_time_;
}

void VoteManager::query_vote_server() {
    std::string root;
    ci::IStreamUrlRef urlRef = ci::IStreamUrl::createRef(
        vote_server_);

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

void VoteManager::parse_votes(Json::Value& val) {
    for (Json::Value::iterator it = val.begin(); it != val.end(); ++it) {
        VoteId id = get_id_from_vote(*it);

        if (known_ids_.find(id) != known_ids_.end())
            continue;

        known_ids_.insert(id);

        register_new_vote(*it);
    }
}

VoteId VoteManager::get_id_from_vote(Json::Value& val) {
    return val["id"].asString();
}

void VoteManager::register_new_vote(Json::Value& val) {

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

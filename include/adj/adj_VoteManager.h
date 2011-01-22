
#pragma once

// this object queries the vote server, and downloads new votes

#include <set>

#include "boost/date_time/posix_time/posix_time.hpp"

namespace adj {

typedef std::string VoteId;

class VoteManager {
public:
    static VoteManager& instance();
    static void cleanup();

    void update();

private:
    VoteManager();
    void init();
    bool enough_time_elapsed();
    void query_vote_server();
    void parse_votes(Json::Value&);
    VoteId get_id_from_vote(Json::Value&);
    void register_new_vote(Json::Value&);

    static VoteManager* instance_;
    
    // time in seconds between queries to server
    int query_time_;
    boost::posix_time::ptime last_query_time_;

    std::string vote_server_;

    std::set<VoteId> known_ids_;
};

}
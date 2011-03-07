
#pragma once

// this object queries the vote server, and downloads new votes

#include <set>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "adj/adj_Song.h"

namespace adj {

typedef std::string VoteId;
typedef std::shared_ptr<class User> UserPtr;

typedef std::shared_ptr<boost::thread> ThreadPtr;

struct Vote {
    VoteId id;
    SongId song_id;
    UserPtr user;
};

typedef std::shared_ptr<Vote> VotePtr;

class VoteServerQuery {
public:
    VoteServerQuery();

    void operator()();

private:
    void query_vote_server();
    void parse_votes(Json::Value&);

    std::string vote_server_;
    std::set<VoteId> known_ids_;
    VoteId last_id_;
};

typedef std::shared_ptr<boost::thread> ThreadPtr;

class VoteManager {
public:
    static VoteManager& instance();
    static void cleanup();

    void update();

    void register_thread_finished();
    void register_new_vote(Json::Value);

    const std::string& vote_server() { return vote_server_; }
    const std::set<VoteId>& known_ids() { return known_ids_; }
    const std::string& last_id() { return last_id_; }

private:
    VoteManager();
    void init();
    bool enough_time_elapsed();
    void query_vote_server();
    VoteId get_id_from_vote(Json::Value&);
    void update_last_query_time();
	bool check_override_vote(Json::Value& val);

    void parse_new_votes();
    void parse_vote(Json::Value&);

    static VoteManager* instance_;
    
    // time in seconds between queries to server
    int query_time_;
    boost::posix_time::ptime last_query_time_;

    std::string vote_server_;

    std::set<VoteId> known_ids_;
    VoteId last_id_;

    std::deque<Json::Value> new_votes_;
    boost::mutex new_votes_mutex_;

    ThreadPtr query_thread_;

    bool thread_finished_;
    boost::mutex thread_finished_mutex_;
};

}

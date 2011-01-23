
#pragma once

// this class stores facebook users, ie the people who actually vote on songs

#include "adj/adj_Adj.h"
#include <map>
#include <deque>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "json/value.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"

#include "adj/adj_Song.h"

/*
Sample Vote:

{
    "id" : "123123123",   <-- unique id for vote
    "user" : {
        "id" : 7100832,   <-- facebook id
        "name" : "Patrick Tierney"
    },
    "vote" : 1            <-- song id
}
*/

namespace adj {

typedef int UserId;

class User {
public:
    User();

    std::string name_;
    UserId id_;
    ci::Surface photo_;
    ci::gl::Texture photo_texture_;

    std::deque<SongId>& voted_songs() { return voted_songs_; }

    void register_vote_for_song(SongId id);

private:
    std::deque<SongId> voted_songs_;
};

typedef std::shared_ptr<User> UserPtr;

class ProfileQuery {
public:
    ProfileQuery(const std::vector<UserId>&, std::string url_base);

    void operator()();

private:
    void download_profile(UserId);

    std::vector<UserId> query_list_;
    std::string url_base_;
};

struct QueryReply {
    UserId id;
    ci::Surface photo;
};

typedef std::shared_ptr<QueryReply> QueryReplyPtr;
typedef std::shared_ptr<boost::thread> ThreadPtr;

class UserFactory {
public:
    void update();

    // takes a facebook user id and returns the appropriate user object
    // if it doesn't exist, returns null pointer
    UserPtr get_user_from_id(const UserId&);

    // takes a user value from a vote, and returns a user object
    // constructs a user object if 
    UserPtr get_user_from_value(Json::Value&); // <-- use this

    UserPtr create_user(Json::Value&);

    static UserFactory& instance();

    const std::string& default_name() { return default_name_; }
    const ci::Surface& default_photo() { return default_photo_; }

    void register_query_reply(QueryReplyPtr);

    UserId get_id_from_value(Json::Value& val) { return val["id"].asInt(); }

    void update_nodes_after_user_change(UserPtr);

private:
    UserFactory();
    void init();
    void parse_query_replies();
    void query_server_for_profile();
    void prune_threads();
    bool enough_time_elapsed();
    void update_last_query_time();

    std::map<UserId, UserPtr> user_map_;

    std::string default_name_;
    ci::Surface default_photo_;

    std::string image_url_base_;

    static UserFactory* instance_;

    std::vector<UserId> pending_queries_;

    int max_threads_;
    int query_rate_; // in seconds
    boost::posix_time::ptime last_query_time_;

    std::deque<ThreadPtr> threads_;

    std::deque<QueryReplyPtr> query_replies_;
    boost::mutex query_reply_mutex_;
};

}
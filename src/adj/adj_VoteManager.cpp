
#include <set>

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/lexical_cast.hpp>

#include "json/reader.h"
#include "json/value.h"

#include "cinder/app/App.h"
#include "cinder/Url.h"

#include "adj/adj_VoteManager.h"
#include "adj/adj_User.h"
#include "adj/adj_GraphNodeFactory.h"
#include <adj/adj_DJController.h>
#include <adj/adj_GraphNode.h>

namespace adj {

VoteServerQuery::VoteServerQuery() {
    vote_server_ = VoteManager::instance().vote_server();
    known_ids_ = VoteManager::instance().known_ids();
    last_id_ = VoteManager::instance().last_id();
}

// THIS RUNS IN A SEPARATE THREAD
void VoteServerQuery::operator()() {
    query_vote_server();

    // if an exception has been thrown, register as finished
    // though no new votes will have been registered
    VoteManager::instance().register_thread_finished();
}

void VoteServerQuery::query_vote_server() {
    std::string root;

    ci::IStreamUrlRef urlRef;

    std::string url_string;

    if (last_id_.empty())
        url_string = vote_server_;
    else {
        url_string = vote_server_;
        url_string += "?since_vote=";
        url_string += last_id_;
    }

    try {
        urlRef = ci::IStreamUrl::createRef(url_string);
    } catch (...) { // it can't connect to the server
        return;
    }

    ci::Buffer buf = loadStreamBuffer(urlRef);
    unsigned char* j = (unsigned char*) buf.getData();
    // assign the char array to string using the size of the buffer array
    root.assign(j,j+buf.getDataSize());

    // root now contains the vote json
    Json::Reader reader;
    Json::Value votes;

    reader.parse(root, votes);

    parse_votes(votes["votes"]);
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
    //vote_server_ = "http://ptierney.com/~patrick/votes/votes.cgi";
    vote_server_ = "http://glebden.com/djdp3000/get_votes.php";
    thread_finished_ = true;
	// last_id_ should remain empty on init
}

void VoteManager::init() {
    update_last_query_time();
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

void VoteManager::update_last_query_time() {
    last_query_time_ = boost::posix_time::second_clock::universal_time();
}

void VoteManager::query_vote_server() {
    update_last_query_time();

    // wait another query_time in length
    if (!thread_finished_)
        return;

    if (query_thread_.get() != NULL)
        query_thread_->join();

    thread_finished_ = false;

    new_votes_mutex_.lock();

    VoteServerQuery query;

    new_votes_mutex_.unlock();

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

        if (last_id_.empty())
            last_id_ = id;
        else if (boost::lexical_cast<int>(id) > boost::lexical_cast<int>(last_id_))
            last_id_ = id;
    }
}

VoteId VoteManager::get_id_from_vote(Json::Value& val) {
    return val["id"].asString();
}
	
bool VoteManager::check_override_vote(Json::Value& val) {
	if (val["transition"].asString() == std::string("0"))
        return false;
    
    return true;
}

void VoteManager::parse_vote(Json::Value& val) {
    bool override = check_override_vote(val);
    bool previously_voted = false;
    
    SongId song_id = boost::lexical_cast<int>(val["song_id"].asString());
    
    // if override check if previously voted
    if (override) {
        std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();
        
        for (std::vector<GraphNodePtr>::iterator it = nodes.begin();
        	it != nodes.end(); ++it) {
            
         	if ((*it)->song().id() == song_id) {
                previously_voted = true;
                break;
            }
        }
    }
    
    // if override vote and no one has voted for this
    // or if not override
    
    if (!override || (override && !previously_voted)) {
        VotePtr vote(new Vote());
        vote->id = get_id_from_vote(val);
        vote->song_id = song_id;
        vote->user = UserFactory::instance().get_user_from_value(val["user"]);
        vote->user->register_vote_for_song(vote->song_id);

        // Inform graph of new vote
        GraphNodeFactory::instance().update_graph_from_vote(vote);
    }
    
    if (override) {
        DJController::instance().set_next_song(song_id);
        //DJController::instance().transition();
        DJController::instance().set_transition_next_loop(true);
    }

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

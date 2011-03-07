
#include <assert.h>

#include <algorithm>
#include <utility>

#include "boost/lexical_cast.hpp"
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/Url.h"
#include "cinder/ip/Resize.h"

#include "adj/adj_User.h"
#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_CalloutBox.h"
#include "Resources.h"

namespace adj {

User::User() {
    // nothing here
}

void User::register_vote_for_song(SongId id) {
    voted_songs_.push_back(id);
}

	ProfileQuery::ProfileQuery(const std::vector<std::pair<UserId, std::string> >& ids, std::string base)
    : query_list_(ids), url_base_(base) {
}

// NOTE THIS RUNS IN A SEPARATE THREAD
void ProfileQuery::operator()() {
    for (std::vector<std::pair<UserId, std::string> >::const_iterator it = query_list_.begin();
        it != query_list_.end(); ++it) {

        download_profile(*it);
    }
}

void ProfileQuery::download_profile(std::pair<UserId, std::string> q) {
    //std::string url = url_base_ + boost::lexical_cast<std::string>(id) +
    //    ".jpg";

	UserId id = q.first;
	
	std::string url = q.second;
	
    QueryReplyPtr reply(new QueryReply());

    try {
        reply->photo = ci::loadImage(ci::loadUrl(url));
    } catch (...) {
        ci::app::console() << "Failed to load image from: " << url << std::endl;
        return;
    }

    UserFactory::resize_photo(reply->photo, reply->resized_photo);

    reply->id = id;

    UserFactory::instance().register_query_reply(reply);
}

void UserFactory::resize_photo(ci::Surface& original, ci::Surface& resized) {
    int photo_width = original.getWidth();
    int photo_height = original.getHeight();

    if (photo_width > photo_height) { // reduce the width
        resized = ci::ip::resizeCopy(original,
            ci::Area(0, 0, photo_width, photo_height), ci::Vec2i(
            CalloutBox::kMaxImageWidth, static_cast<float>(CalloutBox::kMaxImageWidth) / 
            static_cast<float>(photo_width) * static_cast<float>(photo_height)));
    } else { // reduce the height
        resized = ci::ip::resizeCopy(original,
            ci::Area(0, 0, photo_width, photo_height), ci::Vec2i(
            static_cast<float>(CalloutBox::kMaxImageHeight) / 
            static_cast<float>(photo_height) * static_cast<float>(photo_width),
            CalloutBox::kMaxImageHeight));
    }
}

UserFactory::UserFactory() {
    default_name_ = "Someone";
    image_url_base_ = "http://ptierney.com/~patrick/user_photos/";
    max_threads_ = 5;
    query_rate_ = 1; // spawn threads at least query_rate_ seconds apart
}

void UserFactory::init() {
    default_photo_ = ci::loadImage(ci::app::loadResource(RES_DEFAULT_PROFILE));
    resize_photo(default_photo_, default_photo_resized_);
    update_last_query_time();
}

void UserFactory::update() {
    if (enough_time_elapsed())
        query_server_for_profile();

    parse_query_replies();
}

bool UserFactory::enough_time_elapsed() {
    boost::posix_time::time_duration diff = 
        boost::posix_time::second_clock::universal_time() - last_query_time_;

    return diff.total_seconds() > query_rate_;
}

void UserFactory::update_last_query_time() {
    last_query_time_ = boost::posix_time::second_clock::universal_time();
}

UserPtr UserFactory::get_user_from_id(const UserId& id) {
    // if a user has already been created, return it
    // if not, return null

    return user_map_[id]; // should construct new null ptr
}

UserPtr UserFactory::get_user_from_value(Json::Value& val) {
    UserId id = get_id_from_value(val);

    if (user_map_.find(id) != user_map_.end())
        return user_map_[id];

    user_map_[id] = create_user(val);

    assert(user_map_[id]->id_ == id);

    return user_map_[id];
}

UserPtr UserFactory::create_user(Json::Value& val) {
    UserPtr user(new User());
    user->id_ = get_id_from_value(val);
    
    std::string fname = val["fname"].asString();
    std::string lname = val["lname"].asString();
    
    if (fname.empty() && lname.empty())
        user->name_ = default_name_;
    else if (fname.empty())
        user->name_ = lname;
    else if (lname.empty())
        user->name_ = fname;
    else 
        user->name_ = fname + " " + lname;
    
	user->photo_url_ = val["avatar"].asString();

	std::pair<UserId, std::string> p(user->id_, user->photo_url_);
	
    pending_queries_.push_back(p);

    user->photo_ = default_photo_;
    user->photo_resized_ = default_photo_resized_;

    user->photo_texture_ = ci::gl::Texture(user->photo_);
    user->photo_texture_resized_ = ci::gl::Texture(user->photo_resized_);

    return user;
}

void UserFactory::query_server_for_profile() {
    update_last_query_time();

    if (pending_queries_.empty())
        return;

    ProfileQuery query(pending_queries_, image_url_base_);

    prune_threads();

    threads_.push_back(ThreadPtr(new boost::thread(query)));
}

void UserFactory::prune_threads() {
    if (threads_.size() < max_threads_)
        return;

    threads_.front()->join();
    threads_.pop_front();
}

void UserFactory::register_query_reply(QueryReplyPtr reply) {
    boost::mutex::scoped_lock lock(query_reply_mutex_);

    query_replies_.push_back(reply);
}

void UserFactory::parse_query_replies() {
    boost::mutex::scoped_lock lock(query_reply_mutex_);

    if (query_replies_.empty())
        return;

    for (std::deque<QueryReplyPtr>::const_iterator it = query_replies_.begin();
        it != query_replies_.end(); ++it) {

        QueryReplyPtr reply = *it;

        std::vector<std::pair<UserId, std::string> >::iterator pending_it;
		
		for (pending_it = pending_queries_.begin(); pending_it != pending_queries_.end();
			 ++pending_it) {
			
			if (pending_it->first == reply->id)
				break;
		}
		
        // must have already been answered
        if (pending_it == pending_queries_.end())
            continue;

        UserPtr user = user_map_[reply->id];

        assert(user->id_ == reply->id);

        user->photo_ = reply->photo;
        user->photo_resized_ = reply->resized_photo;
        // Note: creating a gl texture MUST be done in the main thread
        user->photo_texture_ = ci::gl::Texture(user->photo_);
        user->photo_texture_resized_ = ci::gl::Texture(user->photo_resized_);

        update_nodes_after_user_change(user);

        pending_queries_.erase(pending_it);
    }    
}

bool UserFactory::has_pending_queries() {
    boost::mutex::scoped_lock lock(query_reply_mutex_);
    
    return !pending_queries_.empty();
}

void UserFactory::update_nodes_after_user_change(UserPtr user) {
    std::map<SongId, GraphNodePtr>& song_map = GraphNodeFactory::instance().song_map();

    for (std::deque<SongId>::iterator it = user->voted_songs().begin();
        it != user->voted_songs().end(); ++it) {
        
        song_map[*it]->update_appearance();
    }
}

UserFactory& UserFactory::instance() {
    if (instance_ == NULL) {
        instance_ = new UserFactory();
        instance_->init();
    }

    return *instance_;
}

UserFactory* UserFactory::instance_ = NULL;

}

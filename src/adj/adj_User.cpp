
#include "boost/lexical_cast.hpp"

#include "cinder/ImageIo.h"
#include "cinder/Surface.h"

#include "adj/adj_User.h"

namespace adj {

User::User() {
    // nothing here
}

UserFactory::UserFactory() {
    default_name_ = "AutoDJ User";
    image_url_base_ = "http://ptierney.com/~patrick/user_photos/";
}

void UserFactory::init() {
    default_photo_ = ci::loadImage("/data/user_photos/2522920.jpg");
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

    return user_map_[id];
}

UserPtr UserFactory::create_user(Json::Value& val) {
    UserPtr user(new User());
    user->id_ = get_id_from_value(val);
    user->name_ = val["name"].asString();

    if (user->name_.empty())
        user->name_ = default_name_;

    std::string image_name = image_url_base_ + boost::lexical_cast<std::string>(
        user->id_) + ".jpg";

    // TODO: Spawn a worker thread, get the image off the network,
    // and replace the default photo with the new photo
    /*
    // create user image
    try {
        // TODO: check that this works
        user->photo_ = ci::loadImage(image_name);
    } catch (...) {
        // set image to default image
        
    }
    */

    user->photo_ = default_photo_;

    user->photo_texture_ = ci::gl::Texture(user->photo_);

    return user;
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


#include <algorithm>

#include "cinder/ImageIo.h"
#include "cinder/Surface.h"

#include "adj/adj_User.h"

namespace adj {

User::User() {
    // nothing here
}


UserPtr UserFactory::get_user_from_id(const std::string& id) {
    // if a user has already been created, return it
    // if not, create a new user, and return that
    return UserPtr();
}

UserPtr UserFactory::create_user(const Json::Value&) {
    // parse through json
    // create user
    // store user in map

    return UserPtr();
}

void UserFactory::update_user_details(const Json::Value&) {

}


UserFactory& UserFactory::instance() {
    if (instance_ == NULL) {
        instance_ = new UserFactory();
        instance_->init();
    }

    return *instance_;
}

UserFactory* UserFactory::instance_ = NULL;

UserFactory::UserFactory() {
    default_name_ = "AutoDJ User";
}

void UserFactory::init() {
    default_photo_ = ci::loadImage("/data/user_photos/2522920.jpg");
}





}
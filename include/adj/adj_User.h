
#pragma once

#include "adj/adj_Adj.h"
#include <map>

#include "json/value.h"

// this class stores facebook users, ie the people who actually vote on songs

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"

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

// not struct b/c I might want to fold creation into it specifically for 
// multithreaded loading of photo

class User {
public:
    User();

    std::string name_;
    UserId id_;
    ci::Surface photo_;
    ci::gl::Texture photo_texture_;
};

typedef std::shared_ptr<User> UserPtr;

class UserFactory {
public:
    // a dummy method to load from a fake database for testing
    void testing_setup();

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

    UserId get_id_from_value(Json::Value& val) {
        return val["id"].asInt();
    }

private:
    UserFactory();
    void init();

    std::map<UserId, UserPtr> user_map_;

    std::string default_name_;
    ci::Surface default_photo_;

    std::string image_url_base_;

    static UserFactory* instance_;
};

}
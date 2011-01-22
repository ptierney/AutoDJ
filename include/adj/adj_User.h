
#pragma once

#include "adj/adj_Adj.h"
#include <map>

#include "json/value.h"

// this class stores facebook users, ie the people who actually vote on songs

#include "cinder/Surface.h"

namespace adj {

typedef int UserId;

class User {
public:
    User();

    std::string name_;
    UserId id_;
    
private:
    ci::Surface photo_;
};

typedef std::shared_ptr<User> UserPtr;

/*
Sample User info
{
"id" : "1234",
"name" : "John Smith"
}

*/

class UserFactory {
public:
    // a dummy method to load from a fake database for testing
    void testing_setup();

    // takes a facebook user id and returns the appropriate user object
    UserPtr get_user_from_id(const UserId&);
    UserPtr create_user(const Json::Value&);
    UserPtr create_user(UserId id, std::string name);
    // if more information comes in on the network, add details to objects
    void update_user_details(const Json::Value&);

    static UserFactory& instance();

    const std::string& default_name() { return default_name_; }
    const ci::Surface& default_photo() { return default_photo_; }

private:
    UserFactory();
    void init();

    static UserFactory* instance_;

    std::map<std::string, UserPtr> user_map_;

    std::string default_name_;
    ci::Surface default_photo_;

    std::string image_url_;
};

}

#pragma once

#include "adj/adj_Adj.h"
#include <map>

#include "json/value.h"

// this class stores facebook users, ie the people who actually vote on songs

#include "cinder/Surface.h"

namespace adj {

class User {
public:
    User();

    std::string name_;
    std::string id_;
    
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
    // takes a facebook user id and returns the appropriate user object
    UserPtr get_user_from_id(const std::string&);
    UserPtr create_user(const Json::Value&);
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
};

}
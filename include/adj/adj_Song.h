
#pragma once

#include "adj/adj_Adj.h"

#include <vector>
#include <map>
#include <string>
#include <deque>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "json/value.h"

#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"

namespace adj {

class Song;
typedef std::shared_ptr<Song> SongPtr;

typedef int SongId;

typedef std::shared_ptr<struct Vote> VotePtr;
typedef std::shared_ptr<class User> UserPtr;
    
class Song {
public:
    // durration is in seconds
    Song(SongId song_id, std::string name, std::string file_name, int duration,
        std::string artist, std::string album);
    void init();

    void play();
    void pause();
    void stop();
    int time_elapsed();
    int time_remaining();
    bool is_playing() { return is_playing_; }

    std::string name() { return name_; }
    std::string file_name() { return file_name_; }
    std::string artist() { return artist_; }
    std::string album() { return album_; }
    int duration() { return duration_; }

    SongId id() { return id_; }

    int num_votes() { return votes_.size(); }
    const std::deque<VotePtr>& votes() { return votes_; }
    // the users who have voted for this song
    std::deque<UserPtr>& users() { return users_; }

    void register_vote(VotePtr);

private:
    SongId id_;
    std::string name_;
    std::string file_name_;
    std::string artist_;
    std::string album_;
    int duration_;
    
    bool is_surprise_;

    bool is_playing_;
    boost::posix_time::ptime play_start_;

    ci::audio::SourceRef source_;
    // this is a handle to the playing instance
    ci::audio::TrackRef track_; 

    std::deque<VotePtr> votes_;
    std::deque<UserPtr> users_;
};

class SongFactory {
public:
    static SongFactory& instance();
    static void cleanup();

    SongPtr lookup_song(SongId);

    void load_song_database();

    SongPtr get_random_song();
    SongId get_random_song_id();

    // input a json
    SongPtr create_song(SongId, Json::Value& song);

    const std::string& base_song_directory_path() { 
        return base_song_directory_path_; }

    boost::posix_time::ptime get_current_time();
    
    SongId surprise_id() { return -1; }

private:
    SongFactory();
    void parse_song_database_file();
    SongPtr create_surprise_song();

    std::string get_uuid();

    std::string base_song_directory_path_;
    Json::Value song_database_;

    std::map<SongId, SongPtr> song_map_;

    static SongFactory* instance_;
};

}

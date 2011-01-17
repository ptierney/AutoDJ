
#include <fstream>
#include <exception>

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "json/reader.h"
#include "json/value.h"

#include "cinder/app/App.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"
#include "cinder/Rand.h"

#include "adj/adj_Song.h"

namespace adj {

Song::Song(SongId id, std::string name, std::string file_name, int duration,
    std::string artist, std::string album)
    : id_(id), name_(name), file_name_(file_name), duration_(duration),
    artist_(artist), album_(album) {
    is_playing_ = false;
}

void Song::init() {
    source_ = ci::audio::load(SongFactory::instance().base_song_directory_path() + 
        file_name_);
}

// TODO: this could cause bugs when dealing with transition times
void Song::play() {
    if (is_playing_)
        return;

    track_ = ci::audio::Output::addTrack(source_);

    play_start_ = SongFactory::instance().get_current_time();

    is_playing_ = true;
}

void Song::pause() {
    if (is_playing_ == false)
        return;

    track_->stop();

    is_playing_ = false;
}

void Song::stop() {
    track_->stop();

    is_playing_ = false;

    // TODO: remove from track
}

int Song::time_elapsed() {
    boost::posix_time::time_duration diff = 
        SongFactory::instance().get_current_time() - play_start_;

    return diff.total_seconds();
}

int Song::time_remaining() {
    return duration_ - time_elapsed();
}

void SongFactory::parse_song_database_file() {
    Json::Reader reader;

    std::string msg;
    std::string line;

    std::ifstream input(song_database_file_.c_str());

    if (input.is_open()) {
        while (input.good()) {
            getline (input, line);
            msg += line;
            msg += "\n"; // restore the chomped endl
        }
        input.close();
    }

    reader.parse(msg, song_database_);
}

// 
void SongFactory::load_song_database() {
    parse_song_database_file();

    Json::Value& songs_array = song_database_["songs"];

    if (songs_array.empty())
        throw (std::runtime_error("Could not find song database file."));

    SongId song_id;

    for (Json::Value::iterator it = songs_array.begin(); it != songs_array.end(); ++it) {
        Json::Value& song = (*it);
        song_id = song["id"].asInt();

        song_map_[song_id] = create_song(song_id, song);
    }
}

boost::posix_time::ptime SongFactory::get_current_time() {
    return boost::posix_time::second_clock::universal_time();
}

// TODO: check map to see if the song has been loaded
// throw exception (or assertion)
SongPtr SongFactory::lookup_song(SongId id) {
    return song_map_[id];
}

SongPtr SongFactory::create_song(SongId id, Json::Value& song) {
    SongPtr song_ptr =  SongPtr(new Song(id, song["title"].asString(),
        song["filename"].asString(), song["length"].asInt(), 
        song["artist"].asString(), song["album"].asString()));
        
    song_ptr->init(); // loads the file

    return song_ptr;
}

std::string SongFactory::get_uuid() {
    boost::uuids::random_generator gen;
    return boost::lexical_cast<std::string>(gen());
}

SongPtr SongFactory::get_random_song() {
    int map_size = song_map_.size(); // technically map<...>::size_type

    ci::Rand rand;
    rand.randomize();

    int index = rand.randInt(map_size - 1);
    int i = 0;

    for (std::map<SongId, SongPtr>::const_iterator it = song_map_.begin();
        it != song_map_.end(); ++it, ++i) {

        if (i != index)
            continue;

        return it->second;
    }

    ci::app::console() << "**WARNING** Could not find random song." << std::endl;

    return song_map_.begin()->second;
}

// TODO: should be nullptr
SongFactory* SongFactory::instance_ = NULL;

SongFactory& SongFactory::instance() {
    if (instance_ == NULL)
        instance_ = new SongFactory();

    return *instance_;
}

void SongFactory::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

SongFactory::SongFactory() {
    base_song_directory_path_ = "/data/songs/";
    song_database_file_ = "/data/songs.txt";
}

}
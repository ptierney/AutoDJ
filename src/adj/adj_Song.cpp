
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"

#include "adj/adj_Song.h"

namespace adj {

Song::Song(SongId id, std::string name, std::string file_name, int duration,
    std::string artist, std::string album)
    : id_(id), name_(name), file_name_(file_name), duration_(duration),
    artist_(artist), album_(album) {
    is_playing_ = false;
}

void Song::init() {
    source_ = ci::audio::load(SongFactory::instance().base_path() + file_name_);
}

void Song::play() {
    ci::audio::Output::play(source_);

    play_start_ = SongFactory::instance().get_current_time();

    is_playing_ = true;
}

int Song::time_elapsed() {
    boost::posix_time::time_duration diff = 
        SongFactory::instance().get_current_time() - play_start_;

    return diff.total_seconds();
}

int Song::time_remaining() {
    return duration_ - time_elapsed();
}

// 
void SongFactory::load_song_database() {
    // for each song in song database, 
    // create new song
    // init song
}

boost::posix_time::ptime SongFactory::get_current_time() {
    return boost::posix_time::second_clock::universal_time();
}

// TODO: check map to see if the song has been loaded
// throw exception (or assertion)
SongPtr SongFactory::lookup_song(SongId id) {
    return song_map_[id];
}

// TODO: make this
SongPtr SongFactory::create_song() {
    return std::shared_ptr<Song>();
}

std::string SongFactory::get_uuid() {
    boost::uuids::random_generator gen;
    return boost::lexical_cast<std::string>(gen());
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
    base_path_ = "/data/";
}

}
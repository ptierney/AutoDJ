
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"

#include "adj/adj_Song.h"

namespace adj {

Song::Song(std::string name, std::string file_name, int durration) {

}

// 
void SongFactory::load_song_database() {

}

// TODO: make this
SongPtr SongFactory::create_song() {
    return std::tr1::shared_ptr<Song>();
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
    // nothing here
}

}
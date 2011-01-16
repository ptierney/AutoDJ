
#pragma once

#include "adj/adj_Adj.h"

#include <vector>
#include <map>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"

namespace adj {

class Song;
typedef std::shared_ptr<Song> SongPtr;

typedef std::string SongId;

/* NOTE: these are probably not needed

struct Artist;
struct Album;
typedef std::shared_ptr<Artist> ArtistPtr;
typedef std::shared_ptr<Album> AlbumPtr;

struct Album {
    std::string name;
    ArtistPtr artist;
    std::vector<SongPtr> songs;
};

struct Artist {
    std::string name;
    std::vector<AlbumPtr> albums;
    std::vector<SongPtr> songs;
};
*/
    
class Song {
public:
    // durration is in seconds
    Song(SongId song_id, std::string name, std::string file_name, int duration,
        std::string artist, std::string album);
    void init();

    void play();
    int time_elapsed();
    int time_remaining();
    bool is_playing() { return is_playing_; }

    std::string name() { return name_; }
    std::string file_name() { return file_name_; }
    std::string artist() { return artist_; }
    std::string album() { return album_; }
    int duration() { return duration_; }

private:
    SongId id_;
    std::string name_;
    std::string file_name_;
    std::string artist_;
    std::string album_;
    int duration_;

    bool is_playing_;
    boost::posix_time::ptime play_start_;

    ci::audio::SourceRef source_;
};

class SongFactory {
public:
    static SongFactory& instance();
    static void cleanup();

    SongPtr lookup_song(SongId);

    void load_song_database();

    // input a json (loaded
    SongPtr create_song();

    const std::string& base_path() { return base_path_; }

    boost::posix_time::ptime get_current_time();

private:
    SongFactory();

    std::string get_uuid();

    std::string base_path_;

    std::map<SongId, SongPtr> song_map_;

    static SongFactory* instance_;
};

}
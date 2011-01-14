
#pragma once

#include "adj/adj_Adj.h"

#include <vector>

namespace adj {

class Song;
struct Artist;
struct Album;
typedef std::tr1::shared_ptr<Artist> ArtistPtr;
typedef std::tr1::shared_ptr<Song> SongPtr;
typedef std::tr1::shared_ptr<Album> AlbumPtr;

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
    
class Song {
public:
    // durration is in seconds
    Song(std::string name, std::string file_name, int durration);

    void play();
    int time_elapsed();
    int time_remaining();
    bool is_playing();

    ArtistPtr artist_;
    AlbumPtr album_;
};

class SongFactory {
    SongFactory& instance();
    static void cleanup();

    void load_song_database();

    // input a json (loaded
    SongPtr create_song();

private:
    SongFactory();

    std::string get_uuid();

    static SongFactory* instance_;
};

}
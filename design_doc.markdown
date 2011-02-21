
Introduction
============

The so called "Auto DJ" system is a collection of system to enable a dance party without the need for a (human) DJ or playlist. It is composed of three connected systems:

* __Mobile-enabled social media voting system.__ Allows participants to request songs to be played. Their identities are accessed via this system, allowing names, and photos (and potentially other things), to be accessed by other parts of the system.
* __Playlist visualizer.__ Displays information about user's votes and upcoming songs in a communal (read: projected) setting. 
* __Graph analyzer (Sylvester).__ Trims graph from a potential > 2000 songs to approx. 20 relevant, timely, important, or otherwise "hot" songs for display on the visualizer.

The adj git repo contains code for the visualizer and graph analyzer. Code for the voting system is stored on a separate repo.

Visualizer Architecture Overview
================================

AdjApp Object
-------------

Lightweight object that creates a window and sets operating system / gui settings (such as fullscreen and framerate). Calls main(...).

adj Namespace
-------------

This namespace concerns objects related to the visualization of data, managing connections with other servers, loading resources, and spawning a new application window.

adj::Visualizer Object
----------------------

Lightweight class that holds pointers to the main objects that do things. Calls update once per draw loop, and cleanup when the program shuts down.

adj::GraphNode Object
---------------------

This is the basic unit of the particle system visualization. A __GraphNode__ contains three main components: a __Song__,  a __graph::Particle__ object linking it back to the physics simulation,
and a __CalloutBox__ object to display relevant information.

adj::Song Object
----------------

__Song__ objects represent songs, and should contain all information about a song, including artist, album, cover art, and some means of playing the song.

adj::SongFactory Object
-----------------------

The __SongFactory__ creates __Songs__, and crucially contains the location of the main directory where all the songs are contained on disk.

adj::User Object
----------------

Similar to __Song__, a __User__ object contains information about a user, in particular their name and a photo. Additionally, each __User__ contains a list of all __Songs__ that the user has voted on.

adj::UserFactory Object
-----------------------

The __UserFactory__ has two main roles: creating a __User__ object from a Json value received from the __SocialConnector__, and acting as a lookup table for other objects which have a __UserID__, but need an actual __User__ object.

adj::GraphOracle Object
-----------------------

The __GraphOracle__ is the main interface for querying Sylvester. See the section on Visualizer - Sylvester connection for more information.

adj::SocialConnector Object
---------------------------

This object opens a HTTP connection with the Social Media Server, and runs requests in a background thread. It is responsible with communicating with the __VoteManager__ and the __UserFactory__ which subsequently create votes and users.

adj::net Namespace
------------------

This namespace contains objects related to connecting with other parts of the system over the network, typically by HTTP, composed of a __Request__, __Reply__, and a __Client__.

adj::Renderer Object
--------------------

Handles all things OpenGL related, and draws all necessary objects.

adj::Camera Object
------------------

The camera changes the global scale and position of the objects being visualized to maintain them all on screen. It should also have functionality to center on a particular node, or set / path of nodes.

adj::PlayManager Object
-----------------------

Manages the playback of __Songs__, and transitions between them. NOTE: this is not used for the current Crowdtap branch

graph Namespace
---------------

This namespace contains objects related to a particle system / spring simulation. It is heavily modeled after the [Traer physics library](http://murderandcreate.com/physics/) (read: C++ port), and as such follows Traer's own coding conventions and styles. It wasn't built from the ground up for the purposes of this visualizer. If its capabilities aren't adequate, it's probably best to rewrite these objects from the ground up rather than modifying Traer's code.

The adj namespace mainly interacts with the __graph::Particle__ object, getting its position and centering the containing __GraphNode__ on that position.

json Namespace
--------------

Files for the JsonCpp library, found [here](http://jsoncpp.sourceforge.net/).


Social Media Server <--> Visualizer Connection
==============================================

This object connects to the "social media" part of the system, currently implemented in HTTP. The exchange is fairly straightforward: 

1. Visualizer sends a request containing the timestamp of the last response it received
2. Social media server sends a response
3. Both sides disconnect

The visualizer's request is fairly basic (and still to be determined), but contains:

* __Timestamp__ of the last response

The media server responds with the following:

* __Votes:__ the songs that have been voted on, and who voted for them (specified by an ID)
* __People:__ at the moment people only have 2 values associated with them, their ID, and their name
* __Blacklist:__ IDs of people who have misbehaved or entered offensive information
* __Timestamp:__ to uniquely identify the request

Note that votes and users voting are split up in this model. This allows the visualizer to create user objects (involving heavyweight operations such as loading an image from the web), in the time between the user logging into the website, and actually casting a vote.


Sylvester (Graph Oracle) <--> Visualizer Conection
==================================================

Initially proposed as a HTTP network connection, this may move into the visualizer binary for speed reasons. The currently proposed exchange involves:

1. The Visualizer compiles a state of its graph nodes, and their connections. This would likely contain the _number of votes for each song_, and the _time when a vote was cast_.
2. Visualizer sends this to Sylvester.
3. Sylvester compiles a more optimal arrangement of this graph, based on how related the songs are, the number of votes, and the time since votes.
4. Sylvester sends a structure containing the more optimal arrangement of the graph.
5. Visualizer merges this optimal arrangement with the currently displaying graph, possible via a "rearrangement" animation.

C++ Coding Conventions
======================

The graphic components of the visualizer are writen using the [Cinder library](http://libcinder.org), and effort should be made to use as many of their built in objects as possible (Vector 3D/2D, Surface, OpenGL texture, Color, math functions such as lerp, floor, ceiling, etc..).

Here are some general points:

Use angle brackets for #include
-------------------------------

__Right:__ #include <adj/adj_GraphNode.h>
__Wrong:__ #include "adj/adj_GraphNode.h"

A lot of the code doesn't conform to this. I'm working on it.

Include namespace in .cpp files
-------------------------------

Example: Client object in the abj::net namespace:

__Right:__ adj_net_Client.cpp 
__Wrong:__ Client.cpp

This is because some copilers put all compiled files into the same directory as .o's, and if the files are named the same, they get clobbered.

Follow Google's style guide
---------------------------

Almost all the code conforms to Google's C++ sytle guidelines, found [here](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). Notable exceptions to Google's style include:

Exceptions are used 
-------------------

A function throws an exception if and only if it fails to do what it claims to do (presumably because it can't). When using exceptions, remember to:

* __Leak no resources__
* __Don't allow data structures to become corrupted.__

See *Effective C++* item 29 for more info.

Use std::shared_ptr for all memory allocation
---------------------------------------------

All instances of "new" should be wrapped in a std::shared_ptr. As a corollary, there shouldn't ever be instances of "delete" in the code. The one exception to this is with singletons objects. In this case, free pointers and calls to delete are allowed.


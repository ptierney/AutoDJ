
Introduction
============

The so called "Auto DJ" system is a collection of system to enable a dance party without the need for a (human) DJ or playlist. It is composed of three connected systems:

* **Mobile-enabled voting sytem.** Allows participants to request songs to be played. Their identities are accessed via this system, allowing names, and photos (and potentially other things), to be accessed by other parts of the system.
* **Playlist visualizer.** Displays information about user's votes and upcomming songs in a communal (read: massively projected) setting. 
* **Graph analyzer (Sylvester).** Trims graph from a potential > 2000 songs to approx. 20 relevant, timely, important, or otherwise "hot" songs for display on the visualizer.

The adj git repo contains code for the visualizer and graph analyzer. Code for the voting system is stored on a separet repo.

Architecture Overview
=====================

graph Namespace
---------------

This namespace contains objects related to a particle system / spring simulation. It is heavily modeled after the [Traer physics library](http://murderandcreate.com/physics/), and as such follows Traer's own coding conventions and styles. It wasn't built from the ground up for the purposes of this visualizer. If its capabilities aren't adequate, it would most likely be best to rewrite these objects rather than modifying Traer's code.

adj Namespace
-------------

This namespace concerns objects related to the visualization of data, connection with other servers, and spawns a new application window.

adj::GraphNode Object
---------------------

This is the fundamental unit of the visualization. Each GraphNode contains a song (which it represents), and a graph::Particle object, linking it back to the physics simulation.

adj::Song Object
----------------



dj::net Namespace
-----------------

This namespace contains objects related to connecting with other parts of the system over the network, typically by HTTP.

Coding Conventions
==================

Almost all the code conforms to Google's C++ sytle guidelines, found [here](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). Notable exceptions to Google's style include:

Exceptions are used 
-------------------

A function throws an exception if and only if it fails to do what it claims to do (presumably because it can't). When using exceptions, remember to:

* **Leak no resources**
* **Don't allow data structures to become corrupted.**

See *Effective C++* item 29 for more info.

Use std::shared_ptr for all memory allocation
---------------------------------------------

All instances of "new" should be wrapped in a std::shared_ptr. As a corollary, there shouldn't ever be instances of "delete" in the code. The one exception to this is with singletons objects. In this case, free pointers and calls to delete are allowed.


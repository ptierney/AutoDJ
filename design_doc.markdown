
Introduction
============



Architecture Overview
=====================




Coding Conventions
==================

Almost all the code conforms to Google's C++ sytle guidelines, found [here](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). Notable exceptions to Google's style include:

Exceptions are used 
-------------------

A funtion throws an exception if and only if it fails to do what it claims to do (presumably because it can't). When using exceptions, remember to:
* **Leak no resources**
* **Don't allow data structures to become corrupted.
* See *Effective C++* item 29 for more info.


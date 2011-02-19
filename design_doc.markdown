
Introduction
============

This document explains the 


Architecture Overview
=====================




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


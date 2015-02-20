bplus_tree
==========

This package includes a C implementation of B+ tree. It exposes typical B+tree operations : insertion, deletion and point query. Range query is to be added.

The implementation allows one-downward pass deletion, i.e., a key deletion from the tree does not have to "back up" along the path.

Code are tested with unit tests (for correctness), memory purification (for memory leak) and coverage tests.






#!/bin/sh

echo "prio_tree"
./prio_tree $1
echo

echo "avltree"
./avltree $1
echo

echo "rbtree"
./rbtree $1
echo

echo "heap"
./heap $1

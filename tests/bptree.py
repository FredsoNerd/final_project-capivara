#!/usr/bin/env python

from capivara import BPlusTree
from random import randint, shuffle

# instance of a b plus tree
# using ramdom sample order
order = randint(3,10)
bptree = BPlusTree(order)

# defines some data
n = 20
data = list(range(n))

# randomly inserts data
shuffle(data)
for i in range(n):
    print("inserting key", data[i])
    bptree.insert(data[i], data[i])

# show tree order and size
bptree.show_tree()
bptree.show_data()

print("tree has order", order)
print("tree has", bptree.size(), "data leafs")

# find data with keys by range
results = list(bptree.range(-10,100))
print("range from -10 to 100", results)

# randomly removes data
shuffle(data)
for i in range(n):
    print("removing key", data[i])
    bptree.remove(data[i])

# find data after removal
results = list(bptree.range(-10,100))
print("range from -10 to 100", results)
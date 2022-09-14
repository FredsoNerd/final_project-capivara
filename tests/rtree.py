#!/usr/bin/env python
from capivara import RTree
from random import randint, shuffle

# instance of a b plus tree
# using ramdom sample order
m = randint(3,10)
M = m**2
rtree = RTree(m, M)

# defines some data
n = 200
data = list(range(n))

# randomly inserts data
shuffle(data)
for i in range(n):
    value = data[i]
    rect = [i*value for i in range(4)]
    print("inserting with key", rect)
    rtree.insert(*rect, value)

# show tree and leaf pointers
rtree.show_tree()
rtree.show_data()

print("tree has orders", m, M)
print("tree has ", rtree.size(), "data leafs")

# find data with keys by range
results = list(rtree.search(-10,10,-100,100))
print("range from -10 to 100", results)

# randomly removes data
shuffle(data)
for i in range(n):
    value = data[i]
    rect = [i*value for i in range(4)]
    print("removing key", rect)
    rtree.remove(*rect)

# find data after removal
results = list(rtree.search(-10,10,-100,100))
print("range from -10 to 100", results)
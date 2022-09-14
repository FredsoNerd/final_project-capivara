#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "rtree/rtree.h"
#include "bptree/bptree.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(structures){
    class_<vector<int>>("Rows")
        .def(vector_indexing_suite<vector<int>>());

    class_<BPlusTree<float,int>>("BPlusTree", init<int>())
        .def("insert", &BPlusTree<float,int>::insert)
        .def("remove", &BPlusTree<float,int>::remove)
        .def("search", &BPlusTree<float,int>::search)
        .def("range", &BPlusTree<float,int>::range)
        // .def("found", &BPlusTree<float,int>::found)
        .def("size", &BPlusTree<float,int>::size)
        .def("show_data", &BPlusTree<float,int>::showData)
        .def("show_tree", &BPlusTree<float,int>::showTree);
    class_<RTree<float,int>>("RTree", init<int, int>())
        .def("insert", &RTree<float,int>::insert)
        .def("remove", &RTree<float,int>::remove)
        .def("search", &RTree<float,int>::search)
        // .def("nearest", &RTree<float,int>::search)
        // .def("intersect", &RTree<float,int>::search)
        .def("size", &RTree<float,int>::size)
        .def("show_data", &RTree<float,int>::showData)
        .def("show_tree", &RTree<float,int>::showTree);
}
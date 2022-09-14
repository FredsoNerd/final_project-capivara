#!/usr/bin/env python

import numpy as np

from tabulate import tabulate
from matplotlib import pyplot as plt

from capivara.structures import RTree
from capivara.structures import BPlusTree

class Index:
    def __init__(self, name=None, data=np.array([])):
        self.name = name
        self.tree = self.get_tree(data)
        # inserts data
        for index, key in enumerate(data):
            self.insert(key, index)
        
    def insert(self, key, value):
        if type(self.tree) == RTree:
            x,y = key
            x,y = float(x),float(y)
            return self.tree.insert(x,x,y,y, value)
        if type(self.tree) == BPlusTree:
            key = float(key)
            return self.tree.insert(key, value)

    def remove(self, key):
        if type(self.tree) == RTree:
            x,y = key
            x,y = float(x),float(y)
            return self.tree.remove(x,x,y,y)
        if type(self.tree) == BPlusTree:
            key = float(key)
            return self.tree.remove(key)

    def get_tree(self, data):
        if len(data.shape) == 1:
            return BPlusTree(10)
        if len(data.shape) == 2:
            return RTree(4,10)
        return None

class DataFrame:
    def __init__(self, data_init=dict()):
        self.index = Index()
        self.data = dict()
        self.cols = list()
        for col_name, col_data in data_init.items():
            # defines the structure as a dict
            self.set_column(col_name, col_data)

    def set_index(self, col_name):
        """indexes the data using a suitable data tree"""
        # sets index
        self.index = Index(col_name, self.data[col_name])

    def get_column(self, col_name):
        return self.data[col_name]

    def set_column(self, col_name, col_data):
        self.cols.append(col_name)
        self.data[col_name] = np.array(col_data)

    def between(self, keya, keyb):
        """returns dataframe when key between given keys"""
        indexes = self.index.tree.range(keya, keyb)
        return self._get_dataframe(indexes)

    def equal(self, key):
        """returns dataframe when key equals given key"""
        indexes = self.index.tree.search(key)
        return self._get_dataframe(indexes)

    def intersection(self, query_rect):
        """returns dataframe when key inside rectangle"""
        indexes = self.index.tree.search(*query_rect)
        return self._get_dataframe(indexes)

    def insert(self, row:dict):
        key = row[self.index.name]
        key_index = self.data[self.index.name].size
        for col_name, col_data in row.items():
            np.append(self.data[col_name], col_data)
        self.index.insert(key, key_index)

    def remove(self, key):
        self.index.remove(key)
    
    def show(self):
        print(self)

    def tabulate(self, *args, **kwargs):
        return tabulate(self.data, *args, **kwargs)

    def plot(self, colx, coly, *args, **kwargs):
        """shows dispersion plot of cols colx and xoly"""
        plt.scatter(self.get_column(colx), self.get_column(coly),  *args, **kwargs)
        plt.show()

    def _get_dataframe(self, indexes):
        df = DataFrame()
        for col_name in self.cols:
            df[col_name] = self.data[col_name][indexes]
        df.set_index(self.index.name)
        return df

    def __getitem__(self, key):
        return self.get_column(col_name=key)
    
    def __setitem__(self, key, value):
        self.set_column(col_name=key, col_data=value)

    # def __delitem__(self, key):
    #     pass

    # def __getattr__(self, name):
    #     return self.__getitem__(key=name)

    # def __setattr__(self, name, value):
    #     self.__setitem__(key=name, value=value)

    # def __delattr__(self, name):
    #     self.__delitem__(key=name)

    def __str__(self):
        if self.cols == []:
            return "Empty DataFrame"
        return self.tabulate(headers='keys', showindex=True)

    def __repr__(self):
        return self.__str__()

    # def __iter__(self):
    #     return self.columns

def points_from_xy(colx, coly):
    """returns a list of tuples representing 2d points"""
    return list(zip(colx, coly))

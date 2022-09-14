#include <iostream>
#include <vector>

using namespace std;
using namespace boost::python;

template <typename Key, typename Data>
class BPlusNode{
    private:
        int order;

        BPlusNode<Key,Data> * parent = NULL;
        vector<Key> keys;
        vector<BPlusNode<Key,Data> *> children;

        vector<Data> data;
        BPlusNode<Key,Data> * pPrev = NULL;
        BPlusNode<Key,Data> * pNext = NULL;

    public:
        BPlusNode(int order){
            this->order = order;
            this->keys.reserve(order);
            this->children.reserve(order+1);
        }

        void insertData(Key key, Data data){
            for(int i = 0; i <= this->order; i++){
                if(i == this->keys.size() || key < this->keys[i]){
                    // inserts key and data
                    this->keys.insert(this->keys.begin()+i, key);
                    this->data.insert(this->data.begin()+i, data);
                    break;
                }
            }
            // checks for overflow
            if(this->keys.size() == this->order) this->split();
        }

        BPlusNode<Key,Data> * removeData(Key key, BPlusNode<Key,Data> * pRoot){
            // finds key index in the node
            int keyIndex = this->indexOf(key);

            this->keys.erase(this->keys.begin()+keyIndex);
            if(this->isLeaf()){
                this->data.erase(this->data.begin()+keyIndex);
                // root: no index and no underflow
                if(this->isRoot()) return this;
            }
            else{
                // removes child
                this->children.erase(this->children.begin()+keyIndex+1);
                
                if(this->isRoot()){
                    // root case
                    if(this->keys.size() == 0){
                        // decrease tree
                        this->children[0]->parent = NULL;
                        return this->children[0];
                    }
                    return this;
                }
            }
            // checks for underflow
            if(this->keys.size() >= this->order/2){
                // case: indexed key
                if(this->isLeaf() && keyIndex == 0){
                    BPlusNode<Key,Data> * pIndex = this->searchIndex(key);
                    if(pIndex) pIndex->keys[pIndex->indexOf(key)] = this->keys[0];
                }
                return pRoot;
            }
            else{
                // this index in children
                int thisIndex = this->parent->indexTo(key);

                // try borrowing from prev sibling
                BPlusNode<Key,Data> * pPrevSibling = thisIndex > 0
                    ? parent->children[thisIndex-1] : NULL;
                if(pPrevSibling && pPrevSibling->keys.size() > this->order/2){
                    // case: borrows key from prev sibling
                    if(this->isLeaf()){
                        // update if indexed key
                        if(keyIndex == 0){
                            BPlusNode<Key,Data> * pIndex = this->searchIndex(key);
                            if(pIndex) pIndex->keys[pIndex->indexOf(key)]
                                = pPrevSibling->keys.back();
                        }
                        // borrows and updates splitting
                        this->keys.insert(this->keys.begin(), pPrevSibling->keys.back());
                        this->data.insert(this->data.begin(), pPrevSibling->data.back());
                        pPrevSibling->keys.pop_back();
                        pPrevSibling->data.pop_back();
                        this->parent->keys[thisIndex-1] = this->keys[0];
                    }
                    else{
                        // borrows key and updates spliting key
                        pPrevSibling->children.back()->parent = this;
                        this->keys.insert(this->keys.begin(), this->parent->keys[thisIndex-1]);
                        this->children.insert(this->children.begin(), pPrevSibling->children.back());
                        this->parent->keys[thisIndex-1] = pPrevSibling->keys.back();
                        pPrevSibling->keys.pop_back();
                        pPrevSibling->children.pop_back();
                    }
                    return pRoot;
                }
                // try borrowing from next sibling
                BPlusNode<Key,Data> * pNextSibling = thisIndex + 1 < parent->children.size()
                    ? parent->children[thisIndex+1] : NULL;
                if(pNextSibling && pNextSibling->keys.size() > this->order/2){
                    if(this->isLeaf()){
                        // update if indexed key
                        if(keyIndex == 0){
                            BPlusNode<Key,Data> * pIndex = this->searchIndex(key);
                            if(pIndex) pIndex->keys[pIndex->indexOf(key)] = this->keys.size() > 0
                                ? this->keys[0] : pNextSibling->keys[0];
                        }
                        // borrows and updates splitting
                        this->keys.push_back(pNextSibling->keys[0]);
                        this->data.push_back(pNextSibling->data[0]);
                        pNextSibling->keys.erase(pNextSibling->keys.begin());
                        pNextSibling->data.erase(pNextSibling->data.begin());
                        this->parent->keys[thisIndex] = pNextSibling->keys[0];
                    }
                    else{
                        // borrows key and updates spliting key
                        pNextSibling->children[0]->parent = this;
                        this->keys.push_back(this->parent->keys[thisIndex]);
                        this->children.push_back(pNextSibling->children[0]);
                        this->parent->keys[thisIndex] = pNextSibling->keys[0];
                        pNextSibling->keys.erase(pNextSibling->keys.begin());
                        pNextSibling->children.erase(pNextSibling->children.begin());
                    }
                    return pRoot;
                }
                if(pPrevSibling){
                    // case: merges with prev sibling
                    if(this->isLeaf()){
                        // merges data
                        pPrevSibling->data.insert(
                            pPrevSibling->data.end(),
                            this->data.begin(), this->data.end());
                        // updates pointers
                        if(this->pNext) this->pNext->pPrev = pPrevSibling;
                        pPrevSibling->pNext = this->pNext;
                    }
                    else{
                        // incorporates parent split key
                        pPrevSibling->keys.push_back(this->parent->keys[thisIndex-1]);
                        // merges children
                        pPrevSibling->children.insert(
                            pPrevSibling->children.end(),
                            this->children.begin(), this->children.end());
                        for(auto child : this->children) child->parent = pPrevSibling;
                    }
                    // merges keys
                    pPrevSibling->keys.insert(pPrevSibling->keys.end(),
                        this->keys.begin(),
                        this->keys.end());
                    // recursivelly remove split key
                    return this->parent->removeData(
                        this->parent->keys[thisIndex-1],
                        pRoot);
                }
                if(pNextSibling){
                    // case: merges with next sibling
                    if(this->isLeaf()){
                        // update if indexed key
                        if(keyIndex == 0){
                            BPlusNode<Key,Data> * pIndex = this->searchIndex(key);
                            if(pIndex) pIndex->keys[pIndex->indexOf(key)]
                                = this->keys.size() > 0
                                    ? this->keys[0] : pNextSibling->keys[0];
                        }
                        // merges data
                        this->data.insert(this->data.end(),
                            pNextSibling->data.begin(),
                            pNextSibling->data.end());
                        // updates pointers
                        this->pNext = pNextSibling->pNext;
                        if(pNextSibling->pNext) pNextSibling->pNext->pPrev = this;
                    }
                    else{
                        // incorporates parent split key
                        this->keys.push_back(this->parent->keys[thisIndex]);
                        // merges children
                        this->children.insert(this->children.end(),
                            pNextSibling->children.begin(), pNextSibling->children.end());
                        for(auto child : pNextSibling->children) child->parent = this;
                    }
                    // merges keys
                    this->keys.insert(this->keys.end(),
                        pNextSibling->keys.begin(), 
                        pNextSibling->keys.end());
                    // recursivelly remove split key
                    return this->parent->removeData(this->parent->keys[thisIndex], pRoot);
                }
                cout << "ops! something went wrong!\n";
            }
            return pRoot;
        }

        void insertChild(Key key, BPlusNode<Key,Data> * pChild){
            pChild->parent = this;
            for(int i = 0; i <= this->order; i++){
                if(i == this->keys.size() || key < this->keys[i]){
                    // inserts key and data
                    this->children.insert(
                        this->children.begin()+i+1, pChild);
                    this->keys.insert(this->keys.begin()+i, key);
                    break;
                }
            }
            // checks for overflow
            if(this->keys.size() == this->order) this->split();
        }

        void split(){
            int medianInd = this->order/2;
            Key medianKey = this->keys[medianInd];
            BPlusNode<Key,Data> * next = new BPlusNode(this->order);
                 
            // splits keys
            next->keys.insert(next->keys.begin(),
                this->keys.begin()+medianInd+(!this->isLeaf()), this->keys.end());
            this->keys.erase(
                this->keys.begin()+medianInd, this->keys.end());
            
            if(this->isLeaf()){
                // splits data
                next->data.insert(next->data.begin(),
                    this->data.begin()+medianInd, this->data.end());
                this->data.erase(
                    this->data.begin()+medianInd, this->data.end());
            }
            else{
                // split children
                next->children.insert(next->children.begin(),
                    this->children.begin()+medianInd+1, this->children.end());
                this->children.erase(
                    this->children.begin()+medianInd+1, this->children.end());
                // update parent pointers
                for(auto nextChild: next->children) nextChild->parent = next;
            }
            
            // update sibling pointers
            if(this->pNext){
                next->pNext = this->pNext;
                this->pNext->pPrev = next;
            }
            this->pNext = next;
            next->pPrev = this;
            // update parent pointers
            if(this->isRoot()){
                this->parent = new BPlusNode(this->order);
                this->parent->children.push_back(this);
            }
            next->parent = this->parent;
            this->parent->insertChild(medianKey , next);
        }

        BPlusNode<Key,Data> * searchRoot(){
            return isRoot()? this:this->parent->searchRoot();
        }

        BPlusNode<Key,Data> * searchLeftmostLeaf(){
                if(this->isLeaf()) return this;
                return this->children[0]->searchLeftmostLeaf();
        }

        BPlusNode<Key,Data> * searchIndex(Key key){
            if(this->isRoot()) return NULL;

            return binary_search(
                parent->keys.begin(),
                parent->keys.end(), key)
                    ? parent
                    : parent->searchIndex(key);
        }

        BPlusNode<Key,Data> * searchLeaf(Key key, BPlusNode<Key,Data> * pNode){
            // searches suitable leaf
            while(!pNode->isLeaf()){
                for(int i = 0; i <= pNode->keys.size(); i++){
                    if(i == pNode->keys.size() || key < pNode->keys[i]){
                        pNode = pNode->children[i];
                        break;
                    }
                }
            }
            return pNode;
        }
        
        BPlusNode<Key,Data> * getLeftmostLeaf(){
            if(this->isLeaf()) return this;
            return this->children[0]->getLeftmostLeaf();
        }

        int indexOf(Key key){
            int index = 0;
            for(; index < this->keys.size(); index++)
                if(key == this->keys[index]) break;
            return index < this->keys.size()? index : -1;
        }

        int indexTo(Key key){
            int index = 0;
            for(; index < this->keys.size(); index++)
                if(key < this->keys[index]) break;
            return index;
        }

        void remove(){
            return;
        }

        bool isRoot(){
            return parent == NULL;
        }

        bool isLeaf(){
            return children.empty();
        }

        BPlusNode<Key,Data> * getNext(){
            return this->pNext;
        }

        BPlusNode<Key,Data> * getPrev(){
            return this->pPrev;
        }

        BPlusNode<Key,Data> * getparent(){
            return this->parent;
        }

        vector<Key> getKeys(){
            return this->keys;
        }

        vector<Data> getData(){
            return this->data;
        }

        vector<BPlusNode<Key,Data> *> getChildren(){
            return this->children;
        }

        void show(int offset){
            if(this->isLeaf()){
                for(int i = 0; i < this->keys.size(); i++){
                    for(int i = 0; i < offset; i++) cout << ("\t");
                    cout << (this->isRoot()?"r:":"l:") << this->getKeys()[i] << "\n";
                }
            }
            else{
                int i = 0;
                for(; i < this->keys.size(); i++){
                    this->children[i]->show(offset+1);
                    for(int i = 0; i < offset; i++) cout << "\t";
                    cout << (this->isRoot()?"r:":"i:") << this->keys[i] << "\n";
                }
                this->children[i]->show(offset+1);
            }
        }
};

template <typename Key, typename Data>
class BPlusTree{
    private:
        BPlusNode<Key,Data> * pRoot = NULL;
        int order;

    public:
        BPlusTree(int order){
            this->order = order;
            pRoot = new BPlusNode<Key,Data>(order);
        }

        void insert(Key key, Data data){
            pRoot->searchLeaf(key, pRoot)->insertData(key, data);
            pRoot = pRoot->searchRoot();
        }

        void remove(Key key){
            pRoot = pRoot->searchLeaf(key, pRoot)->removeData(key, pRoot);
        }

        vector<Data> search(Key key){
            BPlusNode<Key,Data> * pLeaf = pRoot->searchLeaf(key, pRoot);
            Data data = pLeaf->getData()[pLeaf->indexOf(key)];
            vector<Data> results = {data};
            return results;
        }

        vector<Data> range(Key a, Key b){
            BPlusNode<Key,Data> * pLeaf = pRoot->searchLeaf(a, pRoot);
            vector<Data> results;
            while (pLeaf){
                for(int i = 0; i < pLeaf->getKeys().size(); i++){
                    if(pLeaf->getKeys()[i] >= a)
                        if(pLeaf->getKeys()[i] <= b)
                            results.push_back(pLeaf->getData()[i]);
                        else break;
                }
                pLeaf = pLeaf->getNext();
            }
            return results;
        }

        int size(){
            BPlusNode<Key,Data> * pNode = pRoot->getLeftmostLeaf();
            int size = 0;
            while(pNode){
                size += pNode->getData().size();
                pNode = pNode->getNext();
            }
            return size;
        }

        // bool found(Key key){
        //     BPlusNode<Key,Data> * pLeaf = pRoot->searchLeaf(key, pRoot);
        //     return pLeaf->indexOf(key) == -1? false : true;
        // }

        void showData(){
            BPlusNode<Key,Data> * pNode = pRoot->getLeftmostLeaf();
            for(; pNode; pNode = pNode->getNext()){
                cout << "-> ";
                for(auto key : pNode->getKeys())
                    cout << key << " ";
            }
            cout << "\n";
        }

        void showTree(){
            pRoot->show(0);
        } 
};
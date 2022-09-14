#include <iostream>
#include <vector>

using namespace std;

template <typename Key>
class Rectangle{
    private:
        Key x1;
        Key x2;
        Key y1;
        Key y2;

    public: 
        Rectangle(Key x1, Key x2, Key y1, Key y2){
            this->x1 = x1;
            this->x2 = x2;
            this->y1 = y1;
            this->y2 = y2;
        }

        bool overlaps(Rectangle<Key> mbr){
            if(mbr.x1 > x2 || mbr.x2 < x1) return false;   // side to side
            if(mbr.y1 > y2 || mbr.y2 < y1) return false;   // above or under 
            return true;
        }

        void update(Rectangle<Key> mbr){
            this->x1 = min(x1, mbr.x1);
            this->x2 = max(x2, mbr.x2);
            this->y1 = min(y1, mbr.y1);
            this->y2 = max(y2, mbr.y2);
        }

        Key volume(){
            return (x2-x1)*(y2-y1);
        }

        Key volume(Rectangle<Key> mbr){
            // area if includding rectangle mbr given
            Key rangeX = max(x2, mbr.x2) - min(x1, mbr.x1);
            Key rangeY = max(y2, mbr.y2) - max(y1, mbr.y1);
            return rangeX * rangeY;
        }

        Key enlargement(Rectangle<Key> mbr){
            return volume(mbr) - volume();
        }

        void show(){
            cout << "[" << x1 << "," << x2 << "]x";
            cout << "[" << y1 << "," << y2 << "]\n";
        }
};

template <typename Key, typename Data>
class RNode{
    private:
        // node has pointer to parent if not root
        RNode<Key,Data> * pParent = NULL;
        
        // node might be leaf or non-leaf
        vector<Rectangle<Key>> mbrs;
        vector<Data> data;
        vector<RNode<Key,Data> *> children;

    public:

        vector<Data> search(Rectangle<Key> mbr){
            vector<Data> results;
            // searchs overlapping indexes
            for(int i = 0; i < this->mbrs.size(); i++){
                if(this->mbrs[i].overlaps(mbr)){
                    if(this->isLeaf()){
                        results.push_back(this->data[i]);
                    }
                    else{
                        vector<Data> childResults = this->children[i]->search(mbr);
                        results.insert(results.end(),
                            childResults.begin(), childResults.end());
                    }
                }
            }           
            return results;
        }

        void push_back(Rectangle<Key> mbr, Data data){
            this->mbrs.push_back(mbr);
            this->data.push_back(data);
        }

        void push_back(Rectangle<Key> mbr, RNode<Key,Data> * child){
            child->pParent = this;
            this->mbrs.push_back(mbr);
            this->children.push_back(child);
        }

        void pop_back(){
            this->mbrs.pop_back();
            if(this->isLeaf())
                this->data.pop_back();
            else
                this->children.pop_back();
        }

        void pop_index(int index = NULL){
            this->mbrs.erase(this->mbrs.begin()+index);
            if(this->isLeaf())
                this->data.erase(this->data.begin()+index);
            else
                this->children.erase(this->children.begin()+index);
        }

        RNode<Key,Data> * choseLeaf(Rectangle<Key> mbr){
            // leaf check
            if(this->isLeaf()) return this;
            // choose subtree
            int subtree = 0;
            Key bestEnlargement = this->mbrs[0].enlargement(mbr);
            for(int i = 1; i < this->mbrs.size(); i++){
                Key enlargement = this->mbrs[i].enlargement(mbr);
                // strict best enlargement
                if(enlargement < bestEnlargement) subtree = i;
                // resolves ties by volume
                if(enlargement == bestEnlargement){
                    Key volume = this->mbrs[i].volume();
                    Key bestVolume = this->mbrs[subtree].volume();
                    if(volume < bestVolume) subtree = i; 
                }
                // updates best enlargement
                bestEnlargement = min(enlargement, bestEnlargement);
            }
            return this->children[subtree]->choseLeaf(mbr);
        }

        RNode<Key,Data> * findLeaf(Rectangle<Key> mbr, Data data){
            RNode<Key,Data> * pLeaf;
            // fl1: search subtrees
            if(!this->isLeaf()){
                for(int i = 0; i < this->mbrs.size(); i++){
                    if(this->mbrs[i].overlaps(mbr)){
                        pLeaf = this->children[i]->findLeaf(mbr,data);
                        if(pLeaf != NULL) return pLeaf;
                    }
                }
            }
            // fl2: search leaf node for record
            else{
                for(int i = 0; i < this->mbrs.size(); i++){
                    if(this->mbrs[i].overlaps(mbr)){
                        if(this->data[i] == data) return this;
                    }
                }
            }
            // case not found
            return NULL;
        }

        vector<RNode<Key,Data>*> getLeaves(){
            vector<RNode<Key,Data>*> leaves;
            if(this->isLeaf()){
                leaves.push_back(this);
            }
            else{
                for(auto pChild : this->children){
                    vector<RNode<Key,Data>*> childLeaves;
                    childLeaves = pChild->getLeaves();
                    leaves.insert(leaves.end(),
                        childLeaves.begin(), childLeaves.end());
                }
            }
            return leaves;
        }

        bool isRoot(){
            return this->pParent == NULL;
        }

        bool isLeaf(){
            return children.empty();
        }

        int indexOf(RNode<Key,Data> * child){
            for(int i = 0; i < this->children.size(); i++)
                if(this->children[i] == child) return i;
            return -1;
        }

        int indexOf(Data data){
            for(int i = 0; i < this->data.size(); i++)
                if(this->data[i] == data) return i;
            return -1;
        }

        RNode<Key,Data>* & getParent(){
            return this->pParent;
        }

        void setParent(RNode<Key,Data>* pParent){
            this->pParent = pParent;
        }

        Rectangle<Key> getMBR(){
            Rectangle<Key> mbr = Rectangle<Key>(0,0,0,0);
            for(auto rect : this->mbrs) mbr.update(rect);
            return mbr;
        }

        vector<Rectangle<Key>> & getMBRS(){
            return this->mbrs;
        }

        vector<Data> & getData(){
            return this->data;
        }

        vector<RNode<Key,Data>*> & getChildren(){
            return this->children;
        }

        void show(int offset){
            // shows node in a directory structure style
            for(int i = 0; i < offset; i++) cout << ("| ");
            cout << (this->isRoot()?"r:":"n:");   // root or non-root
            cout << (this->isLeaf()?"l:":"i:");   // leaf or internal
            this->getMBR().show();
            // shows data or recurses over children
            for(int i = 0; i < this->mbrs.size(); i++){
                if(this->isLeaf()){
                    for(int i = 0; i < offset+1; i++) cout << ("| ");
                    cout << "d:" << (this->data[i]) << ":";
                    this->mbrs[i].show();
                }
                else{
                    this->children[i]->show(offset+1);
                }
            }
        }
};

template <typename Key, typename Data>
class RTree{
    private:
        int m;  // maximun entries number in a node
        int M;  // minimun entries number in a node

        RNode<Key,Data> * pRoot;

        RNode<Key,Data> * adjustTree(RNode<Key,Data> * pNode, RNode<Key,Data> * pSibling){
            // at2: check if done
            if(pNode->isRoot()){
                if(pSibling != NULL){
                    // i4: grow tree taller
                    RNode<Key,Data> * pParent = new RNode<Key,Data>();
                    pParent->push_back(pNode->getMBR(), pNode);
                    pParent->push_back(pSibling->getMBR(), pSibling);
                    
                    return pParent;
                }
                return pNode;
            }
            // at3: adjust covering rectangle
            int pNodeIndex = pNode->getParent()->indexOf(pNode);
            pNode->getParent()->getMBRS()[pNodeIndex] = pNode->getMBR();
            // at4: propagate node split upward
            RNode<Key,Data> * pUncle = NULL;
            RNode<Key,Data> * pParent = pNode->getParent();
            if(pSibling != NULL){
                pParent->push_back(pSibling->getMBR(), pSibling);
                if(pParent->getMBRS().size() > M){
                    pUncle = splitNode(pParent);
                }
            }
            // at5: move up to next level
            return this->adjustTree(pParent, pUncle);
        }

        void condenseTree(RNode<Key,Data> * pNode){
            // ct1: initialize
            condenseTree(pNode, vector<RNode<Key,Data>*>());
        }

        void condenseTree(RNode<Key,Data> * pNode, vector<RNode<Key,Data>*> Q){
            if(!pNode->isRoot()){
                // ct2: find parent entry
                RNode<Key,Data> * pParent = pNode->getParent();
                int nodeIndex = pParent->indexOf(pNode);
                // ct3: eliminate under-full node
                if(pNode->getMBRS().size() < m){
                    pParent->pop_index(nodeIndex);
                    Q.push_back(pNode);
                }
                // ct4: adjust covering rectangle
                else pParent->getMBRS()[nodeIndex] = pNode->getMBR();
                // ct5: move up one level in tree
                condenseTree(pParent, Q);  
            }
            else{
                // ct6: re-insert orphaned entries
                for(RNode<Key,Data>* pNode : Q){
                    for(RNode<Key,Data>* pLeaf : pNode->getLeaves()){
                        for(int i = 0; i < pLeaf->getMBRS().size(); i++){
                            this->insert_mbr(pLeaf->getMBRS()[i], pLeaf->getData()[i]);
                        }
                    }
                }
            }      
        }

        RNode<Key,Data> * splitNode(RNode<Key,Data> * pNode){
            // obtains L and LL containing E and all entries
            RNode<Key,Data> * pSibling = new RNode<Key,Data>();
            // naive node inplace split
            for(int i = 0; i < M/2; i++){
                if(pNode->isLeaf()){
                    pSibling->push_back(
                        pNode->getMBRS()[M-i],
                        pNode->getData()[M-i]);
                }
                else{
                    pSibling->push_back(
                        pNode->getMBRS()[M-i],
                        pNode->getChildren()[M-i]);
                }
                pNode->pop_back();
            }
            return pSibling;
        }

        void insert_mbr(Rectangle<Key> mbr, Data data){
            // i1: find position for new record 
            RNode<Key,Data> * pLeaf = pRoot->choseLeaf(mbr);
            // i2: add record to leaf node
            pLeaf->push_back(mbr, data);
            RNode<Key,Data> * pSibling = NULL;
            if(pLeaf->getMBRS().size() > M)
                pSibling = splitNode(pLeaf);
            // i3: propagate changes upward
            // i4: grow tree taller
            this->pRoot = this->adjustTree(pLeaf, pSibling);
        }

        void remove_mbr(Rectangle<Key> mbr, Data data){
            // d1: find node containing record
            RNode<Key,Data> * pLeaf;
            pLeaf = pRoot->findLeaf(mbr, data);
            // d2: delete record
            int dataIndex = pLeaf->indexOf(data);
            pLeaf->pop_index(dataIndex);
            // d3: propagate changes
            this->condenseTree(pLeaf);
            // d4: shorten tree
            if(pRoot->getChildren().size()==1)
                this->pRoot = pRoot->getChildren()[0];
                this->pRoot->setParent(NULL);
        }

        void remove_data(Key x1, Key x2, Key y1, Key y2, Data data){
            this->remove_mbr(Rectangle<Key>(x1,x2,y1,y2), data);
        }

    public:
        RTree(int m, int M){
            this->m = m;
            this->M = M;
            this->pRoot = new RNode<Key,Data>();
        }

        vector<Data> search(Key x1, Key x2, Key y1, Key y2){
            return pRoot->search(Rectangle<Key>(x1,x2,y1,y2));
        }

        void insert(Key x1, Key x2, Key y1, Key y2, Data data){
            this->insert_mbr(Rectangle<Key>(x1,x2,y1,y2), data);
        }

        void remove(Key x1, Key x2, Key y1, Key y2){
            for(auto data : pRoot->search(Rectangle<Key>(x1,x2,y1,y2))){
                this->remove_data(x1,x2,y1,y2, data);
            }
        }

        int size(){
            int size = 0;
            for(auto leaf : pRoot->getLeaves())
                size += leaf->getMBRS().size();
            return size;
        }

        void showTree(){
            pRoot->show(0);
        }

        void showData(){
            cout << "data: ";
            for(auto d : this->pRoot->search(pRoot->getMBR()))
                cout << d << " ";
            cout << "\n";
        }
};
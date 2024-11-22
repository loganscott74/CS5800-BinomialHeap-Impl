//
//  main.cpp
//  CS5800-BinomialHeap-Impl
//
//  Created by Logan Gill on 11/15/24.
//

#include <iostream>
#include <map>

using namespace std;

/// Represents a node of a Binomial Heap
class Node {
public:
    int key;
    int degree;
    Node* p;
    Node* right;
    Node* lChild;
    
    Node(int k, int d): key(k), degree(d), p(nullptr), right(nullptr), lChild(nullptr)
    {}
    
    /// Displays this node, all of it's children, and its siblings and their children
    void display() {
        cout << "| ";
        displaySib();
        cout << endl;
        displayChildren(1);
    }
    
    /// Displays this node and it's siblings
    void displaySib() {
        cout << key << "(" << degree << ") | ";
        if (right) {
            right->displaySib();
        }
    }
    
    /// Displays this node's children at all levels and all of it's siblings' children
    /// - Parameters:
    ///   - level: The level to indent
    void displayChildren(int level) {
        if (lChild) {
            for (int i = 0; i < level; ++i) {
                cout << "\t";
            }
            cout << "" << key << ": | ";
            lChild->displaySib();
            cout << endl;
            lChild->displayChildren(level + 1);
        }
        
        if (right) {
            right->displayChildren(level);
        }
    }
};

class BinomialHeap {
private:
    Node* head;
    
public:
    BinomialHeap(): head(nullptr)
    {}
    
    
    /// Creates and returns a new heap with only the given integer as a node
    static BinomialHeap makeHeap(int k) {
        BinomialHeap newHeap = BinomialHeap();
        Node* newNode = new Node(k, 0);
        newHeap.head = newNode;
        return newHeap;
    }
    
    /// Creates and returns a new empty heap
    static BinomialHeap makeHeap() {
        return BinomialHeap();
    }
    
    /// Gets the node with the smallest value
    Node* min() {
        Node* currNode = head;
        Node* minNode = head;
        
        while (currNode) {
            if (currNode->key < minNode->key)
                minNode = currNode;
            currNode = currNode->right;
        }
        return minNode;
    }
    
    /// Gets and removes the minimum value of the
    Node* extractMin() {
        Node* minNode = min();
        
        Node* rootNodes = head;
        if (rootNodes == minNode) {
            head = rootNodes->right;
        } else {
            while (rootNodes && rootNodes->right != minNode) {
                rootNodes = rootNodes->right;
            }
            rootNodes->right = minNode->right;
        }
        
        // Reverse the min's children
        Node* currNode = minNode->lChild;
        Node* frontNode = currNode;
        if (frontNode)
            frontNode->p = nullptr;
        while (currNode && currNode->right) {
            Node* ogRight = currNode->right->right; //->19  //null
            currNode->right->right = frontNode; //6->3->6 //6->19->3->...
            frontNode = currNode->right; //->3 //->19
            frontNode->p = nullptr;
            //currNode = currNode->right; //->3
            currNode->right = ogRight; //3->|6->19| //
            //currNode = currNode->right; //->6
        }
        
        // 6 3 19
        BinomialHeap addHeap = makeHeap();
        addHeap.head = frontNode;
        (*this) = heapUnion(addHeap);
        
        return minNode;
    }
    
    /// Creates a new heap consisting of the union between this heap and the given heap, destorying both in the prosses
    BinomialHeap heapUnion(BinomialHeap& other) {
        // If both are empty/nullpointers, return empty heap
        if (!this->head && !other.head)
            return makeHeap();
        // If this heap is empty
        else if (!this->head)
            return other;
        // If the other heap is empty
        else if (!other.head)
            return *this;
        
        BinomialHeap newHeap = makeHeap();
        newHeap.head = merge(other);
        
        Node* currNode = newHeap.head;
        Node* prevNode = nullptr;
        Node* nextNode = currNode->right;
        while (nextNode) {
            // If curr degree and next degree are not =, or curr degree is equal to the next degree
            if (currNode->degree != nextNode->degree
                || (nextNode->right && currNode->degree == nextNode->right->degree)) {
                prevNode = currNode;
                currNode = nextNode;
                
                nextNode = currNode->right;
            } else if (currNode->key <= nextNode->key) { // If curr key is less than or = to next key
                currNode->right = nextNode->right;
                // Link
                nextNode->p = currNode;
                nextNode->right = currNode->lChild;
                currNode->lChild = nextNode;
                currNode->degree += 1;
                
                nextNode = currNode->right;
            } else { // If curr key is greater than next key
                if (!prevNode)
                    newHeap.head = nextNode;
                else
                    prevNode->right = nextNode;
                // do linking currNode, nextNode
                currNode->p = nextNode;
                currNode->right = nextNode->lChild;
                nextNode->lChild = currNode;
                nextNode->degree += 1;
                currNode = nextNode;
                
                nextNode = currNode->right;
            }
        }
        return newHeap;
    }
    
    /// Merges the two heaps by order of degree, destoying both in the process, and returns the new head
    /// Node: Does not fix resulting heap to follow rules of binomial heaps
    Node* merge(BinomialHeap& other) {
        Node* newHead = nullptr;
        Node* currNode = newHead;
        
        Node* currThis = this->head;
        Node* currOther = other.head;
        
        while (currThis || currOther) {
            if (!currOther || (currThis && currThis->degree <= currOther->degree)) {
                if (!newHead) {
                    newHead = currThis;
                    currNode = newHead;
                } else {
                    currNode->right = currThis;
                    currNode = currNode->right;
                }
                currThis = currThis->right;
            } else {
                if (!newHead) {
                    newHead = currOther;
                    currNode = newHead;
                } else {
                    currNode->right = currOther;
                    currNode = currNode->right;
                }
                currOther = currOther->right;
            }
        }
        return newHead;
    }
    
    /// Inserts the given key into the heap
    Node* insert(int k) {
        BinomialHeap addHeap = makeHeap(k);
        Node* newNode = addHeap.head;
        (*this) = heapUnion(addHeap);
        return newNode;
    }
    
    /// Decreases the key of the node to the value and fixes the heap as needed
    void decreaseKey(Node* currNode, int k) {
        if (k > currNode->key)
            throw invalid_argument("The new key cannot exceed that of the original key");
        
        currNode->key = k;
        Node* currPar = currNode->p;
        
        while (currPar && currPar->key > currNode->key) {
            Node* oldRight = currNode->right;
            Node* oldChild = currNode->lChild;
            int oldDegree = currNode->degree;
            
            // Updates currPar's old left siblings's right pointer to point to currNode
            Node* parSib = nullptr;
            if (currPar->p) {
                parSib = currPar->p->lChild;
            } else {
                parSib = head;
            }
            
            while (parSib) {
                if (parSib == currPar)
                    break;
                else if (parSib->right == currPar) {
                    parSib->right = currNode;
                    break;
                }
                parSib = parSib->right;
            }
            
            // Updates currNode's old left siblings's right pointer to point to currPar
            Node* changedSib = currPar->lChild;
            while (changedSib) {
                changedSib->p = currNode;
                if (changedSib->right == currNode) {
                    Node* skip = changedSib->right->right;
                    changedSib->right = currPar;
                    changedSib = skip;
                    continue;
                }
                changedSib = changedSib->right;
            }
            
            currNode->p = currPar->p;
            currNode->right = currPar->right;
            currNode->lChild = currPar->lChild == currNode ? currPar : currPar->lChild;
            currNode->degree = currPar->degree;
            
            currPar->p = currNode;
            currPar->right = oldRight;
            currPar->lChild = oldChild;
            currPar->degree = oldDegree;
            
            currPar = currNode->p;
        }
    }
    
    /// Removes the given node from the tree
    void remove(Node* removeNode) {
        int minKey = min()->key;
        decreaseKey(removeNode, minKey-1);
        extractMin();
    }
    
    /// Finds a node with the given value. If multiple nodes have the same value, it returns the one who's parent (or itself) is closest to the head, prioritizing children over siblings
    Node* search(int k) {
        Node* currNode = head;
        while (currNode) {
            if (currNode->key == k)
                return currNode;
            else if (currNode->key < k && currNode->lChild)
                currNode = currNode->lChild;
            else if (currNode->right)
                currNode = currNode->right;
            else if (currNode->p)
                currNode = currNode->p->right;
            else
                currNode = currNode->right;
        }
        
        // TODO: throw if here?
        throw invalid_argument("Cannot find a node with the given key in this heap");
        return currNode;
    }
    
    void display() {
        head->display();
    }
};

int main(int argc, const char * argv[]) {
    map<string, BinomialHeap> heaps;
    
    vector<int> setupHeap;
    setupHeap.push_back(1);
    setupHeap.push_back(19);
    setupHeap.push_back(3);
    setupHeap.push_back(22);
    setupHeap.push_back(81);
    setupHeap.push_back(6);
    setupHeap.push_back(44);
    setupHeap.push_back(100);
    setupHeap.push_back(75);
    
    BinomialHeap starterHeap = BinomialHeap().makeHeap();
    heaps["start"] = starterHeap;
    
    for (int i : setupHeap) {
        starterHeap.insert(i);
        try {
            cout << starterHeap.search(i)->key << endl;
        } catch(const invalid_argument &e) {
            cout << e.what();
        }
    }
    
    Node* n = starterHeap.search(1);
    starterHeap.remove(n);
    heaps["start"] = starterHeap;
    
    //cout << starterHeap.search(1) << endl;
    
    while (true) {
        cout << "Enter a command: ";
        string command;
        cin >> command;
        
        if (cin.fail()) {
            cout << "Failed to read input, please try again" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        
        string name;
        int num;
        
        try {
            if (command == "quit" || command == "end") {
                cout << "Promt closed" << endl;
                break;
            } else if (command == "search") {
                cin >> name;
                cin >> num;
                if (num) {
                    try {
                        BinomialHeap heap = heaps.at(name);
                        Node* n = heap.search(num);
                        cout << "Found " << n->key;
                    } catch (const invalid_argument &e) {
                        cout << e.what();
                    }
                } else {
                    cout << "Incorrect input for search. Format should be: search [heap name] [new int value]";
                }
            } else if (command == "insert") {
                cin >> name;
                cin >> num;
                if (num) {
                    try {
                        BinomialHeap heap = heaps.at(name);
                        heap.insert(num);
                        heaps[name] = heap;
                        Node* n = heap.search(num);
                        cout << "Inserted " << n->key;
                    } catch (const invalid_argument &e) {
                        cout << "A falure occured during insert operation";
                    }
                } else {
                    cout << "Incorrect input for insert. Format should be: insert [heap name] [new int value]";
                }
            } else if (command == "delete") {
                cin >> name;
                cin >> num;
                if (num) {
                    try {
                        BinomialHeap heap = heaps.at(name);
                        Node* sNode = heap.search(num);
                        heap.remove(sNode);
                        heaps[name] = heap;
                        cout << "Removed " << num;
                    } catch (const invalid_argument &e) {
                        cout << e.what();
                    }
                } else {
                    cout << "Incorrect input for delete. Format should be: delete [heap name] [new int value]";
                }
            } else if (command == "min") {
                cin >> name;
                BinomialHeap heap = heaps.at(name);
                Node* m = heap.min();
                cout << "Smallest in heap " << name << " is " << m->key;
            } else if (command == "extract-min") {
                cin >> name;
                BinomialHeap heap = heaps.at(name);
                Node* m = heap.extractMin();
                heaps[name] = heap;
                cout << "Extracted smallest value " << m->key << " from heap " << name;
            } else if (command == "decrease-key") {
                cin >> name;
                cin >> num;
                int newNum;
                cin >> newNum;
                if (num && newNum) {
                    BinomialHeap heap = heaps.at(name);
                    Node* sNode = heap.search(num);
                    heap.decreaseKey(sNode, newNum);
                    heaps[name] = heap;
                    cout << "Decreased " << num << " to " << newNum << " in heap " << name;
                } else {
                    cout << "Incorrect input for decrease-key. Format should be: insert [heap name] [value to change] [new value]";
                }
            } else if (command == "union") {
                string otherName;
                cin >> name >> otherName;
                BinomialHeap heap = heaps.at(name);
                BinomialHeap otherHeap = heaps.at(otherName);
                BinomialHeap newHeap = heap.heapUnion(otherHeap);
                cout << "Enter a name for the new heap: ";
                string newName;
                cin >> newName;
                heaps.erase(name);
                heaps.erase(otherName);
                heaps.insert(pair<string, BinomialHeap>(newName, newHeap));
                cout << "Create new heap " << newName << " and removed heaps " << name << " and " << otherName;
            } else if (command == "makeHeap") {
                cout << "Enter a name for the new heap: ";
                string newName;
                cin >> newName;
                BinomialHeap newHeap = BinomialHeap().makeHeap();
                heaps.insert(pair<string, BinomialHeap>(newName, newHeap));
            } else if (command == "display") {
                cin >> name;
                BinomialHeap heap = heaps.at(name);
                heap.display();
            } else {
                cout << "Invalid command" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cout << endl;
            
        } catch (const out_of_range &e) {
            cout << "----------" << endl << "Error message:" << endl << e.what() << endl << "----------" << endl;
            cout << "No heap exists with the given name. Please try again" << endl;
        } catch (const invalid_argument &e) {
            cout << e.what() << endl;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    
    std::cout << "Hello, World!\n";
    return 0;
}

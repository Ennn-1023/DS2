// 11127139 陳恩

#include <iostream>
#include <utility>
#include <vector>
#include <fstream>

using namespace std;
struct dataType {
    int no;
    string sName;
    string dName;
    string type;
    string degree;
    int sNum; // student nums
    int gradNum;
    dataType() {
        no = 0;
        gradNum = 0;
    }
};
struct keyType {
    vector<int> noList;
    int gradNum;
    keyType(){
        noList.clear();
        gradNum = -1;
    }
    keyType(int no, int gradNum): gradNum(gradNum) {
        this->noList.push_back(no);
    }
};
class inputFile {
private:
    string fileID;
    vector<dataType> inputData;
    int toInt(string str) {
        int size = str.size();
        int sum = 0;
        for ( int i = 0; i < size; i++ ) {
            if (isdigit(str[i]))
                sum = sum*10 + str[i]-'0';
        }
        return sum;
    }
public:
    inputFile() {
        reset();
    }
    ~inputFile() {
        reset();
    }
    void reset() {
        fileID.clear();
        inputData.clear();
    }
    bool isEmpty() {
        return inputData.empty();
    }
    vector<dataType> getList() {
        return inputData;
    }
    bool readFile(){
        fstream inFile;
        string fileName;
        this->reset();
        cout << endl << "Input a file number ([0] Quit): ";
        cin >> this->fileID;
        if ( fileID == "0" ) {
            reset();
            return false;
        }
        fileName = "input" + fileID + ".txt";
        inFile.open(fileName.c_str(), fstream::in);
        if (!inFile.is_open()){
            cout << endl << "### " << fileName << " does not exist! ###" << endl;
            return false;
        } //end if
        else {
            char cstr[255];
            int fNo, pre, pos;

            // reading the unneeded lines
            inFile.getline(cstr, 255, '\n');
            inFile.getline(cstr, 255, '\n');
            inFile.getline(cstr, 255, '\n');
            // input order number
            int dataNo = 0;
            while (inFile.getline(cstr, 255, '\n')) {
                dataType oneR;
                string buf, cut;
                fNo = 0;
                pre = 0;
                buf.assign(cstr);
                // order number;
                oneR.no = ++dataNo;
                do {
                    pos = buf.find_first_of('\t', pre);
                    cut = buf.substr(pre, pos - pre);
                    switch (++fNo) {
                        case 2:
                            oneR.sName = cut;
                            break;
                        case 4:
                            oneR.dName = cut;
                            break;
                        case 5:
                            oneR.type = cut;
                            break;
                        case 6:
                            oneR.degree = cut;
                            break;
                        case 7:
                            oneR.sNum = toInt(cut);
                            break;
                        case 9:
                            oneR.gradNum = toInt(cut);
                        default:
                            break;
                    }
                    pre = ++pos;
                } while ((pos > 0) && (fNo < 9));
                this->inputData.push_back(oneR); } //end outer-while inFile.close();
        } // end else

        return true;
    } // end of readFile
};

class two3Tree {
private:
    struct two3Node {
        two3Node *parent;
        keyType key1, key2;
        keyType midKey; // keep the extra key value to be split
        two3Node *left, *mid, *right;
        two3Node *tmpChild; // right side of mid
        int numOfKey;
        two3Node(): numOfKey(0), parent(NULL), left(NULL), mid(NULL), right(NULL), tmpChild(NULL) {
        }
        two3Node(two3Node *parent, const keyType& key):
        parent(parent), key1(key), numOfKey(1), left(NULL), mid(NULL), right(NULL), tmpChild(NULL) {}
    };
    two3Node* root;
    /*
     * node: node to be added key
     * produce a 2-node, 3-node or 4-node
     */
    void addKey(two3Node* node, const keyType& key) {

        if (node->numOfKey == 2) { // node is a 3-node
            // check the key value is same as node key or not
            if (node->key1.gradNum == key.gradNum) {
                node->key1.noList.push_back(key.noList[0]);
                return;
            }
            else if (node->key2.gradNum == key.gradNum) {
                node->key2.noList.push_back(key.noList[0]);
                return;
            }
            else {
                // choose one of the three key to be midKey
                if (key.gradNum < node->key1.gradNum) {
                    node->midKey = node->key1;
                    node->key1 = key;
                }
                else if (node->key2.gradNum < key.gradNum) {
                    node->midKey = node->key2;
                    node->key2 = key;
                }
                else
                    node->midKey = key;
            }
        }
        else if (node->numOfKey == 1) { // node is a 2-node
            if (key.gradNum < node->key1.gradNum) {
                node->key2 = node->key1;
                node->key1 = key;
            } else if (key.gradNum == node->key1.gradNum) {
                node->key1.noList.push_back(key.noList[0]);
                return;
            } else
                node->key2 = key;
        }
        else
            node->key1 = key;
        node->numOfKey++;
    }
    void insertFunc(two3Node *node, const keyType& newKey) {
        // helper function for insert new key

        if ( root == NULL ) {
            root = new two3Node(NULL ,newKey);
            return;
        }
        // get position of the node which should be inserted
        two3Node *insertNode = search(newKey);
        addKey(insertNode, newKey);
        if (insertNode->numOfKey > 2) // 4-node, need to be split
            split(insertNode);
    }
    void divideChild(two3Node* parent, two3Node* node) {
        if (parent->numOfKey == 2 && node == parent->left) {
            parent->mid = new two3Node(parent, node->key2);
            node->numOfKey = 1; // node has only one key now
            node->key2 = keyType();
            // turn the 4 child to be the child of two 2-node(left, mid)
            if (!isLeaf(node)) {
                // remapping child to parent
                node->tmpChild->parent = parent->mid;
                parent->mid->left = node->tmpChild;
                node->right->parent = parent->mid;
                parent->mid->right = node->right;

                node->right = node->mid;
                node->mid = NULL; // reset mid to be empty
                node->tmpChild = NULL;
            }
        }
        else if ( parent->numOfKey == 2 && node == parent->right) {
            // node is right child of parent
            parent->mid = new two3Node(parent, node->key1);
            node->numOfKey = 1; // reset node to be a 2-node
            node->key1 = node->key2;
            node->key2 = keyType();
            // turn the 4 child to be the child of two 2-node(left, mid)
            if (!isLeaf(node)) {
                // remapping child
                node->left->parent = parent->mid;
                parent->mid->left = node->left;
                node->mid->parent = parent->mid;
                parent->mid->right = node->mid;

                node->left = node->tmpChild;
                node->mid = NULL;
                node->tmpChild = NULL;
                // node->right did not change
            }
        }
        else if (node == parent->right) { // parent->numOfKey == 3
            parent->tmpChild = new two3Node(parent, node->key1);
            node->numOfKey = 1; // reset node to be a 2-node
            node->key1 = node->key2;
            node->key2 = keyType();
            // divide children
            if (!isLeaf(node)) {
                // remapping child
                node->left->parent = parent->tmpChild;
                parent->tmpChild->left = node->left;
                node->mid->parent = parent->tmpChild;
                parent->tmpChild->right = node->mid;

                node->left = node->tmpChild;
                node->mid = NULL;
                node->tmpChild = NULL;
                // node->right did not change
            }
        }
        else if (node == parent->mid){ // mid, parent->numOfKey == 3
            parent->tmpChild = new two3Node(parent, node->key2);
            node->numOfKey = 1;
            node->key2 = keyType();
            // divide children
            if (!isLeaf(node)) {
                // remapping parent
                node->tmpChild->parent = parent->tmpChild;
                parent->tmpChild->left = node->tmpChild;
                node->right->parent = parent->tmpChild;
                parent->tmpChild->right = node->right;

                // node->left did not change
                node->right = node->mid; // move mid to be right child
                node->mid = NULL; // clear node->mid
                node->tmpChild = NULL;
            }
        }
        else { // left, numOfChild == 3
            parent->tmpChild = parent->mid;
            parent->mid = new two3Node(parent, node->key2);
            node->numOfKey = 1;
            node->key2 = keyType();
            // divide children
            if (!isLeaf(node)) {
                // remapping child
                node->tmpChild->parent = parent->mid;
                parent->mid->left = node->tmpChild;
                node->right->parent = parent->mid;
                parent->mid->right = node->right;

                // node->left did not change
                node->right = node->mid;
                node->mid = NULL;
                node->tmpChild = NULL;
            }
        }
    }
    bool isLeaf(two3Node* node) {
        if (node->left == NULL)
            return true;
        else
            return false;
    }
    void split(two3Node* node) { // done
        two3Node *parent = node->parent;
        if (node == root) {
            parent = new two3Node(NULL, node->midKey);
            node->midKey = keyType(); // clear middle key
            node->parent = parent;
            parent->left = node;
            parent->right = new two3Node(parent, node->key2);
            node->key2 = keyType();
            node->numOfKey = 1;
            if (!isLeaf(node)) {
                // remapping children to their parent
                node->tmpChild->parent = parent->right;
                parent->right->left = node->tmpChild;
                node->right->parent = parent->right;
                parent->right->right = node->right;
                node->tmpChild = NULL;;

                node->right = node->mid; // move node's children
                node->mid = NULL;
                // node->left did not change
            }

            // change the root to the new node
            root = parent;
            return;
        }

        // add the middle key of node into its parent
        addKey(parent, node->midKey);
        node->midKey = keyType(); // clear
        divideChild(parent, node);

        if (parent->numOfKey == 3) // parent is a 4-node, need to split
            split(parent);
    }

    two3Node* search(const keyType& key) {
        two3Node *pre, *cur = root;
        while ( cur != NULL ) {
            pre = cur;
            // 2-node or 3-node
            if (cur->numOfKey == 2) {
                if (cur->key2.gradNum < key.gradNum)
                    cur = cur->right;
                else if (cur->key2.gradNum == key.gradNum || cur->key1.gradNum == key.gradNum)
                    return cur;
                else if (cur->key1.gradNum < key.gradNum && key.gradNum < cur->key2.gradNum)
                    cur = cur->mid;
                else
                    cur = cur->left;
            } // numOfKey == 2
            else { // numOfKey == 1
                if (key.gradNum < cur->key1.gradNum)
                    cur = cur->left;
                else if (key.gradNum == cur->key1.gradNum)
                    return cur;
                else
                    cur = cur->right;
            } // end of else
        } // end of while

        return pre;
    }

public:
    two3Tree() {
        root = NULL;
    }
    ~two3Tree() {
        destroy(root);
    }

    void reset() {
        destroy(root);
        root = NULL;
    }
    void destroy(two3Node* node) {
        // delete the node and its children
        if ( node != NULL ) {
            destroy(node->left);
            destroy(node->mid);
            destroy(node->tmpChild);
            destroy(node->right);
            delete node;
        }
    }

    // build by inserting one by one
    void build(const vector<dataType>& list) {
        unsigned long size = list.size();
        for (int i = 0; i < size; i++) {
            insert(list[i].no, list[i].gradNum);
        }
    }
    void insert(int no, int numOfStu) {
        keyType newItem(no, numOfStu);
        insertFunc(root, newItem);
    }
    int count(two3Node *node) {
        if (node != NULL)
            return count(node->left) + count(node->mid) + count(node->right) +1;
        else
            return 0;
    }
    int getHeight() {
        int height = 0;
        two3Node * tmp = root;
        while (tmp != NULL) {
            tmp = tmp->left;
            height++;
        }
        return height;
    }
    void print(const vector<dataType> &list) {


        cout << "\nTree height = " << getHeight();
        cout << "\nNumber of nodes = " << count(root);
        int i = 0;
        for (int no: root->key1.noList) {
            dataType data = list[no-1];

            cout << endl << ++i << ": [" << no << "] " << data.sName << ", " << data.dName << ", ";
            cout << data.type << ", " << data.degree << ", " << data.sNum << ", " << data.gradNum;
        }
        for (int no: root->key2.noList) {
            dataType data = list[no-1];
            cout << endl << ++i << ": [" << no << "] " << data.sName << ", " << data.dName << ", ";
            cout << data.type << ", " << data.degree << ", " << data.sNum << ", " << data.gradNum;
        }
    }

};

int main() {
    // read txt
    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    // command = { 0: quit, 1: build a max heap, 2: build a DEAP }
    two3Tree a23tree;

    while ( keepRun ) {

        cout << "\n**** Balanced Search Trees ****"
                "\n* 0. QUIT                     *"
                "\n* 1. Build 23 tree            *"
                "\n* 2. Build AVL tree           *"
                "\n*******************************";
        cout << "\nInput a choice(0, 1, 2): ";
        cin >> command;
        switch ( command ) {
            case 0:
                keepRun = false;
                break;
            case 1:
                // check if the file had been read successfully
                aFile.reset();
                if ( !aFile.readFile() )
                    break;
                // reset
                // build head when the file being read successfully
                a23tree.reset();
                a23tree.build(aFile.getList());
                // output information
                a23tree.print(aFile.getList());
                break;
            case 2:
                aFile.reset();
                if ( !aFile.readFile() )
                    break;

                break;
            case 3:
                if (aFile.isEmpty())
                    break;
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        }
    }
}
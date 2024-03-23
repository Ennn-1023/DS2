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
    dataType() {
        no = 0;
        sNum = 0;
    }
};
struct keyType {
    vector<int> noList;
    int sNum;
    keyType(){
        noList.clear();
        sNum = -1;
    }
    keyType(int no, int sNum): sNum(sNum) {
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
                        default:
                            break;
                    }
                    pre = ++pos;
                } while ((pos > 0) && (fNo < 7));
                this->inputData.push_back(oneR); } //end outer-while inFile.close();
        } // end else

        return true;
    } // end of readFile
};

class two3Tree {
private:
    struct two3Node {
        keyType key1, key2;
        keyType tmpKey; // keep the extra key value to be split
        two3Node *left, *mid, *right;
        two3Node *tmpChild; // right side of mid
        int numOfKey;
        two3Node(): numOfKey(0), left(NULL), mid(NULL), right(NULL), tmpChild(NULL) {
        }
        two3Node(const keyType& key): key1(key), numOfKey(1), left(NULL), mid(NULL), right(NULL), tmpChild(NULL) {
        }
    };
    two3Node* root;

    void addKey(two3Node* node, const keyType& newKey) {
        if (node->numOfKey == 2) {
            if ( newKey.sNum < node->key1.sNum ) {
                node->tmpKey = node->key1;
                node->key1 = newKey;
            }
            else if (node->key2.sNum < newKey.sNum) {
                node->tmpKey = node->key2;
                node->key2 = newKey;
            }
            else if (node->key1.sNum == newKey.sNum) { // key1 has a same key
                node->key1.noList.push_back(newKey.noList[0]);
                return;
            }
            else if (node->key2.sNum == newKey.sNum) {// key2 has a same key
                node->key2.noList.push_back(newKey.noList[0]);
                return;
            }
            else
                node->tmpKey = newKey;
        }
        else if (node->numOfKey == 1) {
            // place newKey in key1 if newKey is smaller
            if (node->key1.sNum < newKey.sNum) {
                node->key2 = node->key1;
                node->key1 = newKey;
            }
            else if (node->key1.sNum == newKey.sNum) {
                node->key1.noList.push_back(newKey.noList[0]);
                return;
            }
            else // else place newKey in key2
                node->key2 = newKey;
        } // end of numOfKey == 1
        else // empty
            node->key1 = newKey;

        node->numOfKey++;
    }

    two3Node* insertRecur(two3Node* node, const keyType& newItem) {
        // whether the node is a leaf or not
        if (node->left == NULL) { // is a leaf, add key
            addKey(node, newItem);
        }
        else { // not a leaf
            switch (node->numOfKey) {
                case 2:
                    if (node->key2.sNum < newItem.sNum) {
                        node->right = insertRecur(node->right, newItem);
                        if (node->right->numOfKey == 3) { // need to split
                            split(node, 2); // split node->right
                        }
                        break;
                    }
                    else if (node->key2.sNum == newItem.sNum) {
                        node->key2.noList.push_back(newItem.noList[0]);
                        return node;
                    }
                    else if (node->key1.sNum < newItem.sNum && newItem.sNum < node->key2.sNum) {
                        node->mid = insertRecur(node->mid, newItem);
                        if (node->mid->numOfKey == 3)
                            split(node, 1); // split node->mid
                        break;
                    }
                case 1:
                    if (newItem.sNum < node->key1.sNum) {
                        node->left = insertRecur(node->left, newItem);
                        if (node->left->numOfKey == 3) {
                            split(node, 0); // split node->left
                        }
                    }
                    else if (newItem.sNum == node->key1.sNum) {
                        node->key1.noList.push_back(newItem.noList[0]);
                        return node;
                    }
                    else
                        node->right = insertRecur(node->right, newItem);
                        if (node->right->numOfKey == 3)
                            split(node, 2); // split node->right
                    break;
                default:
                    break;
            } // end of switch

            return node;
        } // end of case: not a leaf
    }


    /*
     * split the child of parent.
     * which: {0: left, 1: mid, 2: right}
    */
    void split(two3Node* parent, int which) {
        // 分成 two node 怎麼處理??
        // parent可能變成 4 children， 怎麼分

        two3Node *tmp;
        switch (which) {
            case 0:
                if (parent->numOfKey == 1) {
                    // setting mid
                    parent->mid = new two3Node(parent->left->key2); // create mid and store left->key2, numOfKey = 1
                    // split children to correct parent
                    parent->mid->left = parent->left->tmpChild;
                    parent->mid->right = parent->left->right;

                    // setting left
                    parent->left->key2 = keyType(); // clear key2
                    parent->left->numOfKey = 1;
                    // move left->mid to left->right
                    parent->left->right = parent->left->mid;
                    parent->left->mid = NULL;

                    // place extra key(tmpKey) to the correct position
                    addKey(parent, parent->left->tmpKey);
                    parent->left->tmpKey = keyType();

                } // end of case 1, parent is a 2-node
                else { // numOfKey == 2, parent is a 3-node
                    addKey(parent, parent->left->tmpKey); // add the middle key into its parent
                    // move the children
                    parent->tmpChild = parent->mid;
                    parent->mid = new two3Node(parent->left->key2);
                    parent->mid->left = parent->left->tmpChild;
                    parent->mid->right = parent->left->right;

                    // children of left
                    // parent->left->left = parent->left->left
                    parent->left->right = parent->left->mid;
                    parent->left->mid = NULL;

                    parent->left->key2 = keyType(); // clear key2
                    parent->left->numOfKey = 1;


                }
                break;
            case 1:
                // numOfKey == 2, parent is a 3-node
                addKey(parent, parent->mid->tmpKey);
                parent->tmpChild = new two3Node(parent->mid->key2); // split the 3-node middle child
                // split 4-child of parent->mid to the children of the two 2-node: parent->mid, parent->tmpChild
                parent->tmpChild->left = parent->mid->tmpChild;
                parent->tmpChild->right = parent->mid->right;
                parent->mid->key2 = keyType(); // clear key2
                parent->mid->tmpChild = NULL; // reset tmpChild of middle child
                parent->mid->right = parent->mid->mid; // move mid to be right child
                parent->mid->mid = NULL; // parent->mid do not have mid now.
                // parent->mid->left do not change
                break;
            case 2:
                if (parent->numOfKey == 1) {
                    addKey(parent, parent->right->tmpKey);
                    parent->right->tmpKey = keyType(); // reset tmpKey
                    parent->right-
                    parent->mid = new two3Node(parent->right->key1);
                    parent->right->key1 = parent->right->key2;
                    parent->right->key2 = keyType(); // clear key2
                    parent->right->numOfKey = 1;
                }
                else {
                    addKey()
                }
                break;

        }
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
    }
    void destroy(two3Node* node) {
        // delete the node and its children
        if ( node != NULL ) {
            destroy(node->left);
            destroy(node->mid);
            destroy(node->right);
            delete node;
        }
    }

    // build by inserting one by one
    void build(const vector<dataType>& list) {
        unsigned long size = list.size();
        for (int i = 0; i < size; i++) {
            insert(list[i].no, list[i].sNum);
        }
    }
    void insert(int no, int numOfStu) {
        keyType newItem(no, numOfStu);
        if (root == NULL)
            root = new two3Node(newItem);
        else
            root = insertRecur(root, newItem);

        //if (root->numOfKey)
    }
    /*
    void insert(int no, int numOfStu) {
        keyType newItem(no, numOfStu);
        if (root == NULL) {
            root = new two3Node;
            root->key1 = newItem;
            root->numOfKey = 1;
            return;
        }
        vector<two3Node*> route; // from root to leaf
        two3Node* cur = root, *pre; // pre: record the parent of cur
        // locate the leaf where the item would be


        while ( cur != NULL ) {
            pre = cur;
            switch (cur->numOfKey) {
                case 2:
                    if (cur->key2.sNum < numOfStu) {
                        cur = cur->right;
                        break;
                    }
                    else if (cur->key2.sNum == numOfStu) {
                        cur->key2.noList.push_back(no);
                        return;
                    }
                    else if (cur->key1.sNum < numOfStu && numOfStu < cur->key2.sNum) {
                        cur = cur->mid;
                        break;
                    }
                case 1:
                    if (numOfStu < cur->key1.sNum)
                        cur = cur->left;
                    else if (numOfStu == cur->key1.sNum) {
                        cur->key1.noList.push_back(no);
                        return;
                    }
                    else
                        cur = cur->right;

                    break;
                default:
                    break;
            } // end of switch
        } // end of while

        // insert new item into leaf
        // if the node has only one key
        if (pre->numOfKey == 1) {
            // determine place the new item in key1 or key2
            if (numOfStu > pre->key1.sNum)
                pre->key2 = newItem;
            else {
                pre->key2 = pre->key1;
                pre->key1 = newItem;
            }
        }
        else { // two keys

        }
    }
    */

};

int main() {
    // read txt
    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    // command = { 0: quit, 1: build a max heap, 2: build a DEAP }
    two3Tree a23tree;

    while ( keepRun ) {

        cout << "\n**** Heap Construction ********"
                "\n* 0. QUIT                     *"
                "\n* 1. Build a max heap         *"
                "\n* 2. Build a DEAP             *"
                "\n* 3. Top-K maximums from DEAP *"
                "\n*******************************";
        cout << "\nInput a choice(0, 1, 2, 3): ";
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

                // output information

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
// 11127139 陳恩

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;
struct heapNode {
    int no;
    int numOfStudent;
    heapNode() {
        no = 0;
        numOfStudent = 0;
    }
    heapNode(int n, int stuNum):no(n), numOfStudent(stuNum) {}
};
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

class maxHeap {
public:
    vector<heapNode> list;
    maxHeap() {
        list.clear();
    }
    ~maxHeap() {
        list.clear();
    }
    void reset() {
        list.clear();
    }
    int empty() {
        return list.empty();
    }
    void reHeap(int index) {
        // compare index with its children, determine it is necessary to swap or not
        int left = 2*index+1, right = left+1;
        if ( left >= list.size() ) { // no child to be checked
            return;
        }

        int max = index;
        // find the largest one in those three nodes
        // compare parent with left child
        if ( list[max].numOfStudent < list[left].numOfStudent )
            max = left;
        // compare the current maximum with right child
        if ( right < list.size() && list[max].numOfStudent < list[right].numOfStudent )
            max = right;

        if ( max != index ) { // need to be swapped
            swap(list[index], list[max]);
            reHeap((index-1)/2); // reHeap down
        }

    }
    void build( const vector<dataType>& dataList ) {
        int size = dataList.size();
        for ( int i = 0; i < size; i++ ) {
            insert(dataList[i].no, dataList[i].sNum);
        }
    }
    void insert(int no, int sNum) {
        // insert a new item at the bottom of heap

        int pos = list.size(); // bottom index( position of the new item )
        heapNode newItem(no, sNum);
        list.push_back(newItem);
        int parent = (pos-1)/2;
        // reHeap up from its parent to root at most
        // if parent < child, swap
        while ( parent >= 0 && list[parent].numOfStudent < list[pos].numOfStudent ) {
            swap(list[parent], list[pos]);

            // update current index of new item and its parent
            pos = parent;
            parent = (pos-1)/2;
        }
    }
    void remove() {
        int bottom = list.size()-1;
        swap(list[0], list[bottom]);
        list.pop_back();
        reHeap(0);
    }
    void print() {
        // print out the requirements
        int bottom = list.size()-1, leftBottom = 0;
        while ( leftBottom*2+1 < list.size() )
            leftBottom = leftBottom*2 + 1;
        cout << "<max heap>";
        cout << "\nroot: [" << list[0].no << "] " << list[0].numOfStudent;
        cout << "\nbottom: [" << list[bottom].no << "] " << list[bottom].numOfStudent;
        cout << "\nleftmost bottom: [" << list[leftBottom].no << "] " << list[leftBottom].numOfStudent;
        cout << endl;
    }
};

class DEAP {
private:
    vector<dataType> heap;
    static bool leftOrRight(int pos) {
        // return true when the pos is in the min heap
        // otherwise, return false
        int left = 1, right = 1;
        while (right < pos ) {
            left = left*2+1;
            right = 2*right+2;
        }
        if (left <= pos && pos <= right ) // on the left subtree
            return true;
        else // on the right subtree
            return false;
    }

    int reHeapDown(int pos, bool side) {
        int left = 2*pos+1, right = left+1;
        int size = heap.size();
        if ( right > size ) // if right == heap.size(), left is available
            return pos;

        if (side) { // it is on the left side
            int min = pos;
            // choose the minimum to be swapped
            if (heap[left].sNum < heap[min].sNum)
                min = left;
            if (right < size && heap[right].sNum < heap[min].sNum)
                min = right;
            if (min != pos) {
                swap(heap[min], heap[pos]);
                return reHeapDown(min, side);
            }
        }
        else {
            int max = pos;
            if (heap[left].sNum > heap[max].sNum)
                max = left;
            if (right < size && heap[right].sNum > heap[max].sNum)
                max = right;
            if (max != pos) {
                swap(heap[max], heap[pos]);
                return reHeapDown(max, side);
            }
        }
        return pos;
    }
    void reHeapUp(int pos, bool side) {
        // reHeap up from the parent of pos to the root of subtree at most
        int parent = (pos-1)/2;
        if (side) { // min heap
            while (parent > 0 && heap[parent].sNum > heap[pos].sNum) {
                swap(heap[parent], heap[pos]);
                pos = parent;
                parent = (pos-1)/2;
            }
        }
        else {
            while (parent > 0 && heap[parent].sNum < heap[pos].sNum) {
                swap(heap[parent], heap[pos]);
                pos = parent;
                parent = (pos-1)/2;
            }
        }
    };
    int getLevel(int pos) {
        int h = 1;
        int left = 1; // the leftmost node of each level
        while (left <= pos) {
            h++;
            left = 2*left+1;
        }
        return h;
    }
    int getNum(int level) {
        // get the maximum number of nodes at level-th level
        int nums = 1;
        while (level > 1) {
            nums*=2;
            level--;
        }
        return nums;
    }
    int getCorrespond(int index) {
        /*
         * return the correspond node on the same level but other heap
         * if index on the min-heap, return the pos of correspond node on max-heap
         * no matter the pos is available or not.
         */
        int level = getLevel(index);
        int leftmost = 0;
        // get the first node index of the level
        for ( int i = 1; i < level; i++ ) {
            leftmost = leftmost*2+1;
        }
        int numOfNode = getNum(level);
        if (index < numOfNode/2+leftmost) { // index is on the left side
            return index+numOfNode/2; // correspond node on the max-heap
        }
        else
            return index - numOfNode/2;

    }
public:
    DEAP() {
        reset();
    }
    ~DEAP() {
        reset();
    }
    void reset() {
        heap.clear();
        dataType dummy;
        heap.push_back(dummy); // dummy root
    }
    void build(const vector<dataType>& aList) {
        int size = aList.size();
        for (int i = 0; i < size; i++) {
            insert(aList[i]);
        }
    }
    int exchange(int pos) {
        // return -1 when nothing change
        // otherwise, return the new index of previous heap[pos]
        if (pos < 1) // no correspond node
            return -1;
        // get the correspond node of pos
        bool side = leftOrRight(pos);

        int coNode = getCorrespond(pos);
        if (side) {
            // case1 correspond position available
            if (coNode < heap.size()) {
                if (heap[coNode].sNum < heap[pos].sNum) {
                    swap(heap[coNode], heap[pos]);
                    return coNode;
                }
            } // end of case1
            else { // case2 correspond node does not exist but its parent does
                int parent =  (coNode-1)/2; // parent of coNode
                if (parent > 0 && heap[parent].sNum < heap[pos].sNum) {
                    swap(heap[parent], heap[pos]);
                    return parent;
                }
            } // end of case2
        } // end of left side check
        else { // pos is on the right side (max-heap)
            // check its correspond node and the children of coNode
            int max = pos;
            if (heap[coNode].sNum > heap[pos].sNum) {
                max = coNode;
            }
            else { // check the children of coNode
                int left = coNode * 2 + 1, right = left + 1; // children of coNode
                int size = heap.size();
                // get the largest one
                if (left < size && heap[max].sNum < heap[left].sNum)
                    max = left;
                if (right < size && heap[max].sNum < heap[right].sNum)
                    max = right;

            }
            // swap if needed
            if (max != pos) {
                swap(heap[max], heap[pos]);
                return max;
            }
        } // end of right side check

        return -1;
    }
    void insert(const dataType& newItem) {
        int pos = heap.size(); // position of the newItem at which is inserted
        // append newItem on the bottom
        heap.push_back(newItem);

        // check corresponding node
        int newPos = exchange(pos);
        if ( newPos == -1 )  // position did not change
            newPos = pos;

        bool side = leftOrRight(newPos); // left side: true, right side: false
        reHeapUp(newPos, side);
    } // end of insert
    dataType removeMax() {
        int bottom = heap.size()-1;
        dataType max;
        if ( bottom == 1 ) {
            max = heap[1];
            heap.pop_back();
        }
        else {
            swap(heap[2], heap[bottom]);
            max = heap[bottom];
            bottom-=1;
            heap.pop_back();

            int pos = 2;// the node on the root
            pos = reHeapDown(pos, false); // get current position of the node
            exchange(pos); // check with correspond node
            //if ( pos*2+1 > bottom ) // if node is a leaf
                //exchange(pos); // check with correspond node
        }
        return max;
    }
    void getTop() {
        vector<dataType> ansList;
        int num;
        int size = heap.size()-1;
        cout << "\nEnter the value of K in [1," << heap.size()-1 << "]: ";
        cin >> num;
        if (num > size)
            return;
        for ( int i = 0; i < num; i++ ) {
            ansList.push_back(removeMax());
        }
        for (int i = 0; i < num; i++) {
            cout << "\nTop  " << setw(2) << i+1 << ": [" << ansList[i].no << "] " << ansList[i].sName;
            cout << ansList[i].dName << ", " <<ansList[i].type << ", " << ansList[i].degree << ", " << ansList[i].sNum;
        }

    }

    void print() {
        int left = 1;
        int size = heap.size();
        while ( 2*left+1 < size ) {
            left = 2*left+1;
        }
        cout << "<DEAP>";
        cout << "\nbottom: [" << heap[size-1].no << "] " << heap[size-1].sNum;
        cout << "\nleftmost bottom: [" << heap[left].no << "] " << heap[left].sNum;
        cout << endl;
    }
};

int main() {
    // read txt
    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    // command = { 0: quit, 1: build a max heap, 2: build a DEAP }
    maxHeap aMaxHeap;
    DEAP aDEAP;

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
                aMaxHeap.reset();
                // build head when the file being read successfully
                aMaxHeap.build(aFile.getList());

                // output information
                aMaxHeap.print();
                break;
            case 2:
                aFile.reset();
                if ( !aFile.readFile() )
                    break;
                aDEAP.reset();
                aDEAP.build(aFile.getList());
                aDEAP.print();
                break;
            case 3:
                if (aFile.isEmpty())
                    break;
                aDEAP.getTop();
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        }
    }
}
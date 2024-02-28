// 11127139 陳恩

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
struct dataType {
    int no;
    int numOfStudent;
    dataType() {
        no = 0;
        numOfStudent = 0;
    }
};

class inputFile {
private:
    string fileID;
    vector<dataType> inputData;
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
    vector<dataType> getList() {
        return inputData;
    }
    bool readFile(){
        fstream inFile;
        string fileName;
        this->reset();
        cout << endl << "Input a file number [0: quit]: ";
        cin >> this->fileID;
        if ( fileID == "0" ) {
            reset();
            return true;
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
                    if (++fNo == 7) {
                        oneR.numOfStudent = atoi(cut.c_str());
                    }
                    pre = ++pos;
                } while ((pos > 0) && (fNo < 7));
                this->inputData.push_back(oneR); } //end outer-while inFile.close();
        } // end else
        if ( this->inputData.empty() ){
            cout << endl << "### Get nothing from the file " << fileName << " ! ### " << endl;
            return false;
        } // end if
        return true;
    } // end of readFile
};

class maxHeap {
public:
    vector<dataType> list;
    maxHeap() {
        list.clear();
    }
    maxHeap(const vector<dataType>& aList): list(aList) {};
    void reset() {
        list.clear();
    }
    int getSize() {
        return this->list.size();
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
            insert(dataList[i]);
        }
    }
    void insert(dataType newItem) {
        // insert a new item at the bottom of heap

        int pos = list.size(); // bottom index( position of the new item )
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
        cout << "\n<max heap>";
        cout << "\nroot: [" << list[0].no << "] " << list[0].numOfStudent;
        cout << "\nbottom: [" << list[bottom].no << "] " << list[bottom].numOfStudent;
        cout << "\nleftmost bottom: [" << list[leftBottom].no << "] " << list[leftBottom].numOfStudent;
    }
};
class minHeap{
public:
    vector<dataType> list;
    minHeap() {
        list.clear();
    }
    void reset() {
        list.clear();
    }
    void reHeap(int index) {
        int left = 2*index+1, right = left+1;
        if ( left >= list.size() ) { // no child to be checked
            return;
        }

        int min = index;
        // find the smallest one in those three nodes
        // compare parent with left child
        if ( list[min].numOfStudent > list[left].numOfStudent )
            min = left;
        if ( right < list.size() && list[min].numOfStudent > list[right].numOfStudent )
            min = right;
        if ( min != index ) { // need to be swapped
            swap(list[index], list[min]);
            reHeap((index-1)/2); // reHeap down
        }
    }
    void insert(dataType newItem) {
        // insert a new item at the bottom of heap

        int pos = list.size(); // bottom index( position of the new item )
        list.push_back(newItem);
        int parent = (pos-1)/2;
        // reHeap up from its parent to root at most
        // if parent > child, swap
        while ( parent >= 0 && list[parent].numOfStudent > list[pos].numOfStudent ) {
            swap(list[parent], list[pos]);

            // update current index of new item and its parent
            pos = parent;
            parent = (pos-1)/2;
        }
    }
    int getSize() {
        return this->list.size();
    }
    int empty() {
        return list.empty();
    }
};

class DEAP{
private:
    minHeap leftHeap;
    maxHeap rightHeap;
    int size;
    bool leftOrRight(int index) {
        // return true when the index is in the min heap
        // otherwise, return false
        int left = 1, right = 1;
        while ( right < index ) {
            left = left*2+1;
            right = 2*right+2;
        }
        if ( left <= index && index <= right )
            return true;
         else
             return false;
    }
    int exchange(int pos, bool leftOrNot) {
        // compare left[pos] with right[pos] if right[pos] exist
        if ( leftOrNot ) { // check left[pos] with right[(pos-1)/2] (the parent)
            int parent = (pos-1)/2;
            if ( parent < rightHeap.getSize()&& leftHeap.list[pos].numOfStudent > rightHeap.list[parent].numOfStudent) {
                swap(leftHeap.list[pos], rightHeap.list[parent]);
                return parent;
            }
        }
        else {
            if (leftHeap.list[pos].numOfStudent > rightHeap.list[pos].numOfStudent) {
                swap(leftHeap.list[pos], rightHeap.list[pos]);
                return pos;
            }
        }
        return -1;
    }
public:
    DEAP() {
        reset();
    }
    ~DEAP() {
        reset();
    }
    void reset() {
        leftHeap.reset();
        rightHeap.reset();
        size = 1; // dummy root
    }
    void build(const vector<dataType>& aList) {
        for ( int i = 0; i < aList.size(); i++ ) {
            insert(aList[i]);
        }
    }
    void insert(const dataType& newItem) {
        // determine it is in min or max heap ( left or right )
        int pos = this->size;
        bool left = leftOrRight(pos);
        if ( left ) { // if it is on the left side
            leftHeap.list.push_back(newItem);
            // check the correspond node on other side
            pos = exchange(leftHeap.getSize()-1, left);
            if ( pos == -1 ) {
                pos = leftHeap.getSize()-1;
                left = true;
            }
            else
                left = false;
        }
        else {
            rightHeap.list.push_back(newItem);
            // check
            pos = exchange(rightHeap.getSize()-1, left);
            if ( pos == -1 ) {
                pos = rightHeap.getSize()-1;
                left = false;
            }
            else
                left = true;
        }
        // reHeap up from current position
        int parent = (pos-1)/2;
        if (left) {
            while (pos > 0 && leftHeap.list[pos].numOfStudent < leftHeap.list[parent].numOfStudent) {
                swap(leftHeap.list[pos], leftHeap.list[parent]);
                pos = parent;
                parent = (pos-1)/2;
            }
        }
        else {
            while (pos > 0 && rightHeap.list[parent].numOfStudent < rightHeap.list[parent].numOfStudent) {
                swap(rightHeap.list[parent], rightHeap.list[pos]);
                pos = parent;
                parent = (pos-1)/2;
            }
        }

        this->size++;
    }

    void print() {
        int left = 0;
        while ( 2*left+1 < leftHeap.getSize() ) {
            left = 2*left+1;
        }
        dataType bottom;
        if (leftOrRight( size-1 ))
            bottom = leftHeap.list[leftHeap.getSize()-1];
        else
            bottom = rightHeap.list[rightHeap.getSize()-1];
        cout << "\n<DEAP>";
        cout << "\nbottom: [" << bottom.no << "] " << bottom.numOfStudent;
        cout << "\nleftmost bottom: [" << leftHeap.list[left].no << "] " << leftHeap.list[left].numOfStudent;
    }
    void printAllLeft() {
        for ( int i = 0; i < leftHeap.getSize(); i++ ) {
            cout << "\n" << i+1 << "\t[" << leftHeap.list[i].no << "] " << leftHeap.list[i].numOfStudent;
        }
    }
};

int main() {
    // read txt
    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    // command { 0: quit, 1: build a max heap, 2: build a DEAP }
    while ( keepRun ) {
        maxHeap aMaxHeap;
        DEAP aDEAP;
        cout << "\nInput a choice(0, 1, 2):";
        cin >> command;
        switch ( command ) {
            case 0:
                keepRun = false;
                break;
            case 1:
                // check if the file had been read successfully
                if ( !aFile.readFile() ) {
                    break;
                }
                // build head when the file being read successfully
                aMaxHeap.build(aFile.getList());

                // output information
                aMaxHeap.print();
                break;
            case 2:
                if ( !aFile.readFile() ) {
                    break;
                }
                aDEAP.build(aFile.getList());
                aDEAP.printAllLeft();
                aDEAP.print();
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        }
    }
}
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
private:
    vector<dataType> list;

public:
    maxHeap() {
        list.clear();
    }
    maxHeap(const vector<dataType>& aList): list(aList) {};
    void reset() {
        list.clear();
    }
    int getSize() {
        return list.size();
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
            this->reHeap((index-1)/2); // reHeap down
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
class minHeap: public maxHeap {
private:
    vector<dataType> list;
public:
    minHeap() {
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
        while ( parent >= 0 && list[parent].numOfStudent > list[pos].numOfStudent ) {
            swap(list[parent], list[pos]);

            // update current index of new item and its parent
            pos = parent;
            parent = (pos-1)/2;
        }
    }
};

class DEAP{
private:
    vector<dataType> list;

    bool leftOrRight(int index) {
        // return true when the index is in the min heap
        // otherwise, return false
        int left = 1, right = 1;
        while ( right < index ) {
            left = left*2+1;
            right = 2*right+2;
            if ( left <= index && index <= right )
                return true;
        }
        return false;
    }
public:
    DEAP() {
        list.clear();
        dataType dummy;
        list.push_back( dummy ); // insert a root
    }
    ~DEAP() {
        reset();
    }
    void reset() {
        list.clear();
        dataType dummy;
        list.push_back( dummy ); // insert a root
    }
    void insert(const dataType& newItem) {
        // determine it is in min or max heap ( left or right )

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
                // not done yet
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        }
    }
}
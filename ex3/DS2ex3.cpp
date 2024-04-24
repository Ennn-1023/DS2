// 11127130 ½²²E¥þ 11127139 ³¯®¦
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // using strcpy()
using namespace std;
struct DataType {
    char sID[10] = {0}; // student id
    char sName[10]; // student name
    unsigned char score[6] = {0}; // six different scores
    float avgScore = 0.0; // average score
};

class inputFile {
private:
    string fileID;
    vector<DataType> inputData;
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
    vector<DataType> getList() {
        return inputData;
    }
    bool isEmpty() {
        // check if there is containing a fileID
        // if not, it is empty
        return fileID.empty();
    }
    bool getBinFile() {
        fstream inFile;
        string fileName = "input" + fileID + ".bin";
        inFile.open(fileName.c_str(), fstream::in | fstream::binary);
        if (!inFile.is_open()) {
            cout << endl << "### " << fileName << " does not exist! ###" << endl;
            return false;
        }
        else { // open successfully
            inFile.seekg(0, fstream::end); // move file pointer to the end of file
            DataType oneR;
            long long fileSize = inFile.tellg(); // get last char position
            int totalRow = fileSize / sizeof(oneR); // count how many rows do the file have
            inFile.seekg(0, fstream::beg); // move file pointer to the begin and start to read
            for (int i = 0; i < totalRow; i++) { // read rows till reach the end
                inFile.read((char *)&oneR, sizeof(oneR)); // read one row
                this->inputData.push_back(oneR); // store in list
            }
            return true;
        }
    }
    bool getTextFile() {
        fstream inFile, binFile; // get input from inFile and write output into binFile
        string fileName = "input" + fileID;
        inFile.open((fileName + ".txt").c_str(), fstream::in | fstream::binary);
        if (inFile.fail()) { // check if it is not opened successfully
            cout << endl << "### " << fileName + ".txt" << " does not exist! ###" << endl;
            return false;
        }

        // create binary file
        binFile.open((fileName + ".bin").c_str(), fstream::out|fstream::binary);
        if (! binFile.is_open())
            return false; // failed to open/create bin file
        else { // read from txt file
            char cstr[255];
            int fNo, pre, pos;
            while (inFile.getline(cstr, 255, '\n')) {
                DataType oneR;
                string buf, cut;
                fNo = 0;
                pre = 0;
                buf.assign(cstr);
                do {
                    pos = buf.find_first_of('\t', pre);
                    cut = buf.substr(pre, pos - pre);
                    switch (++fNo) {
                        case 1:
                            strcpy(oneR.sID, cut.c_str()); break;
                        case 2:
                            strcpy(oneR.sName, cut.c_str()); break;
                        case 3:
                            oneR.score[0] = toInt(cut); break;
                        case 4:
                            oneR.score[1] = toInt(cut); break;
                        case 5:
                            oneR.score[2] = toInt(cut); break;
                        case 6:
                            oneR.score[3] = toInt(cut); break;
                        case 7:
                            oneR.score[4] = toInt(cut); break;
                        case 8:
                            oneR.score[5] = toInt(cut); break;
                        case 9:
                            oneR.avgScore = stof(cut); break;
                    }
                    pre = ++pos;
                } while ((pos > 0) && (fNo < 9));
                binFile.write((char *) &oneR, sizeof(oneR)); // write into binary file
                this->inputData.push_back(oneR); // store data in list
            } //end outer-while
            // close the files
            inFile.close();
            binFile.close();
            return true;
        }
    }

    bool readFile() {
        fstream inFile;
        this->reset();
        cout << endl << "Input a file number [0: quit]: ";
        cin >> this->fileID;
        if ( fileID == "0" ) { // check if is 0
            reset();
            return false;
        }
        else if (getBinFile()) // try to get binary file first
            return true;
        else if (getTextFile())  // try to get txt file and write binary file
            return true;
        else { // failed to get file
            reset(); // reset fileID
            return false;
        }
    } // end of readFile
    void printAll() {
        for (auto oneR:inputData) {
            cout << endl << oneR.sID << "\t" << oneR.sName << "\t";
            for (int i = 0; i < 6; i++)
                printf("%d\t", oneR.score[i]);
            cout << oneR.avgScore;
        }
    }
};



class HashTable {
private:
    struct HashItemType {
        string sID; // key
        string sName;
        int avgScore = 0;
        int hashValue = 0;
        HashItemType() = default;
        HashItemType(const DataType& data):sID(data.sID), sName(data.sName), avgScore(data.avgScore) {}
    };
    HashItemType *list;
    int size;
    static bool isPrime(int num) {
        // given a positive integer, check if there is any factor of num
        // return true when no factor been found
        // otherwise, return false
        if (num <= 1)
            return false;
        if (num < 4) // num == 2 or 3
            return true;
        else if (num % 2 == 0)
            return false;
        else {
            for (int factor = 3; factor*factor <= num; factor+=2) { // check all odd integer between 2 and sqrt(num)
                if (num % factor == 0)
                    return false;
            }
            return true;
        }
    }
    static int getNextPrime(int num) {
        // get the next prime larger than num

        int i = num+1; // only check number > num
        if (i% 2 == 0) // is a even number
            i++;
        // only check the odd numbers
        while(true) {
            if (isPrime(i))
                return i;
        }
    }

    int hash(string& key) {
        int value = 0;
        for (char ch:key) {

        }
    }
    void allocTable(int numOfData) {
        // allocate memory space for hash table
        // and set up data member: size.
        this->size = getNextPrime(numOfData*1.15);
        list = new HashItemType[this->size];
    }
public:
    HashTable() {
        size = 0;
        list = NULL;
    }
    ~HashTable() {
        delete[] list;
    }

    void reset() {
        delete [] list;
        size = 0;
    }

    void build(const vector<DataType>& inputList) {
        // allocate memory space
        allocTable(inputList.size());
        // insert one by one
        for (auto data:inputList) {
            insert(data);
        }
    }
    void insert(const DataType& item) {
        HashItemType value(item);
        value.hashValue = hash(value.sID); // set up hash value of the new item

    }
};


int main() {

    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    HashTable quadraticTable;

    while ( keepRun ) {
        cout << "\n******* Hash Table *****"
                "\n* 0. QUIT              *"
                "\n* 1. Quadratic probing *"
                "\n* 2. Double hashing    *"
                "\n************************";
        cout << "\nInput a choice(0, 1, 2): ";
        cin >> command;
        switch ( command ) {
            case 0:
                keepRun = false;
                break;
            case 1:
                // check if the file had been read successfully
                if ( !aFile.readFile()) {
                    break;
                }
                quadraticTable.build(aFile.getList());
                // aFile.printAll();

                break;
            case 2:


                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        }
    }
}
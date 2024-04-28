// 11127130 ½²²E¥þ 11127139 ³¯®¦
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcpy()
#include <iomanip> // std::setprecision()
using namespace std;
struct DataType { // input data type
    char sID[10] = {0}; // student id
    char sName[10]; // student name
    unsigned char score[6] = {0}; // six different scores
    float avgScore = 0.0; // average score
};
struct HashItemType { // hash table item type
    int hashValue = 0;
    string sID; // key
    string sName;
    float avgScore = 0;
    bool empty = true; // keep whether the space is empty
    HashItemType() = default;
    HashItemType(const DataType& data):sID(data.sID), sName(data.sName), avgScore(data.avgScore), empty(false) {}
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

    void writeTable(const vector<HashItemType>& list, const string& type) {
        // type: quadratic or double
        string fileName = type+fileID+".txt";
        fstream file;
        file.open(fileName.c_str(), fstream::out);
        if (file.is_open()) { // write file
            string oneRow;
            if (type == "quadratic")
                oneRow = "--- Hash table created by Quadratic probing ---";
            else
                oneRow = "--- Hash table created by Double hashing ---";
            file << oneRow;
            for (int i = 0; i < list.size(); i++) {
                oneRow.clear();
                if (list[i].empty) {
                    file << endl << "[" << i << "] ";
                }
                else {
                    file << endl << "[" << i << "] " << list[i].hashValue << ", " << list[i].sID << ", " << list[i].sName;
                    file << ", " << list[i].avgScore;
                }
            }
        }
    }
};

class QuadraticHash {
private:
    // data members
    vector<HashItemType> list;
    int size;
    int successNum;
    int unsuccessNum;
    // private member function
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
            else
                i+=2;
        }
    }

    int hash(string& key) {
        long long value = 1;
        for (char ch:key) { // times each char together
            value *= ch;
        }
        return value%this->size;
    }
    int step(int times) {
        // according to the step times to return how much step should be done
        // when time = 1, return 1
        return times*times;
    }

    void allocTable(int numOfData) {
        // allocate memory space for hash table
        // and set up data member: size.
        this->size = getNextPrime(numOfData*1.15);
        list.resize(this->size);
    }
public:
    QuadraticHash() {
        size = 0;
        list.clear();
        successNum = 0;
        unsuccessNum = 0;
    }
    ~QuadraticHash() {
        list.clear();
    }

    void reset() {
        list.clear();
        size = 0;
        successNum = 0;
        unsuccessNum = 0;
    }

    void build(const vector<DataType>& inputList) {
        // allocate memory space
        allocTable(inputList.size());
        // insert one by one
        for (auto data:inputList) {
            insert(data);
        }
        cout << endl << "Hash table has been successfully created by Quadratic probing";
    }
    void insert(const DataType& item) {
        // insert item into hash table by using sID as key
        // add up total search time which the successful searches will cost
        HashItemType value(item);
        value.hashValue = hash(value.sID); // set up hash value of the new item

        int stepTimes = 0;
        unsigned int loc = value.hashValue;
        // find a empty location by quadratic probing
        while (!list[loc].empty) {
            stepTimes++; // move to next location
            loc = value.hashValue + step(stepTimes); // loc added by a quadratic number
            loc %= this->size;
        } // end of probing
        // set up success search times
        this->successNum += stepTimes+1;
        // put into the list
        list[loc] = value;
    }
    vector<HashItemType> outputTable() {
        return list;
    }
    void nonexistSearch() {
        // simulate how many probing is needed totally
        // set up the total search time
        // simulation of each hash value does not exist in the table
        for (int hValue = 0; hValue < this->size; hValue++) {
            int times = 0; // how many probing occur during retrieving one value
            int loc = hValue;
            while (!list[loc].empty) {
                times++;
                loc = hValue + step(times);
                loc %= this->size;
            }
            this->unsuccessNum += times;
        } // end of simulate
    }
    void mission1Print(int dataNum) {
        float avgSuccess, avgUnsuccess;
        nonexistSearch();
        avgUnsuccess = (float)unsuccessNum/ this->size;
        avgSuccess = (float)successNum / dataNum;
        cout << endl << std::fixed << std::setprecision(4) << "unsuccessful search: ";
        cout << avgUnsuccess << " comparisons on average";
        cout << endl << "successful search: " << avgSuccess << " comparisons on average";
        cout << endl;
    }
};


int main() {

    inputFile aFile;
    bool keepRun = true;
    int command = -1;
    QuadraticHash quadraticTable;

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
                // read file first
                aFile.reset();
                if ( !aFile.readFile()) {
                    break;
                }
                quadraticTable.reset();
                quadraticTable.build(aFile.getList());
                aFile.writeTable(quadraticTable.outputTable(), "quadratic");
                quadraticTable.mission1Print(aFile.getList().size());
                break;
            case 2:
                // check if the file had been read successfully
                if (aFile.isEmpty()) {
                    cout << "### Command 1 first. ###\n";
                }
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        } // end of switch

    }
}
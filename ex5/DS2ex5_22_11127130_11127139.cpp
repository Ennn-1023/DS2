// 11127130 蔡淳全 11127139 陳恩
#include <iostream>
#include <vector>
#include <fstream>

#include <algorithm> // sort()
#include <chrono>
#include <cstdio>
using namespace std;

#define SIZE 300

struct DataType { // input data type
    char putID[10] = {0}; // 發訊者學號
    char getID[10] = {0}; // 收訊者學號
    float weight = 0.0; // average score
};

class ExternalSort {
private:
    string fileID;
    vector<DataType> buffer;
public:
    ExternalSort() {
        reset();
    }
    ~ExternalSort() {
        reset();
    }
    void reset() {
        fileID.clear();
    }
    bool getID() {
        // get input file ID, check if it exists
        // if valid, store in data member:　fileID
        string fID;
        // checking input fileID
        do {
            cout << "\nInput the file name: [0]Quit\n";
            cin >> fID;
            if (fID == "0")
                return false;
        } while (!checkFIle(fID));
        // store the valid fileID
        fileID = fID;
        return true;
    }
    bool checkFIle(const string& fID) {
        // check the data member: fileID exists or not
        string fileName = "pairs" + fID + ".bin";
        ifstream aFile(fileName.c_str(), ios::binary);
        // check if the file can be opened or not
        bool isOpen = aFile.is_open();
        aFile.close(); // close the file
        if (!isOpen)
            cout << "\npairs500.bin does not exist!!!\n";
        return isOpen;
    }

    static bool compareFunc(const DataType& o1, const DataType& o2) {
        return o1.weight > o2.weight;
    }

    int partialSort() {
        // sort the input file by at most 300 rows
        // open file
        ifstream inFile(("pairs" + fileID + ".bin").c_str(), ios::binary);
        ofstream outFile;
        int runs = 0; // record how many runs being produced
        DataType oneR;
        int size = sizeof(oneR);
        while (true) {
            int i = 0;
            buffer.clear();
            while (i < SIZE && inFile.read((char*)&oneR, size)) {
                buffer.push_back(oneR);
                i++;
            }
            if (buffer.empty()) // check if there is no data to be read
                break;
            // using stable sorting algorithm
            stable_sort(buffer.begin(), buffer.end(), compareFunc);
            runs++;
            // write results back to a bin file;
            writeTmpFile("tmp0_" + to_string(runs) + ".bin");
        }
        inFile.close();
        return runs;
    }
    void writeTmpFile(const string& fName) {
        // write buffer into file
        ofstream outFile;
        int size = sizeof(DataType);
        outFile.open(fName.c_str(), ios::binary);
        for (DataType& oneR: buffer) {
            outFile.write((char*)&oneR, size);
        }
        outFile.close();
    }
    void mergeAll(int totalRuns) {
        int round = 0;
        // calling mergeFiles do file merging and count rounds, total runs
        while (totalRuns > 1) {
            cout << "\nNow there are "<< totalRuns << " runs.\n";
            round++;
            mergeFiles(totalRuns, round);
            totalRuns = totalRuns%2 + totalRuns/2;
        }
        rename(("tmp"+ to_string(round)+ "_1.bin").c_str()
                , ("order"+ fileID + ".bin").c_str());
        cout << "\nNow there are "<< totalRuns << " runs.\n";
    }
    void mergeFiles(int runs, int round) {
        // merge two sorted file each time till there is no more than one file

        DataType row1, row2;
        int no;
        for (no = 1; no < runs; no+=2) {
            // for each round, merge no and no+1 if no+1 <= runs
            string inFile1 = "tmp"+ to_string(round-1) + "_"+ to_string(no) + ".bin";
            string inFile2 = "tmp"+ to_string(round-1) + "_"+ to_string(no+1) + ".bin";
            string outFile = "tmp"+ to_string(round) + "_" + to_string(no/2 +1) + ".bin";
            mergePair(inFile1, inFile2, outFile);
        }
        if (no == runs) { // odd number, last file without pair to be merged
            // rename the file with no pair to merge for next round
            rename( ("tmp"+ to_string(round-1) + "_"+ to_string(no) + ".bin").c_str()
                    ,("tmp"+ to_string(round) + "_" + to_string(no/2 +1) + ".bin").c_str());
        }
    }
    void mergePair(const string& inFileName1, const string& inFileName2, const string& outFileName) {
        ifstream inFile1, inFile2;
        ofstream outFile;
        inFile1.open(inFileName1.c_str(), ios::binary);
        inFile2.open(inFileName2.c_str(), ios::binary);
        outFile.open(outFileName.c_str(), ios::binary);
        // compare the largest weight and put it into outFile
        vector<DataType> tmp1, tmp2;
        DataType row;
        int size = sizeof(row);
        while ( !inFile1.eof() && !inFile2.eof()) {
            // get row from file
            if (tmp1.empty()) {
                inFile1.read((char*) &row, size);
                tmp1.push_back(row);
            }
            if (tmp2.empty()) {
                inFile2.read((char*) &row, size);
                tmp2.push_back(row);
            }

            // compare and write file
            if (tmp1[0].weight >= tmp2[0].weight) {
                row = tmp1[0];
                tmp1.pop_back();
            }
            else {
                row = tmp2[0];
                tmp2.pop_back();
            }
            outFile.write((char*)&row, size);
        }
        // one of the file is empty now
        if (tmp1.empty()) { // inFile1 is empty
            outFile.write((char*)&tmp2[0], size);
            while (!inFile2.eof()) {
                inFile2.read((char*)&row, size);
                outFile.write((char*)&row, size);
            }
        }
        else {
            outFile.write((char*)&tmp1[0], size);
            while (!inFile1.eof()) {
                inFile1.read((char*)&row, size);
                outFile.write((char*)&row, size);
            }
        }
        inFile1.close();
        inFile2.close();
        // remove previous file
        remove(inFileName1.c_str());
        remove(inFileName2.c_str());
        outFile.close();
    }

};



int main() {
    ExternalSort sorter;

    bool keepRun = true;
    int fileID;
    chrono::time_point<chrono::system_clock> inStart, inEnd;
    chrono::time_point<chrono::system_clock> exStart, exEnd;
    chrono::microseconds inTime, exTime;
    // srand(time(NULL)); // setting random seed
    while ( keepRun ) {
        cout << "\n*** The buffer size is 300"
                "\n***********************************************"
                "\n On-machine Exercise 05                       *"
                "\n Mission 1: External Merge Sort on a Big File *"
                "\n Mission 2: Construction of Primary Index     *"
                "\n***********************************************";
        // try to get input file number first
        if (sorter.getID()) {
            inStart = chrono::system_clock::now(); // internal start time point
            int totalRuns = sorter.partialSort(); // sort by maxSize = 300
            inEnd = chrono::system_clock::now(); // internal sort end time point
            cout << "\nThe internal sort is completed. Check the initial sorted runs!\n";
            exStart = chrono::system_clock::now(); // external start time point
            sorter.mergeAll(totalRuns); // merge all file into a order#.bin
            exEnd = chrono::system_clock::now(); // external end time point

            // output time information
            inTime = chrono::duration_cast<chrono::microseconds>(inEnd - inStart);
            exTime = chrono::duration_cast<chrono::microseconds>(exEnd - exStart);
            cout << "\nThe execution time ...";
            cout << "\nInternal Sort = " << inTime.count()/1000.0 << " ms";
            cout << "\nExternal Sort = " << exTime.count()/1000.0 << " ms";
            cout << "\nTotal Execution Time = " << (inTime.count()+exTime.count())/1000.0 << endl;
        }

        // ask user quit or not
        cout << "\n[0]Quit or [Any other key]continue?\n";
        string keyInValue;
        cin >> keyInValue;

        if (keyInValue == "0")
            keepRun = false;
    }
}
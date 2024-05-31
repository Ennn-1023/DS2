// 11127130 蔡淳全 11127139 陳恩
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
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
    vector<pair<float, int>> indexArr;
public:
    ExternalSort() {
        reset();
    }
    ~ExternalSort() {
        reset();
    }
    void reset() {
        fileID.clear();
        indexArr.clear();
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
        ifstream aFile(fileName, ios::binary);
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
        ifstream inFile(("pairs" + fileID + ".bin"), ios::binary);
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
        outFile.open(fName, ios::binary);

        outFile.write((char*)buffer.data(), size*buffer.size());
        buffer.clear();
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

        int no;
        for (no = 1; no < runs; no+=2) {
            // for each round, merge no and no+1 if no+1 <= runs
            char inFile1[20], inFile2[20], outFile[20];
            sprintf(inFile1, "tmp%d_%d.bin", round - 1, no);
            sprintf(inFile2, "tmp%d_%d.bin", round-1, no+1);
            if (runs == 2) {
                sprintf(outFile, "order%s.bin", fileID.c_str());
            }
            else
             sprintf(outFile, "tmp%d_%d.bin", round, no/2 +1);
            mergePair(inFile1, inFile2, outFile);
        }
        if (no == runs) { // odd number, last file without pair to be merged
            // rename the file with no pair to merge for next round
            char inFile_name[20], newName[20];
            sprintf(inFile_name, "tmp%d_%d.bin", round - 1, no);
            sprintf(newName, "tmp%d_%d.bin", round, no/2 +1);
            rename( inFile_name, newName);
        }
    }
    void mergePair(char* inFileName1, char* inFileName2, char* outFileName) {
        ifstream inFile1, inFile2;
        ofstream outFile;
        inFile1.open(inFileName1, ios::binary);
        inFile2.open(inFileName2, ios::binary);
        outFile.open(outFileName, ios::binary);
        // compare the largest weight and put it into outFile
        DataType tmp1, tmp2;
        int size = sizeof(DataType);
        inFile1.read((char*) &tmp1, size);
        inFile2.read((char*) &tmp2, size);
        bool file1Empty = false;
        bool file2Empty = false;
        while (true) {

            if (tmp1.weight >= tmp2.weight) {
                outFile.write((char*)&tmp1, size);
                // get row from file
                if (!inFile1.read((char*)&tmp1, size)) {
                    file1Empty = true;
                    break;
                }
            }
            else {
                outFile.write((char*)&tmp2, size);
                // get row from file
                if (!inFile2.read((char*)&tmp2, size)) {
                    file2Empty = true;
                    break;
                }
            }
        }
        // one of the file is empty now
        if (file1Empty) { // inFile1 is empty
            outFile.write((char*)&tmp2, size);
            while (inFile2.read((char*)&tmp2, size)) {
                outFile.write((char*)&tmp2, size);
            }
        }
        else {
            outFile.write((char*)&tmp1, size);
            while (inFile1.read((char*)&tmp1, size)) {
                outFile.write((char*)&tmp1, size);
            }
        }
        inFile1.close();
        inFile2.close();
        // remove previous file
        remove(inFileName1);
        remove(inFileName2);
        outFile.close();
    }
    void getIndexArray() {
        // read sorted file from memory
        ifstream inFile("order"+fileID+".bin", ios::binary);
        int len = sizeof(DataType);
        int iteration = 0;
        float lastWeight = 1.1;
        int offset = -1;
        while(true) {
            buffer.resize(SIZE); // read at most 300 each time
            inFile.read((char*)buffer.data(), SIZE*len);
            int bytesRead = inFile.gcount();
            if (bytesRead == 0) {
                break;
            }
            buffer.resize(bytesRead/len);
            for (int i = 0; i < SIZE; i++) {
                // check each key
                if (buffer[i].weight < lastWeight) {
                    lastWeight = buffer[i].weight;
                    offset = i + iteration*SIZE;
                    pair<float,int> tmp(lastWeight, offset);
                    indexArr.push_back(tmp);
                }
            }
            iteration++;
        }
    }
    void printIndex() {
        int size = indexArr.size();
        cout << "\n<Primary index>: (key, offset)";
        for(int i = 0; i < size; i++) {
            cout << endl << "[" << i+1 << "] (" << indexArr[i].first << ", " << indexArr[i].second << ")";
        }
    }
};



int main() {
    ExternalSort sorter;

    bool keepRun = true;
    int fileID;
    chrono::time_point<chrono::system_clock> inStart, inEnd;
    chrono::time_point<chrono::system_clock> exStart, exEnd;
    chrono::microseconds inTime, exTime;

    while ( keepRun ) {
        cout << "\n*** The buffer size is 300"
                "\n***********************************************"
                "\n On-machine Exercise 05                       *"
                "\n Mission 1: External Merge Sort on a Big File *"
                "\n Mission 2: Construction of Primary Index     *"
                "\n***********************************************";
        // try to get input file number first
        sorter.reset();
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

            // mission2
            cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                    "\nMission 2: Build the primary index"
                    "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            sorter.getIndexArray();
            sorter.printIndex();
        }

        // ask user quit or not
        cout << "\n[0]Quit or [Any other key]continue?\n";
        string keyInValue;
        cin >> keyInValue;

        if (keyInValue == "0")
            keepRun = false;
    }
}
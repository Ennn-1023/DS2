// 11127130 蔡淳全 11127139 陳恩
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcmp()
#include <iomanip> // setw()
#include <algorithm> // sort()
#include <chrono>
#include <ctime>

using namespace std;

#define SIZE = 300;

struct DataType { // input data type
    char putID[12] = {0}; // 發訊者學號
    char getID[12] = {0}; // 收訊者學號
    float weight = 0.0; // average score
};

class ExternalSort {
private:
    string fileID;
    ifstream inFile;
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
    void getID() {
        // get input file ID, check if it exists
        string fID;
        do {
            cout << "\nInput the file name: [0]Quit";
            cin >> fID;
            if (fID == "0")
                return;
        } while (!checkFIle(fID))

    }
    bool checkFIle(const string& fID) {
        // check the data member: fileID exists or not
        string fileName = "pairs" + fID + ".bin";
        ifstream aFile(fileName.c_str(), ios::binary);
        // check if the file can be opened or not
        bool isOpen = aFile.is_open();
        aFile.close(); // close the file
        return isOpen;
    }

    bool getBinFile() {
        string fileName = "pairs" + fileID + ".bin";
        ifstream inFile(fileName.c_str(), ios::binary);
        if (!inFile.is_open()) {
            cout << endl << "### " << fileName << " does not exist! ###" << endl;
            return false;
        }
        else { // open successfully
            DataType oneR;
            while(inFile.read((char *)&oneR, sizeof(oneR))) {
                oneR.putID[sizeof(oneR.putID)-1] = '\0';
                oneR.getID[sizeof(oneR.getID)-1] = '\0';
                inputData.push_back(oneR); // store in list
            }
            inFile.close();
            return true;
        }
    }

    bool readFile() {
        this->reset();
        cout << endl << "Input a file number ([0] Quit): ";
        cin >> this->fileID;
        if ( fileID == "0" ) { // check if is 0
            reset();
            return false;
        }
        else if (getBinFile()) // try to get binary file
            return true;
        else { // failed to get file
            reset(); // reset fileID
            return false;
        }
    } // end of readFile


};



int main() {
    ExternalSort sorter;

    bool keepRun = true;
    int fileID;
    // srand(time(NULL)); // setting random seed
    while ( keepRun ) {
        cout << "\n*** The buffer size is 300"
                "\n***********************************************"
                "\n On-machine Exercise 05                       *"
                "\n Mission 1: External Merge Sort on a Big File *"
                "\n Mission 2: Construction of Primary Index     *"
                "\n***********************************************";
        sorter.getID();
        cout << "\n[0]Quit or [Any other key]continue?";
        int keyInValue;
        cin >> keyInValue;

        if (keyInValue == 0)
            keepRun = false;
    }
    chrono::time_point<chrono::system_clock> start, end;
    chrono::milliseconds elapsed_time;
}

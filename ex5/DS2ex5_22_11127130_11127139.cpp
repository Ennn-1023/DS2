// 11127130 ���E�� 11127139 ����
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcpy()
#include <algorithm> // sort()
#include <chrono>

using namespace std;

#define BUFFER_SIZE 300

struct DataType { // input data type
    char putID[10] = {0}; // �o�T�̾Ǹ�
    char getID[10] = {0}; // ���T�̾Ǹ�
    float weight = 0.0; // �v��
};

struct smallDataType { // �~���ƧǨϥΪ�data type
    int index = 0; // ��Ʀbinput�ɤ���m
    float weight = 0.0; // �v��
};

void writeFile(vector<smallDataType> &buffer, string &fileName) { // �g��
    // ��buffer�g�ifileName
    int smallDataTypeSize = sizeof(smallDataType);
    ofstream outFile(fileName, ios::binary);
    for (smallDataType smalloneR : buffer) {
        outFile.write((char *) &smalloneR, smallDataTypeSize);
    }

    outFile.close();
}

struct smallDataTypecmp { // sort vector �Ϊ������
    bool  operator ()(const smallDataType &d1, const smallDataType &d2) {
        return d1.weight > d2.weight;
    }
} mycmp;

bool internalSort(string fileID) { // �����Ƨ�(�]�tŪ�ɼg��)
    string fileName = "pairs" + fileID + ".bin";
    ifstream inFile(fileName, ios::binary);
    if (!inFile.is_open()) { // Ū�ɥ���
        cout << endl << fileName << " does not exist!!!" << endl;
        return false;
    }
    else {
        int DataTypeSize = sizeof(DataType);
        vector<smallDataType> buffer; // �Ŷ��W��300�����
        DataType oneR;
        smallDataType smalloneR;
        int outputFileNum = 0;
        int index = 0; // input�ɤ���ƪ��Ǹ�(�q0�}�l)
        bool finish = false;
        while(!finish) {
            // �@���̦hŪ300�����
            for (int i = 0; i < BUFFER_SIZE; i++) {
                if (!inFile.read((char *)&oneR, DataTypeSize)) {
                    finish = true;
                    break;
                }
                smalloneR.index = index;
                smalloneR.weight = oneR.weight;
                buffer.push_back(smalloneR); // ��ibuffer
                index++;
            }

            outputFileNum++;
            stable_sort(buffer.begin(), buffer.end(), mycmp); // �Ƨ�
            string fileName = "1output" + to_string(outputFileNum) + ".bin";
            writeFile(buffer, fileName); // �C300����Ƽg�@����

            buffer.clear();
        }

        inFile.close();
        cout << "\nThe internal sort is completed. Check the initial sorted runs!\n";
        return true;
    }
}

void externalSort(string fileID) {
    int smallDataTypeSize = sizeof(smallDataType);
    int DataTypeSize = sizeof(DataType);
    // ���buffer�A�C�ӥu�s�@�����
    smallDataType buffer1;
    smallDataType buffer2;

    string fileName1;
    string fileName2;

    int round = 0; // merge�^�X(�q1�}�l)�A�ΨөR�W�ɮ�
    bool keepRun = true;
    while (keepRun) {
        bool first = true;
        round++;
        smallDataType smalloneR;
        int fileNum = 0;
        int newFileNum = 0;
        int runs = 0; // �C�^�X���X���ɮ׭nmerge

        while(true) {
            fileNum++;
            fileName1 = to_string(round) + "output" + to_string(fileNum) + ".bin";
            ifstream inFile1(fileName1, ios::binary);
            if (!inFile1.is_open()) break; // �@��merge�^�X����
            runs++;

            fileNum++;
            fileName2 = to_string(round) + "output" + to_string(fileNum) + ".bin";
            ifstream inFile2(fileName2, ios::binary);
            if (!inFile2.is_open()) { // �@��merge�^�X���u�ѤU�@���ɮ�
                if (first) { // �@��merge�^�X���u���@���ɮסA�N��merge����
                    keepRun = false;
                    inFile1.close();
                    break;
                }

                newFileNum++;
                inFile1.close();
                // �N���ɦW�令�U�@��merge�n�Ϊ��ɦW
                rename(fileName1.c_str(), (to_string(round+1) + "output" + to_string(newFileNum) + ".bin").c_str());
                first = false;
                break;
            }

            runs++;
            newFileNum++;
            ofstream outFile(to_string(round+1) + "output" + to_string(newFileNum) + ".bin", ios::binary);
            // Ū����ɮת��Ĥ@�����
            inFile1.read((char *)&buffer1, smallDataTypeSize);
            inFile2.read((char *)&buffer2, smallDataTypeSize);
            bool inFile1hasData = true; // �ɮפ@�O�_�٦����
            // �j������䤤�@���ɮ�Ū��
            while (true) {
                // �v���j����Ƽg�ioutput�ɡA�ñq���Ӹ�ƪ��ɮצAŪ�@�����
                if (buffer1.weight >= buffer2.weight) {
                    outFile.write((char *) &buffer1, smallDataTypeSize);
                    if (!inFile1.read((char *)&buffer1, smallDataTypeSize)) {
                        inFile1hasData = false;
                        outFile.write((char *) &buffer2, smallDataTypeSize);
                        break;
                    }
                }
                else {
                    outFile.write((char *) &buffer2, smallDataTypeSize);
                    if (!inFile2.read((char *)&buffer2, smallDataTypeSize)) {
                        outFile.write((char *) &buffer1, smallDataTypeSize);
                        break;
                    }
                }
            }

            // �q�٦���ƪ��ɮ�Ū�@����ơA�üg�ioutput�ɡA�����ƥ���Ū��
            if (inFile1hasData) {
                while (inFile1.read((char *)&buffer1, smallDataTypeSize)) {
                    outFile.write((char *) &buffer1, smallDataTypeSize);
                }
            }
            else {
                while (inFile2.read((char *)&buffer2, smallDataTypeSize)) {
                    outFile.write((char *) &buffer2, smallDataTypeSize);
                }
            }

            // �R����ӳQŪ�����ɮ�
            inFile1.close();
            remove(fileName1.c_str());
            inFile2.close();
            remove(fileName2.c_str());

            outFile.close();
            first = false;
        }

        cout << "\nNow there are " << runs << " runs.\n";
    }


    string fileName = "pairs" + fileID + ".bin";
    ifstream inputFile(fileName, ios::binary); // ��linput�� (DataType)
    ifstream inFile(fileName1, ios::binary);
    ofstream outFile("order" + fileID + ".bin", ios::binary); // ���Ȥ@�̲�output�� (DataType)
    smallDataType smalloneR;
    DataType oneR;
    while (inFile.read((char *)&smalloneR, smallDataTypeSize)) { // Ū�g�~���ƧǦn���̲��ɮ� (smallDataType)
        inputFile.seekg(smalloneR.index * DataTypeSize); // �ھ�index�A�NŪ�g�Y���ʦܸ�Ʀb��linput�ɤ���m
        inputFile.read((char *)&oneR, DataTypeSize); // Ū
        outFile.write((char *) &oneR, DataTypeSize); // �g
    }

    inputFile.close();
    inFile.close();
    outFile.close();
    remove(fileName1.c_str()); // �R���g�~���ƧǦn���̲��ɮ� (smallDataType)
}

void    getIndexArray(vector<pair<float, int>>& indexArr, const string& fileID) {
    // read sorted file from memory
    ifstream inFile("order"+fileID+".bin", ios::binary);
    int len = sizeof(DataType);
    int iteration = 0;
    float lastWeight = 1.1;
    int offset = -1;
    vector<DataType> buffer;
    while(true) {
        buffer.resize(BUFFER_SIZE); // read at most 300 each time
        inFile.read((char*)buffer.data(), BUFFER_SIZE*len);
        int bytesRead = inFile.gcount();
        if (bytesRead == 0) {
            break;
        }
        buffer.resize(bytesRead/len);
        int size = buffer.size();
        for (int i = 0; i < size; i++) {
            // check each key
            if (buffer[i].weight < lastWeight) {
                lastWeight = buffer[i].weight;
                offset = i + iteration*BUFFER_SIZE;
                pair<float,int> tmp(lastWeight, offset);
                indexArr.push_back(tmp);
            }
        }
        iteration++;
    }
}
void printIndex(const vector<pair<float, int>>& indexArr) {
    int size = indexArr.size();
    cout << "\n<Primary index>: (key, offset)";
    for(int i = 0; i < size; i++) {
        cout << endl << "[" << i+1 << "] (" << indexArr[i].first << ", " << indexArr[i].second << ")";
    }
}

int main() {
    bool keepRun = true;
    string fileID; // �ɮ׽s��
    string command; // ���O
    while ( keepRun ) {
        cout << "\n*** The buffer size is 300"
                "\n***********************************************"
                "\n On-machine Exercise 05                       *"
                "\n Mission 1: External Merge Sort on a Big File *"
                "\n Mission 2: Construction of Primary Index     *"
                "\n***********************************************"
                "\n########################################################"
                "\nMission 1: External merge sort "
                "\n########################################################\n";
        chrono::time_point<chrono::system_clock> start, end;
        chrono::microseconds elapsed_time_in;
        chrono::microseconds elapsed_time_ex;
        while (true) {
            cout <<	"\nInput the file name: [0]Quit\n";
            cin >> fileID;
            if (fileID == "0") {
                cout << "\n[0]Quit or [Any other key]continue?\n";
                cin >> command; // get command
                if (command == "0") keepRun = false;
                break;
            }
            else {
                start = chrono::system_clock::now();
                bool inSuccess = internalSort(fileID); // �����Ƨ�
                end = chrono::system_clock::now();
                elapsed_time_in = chrono::duration_cast<chrono::microseconds>(end - start);
                if (inSuccess) {
                    start = chrono::system_clock::now();
                    externalSort(fileID); // �~���Ƨ�
                    end = chrono::system_clock::now();
                    elapsed_time_ex = chrono::duration_cast<chrono::microseconds>(end - start);
                    cout << "\nThe execution time ...";
                    cout << "\nInternal Sort = " << elapsed_time_in.count()/1000.0 << " ms";
                    cout << "\nExternal Sort = " << elapsed_time_ex.count()/1000.0 << " ms";
                    cout << "\nTotal Execution Time = " << (elapsed_time_in.count() + elapsed_time_ex.count())/1000.0 << " ms\n\n\n";

                    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
                    cout << "Mission 2: Build the primary index\n";
                    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
                    /*
                    mission2
                    */
                    vector<pair<float,int>> indexArr;
                    getIndexArray(indexArr, fileID);
                    printIndex(indexArr);

                    cout << "\n[0]Quit or [Any other key]continue?\n";
                    cin >> command; // get command
                    if (command == "0") keepRun = false;
                    break;
                }
            }
        }
    }
}
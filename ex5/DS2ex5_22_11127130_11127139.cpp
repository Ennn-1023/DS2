// 11127130 蔡淳全 11127139 陳恩
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcpy()
#include <algorithm> // sort()
#include <chrono>

using namespace std;

#define BUFFER_SIZE 300

struct DataType { // input data type
    char putID[10] = {0}; // 發訊者學號
    char getID[10] = {0}; // 收訊者學號
    float weight = 0.0; // 權重
};

struct smallDataType { // 外部排序使用的data type
    int index = 0; // 資料在input檔中位置
    float weight = 0.0; // 權重
};

void writeFile(vector<smallDataType> &buffer, string &fileName) { // 寫檔
    // 把buffer寫進fileName
    int smallDataTypeSize = sizeof(smallDataType);
    ofstream outFile(fileName, ios::binary);
    for (smallDataType smalloneR : buffer) {
        outFile.write((char *) &smalloneR, smallDataTypeSize);
    }

    outFile.close();
}

struct smallDataTypecmp { // sort vector 用的比較器
    bool  operator ()(const smallDataType &d1, const smallDataType &d2) {
        return d1.weight > d2.weight;
    }
} mycmp;

bool internalSort(string fileID) { // 內部排序(包含讀檔寫檔)
    string fileName = "pairs" + fileID + ".bin";
    ifstream inFile(fileName, ios::binary);
    if (!inFile.is_open()) { // 讀檔失敗
        cout << endl << fileName << " does not exist!!!" << endl;
        return false;
    }
    else {
        int DataTypeSize = sizeof(DataType);
        vector<smallDataType> buffer; // 空間上限300筆資料
        DataType oneR;
        smallDataType smalloneR;
        int outputFileNum = 0;
        int index = 0; // input檔中資料的序號(從0開始)
        bool finish = false;
        while(!finish) {
            // 一次最多讀300筆資料
            for (int i = 0; i < BUFFER_SIZE; i++) {
                if (!inFile.read((char *)&oneR, DataTypeSize)) {
                    finish = true;
                    break;
                }
                smalloneR.index = index;
                smalloneR.weight = oneR.weight;
                buffer.push_back(smalloneR); // 放進buffer
                index++;
            }

            outputFileNum++;
            stable_sort(buffer.begin(), buffer.end(), mycmp); // 排序
            string fileName = "1output" + to_string(outputFileNum) + ".bin";
            writeFile(buffer, fileName); // 每300筆資料寫一次檔

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
    // 兩個buffer，每個只存一筆資料
    smallDataType buffer1;
    smallDataType buffer2;

    string fileName1;
    string fileName2;

    int round = 0; // merge回合(從1開始)，用來命名檔案
    bool keepRun = true;
    while (keepRun) {
        bool first = true;
        round++;
        smallDataType smalloneR;
        int fileNum = 0;
        int newFileNum = 0;
        int runs = 0; // 每回合有幾個檔案要merge

        while(true) {
            fileNum++;
            fileName1 = to_string(round) + "output" + to_string(fileNum) + ".bin";
            ifstream inFile1(fileName1, ios::binary);
            if (!inFile1.is_open()) break; // 一個merge回合結束
            runs++;

            fileNum++;
            fileName2 = to_string(round) + "output" + to_string(fileNum) + ".bin";
            ifstream inFile2(fileName2, ios::binary);
            if (!inFile2.is_open()) { // 一個merge回合中只剩下一個檔案
                if (first) { // 一個merge回合中只有一個檔案，代表merge結束
                    keepRun = false;
                    inFile1.close();
                    break;
                }

                newFileNum++;
                inFile1.close();
                // 將此檔名改成下一輪merge要用的檔名
                rename(fileName1.c_str(), (to_string(round+1) + "output" + to_string(newFileNum) + ".bin").c_str());
                first = false;
                break;
            }

            runs++;
            newFileNum++;
            ofstream outFile(to_string(round+1) + "output" + to_string(newFileNum) + ".bin", ios::binary);
            // 讀兩個檔案的第一筆資料
            inFile1.read((char *)&buffer1, smallDataTypeSize);
            inFile2.read((char *)&buffer2, smallDataTypeSize);
            bool inFile1hasData = true; // 檔案一是否還有資料
            // 迴圈持續到其中一個檔案讀完
            while (true) {
                // 權重大的資料寫進output檔，並從那個資料的檔案再讀一筆資料
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

            // 從還有資料的檔案讀一筆資料，並寫進output檔，直到資料全部讀完
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

            // 刪除兩個被讀完的檔案
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
    ifstream inputFile(fileName, ios::binary); // 原始input檔 (DataType)
    ifstream inFile(fileName1, ios::binary);
    ofstream outFile("order" + fileID + ".bin", ios::binary); // 任務一最終output檔 (DataType)
    smallDataType smalloneR;
    DataType oneR;
    while (inFile.read((char *)&smalloneR, smallDataTypeSize)) { // 讀經外部排序好的最終檔案 (smallDataType)
        inputFile.seekg(smalloneR.index * DataTypeSize); // 根據index，將讀寫頭移動至資料在原始input檔中位置
        inputFile.read((char *)&oneR, DataTypeSize); // 讀
        outFile.write((char *) &oneR, DataTypeSize); // 寫
    }

    inputFile.close();
    inFile.close();
    outFile.close();
    remove(fileName1.c_str()); // 刪除經外部排序好的最終檔案 (smallDataType)
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
    string fileID; // 檔案編號
    string command; // 指令
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
                bool inSuccess = internalSort(fileID); // 內部排序
                end = chrono::system_clock::now();
                elapsed_time_in = chrono::duration_cast<chrono::microseconds>(end - start);
                if (inSuccess) {
                    start = chrono::system_clock::now();
                    externalSort(fileID); // 外部排序
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
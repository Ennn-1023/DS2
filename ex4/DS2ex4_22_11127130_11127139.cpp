// 11127130 ���E�� 11127139 ����  
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcmp()
#include <iomanip> // setw()
#include <algorithm> // sort()
#include <queue>
#include <unordered_map>

using namespace std;
struct DataType { // input data type
    char putID[12] = {0}; // �o�T�̾Ǹ� 
    char getID[12] = {0}; // ���T�̾Ǹ� 
    float weight = 0.0; // average score
};

struct Node { // �۾F��C�`�I 
    string id; // �Ǹ�
    float weight; // �v��
};

class File {
private:
    string fileID;
    vector<DataType> inputData;

public:
    File() {
        reset();
    }
    ~File() {
        reset();
    }
    void reset() {
        fileID.clear();
        inputData.clear();
    }
    vector<DataType> getList() {
        return inputData;
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

    bool writeFile1(vector<vector<Node> > list, int idnum, int nodenum) { // �g��
        ofstream outFile;
        outFile.open("pairs" + fileID + ".adj");
        if (!outFile) {
            cout << "�g�J����\n";
            outFile.close();
            return false;
        }
        else {
            outFile << "<<< There are " << idnum << " IDs in total. >>>\n";

            for (int i = 0; i < list.size(); i++) {
                outFile << "[" << setw(3) << i+1 << "] " << list[i][0].id << ": \n";
                for (int j = 1; j < list[i].size(); j++) {
                    outFile << "\t(" << setw(2) << j << ") " << list[i][j].id << "," << setw(7) << list[i][j].weight;
                    if (j % 12 == 0) outFile << "\n";
                }
                outFile << "\n";
            }

            outFile << "<<< There are " << nodenum << " nodes in total. >>>\n";
            outFile.close();
            return true;
        }
    }
    bool writeFile2(const vector<pair<string, vector<string>>>& cntList, int idnum) { // �g��
        ofstream outFile;
        outFile.open("pairs" + fileID + ".cnt");
        if (!outFile) {
            cout << "�g�J����\n";
            outFile.close();
            return false;
        }
        else {
            outFile << "<<< There are " << idnum << " IDs in total. >>>\n";
            int width = to_string(cntList.size()).length();
            for (int i = 0; i < cntList.size(); i++) {
                int connect = cntList[i].second.size();
                outFile << "[" << setw(3) << i+1 << "] " << cntList[i].first << "(" << connect << "): \n";
                int j = 0;
                for (auto node : cntList[i].second) {
                    outFile << "\t(" << setw(2) << j+1 <<") " << node;
                    if ((j+1) % 12 == 0)
                        outFile << "\n";
                    j++;
                }
                outFile << "\n";
            }

            outFile.close();
            return true;
        }
    }
};

class adjList { // �۾F��C

private:
    vector<vector<Node> > mainList; // �D�}�C
    int idnum = 0; // �۲��Ǹ��`��
    int nodenum = 0; // �`�`�I��(���]�A�D�}�C)

    vector<string> getOnlyID(const vector<DataType>& list) { // ���o�u�����T���Ҧ��Ǹ�
        bool getOnly;
        vector<string> temp;
        temp.resize(0);
        for (int i = 0; i < list.size(); i++) {
            getOnly = true;
            for (int j = 0; j < list.size(); j++) {
                if (strcmp(list[i].getID, list[j].putID) == 0) { // ���Ǹ����o�T
                    getOnly = false;
                    break;
                }
            }

            if (getOnly) // ���Ǹ��u���T
                temp.push_back(list[i].getID);

            return temp;
        }
    }

    struct nodecmp {
        bool  operator ()( const Node &n1, const Node & n2) {
            return n1.id < n2.id;
        }
    } mycmp1;

    struct vectornodecmp {
        bool  operator ()( const vector<Node> &n1, const vector<Node> & n2) {
            return n1[0].id < n2[0].id;
        }
    } mycmp2;

    void sorting() { // �ƧǦ��T��C�M�D�}�C(�ɧ�)
        for (int i = 0; i < mainList.size(); i++) {
            if (mainList[i].size() > 2)
                sort(mainList[i].begin()+1, mainList[i].end(), mycmp1);
        }

        sort(mainList.begin(), mainList.end(), mycmp2);
    }

    void calNodeNum() { // �p���`�`�I��
        for (int i = 0; i < mainList.size(); i++) {
            nodenum += mainList[i].size()-1;
        }
    }

public:
    void build(vector<DataType> list) { // �إ߬۾F��C
        int last = 0;
        vector<Node> temp;
        // �[�J�Ҧ��o�T�̥H�Ψ䦬�T��C
        for (int i = 0; i < list.size(); i++) {
            if (last != atoi(list[i].putID)) {
                last = atoi(list[i].putID);
                if (i != 0) {
                    mainList.push_back(temp);
                    temp.clear();
                    temp.resize(0);
                }
                Node main;
                main.id = list[i].putID;
                main.weight = 0.0;
                temp.push_back(main);
                idnum++;
            }

            Node adj;
            adj.id = list[i].getID;
            adj.weight = list[i].weight;
            temp.push_back(adj);
            if (i == list.size() - 1)
                mainList.push_back(temp);
        }

        // �[�J�u���T���Ǹ��ܥD�}�C
        vector <string> getOnly = getOnlyID(list);
        for (int i = 0; i < getOnly.size(); i++) {
            temp.clear();
            temp.resize(0);
            Node main;
            main.id = getOnly[i];
            main.weight = 0.0;
            temp.push_back(main);
            mainList.push_back(temp);
            idnum++;
        }

        sorting(); // �Ƨ�
    }

    int getNodeNum() { // ���o�`�`�I��
        return nodenum;
    }

    int getIdNum() { // ���o�۲��Ǹ��`��
        return idnum;
    }

    vector<vector<Node>>& getList() { // ���o�۾F��C
        return mainList;
    }

    void outPut() { // ��X
        calNodeNum(); // �p���`�`�I��
        cout << "\n<<< There are " << idnum << " IDs in total. >>>\n";
        cout << "\n<<< There are " << nodenum << " nodes in total. >>>\n";

    }

    void reset() { // ���m
        idnum = 0;
        nodenum = 0;
        mainList.clear();
        mainList.resize(0);
    }
};
class DirectGraph {
private:
    // data member
    unordered_map<string, vector<string>> adjList;
    vector<pair<string, vector<string>>> connectedList;

public:
    DirectGraph() {
    }
    ~DirectGraph() {
        adjList.clear();
        connectedList.clear();
    }
    bool setList(const vector<vector<Node>>& inputList) {
        if (inputList.size() == 0) {
            return false;
        }
        else { // setting
            for (auto row : inputList) {
                vector<string> temp;
                for (int i = 1; i < row.size(); i++) {
                    temp.push_back(row[i].id);
                }
                adjList[row[0].id] = temp; // insert into map
            }
            return true;
        }
    }
    void computeBFS() {
        // compute the connection of each node one by one
        for (auto &list: adjList) {
            connectedList.emplace_back(list.first, findBFS(list.first));
        }

        sort(connectedList.begin(), connectedList.end(), compareBySize);
    }
    vector<string> findBFS(const string& sID) {
        queue<string> aQueue;
        aQueue.push(sID);
        vector<string> returnList; //store the return visited node
        unordered_map<string, int> visitedList; // record visited node
        visitedList[sID] = 1;
        while (!aQueue.empty()) {
            string curNode = aQueue.front(); // get front
            vector<string> adjNodes = adjList[curNode];
            aQueue.pop();
            for (string adjNode : adjNodes) { // check every node which is adjacent to curNode;

                if ( visitedList[adjNode] != 1 ) {
                    // not found in visitedList
                    aQueue.push(adjNode); // enqueue
                    visitedList[adjNode] = 1;
                    returnList.emplace_back(adjNode);
                }
            }
        }

        sort(returnList.begin(), returnList.end(), sortNode);
        return returnList;
    }
    void reset() {
        connectedList.clear();
        adjList.clear();
    }
    vector<pair<string, vector<string>>>& getList() {
        return connectedList;
    }
    // comparator
    static bool compareBySize(const pair<string, vector<string>>& n1, const pair<string, vector<string>>& n2) {
        int n1Size = n1.second.size(), n2Size = n2.second.size();
        if (n1Size == n2Size) { // order by sID ascii
            return n1.first < n2.first ;
        }
        else // order by num of connection
            return n1Size > n2Size;
    }
    static bool sortNode(const string& id1, const string& id2) {
        return id1 < id2;
    }
};

int main() {
    File aFile;
    adjList aList;
    DirectGraph aGraph;
    bool keepRun = true;
    int command = -1;
    while ( keepRun ) {
        cout << "\n**** Graph data manipulation *****"
                "\n* 0. QUIT                        *"
                "\n* 1. Build adjacency lists       *"
                "\n* 2. Compute connection counts   *"
                "\n**********************************";
        cout << "\nInput a choice(0, 1, 2): ";
        cin >> command;
        switch ( command ) {
            case 0:
                keepRun = false;
                break;
            case 1:
                // read file first
                aFile.reset();
                aList.reset();
                if ( !aFile.readFile()) {
                    break;
                }

                aList.build(aFile.getList());
                aList.outPut();
                aFile.writeFile1(aList.getList(), aList.getIdNum(), aList.getNodeNum());
                break;
            case 2:
                // mission 2
                aGraph.reset();
                // check adjList existence first
                if (!aGraph.setList(aList.getList())) {
                    cout << "\n### There is no graph and choose 1 first. ###\n";
                    break;
                }
                // compute
                aGraph.computeBFS();
                cout << "\n<<< There are " << aList.getIdNum() << " IDs in total. >>>\n";
                aFile.writeFile2(aGraph.getList(), aList.getIdNum());
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        } // end of switch
    }
}

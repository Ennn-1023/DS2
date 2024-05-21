// 11127130 蔡淳全 11127139 陳恩  
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcmp()
#include <iomanip> // setw()
#include <algorithm> // sort()
#include <queue>
#include <stack>
#include <set>
#include <unordered_map>
#include <random>
#include <chrono>
using namespace std;
struct DataType { // input data type
    char putID[12] = {0}; // 發訊者學號 
    char getID[12] = {0}; // 收訊者學號 
    float weight = 0.0; // average score
};

struct Node { // 相鄰串列節點 
    string id; // 學號
    float weight = 0.0; // 權重
    float threshold = 0.0;
    bool operator<(const Node& other) const {
        return id < other.id;
    }
    bool operator>(const Node& other) const {
        return id > other.id;
    }
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

    bool writeFile1(vector<vector<Node> > list, int idnum, int nodenum) { // 寫檔
        ofstream outFile;
        outFile.open("pairs" + fileID + ".adj");
        if (!outFile) {
            cout << "寫入失敗\n";
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
    bool writeFile2(const vector<pair<string, vector<Node>>>& cntList, int idnum) { // 寫檔
        ofstream outFile;
        outFile.open("pairs" + fileID + ".cnt");
        if (!outFile) {
            cout << "寫入失敗\n";
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
                    outFile << "\t(" << setw(2) << j+1 <<") " << node.id;
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
    bool writeFile3(const vector<pair<string, vector<Node>>>& infList) { // 寫檔
        ofstream outFile;
        outFile.open("pairs" + fileID + ".inf");
        if (!outFile) {
            cout << "寫入失敗\n";
            outFile.close();
            return false;
        }
        else {
            outFile << "<<< There are " << infList.size() << " IDs in total. >>>\n";
            int width = to_string(infList.size()).length();
            for (int i = 0; i < infList.size(); i++) {
                int connect = infList[i].second.size();
                outFile << "[" << setw(3) << i+1 << "] " << infList[i].first << "(" << connect << "): \n";
                int j = 0;
                for (auto node : infList[i].second) {
                    outFile << "\t(" << setw(2) << j+1 <<") " << node.id;
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
    bool writeFile4(const vector<pair<string, set<Node>>>& cntList, int idnum) { // 寫檔
        ofstream outFile;
        outFile.open("pairs" + fileID + ".txt");
        if (!outFile) {
            cout << "寫入失敗\n";
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
                    outFile << "\t(" << setw(2) << j+1 <<") " << node.id;
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

class adjList { // 相鄰串列

private:
    vector<vector<Node> > mainList; // 主陣列
    int idnum = 0; // 相異學號總數
    int nodenum = 0; // 總節點數(不包括主陣列)

    vector<string> getOnlyID(const vector<DataType>& list) { // 取得只有收訊的所有學號
        bool getOnly;
        vector<string> temp;
        temp.resize(0);
        for (int i = 0; i < list.size(); i++) {
            getOnly = true;
            for (int j = 0; j < list.size(); j++) {
                if (strcmp(list[i].getID, list[j].putID) == 0) { // 此學號有發訊
                    getOnly = false;
                    break;
                }
            }

            if (getOnly) // 此學號只收訊
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

    void sorting() { // 排序收訊串列和主陣列(升序)
        for (int i = 0; i < mainList.size(); i++) {
            if (mainList[i].size() > 2)
                sort(mainList[i].begin()+1, mainList[i].end(), mycmp1);
        }

        sort(mainList.begin(), mainList.end(), mycmp2);
    }

    void calNodeNum() { // 計算總節點數
        for (int i = 0; i < mainList.size(); i++) {
            nodenum += mainList[i].size()-1;
        }
    }

public:
    void build(vector<DataType> list) { // 建立相鄰串列
        int last = 0;
        vector<Node> temp;
        // 加入所有發訊者以及其收訊串列
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

        // 加入只收訊的學號至主陣列
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

        sorting(); // 排序
    }

    int getNodeNum() { // 取得總節點數
        return nodenum;
    }

    int getIdNum() { // 取得相異學號總數
        return idnum;
    }

    vector<vector<Node>>& getList() { // 取得相鄰串列
        return mainList;
    }

    void outPut() { // 輸出
        calNodeNum(); // 計算總節點數
        cout << "\n<<< There are " << idnum << " IDs in total. >>>\n";
        cout << "\n<<< There are " << nodenum << " nodes in total. >>>\n";

    }

    void reset() { // 重置
        idnum = 0;
        nodenum = 0;
        mainList.clear();
        mainList.resize(0);
    }
};
class DirectGraph {
protected:
    // data member
    unordered_map<string, vector<Node>> adjList;
    vector<pair<string, vector<Node>>> connectedList;

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
                vector<Node> temp;
                for (int i = 1; i < row.size(); i++) {
                    temp.push_back(row[i]);
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
    vector<Node> findBFS(const string& sID) {
        queue<string> aQueue;
        aQueue.push(sID);
        vector<Node> returnList; //store the return visited node
        unordered_map<string, int> visitedList; // record visited node
        visitedList[sID] = 1;
        while (!aQueue.empty()) {
            string curNode = aQueue.front(); // get front
            vector<Node> adjNodes = adjList[curNode];
            aQueue.pop();
            for (Node adjNode : adjNodes) { // check every node which is adjacent to curNode;

                if ( visitedList[adjNode.id] != 1 ) {
                    // not found in visitedList
                    aQueue.push(adjNode.id); // enqueue
                    visitedList[adjNode.id] = 1;
                    returnList.emplace_back(adjNode);
                }
            }
        }

        sort(returnList.begin(), returnList.end(), sortNode);
        return returnList;
    }
    void computeDFS(float th) {
        // compute the connection of each node one by one
        unordered_map<string, vector<Node>> searched; // 以某點做DFS後的結果
        for (auto &list: adjList) {
            if (list.second.size() > 0) {
                vector<Node> infList = findDFS(list.first, th, searched);
                if (infList.size() != 0) {
                    connectedList.emplace_back(list.first, infList);
                    searched[list.first] = infList; // 把list.first的DFS結果加入searched
                }
            }
        }

        sort(connectedList.begin(), connectedList.end(), compareBySize);
    }
    vector<Node> findDFS(const string& sID, float th, unordered_map<string, vector<Node>> &searched) {
        stack<string> aStack;
        aStack.push(sID);
        vector<Node> returnList; //store the return visited node
        unordered_map<string, int> visitedList; // record visited node
        visitedList[sID] = 1;
        while (!aStack.empty()) {
            string curNode = aStack.top(); // get front
            vector<Node> adjNodes = adjList[curNode];
            Node adjNode;
            bool allVisited = true; //此節點的鄰居是否全部走過了
            for (int i = 0; i < adjNodes.size(); i++) {
                if (visitedList[adjNodes[i].id] != 1 && adjNodes[i].weight >= th) { // 還沒走過而且走得過去
                    adjNode = adjNodes[i];
                    allVisited = false;
                    break;
                }
            }

            if (!allVisited) { // 還有沒走過的鄰居
                bool noVisited = true; // 此鄰居的深度搜索結果是否都未走過
                if (searched[adjNode.id].size() > 1) {
                    for (int i = 0; i < searched[adjNode.id].size(); i++) {
                        if (visitedList[searched[adjNode.id][i].id] == 1) { // 此鄰居的深度搜索結果有走過的
                            noVisited = false;
                            break;
                        }
                    }
                }

                if (noVisited) { // 此鄰居的深度搜索結果都未走過
                    // 把此鄰居和它深度搜索結果加入returnList
                    returnList.emplace_back(adjNode);
                    returnList.insert(returnList.end(), searched[adjNode.id].begin(), searched[adjNode.id].end());

                    // 把所有走過的點標示"走過了"
                    visitedList[adjNode.id] = 1;
                    for (int i = 0; i < searched[adjNode.id].size(); i++) {
                        visitedList[searched[adjNode.id][i].id] = 1;
                    }
                }
                else { // 此鄰居的深度搜索結果有走過的
                    // 乖乖做深度搜索
                    aStack.push(adjNode.id);
                    visitedList[adjNode.id] = 1;
                    returnList.emplace_back(adjNode);
                }
            }

            aStack.pop();
        }

        if (returnList.size() > 0)
            sort(returnList.begin(), returnList.end(), sortNode);

        return returnList;
    }
    void reset() {
        connectedList.clear();
        adjList.clear();
    }
    vector<pair<string, vector<Node>>>& getList() {
        return connectedList;
    }
    // comparator
    static bool compareBySize(const pair<string, vector<Node>>& n1, const pair<string, vector<Node>>& n2) {
        int n1Size = n1.second.size(), n2Size = n2.second.size();
        if (n1Size == n2Size) { // order by sID ascii
            return n1.first < n2.first ;
        }
        else // order by num of connection
            return n1Size > n2Size;
    }
    static bool sortNode(const Node& id1, const Node& id2) {
        return id1.id < id2.id;
    }

};


class WeightedGraph : public DirectGraph {
private:
    unordered_map<string, set<Node>> connectedMap;
    unordered_map<string, set<Node>> doneList;
    vector<pair<string, set<Node>>> connectedList;
public:
    void reset() {
        connectedList.clear();
        doneList.clear();
        DirectGraph::reset();
    }
    vector<pair<string, set<Node>>>& getList() {
        return this->connectedList;
    }
    void getTopK() {
        int total = this->connectedList.size();
        int topK = 0;
        cout << "Input an integer to show top-K in [1" << "," << total << "]: ";
        cin >> topK;
        // check range
        while (topK < 1 || topK > total) {
            cout << "\n###" << topK << " is NOT in [1," << total << "] ###";
            cin >> topK;
        }

        // print topK
        for (int i = 0; i < topK; i++) {
            cout << "\n<" << i+1 << "> " << this->connectedList[i].first << ": " << this->connectedList[i].second.size();
        }
    }
    void traverse() {
        // compute the connection of each node one by one
        for (auto &row: adjList) {
            if (row.second.size() > 0) {
                set<Node> infList = findBFS(row.first);
                if (infList.size() != 0) {
                    connectedList.emplace_back(row.first, infList);
                }
                doneList[row.first] = infList;
            }
        }

        sort(connectedList.begin(), connectedList.end(), compareBySize);
    }
    bool setThreshold(const string& putID, Node& getID) {
        if (getID.threshold > 0.5) // had been set up
            return false;
        else {
            // if the valid gate weight have not been set up
            // set gate weight
            srand((unsigned int)(getID.weight*time(NULL)));
            getID.threshold = 0.8 + static_cast<double>(rand()) / RAND_MAX * 0.2;
            return true;
        }
    }

    set<Node> findBFS(const string& sID) {
        queue<string> aQueue;
        aQueue.push(sID);
        set<Node> returnList; // store the return visited node

        unordered_map<string, int> visitedMap; // record visited node
        visitedMap[sID] = 1;
        aQueue.push(sID);
        while (!aQueue.empty()) {
            string curNode = aQueue.front(); // get front
            aQueue.pop();
            // check the connected node of curNode has been added or not
            if (!doneList[curNode].empty()) // skip if visited
                continue;
            // check all adjacent nodes
            for (auto& adjNode: adjList[curNode]) {
                setThreshold(curNode, adjNode);
                // check if the node is valid to be visited
                if (visitedMap[adjNode.id] != 1 && adjNode.weight >= adjNode.threshold) {
                    visitedMap[adjNode.id] = 1; // mark as visited
                    if (!doneList[adjNode.id].empty()) {
                        // copy from other list if it had done before
                        for (auto node: doneList[adjNode.id]) {
                            visitedMap[node.id] = 1;
                            returnList.insert(node);
                        }
                    }
                    else { // have not been visited yet
                        returnList.insert(adjNode);
                        aQueue.push(adjNode.id);
                    }
                }
                else // also mark as visited if weight is not enough
                    visitedMap[adjNode.id] = 1;
            }
        }

        // sort(returnList.begin(), returnList.end(), sortNode);
        return returnList;
    }

    // comparator
    static bool compareBySize(const pair<string, set<Node>>& n1, const pair<string, set<Node>>& n2) {
        int n1Size = n1.second.size(), n2Size = n2.second.size();
        if (n1Size == n2Size) { // order by sID ascii
            return n1.first < n2.first ;
        }
        else // order by num of connection
            return n1Size > n2Size;
    }

    vector<Node> findDFS(const string& sID, unordered_map<string, vector<Node>> &searched) {
        stack<string> aStack;
        aStack.push(sID);
        vector<Node> returnList; //store the return visited node
        unordered_map<string, int> visitedList; // record visited node
        visitedList[sID] = 1; // mark as visited
        while (!aStack.empty()) {
            string curNode = aStack.top(); // get front
            vector<Node> adjNodes = adjList[curNode];
            Node adjNode;
            bool allVisited = true; // 此節點的鄰居是否全部走過了
            for (int i = 0; i < adjNodes.size(); i++) {
                // set the threshold value randomly
                setThreshold(curNode, adjNodes[i]);
                if (visitedList[adjNodes[i].id] != 1 && adjNodes[i].weight >= adjNodes[i].threshold) { // 還沒走過而且走得過去
                    adjNode = adjNodes[i];
                    allVisited = false;
                    break;
                }
            }

            if (!allVisited) { // 還有沒走過的鄰居
                bool noVisited = true; // 此鄰居的深度搜索結果是否都未走過
                if (searched[adjNode.id].size() > 1) {
                    for (int i = 0; i < searched[adjNode.id].size(); i++) {
                        if (visitedList[searched[adjNode.id][i].id] == 1) { // 此鄰居的深度搜索結果有走過的
                            noVisited = false;
                            break;
                        }
                    }
                }

                if (noVisited) { // 此鄰居的深度搜索結果都未走過
                    // 把此鄰居和它深度搜索結果加入returnList
                    returnList.emplace_back(adjNode);
                    returnList.insert(returnList.end(), searched[adjNode.id].begin(), searched[adjNode.id].end());

                    // 把所有走過的點標示"走過了"
                    visitedList[adjNode.id] = 1;
                    for (int i = 0; i < searched[adjNode.id].size(); i++) {
                        visitedList[searched[adjNode.id][i].id] = 1;
                    }
                }
                else { // 此鄰居的深度搜索結果有走過的
                    // 乖乖做深度搜索
                    aStack.push(adjNode.id);
                    visitedList[adjNode.id] = 1;
                    returnList.emplace_back(adjNode);
                }
            }

            aStack.pop();
        }

        if (returnList.size() > 0)
            sort(returnList.begin(), returnList.end(), sortNode);

        return returnList;
    }


};

class time_point;

int main() {
    File aFile;
    adjList aList;
    DirectGraph aGraph;
    WeightedGraph aGraph4;
    bool keepRun = true;
    int command = -1;
    while ( keepRun ) {
        cout << "\n**** Graph data manipulation *****"
                "\n* 0. QUIT                        *"
                "\n* 1. Build adjacency lists       *"
                "\n* 2. Compute connection counts   *"
                "\n* 3. Estimate influence values   *"
                "\n* 4. Probability-based influence *"
                "\n**********************************";
        cout << "\nInput a choice(0, 1, 2, 3, 4): ";
        cin >> command;
        chrono::time_point<chrono::system_clock> start, end;
        chrono::milliseconds elapsed_time;
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
            case 3:
                aGraph.reset();
                // check adjList existence first
                if (!aGraph.setList(aList.getList())) {
                    cout << "### There is no graph and choose 1 first. ###\n";
                    break;
                }

                float th;
                cout << endl << "Input a real number in [0.5,1]: ";
                cin >> th;
                // compute
                aGraph.computeDFS(th);
                cout << "\n<<< There are " << aGraph.getList().size() << " IDs in total. >>>\n";
                aFile.writeFile3(aGraph.getList());
                break;
            case 4:
                aGraph4.reset();
                // check adjList existence first
                if (!aGraph4.setList(aList.getList())) {
                    cout << "\n### There is no graph and choose 1 first. ###\n";
                    break;
                }
                start = chrono::system_clock::now(); // start time
                // compute
                aGraph4.traverse();
                end = chrono::system_clock::now(); // end time
                elapsed_time = chrono::duration_cast<chrono::milliseconds>(end - start);
                cout << "[Elapsed time] " << elapsed_time.count() << " ms" << endl;
                aGraph4.getTopK();
                break;
            default:
                cout << "\nCommand does not exist!";
                break;
        } // end of switch
    }
}

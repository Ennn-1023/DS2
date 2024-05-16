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
	int id; // �Ǹ� 
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
				outFile << "[" << setw(2) << i+1 << "] " << list[i][0].id << ":\n";
				for (int j = 1; j < list[i].size(); j++) {
					outFile << "(" << setw(2) << j << ") " << list[i][j].id << ", " << list[i][j].weight << " ";
				}
				outFile << "\n";
			}
			
			outFile << "<<< There are " << nodenum << " nodes in total. >>>";
            outFile.close();
            return true;
		}
	}
    bool writeFile2(const vector<pair<int, vector<int>>>& cntList, int idnum) { // �g��
        ofstream outFile;
        outFile.open("pairs" + fileID + ".cnt");
        if (!outFile) {
            cout << "�g�J����\n";
            outFile.close();
            return false;
        }
        else {
            outFile << "<<< There are " << idnum << " IDs in total. >>>\n";

            for (int i = 0; i < cntList.size(); i++) {
                int connect = cntList[i].second.size();
                outFile << "[ " << i+1 << "] " << cntList[i].first << "(" << connect << "):";
                int j = 0, k = 0;
                for ( ; j < cntList[i].second.size(); j++, k++, k%=12) {
                    if (k == 0)
                        outFile << "\n";
                    outFile << " (" << setw(2) << j+1 << ") " << cntList[i].second[j];
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
	
		vector<int> getOnlyID(vector<DataType> list) { // ���o�u�����T���Ҧ��Ǹ� 
			bool getOnly;
			vector<int> temp;
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
					temp.push_back(atoi(list[i].getID));
					
				return temp;
			}
		}
		
		struct nodecmp {
			bool  operator ()( const Node &n1, const Node & n2) {
				string s1 = to_string(n1.id);
				string s2 = to_string(n2.id);
			    return s1 < s2;
			} 
		} mycmp1;
		
		struct vectornodecmp {
			bool  operator ()( const vector<Node> &n1, const vector<Node> & n2) {
			    string s1 = to_string(n1[0].id);
				string s2 = to_string(n2[0].id);
			    return s1 < s2;
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
					main.id = atoi(list[i].putID);
					main.weight = 0.0;
					temp.push_back(main);
					idnum++;
				}
				
				Node adj;
				adj.id = atoi(list[i].getID);
				adj.weight = list[i].weight;
				temp.push_back(adj);
				if (i == list.size() - 1)
					mainList.push_back(temp);
			}
			
			// �[�J�u���T���Ǹ��ܥD�}�C 
			vector <int> getOnly = getOnlyID(list);
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
		
		vector<vector<Node> > getList() { // ���o�۾F��C 
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
    unordered_map<int, vector<int>> adjList;
    vector<pair<int, vector<int>>> connectedList;

public:
    DirectGraph() {
    }
    ~DirectGraph() {
        adjList.clear();
    }
    bool setList(const vector<vector<Node>>& inputList) {
        if (inputList.size() == 0) {
            return false;
        }
        else { // setting
            for (auto row : inputList) {
                vector<int> temp;
                for (int i = 1; i < row.size(); i++) {
                    temp.push_back(row[i].id);
                }
                adjList[row[0].id] = temp;
            }
            return true;
        }
    } // 4106034
    //   10027127
    void computeBFS() {
        for (auto list = adjList.begin(); list != adjList.end(); list++) {
            connectedList.emplace_back(list->first, findBFS(list->first));
        }
        sort(connectedList.begin(), connectedList.end(), compareBySize);
    }
    vector<int> findBFS(int sID) {
        queue<int> aQueue;
        aQueue.push(sID);
        vector<int> visitedList;
        // visitedList_ptr.push_back(sID);
        while (!aQueue.empty()) {
            int curNode = aQueue.front(); // get front
            vector<int> adjNodes = adjList.at(curNode);
            aQueue.pop();
            for (int idx = 0; idx < adjNodes.size(); idx++ ) { // check every node which is adjacent to curNode;
                auto iter = find(visitedList.begin(), visitedList.end(), adjNodes[idx]);
                if (visitedList.end() == iter && adjNodes[idx] != sID ) {
                    // not found in visitedList_ptr
                    aQueue.push(adjNodes[idx]); // enqueue
                    visitedList.push_back(adjNodes[idx]);
                }
            }
        }

        sort(visitedList.begin(), visitedList.end(), sortNode);
        return visitedList;
    }
    void reset() {
        connectedList.clear();
    }
    vector<pair<int, vector<int>>> getList() {
        return connectedList;
    }
    // comparator
    static bool compareBySize(const pair<int, vector<int>>& n1, const pair<int, vector<int>>& n2) {
        if (n1.second.size() == n2.second.size()) { // order by sID ascii
            string s1 = to_string(n1.first);
            string s2 = to_string(n2.first);
            return s1 < s2;
        }
        else
            return n1.second.size() > n2.second.size();
    }
    static bool sortNode(int id1, int id2) {
        string s1 = to_string(id1);
        string s2 = to_string(id2);
        return s1 < s2;
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
                    cout << "\n### There is no graph and choose 1 first. ###";
                    break;
                }
                // compute
                aGraph.computeBFS();
                aFile.writeFile2(aGraph.getList(), aList.getIdNum());
                cout << "\n<<< There are " << aList.getIdNum() << " IDs in total. >>>\n";
            	break;
            default:
                cout << "\nCommand does not exist!";
                break;
        } // end of switch
    }
}

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <omp.h>
#include <bitset>

using namespace std;

typedef pair<int, int> PII;

struct myCmp
{
    bool operator()(const PII &a, const PII &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

void getGraph(const string &file, vector<vector<int>> &hyperEdge, vector<vector<int>> &hyperNode)
{
    // Construct the file path
    string filename = "/home/C++Projects/graphData/hypergraph/" + file;

    // Open the file
    ifstream fin(filename, ios::in);
    if (!fin.is_open())
    {
        cerr << "Error opening file " << filename << endl;
        return;
    }

    int count = -1;
    int maxID = INT_MIN;

    while (true)
    {
        string str;
        getline(fin, str);

        if (str == "")
            break;

        istringstream ss(str);
        int tmp;
        vector<int> e;

        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
            {
                e.push_back(tmp);
                maxID = max(maxID, tmp);
            }
        }

        if (e.size() == 1)
            continue;

        count++;
        hyperEdge.push_back(e);
    }

    hyperNode.resize(maxID + 1);

    for (int i = 0; i < hyperEdge.size(); i++)
    {
        for (int j = 0; j < hyperEdge[i].size(); j++)
        {
            hyperNode[hyperEdge[i][j]].push_back(i);
        }
    }
}

void kCoreDecomp(vector<vector<int>> &hyperEdge, vector<vector<int>> &hyperNode, vector<int> &cV, vector<int> &cE)
{
    cV.resize(hyperNode.size(), 0);
    cE.resize(hyperEdge.size(), 0);
    set<PII, myCmp> S;
    vector<int> deg(hyperNode.size(), 0);
    vector<bool> visitEdge(hyperEdge.size(), false);
    vector<bool> visitNode(hyperNode.size(), false);

    for (int i = 0; i < hyperNode.size(); i++)
    {
        deg[i] = hyperNode.at(i).size();
        S.insert(make_pair(i, deg[i]));
        visitNode[i] = false;
    }

    int K = 0;

    while (!S.empty())
    {
        pair<int, int> p = *S.begin();
        S.erase(S.begin());
        K = max(K, p.second);
        cV[p.first] = K;
        visitNode[p.first] = true;

        for (auto &edge : hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;

            visitEdge[edge] = true;

            for (auto &node : hyperEdge[edge])
            {
                if (visitNode[node])
                    continue;

                if (S.erase(make_pair(node, deg[node])))
                {
                    deg[node]--;
                    S.insert(make_pair(node, deg[node]));
                }
            }
        }
    }

    for (int i = 0; i < hyperEdge.size(); i++)
    {
        int minValue = INT_MAX;

        for (auto &node : hyperEdge[i])
        {
            minValue = min(minValue, cV[node]);
        }

        cE[i] = minValue;
    }
}

void output(string &file, vector<int> &cV, vector<int> &cE)
{
    ofstream fout("./result/kCoreValue/" + file + "-coreValue.txt");
    if (!fout.is_open())
    {
        cout << "Error opening file\n";
        exit(1);
    }

    for (int i = 0; i < cE.size(); i++)
    {
        fout << cE[i] << " ";
    }

    fout << endl;
    fout.close();
}

int main()
{
    omp_set_num_threads(100);
    string fileList = "NDCC TaMS NDCS TaAU ThAU ThMS CoMH CoGe";

    // Split the fileList into individual file names
    vector<string> files;
    istringstream iss(fileList);
    string file;

    while (getline(iss, file, ' '))
    {
        files.push_back(file);
    }

#pragma omp parallel for
    for (int i = 0; i < int(files.size()); i++)
    {
        vector<vector<int>> hyperEdge;
        vector<vector<int>> hyperNode;
        getGraph(files[i], hyperEdge, hyperNode);
        vector<int> cV;
        vector<int> cE;
        kCoreDecomp(hyperEdge, hyperNode, cV, cE);
        output(files[i], cV, cE);

#pragma omp critical (cout)
        {
            cout << files[i] << " finished" << endl;
        }
    }
}

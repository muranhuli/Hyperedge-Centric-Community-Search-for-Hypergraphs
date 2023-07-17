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

void getGraph(const string &str, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    // Construct the file path
    string filename = "/home/C++Projects/graphData/hypergraph/" + str;

    // Open the file
    ifstream fin(filename, ios::in);
    if (!fin)
        throw runtime_error("Could not open file " + str);

    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);

        if (str == "")
            break;

        istringstream ss(str);
        int tmp;
        vector<int> e;

        // Read the elements in each line
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }

        // Skip if the line contains only one element
        if (e.size() == 1)
            continue;

        count++;
        hyperEdge.push_back(e);

        // Build the mapping of nodes to hyperedges
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

int compute(vector<int> &a, vector<int> &b)
{
    unordered_map<int, bool> map;
    for (auto x : a)
    {
        map[x] = true;
    }

    int count = 0;
    for (auto x : b)
    {
        if (map.count(x) > 0)
        {
            count++;
        }
    }
    return count;
}

void compute(vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x)
{
    incidentHyperedge.resize(hyperEdge.size());
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        for (int j = i + 1; j < hyperEdge.size(); j++)
        {
            if (compute(hyperEdge[i], hyperEdge[j]) >= x)
            {
                incidentHyperedge[i].push_back(j);
                incidentHyperedge[j].push_back(i);
            }
        }
    }
}

void output(vector<vector<int>> &incidentHyperedge, const string &str)
{
    string filename = "./result/incidentHypergraph/" + str;
    ofstream fout(filename, ios::out);
    if (!fout)
        throw runtime_error("Could not open file " + str);
    for (int i = 0; i < incidentHyperedge.size(); i++)
    {
        for (auto &j : incidentHyperedge[i])
            fout << j << " ";
        fout << endl;
    }
    fout.close();
}

int main()
{
    string fileList = "NDCC TaMS NDCS TaAU ThAU ThMS CoMH CoGe";
    vector<string> files;
    istringstream iss(fileList);
    string file;
    while (getline(iss, file, ' '))
    {
        files.push_back(file);
    }

#pragma omp parallel for collapse(2)
    for (int i = 0; i < int(files.size()); i++)
    {
        for (int j = 2; j <= 10; j++)
        {
            vector<vector<int>> hyperEdge;
            unordered_map<int, vector<int>> hyperNode;
            getGraph(files[i], hyperEdge, hyperNode);
            vector<vector<int>> incidentHyperedge;
            compute(hyperEdge, incidentHyperedge, j);
            output(incidentHyperedge, files[i] + "_" + to_string(j));
        }
    }
}

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
const int N = 2 * 1e6;

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

int compute(vector<int> &a, vector<int> &b)
{
    // Compute the intersection count between vectors a and b
    std::unordered_map<int, bool> map;
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

void getGraph(const string &str, vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x)
{
    // Get the hypergraph from the file specified by the string 'str'
    string filename = "/home/C++Projects/graphData/hypergraph/" + str;
    ifstream fin(filename, ios::in);
    if (!fin)
        throw runtime_error("Could not open file " + str);
    int count = -1;
    unordered_map<int, vector<int>> tmpnode;
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
            e.push_back(tmp);
        }
        if (e.size() < x)
            continue;
        count++;
        hyperEdge.push_back(e);
    }
    incidentHyperedge.resize(count + 1);
    vector<bitset<N>> bitmap(count + 1, bitset<N>());
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        for (int j : hyperEdge[i])
        {
            bitmap[i].set(j);
        }
    }
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        for (int j = i + 1; j < hyperEdge.size(); j++)
        {
            if ((bitmap[i] & bitmap[j]).count() >= x)
            {
                incidentHyperedge[i].push_back(j);
                incidentHyperedge[j].push_back(i);
            }
        }
    }
}

void getGraph(const string &str, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    // Get the hypergraph from the file specified by the string 'str'
    string filename = "/home/C++Projects/graphData/hypergraph/" + str;
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
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

void SearchSpaceExpansion(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, vector<vector<int>> &incidentHyperedge, int k, int x, int q, vector<bool> &result)
{
    // Perform search space expansion algorithm
    queue<int> Q;
    vector<int> visited(hyperEdge.size(), 0);
    result.resize(hyperEdge.size());
    // Insert the incident hyperedge of q into the queue Q
    for (auto &e : hyperNode[q])
    {
        Q.push(e);
        visited[e] = true;
    }
    while (!Q.empty())
    {
        int e = Q.front();
        Q.pop();
        if (int(incidentHyperedge[e].size()) < k)
            continue;
        int cnt = 0;
        for (auto &f : incidentHyperedge[e])
        {
            if (int(incidentHyperedge[f].size()) >= k)
                ++cnt;
        }
        if (cnt < k)
            continue;
        result[e] = true;
        for (auto &f : incidentHyperedge[e])
        {
            if (int(incidentHyperedge[f].size()) >= k && !visited[f])
            {
                if (int(incidentHyperedge[f].size()) >= k && !visited[f])
                {
                    Q.push(e);
                    visited[f] = true;
                }
            }
        }
    }
}

// Check if q can reach the hyperedges in result
void judeg(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, int q, vector<bool> &result)
{
    vector<int> visited(hyperEdge.size(), 0);
    queue<int> Q;
    for (auto &e : hyperNode[q])
    {
        Q.push(e);
        visited[e] = true;
    }
    while (!Q.empty())
    {
        int e = Q.front();
        Q.pop();
        if (visited[e])
            continue;
        for (auto &f : hyperNode[e])
        {
            if (!visited[f])
            {
                Q.push(f);
                visited[f] = true;
            }
        }
    }
    for (int i = 0; i < visited.size(); i++)
        if (visited[i] && result[i])
        {
            result[i] = true;
        }
        else
        {
            result[i] = false;
        }
}

double search(vector<vector<int>> &incidentHyperedge, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, int k, int x, int q, vector<bool> &result)
{
    auto t1 = std::chrono::steady_clock::now();
    // Perform kxLocalSearch
    // vector<bool> result;
    SearchSpaceExpansion(hyperEdge, hyperNode, incidentHyperedge, k, x, q, result);
    // end kxLocalSearch
    vector<int> dE(hyperEdge.size(), 0);
    // Compute the degree of each hyperedge
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        if (result[i])
        {
            for (auto &j : incidentHyperedge[i])
            {
                if (result[j])
                {
                    dE[i]++;
                }
            }
        }
    }
    set<PII, myCmp> Q;
    for (int i = 0; i < dE.size(); i++)
    {
        if (result[i])
        {
            Q.insert(make_pair(i, dE[i]));
        }
    }
    while (!Q.empty())
    {
        PII p = *Q.begin();
        Q.erase(Q.begin());
        if (p.second >= k)
            break;
        result[p.first] = false;
        for (auto edge : incidentHyperedge[p.first])
        {
            if (!result[edge])
                continue;
            if (Q.erase(make_pair(edge, dE[edge])))
            {
                dE[edge]--;
                Q.insert(make_pair(edge, dE[edge]));
            }
        }
    }
    // Check if the hyperedges in result can be reached by q
    judeg(hyperEdge, hyperNode, q, result);
    auto t2 = std::chrono::steady_clock::now();
    double dr_ns = std::chrono::duration<double, std::nano>(t2 - t1).count();
    return dr_ns;
}

int main()
{
    omp_set_num_threads(100);
    string fileList = "NDCC TaMS NDCS TaAU ThAU ThMS CoMH CoGe";
    // string fileList = "NDCC";
    // Read the file names from fileList into a vector
    std::vector<std::string> files;
    std::istringstream iss(fileList);
    std::string file;
    while (std::getline(iss, file, ' '))
    {
        files.push_back(file);
    }
    ofstream fout("./result/kxLocalSearch/result.txt", ios::out);
    int cnt = 0;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < int(files.size()); i++)
    {
        for (int j = 2; j <= 10; j++)
        {
            vector<vector<int>> hyperEdge;
            unordered_map<int, vector<int>> hyperNode;
            getGraph(files[i], hyperEdge, hyperNode);
            vector<bool> result;
            int k = 5;
            int q = 5;
            ifstream fin("./result/incidentHypergraph/" + files[i] + "_" + to_string(j));
            vector<vector<int>> incidentHyperedge(hyperEdge.size());
            unordered_map<int,int> ID;
            if (fin)
            {
                cout << "Reading..." << endl;
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
                    while (ss >> tmp)
                    {
                        e.push_back(tmp);
                    }
                    count++;
                    incidentHyperedge[count] = e;
                }
            }
            fin.close();
            fin.open("./result/kxCoreValue/" + files[i] + "-" + to_string(j) + "-coreValue.txt");
            std::string line;
            std::getline(fin, line); // Read the line of the file

            std::istringstream iss(line);
            std::unordered_set<int> uniqueElements;

            int number;
            int cnt0=0;
            while (iss >> number)
            {
                ID[number] = hyperEdge[cnt0][0];
                uniqueElements.insert(number);
                ++cnt0;
            }
            fin.close();

            for (auto p:uniqueElements)
            {
                double t = search(incidentHyperedge, hyperEdge, hyperNode, p, j, ID[p], result);
#pragma omp critical
                {
                    fout << files[i] << " " << p << " " << j << " " << ID[p] << " " << t << " ";
                    for (int i = 0; i < result.size(); i++)
                    {
                        if (result[i])
                        {
                            fout << i << " ";
                        }
                    }
                    fout << endl;
                    fout.flush();
                    ++cnt;
                    cout << "cnt = " << cnt << endl;
                }
            }
        }
    }
    fout.close();
}

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
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

double kxCoreDecomp(const string &file, vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x, vector<int> &cE)
{
    // Perform k-x core decomposition on the hypergraph
    auto t1 = std::chrono::steady_clock::now();
    cE.resize(hyperEdge.size(), 0);
    vector<int> dE(hyperEdge.size(), 0);
    vector<bool> visitedEdge(hyperEdge.size(), false);
    // Compute the degree of each edge
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        dE[i] = int(incidentHyperedge.at(i).size());
    }
    set<PII, myCmp> Q;
    for (int i = 0; i < dE.size(); i++)
    {
        Q.insert(make_pair(i, dE[i]));
    }
    int k = 1;
    while (!Q.empty())
    {
        PII p = *Q.begin();
        Q.erase(Q.begin());
        k = max(k, p.second);
        cE[p.first] = k;
        visitedEdge[p.first] = true;
        for (auto edge : incidentHyperedge[p.first])
        {
            if (visitedEdge[edge])
                continue;
            if (Q.erase(make_pair(edge, dE[edge])))
            {
                dE[edge]--;
                Q.insert(make_pair(edge, dE[edge]));
            }
        }
    }
    auto t2 = std::chrono::steady_clock::now();
    double dr_ns = std::chrono::duration<double, std::nano>(t2 - t1).count();
    return dr_ns;
}

struct DSU
{
    vector<int> parent;
    DSU(int n) : parent(n)
    {
        for (int i = 0; i < n; ++i)
        {
            parent[i] = i;
        }
    }
    int find(int x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    void merge(int x, int y)
    {
        parent[find(x)] = find(y);
    }
};

void constructTree(vector<vector<int>> &hyperEdge, vector<int> &cE, unordered_map<int, vector<vector<int>>> &tree)
{
    // Construct the tree from the hypergraph using the k-x core decomposition
    int n = hyperEdge.size();
    unordered_map<int, DSU> dsus;
    for (int i = 0; i < n; ++i)
    {
        int c = cE[i];
        auto it = dsus.find(c);
        if (it == dsus.end())
        {
            dsus.emplace(c, DSU(n));
            it = dsus.find(c);
        }
        for (int j = 1; j < hyperEdge[i].size(); ++j)
        {
            it->second.merge(hyperEdge[i][0], hyperEdge[i][j]);
        }
    }
    for (auto &p : dsus)
    {
        int c = p.first;
        DSU &dsu = p.second;
        unordered_map<int, vector<int>> components;
        for (int i = 0; i < n; ++i)
        {
            components[dsu.find(i)].push_back(i);
        }
        for (auto &p : components)
        {
            tree[c].push_back(p.second);
        }
    }
}

vector<int> search(int node, int k, unordered_map<int, vector<vector<int>>> &tree)
{
    // Search for the given node in the k-x core decomposition tree
    for (auto &component : tree[k])
    {
        if (find(component.begin(), component.end(), node) != component.end())
        {
            return component;
        }
    }

    // Return an empty vector if the node is not found in any component
    return vector<int>();
}

int main()
{
    omp_set_num_threads(100);
    string fileList = "NDCC TaMS NDCS TaAU ThAU ThMS CoMH CoGe";
    // Read the file names from fileList into a vector
    std::vector<std::string> files;
    std::istringstream iss(fileList);
    std::string file;
    while (std::getline(iss, file, ' '))
    {
        files.push_back(file);
    }
#pragma omp parallel for collapse(2)
    for (int i = 0; i < int(files.size()); i++)
    {
        for (int j = 2; j <= 10; j++)
        {
            vector<vector<int>> hyperEdge;
            vector<vector<int>> incidentHyperedge;
            getGraph(files[i], hyperEdge, incidentHyperedge, j);
            vector<int> cE;
            double t = 0;
            t = kxCoreDecomp(files[i], hyperEdge, incidentHyperedge, j, cE);
            unordered_map<int, vector<vector<int>>> tree;
            constructTree(hyperEdge, cE, tree);
            int node = 2345;
            int k = 23;
            search(node, k, tree);
            cout << "node " << node << " k " << k << " time " << t << endl;
        }
    }
}

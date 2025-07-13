#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

class Graph {
    int V;
    list<int>* adj;
public:
    Graph(int V) {
        this->V = V;
        adj = new list<int>[V];
    }

    ~Graph() {
        delete[] adj;
    }

    void addEdge(int v, int w) {
        adj[v].push_back(w);
        adj[w].push_back(v);
    }

    void DFS(int v, vector<bool>& visited) {
        visited[v] = true;
        for (int u : adj[v])
            if (!visited[u])
                DFS(u, visited);
    }

    bool isConnected() {
        vector<bool> visited(V, false);
        int i;
        for (i = 0; i < V; i++) {
            if (!adj[i].empty()) break;
        }
        if (i == V) return true;

        DFS(i, visited);
        for (int j = 0; j < V; j++) {
            if (!visited[j] && !adj[j].empty())
                return false;
        }
        return true;
    }

    bool isEulerian() {
        if (!isConnected()) return false;
        for (int i = 0; i < V; i++) {
            if (adj[i].size() % 2 != 0)
                return false;
        }
        return true;
    }
};

void addRandomHamiltonianCycle(vector<vector<int>>& G, int n) {
    vector<int> nodes(n);
    for (int i = 0; i < n; i++) nodes[i] = i;
    shuffle(nodes.begin(), nodes.end(), mt19937(random_device()()));
    for (int i = 0; i < n - 1; i++) {
        G[nodes[i]][nodes[i + 1]] = 1;
        G[nodes[i + 1]][nodes[i]] = 1;
    }
    G[nodes[n - 1]][nodes[0]] = 1;
    G[nodes[0]][nodes[n - 1]] = 1;
}

void fillGraphToDensity(vector<vector<int>>& G, int n, int targetEdges) {
    int currentEdges = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (G[i][j]) currentEdges++;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);

    while (currentEdges < targetEdges) {
        int u = dis(gen);
        int v = dis(gen);
        if (u == v || G[u][v]) continue;
        G[u][v] = G[v][u] = 1;
        currentEdges++;
    }
}

void timeDFSMatrix(const vector<vector<int>>& G, int n) {
    vector<bool> visited(n, false);
    auto start = chrono::high_resolution_clock::now();
    function<void(int)> dfs = [&](int v) {
        visited[v] = true;
        for (int u = 0; u < n; u++) {
            if (G[v][u] && !visited[u])
                dfs(u);
        }
    };
    dfs(0);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << duration.count() << " ms" << endl;
}

int main() {
    int ns[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    double bs[] = {0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875};

    for (int i = 0; i < 10; i++) {
        int n = ns[i];
        for (int x = 0; x < 7; x++) {
            double b = bs[x];
            int maxEdges = n * (n - 1) / 2;
            int targetEdges = static_cast<int>(b * maxEdges);

            vector<vector<int>> G(n, vector<int>(n, 0));
            addRandomHamiltonianCycle(G, n);
            fillGraphToDensity(G, n, targetEdges);

            Graph g(n);
            for (int u = 0; u < n; u++)
                for (int v = u + 1; v < n; v++)
                    if (G[u][v]) g.addEdge(u, v);

            cout << "V: " << n << " Density: " << b;
            cout << " | Eulerian: " << (g.isEulerian() ? "Yes" : "No");
            cout << " | DFS Time: ";
            timeDFSMatrix(G, n);
        }
        cout << endl;
    }

    return 0;
}

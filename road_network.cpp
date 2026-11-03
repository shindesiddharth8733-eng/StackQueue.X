/*
 * Project : Road Network and Route Finder using Graphs (with BST city index)
 * Course  : Data Structures and Algorithms [R25-EC-PCC-202]
 * CCE III : Unit V - Trees and Graphs
 *
 * Data structures (all implemented manually, NO STL containers):
 *   - Graph (adjacency list using linked lists)   -> road network
 *   - Binary Search Tree                          -> fast city-name index
 *   - Circular Queue  (for BFS)
 *   - Stack           (for path reconstruction)
 *   - Min-Heap        (priority queue for Dijkstra)
 *
 * Algorithms: BFS, DFS, Dijkstra shortest path, connected components,
 *             BST insert/search/inorder, insertion sort, selection sort.
 * OOP       : classes, constructors, destructors, operator overloading.
 */

#include <iostream>
#include <cstring>
#include <cctype>
using namespace std;

const int MAX_CITIES = 50;       // maximum number of cities
const int NAME_LEN   = 40;       // maximum length of a city name
const int INF        = 1000000000;

/* ---------------------------------------------------------------
 * Helper: case-insensitive string comparison (like strcmp)
 * --------------------------------------------------------------- */
int cmpIgnoreCase(const char* a, const char* b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++; b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* ---------------------------------------------------------------
 * Class Distance : wraps road length in km, with operator overloading
 * --------------------------------------------------------------- */
class Distance {
    int km;
public:
    Distance(int k = 0) : km(k) {}                       // constructor
    int getKm() const { return km; }

    Distance operator+(const Distance& o) const { return Distance(km + o.km); }
    bool operator<(const Distance& o) const { return km < o.km; }
    bool operator>(const Distance& o) const { return km > o.km; }
    bool operator==(const Distance& o) const { return km == o.km; }

    friend ostream& operator<<(ostream& os, const Distance& d) {
        os << d.km << " km";
        return os;
    }
};

/* ---------------------------------------------------------------
 * Circular Queue of ints (used in BFS)
 * --------------------------------------------------------------- */
class IntQueue {
    int* data;
    int cap, front, rear, count;
public:
    IntQueue(int capacity) : cap(capacity), front(0), rear(0), count(0) {
        data = new int[cap];
    }
    ~IntQueue() { delete[] data; }
    bool empty() const { return count == 0; }
    bool full() const { return count == cap; }
    void enqueue(int x) {
        if (full()) return;
        data[rear] = x;
        rear = (rear + 1) % cap;
        count++;
    }
    int dequeue() {
        int x = data[front];
        front = (front + 1) % cap;
        count--;
        return x;
    }
};

/* ---------------------------------------------------------------
 * Stack of ints (used to reverse the shortest path)
 * --------------------------------------------------------------- */
class IntStack {
    int* data;
    int cap, topIdx;
public:
    IntStack(int capacity) : cap(capacity), topIdx(-1) { data = new int[cap]; }
    ~IntStack() { delete[] data; }
    bool empty() const { return topIdx == -1; }
    void push(int x) { if (topIdx < cap - 1) data[++topIdx] = x; }
    int pop() { return data[topIdx--]; }
};

/* ---------------------------------------------------------------
 * Min-Heap of (key, value) pairs - priority queue for Dijkstra
 * --------------------------------------------------------------- */
class MinHeap {
    int* key;       // priority (distance)
    int* val;       // city id
    int size, cap;

    void swapAt(int i, int j) {
        int t = key[i]; key[i] = key[j]; key[j] = t;
        t = val[i]; val[i] = val[j]; val[j] = t;
    }
public:
    MinHeap(int capacity) : size(0), cap(capacity) {
        key = new int[cap];
        val = new int[cap];
    }
    ~MinHeap() { delete[] key; delete[] val; }
    bool empty() const { return size == 0; }

    void push(int k, int v) {
        if (size == cap) return;
        int i = size++;
        key[i] = k; val[i] = v;
        while (i > 0 && key[(i - 1) / 2] > key[i]) {      // sift up
            swapAt(i, (i - 1) / 2);
            i = (i - 1) / 2;
        }
    }
    void pop(int& k, int& v) {
        k = key[0]; v = val[0];
        size--;
        key[0] = key[size]; val[0] = val[size];
        int i = 0;
        while (true) {                                    // sift down
            int l = 2 * i + 1, r = 2 * i + 2, m = i;
            if (l < size && key[l] < key[m]) m = l;
            if (r < size && key[r] < key[m]) m = r;
            if (m == i) break;
            swapAt(i, m);
            i = m;
        }
    }
};

/* ---------------------------------------------------------------
 * Binary Search Tree : city name -> city id
 * --------------------------------------------------------------- */
struct BSTNode {
    char name[NAME_LEN];
    int id;
    BSTNode *left, *right;
    BSTNode(const char* n, int i) : id(i), left(nullptr), right(nullptr) {
        strncpy(name, n, NAME_LEN - 1);
        name[NAME_LEN - 1] = '\0';
    }
};

class CityBST {
    BSTNode* root;

    BSTNode* insert(BSTNode* node, const char* name, int id) {
        if (node == nullptr) return new BSTNode(name, id);
        if (cmpIgnoreCase(name, node->name) < 0)
            node->left = insert(node->left, name, id);
        else if (cmpIgnoreCase(name, node->name) > 0)
            node->right = insert(node->right, name, id);
        return node;                                      // duplicates ignored
    }
    void inorder(BSTNode* node, int& counter) const {
        if (node == nullptr) return;
        inorder(node->left, counter);
        cout << "  " << ++counter << ". " << node->name << endl;
        inorder(node->right, counter);
    }
    void destroy(BSTNode* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
public:
    CityBST() : root(nullptr) {}
    ~CityBST() { destroy(root); }

    void insert(const char* name, int id) { root = insert(root, name, id); }

    int search(const char* name) const {                  // returns id or -1
        BSTNode* cur = root;
        while (cur != nullptr) {
            int c = cmpIgnoreCase(name, cur->name);
            if (c == 0) return cur->id;
            cur = (c < 0) ? cur->left : cur->right;
        }
        return -1;
    }
    void printInorder() const {
        int counter = 0;
        inorder(root, counter);
        if (counter == 0) cout << "  (no cities)" << endl;
    }
};

/* ---------------------------------------------------------------
 * Graph edge node (adjacency list, singly linked list)
 * --------------------------------------------------------------- */
struct EdgeNode {
    int dest;
    Distance weight;
    EdgeNode* next;
    EdgeNode(int d, Distance w, EdgeNode* n) : dest(d), weight(w), next(n) {}
};

/* ---------------------------------------------------------------
 * Class RoadNetwork : undirected weighted graph of cities
 * --------------------------------------------------------------- */
class RoadNetwork {
    char names[MAX_CITIES][NAME_LEN];
    EdgeNode* adj[MAX_CITIES];
    int n;                       // number of cities
    CityBST index;               // BST for quick name -> id lookup

    void insertEdge(int u, int v, Distance w) {
        for (EdgeNode* e = adj[u]; e != nullptr; e = e->next)
            if (e->dest == v) { e->weight = w; return; }   // update existing
        adj[u] = new EdgeNode(v, w, adj[u]);
    }
    bool deleteEdge(int u, int v) {
        EdgeNode *cur = adj[u], *prev = nullptr;
        while (cur != nullptr && cur->dest != v) { prev = cur; cur = cur->next; }
        if (cur == nullptr) return false;
        if (prev == nullptr) adj[u] = cur->next; else prev->next = cur->next;
        delete cur;
        return true;
    }
    int degree(int u) const {
        int d = 0;
        for (EdgeNode* e = adj[u]; e != nullptr; e = e->next) d++;
        return d;
    }
    void dfsUtil(int u, bool visited[]) const {
        visited[u] = true;
        cout << names[u] << "  ";
        for (EdgeNode* e = adj[u]; e != nullptr; e = e->next)
            if (!visited[e->dest]) dfsUtil(e->dest, visited);
    }
public:
    RoadNetwork() : n(0) {
        for (int i = 0; i < MAX_CITIES; i++) adj[i] = nullptr;
    }
    ~RoadNetwork() {
        for (int i = 0; i < n; i++) {
            EdgeNode* e = adj[i];
            while (e != nullptr) { EdgeNode* t = e; e = e->next; delete t; }
        }
    }

    int cityCount() const { return n; }
    const char* cityName(int id) const { return names[id]; }
    int findCity(const char* name) const { return index.search(name); }

    int addCity(const char* name) {
        if (n == MAX_CITIES) { cout << "Network is full.\n"; return -1; }
        if (findCity(name) != -1) { cout << "City already exists.\n"; return -1; }
        strncpy(names[n], name, NAME_LEN - 1);
        names[n][NAME_LEN - 1] = '\0';
        index.insert(names[n], n);
        return n++;
    }
    void addRoad(int u, int v, int km) {
        insertEdge(u, v, Distance(km));
        insertEdge(v, u, Distance(km));
    }
    bool removeRoad(int u, int v) {
        bool a = deleteEdge(u, v);
        bool b = deleteEdge(v, u);
        return a && b;
    }

    void display() const {
        cout << "\n--- Road Network (adjacency list) ---\n";
        if (n == 0) { cout << "No cities yet.\n"; return; }
        for (int i = 0; i < n; i++) {
            cout << names[i] << " -> ";
            if (adj[i] == nullptr) cout << "(no roads)";
            for (EdgeNode* e = adj[i]; e != nullptr; e = e->next)
                cout << names[e->dest] << "(" << e->weight << ")  ";
            cout << endl;
        }
    }

    /* Breadth First Search using manual queue */
    void bfs(int start) const {
        bool visited[MAX_CITIES] = {false};
        IntQueue q(MAX_CITIES);
        visited[start] = true;
        q.enqueue(start);
        cout << "BFS order: ";
        while (!q.empty()) {
            int u = q.dequeue();
            cout << names[u] << "  ";
            for (EdgeNode* e = adj[u]; e != nullptr; e = e->next)
                if (!visited[e->dest]) {
                    visited[e->dest] = true;
                    q.enqueue(e->dest);
                }
        }
        cout << endl;
    }

    /* Depth First Search (recursive) */
    void dfs(int start) const {
        bool visited[MAX_CITIES] = {false};
        cout << "DFS order: ";
        dfsUtil(start, visited);
        cout << endl;
    }

    /* Dijkstra's algorithm with a min-heap */
    void shortestPath(int src, int dst) const {
        Distance dist[MAX_CITIES];
        int parent[MAX_CITIES];
        bool done[MAX_CITIES];
        for (int i = 0; i < n; i++) { dist[i] = Distance(INF); parent[i] = -1; done[i] = false; }
        dist[src] = Distance(0);

        MinHeap heap(MAX_CITIES * MAX_CITIES * 2 + 10);
        heap.push(0, src);
        while (!heap.empty()) {
            int d, u;
            heap.pop(d, u);
            if (done[u]) continue;                       // stale heap entry
            done[u] = true;
            if (u == dst) break;
            for (EdgeNode* e = adj[u]; e != nullptr; e = e->next) {
                Distance nd = dist[u] + e->weight;       // operator+
                if (nd < dist[e->dest]) {                // operator<
                    dist[e->dest] = nd;
                    parent[e->dest] = u;
                    heap.push(nd.getKm(), e->dest);
                }
            }
        }
        if (dist[dst].getKm() == INF) {
            cout << "No route exists between " << names[src] << " and " << names[dst] << ".\n";
            return;
        }
        IntStack path(MAX_CITIES);                       // reverse the path
        for (int v = dst; v != -1; v = parent[v]) path.push(v);
        cout << "Shortest route: ";
        while (!path.empty()) {
            cout << names[path.pop()];
            if (!path.empty()) cout << " -> ";
        }
        cout << "\nTotal distance : " << dist[dst] << endl;
    }

    /* Count connected components using DFS-style marking with a stack */
    int countComponents() const {
        bool visited[MAX_CITIES] = {false};
        int comps = 0;
        for (int s = 0; s < n; s++) {
            if (visited[s]) continue;
            comps++;
            IntStack st(MAX_CITIES * 2);
            st.push(s); visited[s] = true;
            while (!st.empty()) {
                int u = st.pop();
                for (EdgeNode* e = adj[u]; e != nullptr; e = e->next)
                    if (!visited[e->dest]) { visited[e->dest] = true; st.push(e->dest); }
            }
        }
        return comps;
    }

    /* Insertion sort on city ids by name (A-Z) */
    void listSortedByName() const {
        int order[MAX_CITIES];
        for (int i = 0; i < n; i++) order[i] = i;
        for (int i = 1; i < n; i++) {
            int key = order[i], j = i - 1;
            while (j >= 0 && cmpIgnoreCase(names[order[j]], names[key]) > 0) {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = key;
        }
        cout << "\n--- Cities sorted by name (insertion sort) ---\n";
        for (int i = 0; i < n; i++) cout << "  " << i + 1 << ". " << names[order[i]] << endl;
    }

    /* Selection sort on city ids by number of roads (descending) */
    void listByConnections() const {
        int order[MAX_CITIES];
        for (int i = 0; i < n; i++) order[i] = i;
        for (int i = 0; i < n - 1; i++) {
            int best = i;
            for (int j = i + 1; j < n; j++)
                if (degree(order[j]) > degree(order[best])) best = j;
            int t = order[i]; order[i] = order[best]; order[best] = t;
        }
        cout << "\n--- Cities by number of roads (selection sort) ---\n";
        for (int i = 0; i < n; i++)
            cout << "  " << names[order[i]] << " : " << degree(order[i]) << " road(s)\n";
    }

    void listAlphabeticalBST() const {
        cout << "\n--- Cities in alphabetical order (BST inorder traversal) ---\n";
        index.printInorder();
    }
};

/* ---------------------------------------------------------------
 * Input helpers
 * --------------------------------------------------------------- */
int readInt(const char* prompt) {
    int x;
    cout << prompt;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid number, try again: ";
    }
    cin.ignore(10000, '\n');
    return x;
}

void readLine(const char* prompt, char* buf) {
    cout << prompt;
    cin.getline(buf, NAME_LEN);
    // trim trailing spaces / CR
    int len = (int)strlen(buf);
    while (len > 0 && (buf[len - 1] == ' ' || buf[len - 1] == '\r')) buf[--len] = '\0';
}

int askCity(const RoadNetwork& net, const char* prompt) {
    char name[NAME_LEN];
    readLine(prompt, name);
    int id = net.findCity(name);
    if (id == -1) cout << "City '" << name << "' not found.\n";
    return id;
}

/* Pre-load a sample map of Maharashtra (approximate road distances) */
void loadSampleData(RoadNetwork& net) {
    const char* cities[] = {"Nashik", "Sangamner", "Shirdi", "Ahmednagar",
                            "Pune", "Mumbai", "Aurangabad", "Nagpur"};
    for (int i = 0; i < 8; i++) net.addCity(cities[i]);
    net.addRoad(0, 1, 85);    // Nashik - Sangamner
    net.addRoad(0, 2, 90);    // Nashik - Shirdi
    net.addRoad(0, 5, 167);   // Nashik - Mumbai
    net.addRoad(0, 6, 180);   // Nashik - Aurangabad
    net.addRoad(1, 2, 35);    // Sangamner - Shirdi
    net.addRoad(1, 4, 130);   // Sangamner - Pune
    net.addRoad(2, 3, 85);    // Shirdi - Ahmednagar
    net.addRoad(3, 4, 120);   // Ahmednagar - Pune
    net.addRoad(3, 6, 115);   // Ahmednagar - Aurangabad
    net.addRoad(4, 5, 150);   // Pune - Mumbai
    net.addRoad(4, 6, 235);   // Pune - Aurangabad
    // Nagpur is intentionally left disconnected to demonstrate connectivity check
}

void showMenu() {
    cout << "\n=========== ROAD NETWORK & ROUTE FINDER ===========\n"
         << " 1. Display road network\n"
         << " 2. Add a city\n"
         << " 3. Add / update a road\n"
         << " 4. Remove a road\n"
         << " 5. Search a city (BST)\n"
         << " 6. BFS traversal\n"
         << " 7. DFS traversal\n"
         << " 8. Find shortest route (Dijkstra)\n"
         << " 9. Check network connectivity\n"
         << "10. List cities sorted by name (insertion sort)\n"
         << "11. List cities by number of roads (selection sort)\n"
         << "12. List cities alphabetically (BST inorder)\n"
         << " 0. Exit\n"
         << "===================================================\n";
}

int main() {
    RoadNetwork net;
    loadSampleData(net);
    int choice;
    do {
        showMenu();
        choice = readInt("Enter your choice: ");
        char name[NAME_LEN];
        switch (choice) {
        case 1:
            net.display();
            break;
        case 2:
            readLine("Enter new city name: ", name);
            if (strlen(name) == 0) { cout << "Name cannot be empty.\n"; break; }
            if (net.addCity(name) != -1) cout << "City added.\n";
            break;
        case 3: {
            int u = askCity(net, "Enter first city : ");
            if (u == -1) break;
            int v = askCity(net, "Enter second city: ");
            if (v == -1) break;
            if (u == v) { cout << "A road needs two different cities.\n"; break; }
            int km = readInt("Enter distance in km: ");
            if (km <= 0) { cout << "Distance must be positive.\n"; break; }
            net.addRoad(u, v, km);
            cout << "Road saved.\n";
            break;
        }
        case 4: {
            int u = askCity(net, "Enter first city : ");
            if (u == -1) break;
            int v = askCity(net, "Enter second city: ");
            if (v == -1) break;
            cout << (net.removeRoad(u, v) ? "Road removed.\n" : "No such road.\n");
            break;
        }
        case 5: {
            readLine("Enter city name to search: ", name);
            int id = net.findCity(name);
            if (id == -1) cout << "City not found.\n";
            else cout << "Found: " << net.cityName(id) << " (id " << id << ")\n";
            break;
        }
        case 6: {
            int s = askCity(net, "Start city: ");
            if (s != -1) net.bfs(s);
            break;
        }
        case 7: {
            int s = askCity(net, "Start city: ");
            if (s != -1) net.dfs(s);
            break;
        }
        case 8: {
            int s = askCity(net, "Source city     : ");
            if (s == -1) break;
            int d = askCity(net, "Destination city: ");
            if (d == -1) break;
            net.shortestPath(s, d);
            break;
        }
        case 9: {
            int c = net.countComponents();
            if (c <= 1) cout << "The network is fully connected.\n";
            else cout << "The network is NOT connected. Separate groups: " << c << endl;
            break;
        }
        case 10: net.listSortedByName(); break;
        case 11: net.listByConnections(); break;
        case 12: net.listAlphabeticalBST(); break;
        case 0:  cout << "Thank you!\n"; break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
    return 0;
}

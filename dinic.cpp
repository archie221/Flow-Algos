#include<bits/stdc++.h>
using namespace std;
 
#define ll long long
#define ar array
#define pb push_back
#define mp make_pair
#define ff first
#define ss second
#define pii pair<int, int>
#define vi vector<int>
#define si set<int>
#define mset(arr, val) memset(arr, val, sizeof(arr))
#define f(i, n) for(int (i)=0;(i)<(n);(i)++)
#define dbg(x) cout<<#x<<" - "<<x<<"\n";
#define all(x) (x).begin(), (x).end()
 
const int MOD = 1e9+7;
const int mxN = 1e5;
 
 
// *****************************START**********************************
// *****************Dinic's algorithm for Max Flow*********************
 
// FlowEdge has to, from, flow and cap
struct FlowEdge {
    int from, to;
    ll cap, flow = 0;
    FlowEdge(int from, int to, ll cap) : from(from), to(to), cap(cap) {}
};
 
struct Dinic{
    // ----------initialize----------
    const ll FINF = 1e18;
    vector<FlowEdge> edges; // store all the edges
    vector<vector<int>> adj; // adjecency list
    int n, s, t, m = 0; // n - no. of vertices, s- source, t - sink, m - edge counter 
    vector<int> level, next; // level - for level graph, next - for dfs dead end heuristics
    queue<int> q; // for level graph bfs
 
    Dinic(int n, int s, int t): n(n), s(s), t(t) {
        // resize all required space
        adj.resize(n);
        level.resize(n);
        next.resize(n);
    }
 
    // -----------add edges to the graph--------------
    void add_edge(int from, int to, int cap){
        edges.emplace_back(from, to, cap); // graph edge
        edges.emplace_back(to, from, 0); // reverse edge
        adj[from].pb(m++); // graph edge
        adj[to].pb(m++); // reverse edge
    }
 
    // ------------create level graph---------------
    bool bfs(){
        while(!q.empty()){
            int from = q.front();
            q.pop();
            for(int edge_id : adj[from]){ // all the edges from source
                // continue if next vertex is already visited or has negative capacity
                if((edges[edge_id].cap - edges[edge_id].flow <= 0)||(level[edges[edge_id].to] != -1)) continue;
                level[edges[edge_id].to] = level[from] + 1; // extend level graph
                q.push(edges[edge_id].to); // add next vertex to bfs queue
            }
        }
        return level[t]!=-1;
    }
 
    // ------------find augmented path----------------
    ll dfs(int v, ll pushed){ // pushed - bottleneck flow in current augmented path
        if(pushed==0)return 0; // bottleneck reached 0
        if(v == t) return pushed; // reached sink
        for(;next[v]<adj[v].size();next[v]++){ // increment next[v] if dead end found (heuristic)
            int edge_id = adj[v][next[v]];
            int _to = edges[edge_id].to;
            // continue if there is no remaining capacity or level is not adjacent
            if( edges[edge_id].cap - edges[edge_id].flow <= 0 || level[_to] != level[v]+1 ) continue;
            // extend the path with updated bottleneck value on this path
            ll bottleneck = dfs(_to, min(pushed, edges[edge_id].cap - edges[edge_id].flow)); 
            if(bottleneck>0){
                edges[edge_id].flow += bottleneck; // update flow edge
                edges[edge_id^1].flow -= bottleneck; // update reverse edge
                return bottleneck;
            }
        }
        return 0; 
    }
 
    // ---------------find flow-----------------
    ll flow(){
        ll f = 0;
        while(true){ // loop until blocking flow is found
            fill(level.begin(), level.end(), -1); // recreate level graph after each iteration
            level[s] = 0; // distance of source from source is 0
            q.push(s); // for bfs
            if(!bfs()) break; // bfs returns 0 if blocking flow found, then break
            fill(next.begin(), next.end(), 0); // for finding augmented path
            while(ll pushed = dfs(s, FINF)) // loop until blocking flow is found
                f += pushed; // add the bottleneck flow to the total flow
        }
        return f;
    }
};
// ********Dinic's algorithm for Max Flow**********
// ********************END*************************
 
int main() {
    // ios_base::sync_with_stdio(false);
    // cin.tie(NULL);
    // cout.tie(NULL);
    int n, m;
    cin >> n >> m;
    Dinic graph(n, 0, n-1);
    f(i, m){
        int u, v;
        ll c;
        cin >> u >> v >> c;
        graph.add_edge(u-1, v-1, c);
    }
    cout << graph.flow() << "\n";
}
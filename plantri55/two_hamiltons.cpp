#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
    os << "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i != 0) os << ", ";
        os << v[i];
    }
    os << "]";
    return os;
}

string vec_to_str(vector<int> v){
    string res = "";
    res.append("(");
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i != 0) res.append(",");
        res.append(to_string(v[i]));
    }
    res.append(")");

    return res;
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m)
{
    os << "{";
    bool first = true;
    for (const auto& [key, value] : m) {
        if (!first) os << ", ";
        first = false;
        os << "\"" << key << "\": " << value;
    }
    os << "}";
    return os;
}

vector<int> calc_canon_code(vector<int> square, int n, int m, vector<vector<int>> &neigh){
    int cannon_edge = m + 1;
    vector<int> real_cannon_code(2 * m + n + 1, 2 * n + 1);
    for (auto v: square){
        for (int dir = -1; dir < 2; dir += 2){
            for (int e = 0; e < neigh[v].size(); e++){
                // colour of vertex v
                // if some vertex does not have colour, it was discovered by that edge
                vector<int> colour(n, -1);
                colour[v] = n + 1;

                // label[i] is which vertex whas discovered i-th
                vector<int> label(1);
                label.push_back(v);

                // first edge of i-th vertex
                vector<int> first_edge(n);
                first_edge[v] = e;

                // best code for this vertex
                vector<int> cannon_code;
                cannon_code.push_back(colour[v]);

                for (int i = 1; i <= n; i++){
                    int curr = label[i];
                    int size = neigh[curr].size();
                    int fe = first_edge[curr];
                    for (int j = 0; j < size; j++){
                        int next = neigh[curr][(size + (fe + j * dir))%size];
                        if (colour[next] == -1){
                            colour[next] = n + label.size();
                            label.push_back(next);
                            cannon_code.push_back(colour[next]);
                            for (int k = 0; k < neigh[next].size(); k++){
                                if (neigh[next][k] == curr){
                                    first_edge[next] = k;
                                    break;
                                }
                            }
                        }
                        else {
                            cannon_code.push_back(colour[next] - n);
                        }
                    }
                    cannon_code.push_back(0);
                }
                for (int i = 0; i < cannon_code.size(); i++){
                    if (real_cannon_code[i] < cannon_code[i]){
                        break;
                    }

                    if (cannon_code[i] < real_cannon_code[i]){
                        real_cannon_code = cannon_code;
                        break;
                    }
                }
            }
        }
    }

    return real_cannon_code;
}

vector<int> common_neighbours(int v1, int v2, vector<vector<int>> &adj_list, vector<vector<int>> &adj_matrix){
    vector<int> ans;
    for (int neigh: adj_list[v1]){
        if(adj_matrix[v2][neigh]){
            ans.push_back(neigh);
        }
    }

    return ans;
}

bool check_single(int start, int end, long long n, vector<int> forb, vector<vector<int>>& adj) {
    long long full = (1 << n) - 1;

    // dp[mask][v] is true if some path visits exactly the vertices
    // in mask and ends at vertex v
    vector<vector<bool>> dp(1 << n, vector<bool>(n, false));
    dp[1 << start][start] = true;

    for (long long mask = 1; mask <= full; mask++) {
        for (int v = 0; v < n; v++) {

            // skip vertices that are not in the mask or cannot end a path
            if (!((mask >> v) & 1) || !dp[mask][v])
                continue;

            // extend the path to an unused neighbour u
            for (int u = 0; u < n; u++) {
                if (!((mask >> u) & 1) && adj[v][u])
                    dp[mask | (1 << u)][u] = true;
            }
        }
    }

    for (auto i: forb){
        full = full ^ (1 << i);
    }

    return dp[full][end];
}

bool check_double(int s1, int e1, int s2, int e2, long long n, vector<vector<int>> adj){
    vector<vector<int>> adj_copy = adj;
    adj_copy[s1][e1] = 0;
    adj_copy[s1][s2] = 0;
    adj_copy[s1][e2] = 0;
    adj_copy[e1][s2] = 0;
    adj_copy[e1][e2] = 0;
    adj_copy[s2][e2] = 0;

    long long full = (1 << n) - 1;

    // dp[mask][v] is true if some path visits exactly the vertices
    // in mask and ends at vertex v
    vector<vector<bool>> dp(1 << n, vector<bool>(n, false));
    dp[1 << s1][s1] = true;
    dp[1 << s2][s2] = true;

    for (long long mask = 1; mask <= full; mask++) {
        for (int v = 0; v < n; v++) {

            // skip vertices that are not in the mask or cannot end a path
            if (!((mask >> v) & 1) || !dp[mask][v])
                continue;

            // extend the path to an unused neighbour u
            for (int u = 0; u < n; u++) {
                if (!((mask >> u) & 1) && adj_copy[v][u])
                    dp[mask | (1 << u)][u] = true;
            }
        }
    }

    for (long long mask1 = 1; mask1 <= full; mask1++){
        if ((mask1 & (1 << s1)) && (mask1 & (1 << e1))){
            long long mask2 = full ^ mask1;
            if ((mask2 & (1 << s2)) && (mask2 & (1 << e2))){
                if (dp[mask1][e1] && dp[mask2][e2]){
                    return true;
                }
            }
        }
    }

    return false;
}

vector<string> solve(vector<int> square, vector<vector<int>> &adj_list, vector<vector<int>> &adj_matrix){
    int n = adj_matrix.size();
    vector<string> ans;
    int first = square[0];
    int second = square[1];
    int third = square[2];
    int fourth = square[3];

    if (check_single(first, second, n, {third, fourth}, adj_matrix)){
        ans.push_back("[\"0:1\"]");
    }
    if (check_single(first, third, n, {second, fourth}, adj_matrix)){
        ans.push_back("[\"0:2\"]");
    }
    if (check_single(first, fourth, n, {second, third}, adj_matrix)){
        ans.push_back("[\"0:3\"]");
    }

    if (check_single(second, third, n, {first, fourth}, adj_matrix)){
        ans.push_back("[\"1:2\"]");
    }
    if (check_single(second, fourth, n, {first, third}, adj_matrix)){
        ans.push_back("[\"1:3\"]");
    }

    if (check_single(third, fourth, n, {first, second}, adj_matrix)){
        ans.push_back("[\"2:3\"]");
    }

    if (check_double(first, second, third, fourth, n, adj_matrix)){
        ans.push_back("[\"0:1\", \"2:3\"]");
    }
    // because G is planar and jordan curve theorem, this can never happen
    /*if (check_double(first, third, second, fourth, n, adj_matrix)){
        ans.push_back("[\"0:2\", \"1:3\"]");
    }//*/
    if (check_double(first, fourth, second, third, n, adj_matrix)){
        ans.push_back("[\"0:3\", \"1:2\"]");
    }

    return ans;
}

int main(int argc, char *argv[]){
    ifstream f(argv[1]);

    map<string, vector<string>> sols;

    vector<vector<int>> squares;
    vector<int> neigh_8;
    string line;
    vector<int> cn;
    string canon_code;
    while(getline(f, line)){
        int num;
        string str;
        stringstream ss(line);
        ss >> num;
        ss >> str;
        vector<vector<int>> matrix(num, vector<int>(num, 0));
        vector<vector<int>> adj_list(num);

        int vert = 0;
        for (char c: str){
            if (c == ','){
                vert++;
            }
            else {
                matrix[vert][c - 'a'] = 1;
                adj_list[vert].push_back(c - 'a');
            }
        }

        squares.clear();
        // find all squares
        vector<int> square_count(num, 0);
        for (int vert = 0; vert < num; vert++){
            vector<int> common_neigh = common_neighbours(adj_list[vert][0], adj_list[vert][1], adj_list, matrix);
            if (common_neigh.size() == 2){
                square_count[common_neigh[0]]++;
                square_count[common_neigh[1]]++;
                square_count[adj_list[vert][0]]++;
                square_count[adj_list[vert][1]]++;
                squares.push_back({common_neigh[0], adj_list[vert][0], common_neigh[1], adj_list[vert][1]});
            }

            common_neigh = common_neighbours(adj_list[vert][0], adj_list[vert][2], adj_list, matrix);
            if (common_neigh.size() == 2){
                square_count[common_neigh[0]]++;
                square_count[common_neigh[1]]++;
                square_count[adj_list[vert][0]]++;
                square_count[adj_list[vert][2]]++;
                squares.push_back({common_neigh[0], adj_list[vert][0], common_neigh[1], adj_list[vert][2]});
            }
            common_neigh = common_neighbours(adj_list[vert][1], adj_list[vert][2], adj_list, matrix);
            if (common_neigh.size() == 2){
                square_count[common_neigh[0]]++;
                square_count[common_neigh[1]]++;
                square_count[adj_list[vert][1]]++;
                square_count[adj_list[vert][2]]++;
                squares.push_back({common_neigh[0], adj_list[vert][1], common_neigh[1], adj_list[vert][2]});
            }
        }

        int has_one_place = 0;
        for (auto sqr_cnt: square_count){
            has_one_place += sqr_cnt == 8;
        }

        if (has_one_place > 0){
            for (int i = 0; i < num; i++){
                // is ladder or double ladder
                if (square_count[i] == 8){
                    // neighbours which are part of 8 squares
                    neigh_8.clear();
                    for (auto neig: adj_list[i]){
                        if (square_count[neig] == 8){
                            neigh_8.push_back(neig);
                        }
                    }

                    // ladder
                    if (neigh_8.size() == 1){
                        int other = neigh_8[0];
                        for (auto v: adj_list[i]){
                            if (other != v){
                                // not solving the same problem twice -> canonical codes
                                cn = common_neighbours(v, other, adj_list, matrix);
                                canon_code = vec_to_str(calc_canon_code({other, cn[0], v, cn[1]}, num, 3 * num, adj_list));
                                if (!sols.contains(canon_code)){
                                    sols[canon_code] = solve({other, cn[0], v, cn[1]}, adj_list, matrix);
                                }
                            }
                        }
                    }

                    // double ladder
                    if (neigh_8.size() >= 2){
                        // not solving the same problem twice -> canonical codes
                        cn = common_neighbours(neigh_8[0], neigh_8[1], adj_list, matrix);
                        canon_code = vec_to_str(calc_canon_code({neigh_8[0], cn[0], neigh_8[1], cn[1]}, num, 3 * num, adj_list));
                        if (!sols.contains(canon_code)){
                            sols[canon_code] = solve({neigh_8[0], cn[0], neigh_8[1], cn[1]}, adj_list, matrix);
                        }
                    }
                }
            }
        }

        else {
            for (auto sqr: squares){
                // not solving the same problem twice -> canonical codes
                canon_code = vec_to_str(calc_canon_code(sqr, num, 3 * num, adj_list));
                if (!sols.contains(canon_code)){
                    sols[canon_code] = solve(sqr, adj_list, matrix);
                }
            }
        }
    }
    std::cout << sols << "\n";
}
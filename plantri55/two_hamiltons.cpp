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
    os << "{\n";
    bool first = true;
    for (const auto& [key, value] : m) {
        if (!first) os << ",\n";
        first = false;
        os << "\"" << key << "\": " << value;
    }
    os << "\n}";
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

/*
@param curr current vertex that is being processed
@param end final vertex. needs to be set appropriatelly for multiple hamiltons paths crossing this gadget
@param n total number of vertices in this gadget (ends of half edges are represented as vertices)
@param need_to_cover edges, which needs to be covered by hamiltonians paths. It is used when this algorithm
is used to check for multiple hamiltonians paths. For 6+ poles you may need to add some edges so that this can work.
Key - v1:v2
@param not_covered number of edges which need to covered by hamiltonian paths but are not yet covered. Hamiltonian path
needs to cover some edges in case of mutliple paths in one gadget
@param forbiden edges which can not be used
@param mask mask of visited vertices. Used to indicate unavailable vertices for paths, which leave some of the
half edges not covered
@param adj_list adjacency list

@return true if there exists hamilton path starting at curr ending at end, not visiting masked vertices and covering all edges from need_to_cover.
*/
bool find_hamilton(int curr, int end, int n, unordered_set<string> &need_to_cover, int not_covered_count, unordered_set<string> &forbiden, long long int mask, vector<vector<int>> &adj_list){
    mask = mask | (1 << curr);
    if (curr == end){
        return mask == ((1 << n) - 1) && (not_covered_count == 0);
    }

    for (auto vert: adj_list[curr]){
        int is_satisfied = 0;
        string key = to_string(curr) + ":" + to_string(vert);
        if (need_to_cover.contains(key)){
            is_satisfied = 1;
        }
        if (!(mask & (1 << vert)) && !forbiden.contains(key)){
            if (find_hamilton(vert, end, n, need_to_cover, not_covered_count - is_satisfied, forbiden, mask, adj_list)){
                return true;
            }
        }
    }

    return false;
}

vector<string> solve_square(vector<int> square, vector<vector<int>> &adj_list){
    int n = adj_list.size();
    vector<string> ans;
    int first = square[0];
    int second = square[1];
    int third = square[2];
    int fourth = square[3];

    unordered_set<string> need_to_cover;
    unordered_set<string> forbiden;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            forbiden.insert(to_string(square[i]) + ":" + to_string(square[j]));
        }
    }
    if (find_hamilton(first, second, n, need_to_cover, 0, forbiden, (1<<third) | (1<<fourth), adj_list)){
        ans.push_back("[\"0:1\"]");
    }
    if (find_hamilton(first, third, n, need_to_cover, 0, forbiden, (1<<second) | (1<<fourth), adj_list)){
        ans.push_back("[\"0:2\"]");
    }
    if (find_hamilton(first, fourth, n, need_to_cover, 0, forbiden, (1<<second) | (1<<third), adj_list)){
        ans.push_back("[\"0:3\"]");
    }
    if (find_hamilton(second, third, n, need_to_cover, 0, forbiden, (1<<first) | (1<<fourth), adj_list)){
        ans.push_back("[\"1:2\"]");
    }
    if (find_hamilton(second, fourth, n, need_to_cover, 0, forbiden, (1<<first) | (1<<third), adj_list)){
        ans.push_back("[\"1:3\"]");
    }
    if (find_hamilton(third, fourth, n, need_to_cover, 0, forbiden, (1<<first) | (1<<second), adj_list)){
        ans.push_back("[\"2:3\"]");
    }
    
    forbiden = {};
    for (int i = 0; i < 3; i += 2){
        for (int j = i + 1; j < 4; j += 2){
            string v1 = to_string(square[i]);
            string v2 = to_string(square[j]);
            forbiden.insert(v1 + ":" + v2);
            forbiden.insert(v2 + ":" + v1);
        }
    }
    need_to_cover = {to_string(second) + ":" + to_string(third)};
    if (find_hamilton(first, fourth, n, need_to_cover, 1, forbiden, 0, adj_list)){
        ans.push_back("[\"0:1\", \"2:3\"]");
    }
    
    // because G is planar and jordan curve theorem, this can never happen
    /*if (check_double(first, third, second, fourth, n, adj_matrix)){
        ans.push_back("[\"0:2\", \"1:3\"]");
    }//*/
    forbiden = {};
    for (int i = 0; i < 2; i++){
        for (int j = i + 1; j < 4; j += 2){
            string v1 = to_string(square[i]);
            string v2 = to_string(square[j]);
            forbiden.insert(v1 + ":" + v2);
            forbiden.insert(v2 + ":" + v1);
        }
    }
    need_to_cover = {to_string(fourth) + ":" + to_string(third)};
    if (find_hamilton(first, second, n, need_to_cover, 1, forbiden, 0, adj_list)){
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
                                    sols[canon_code] = solve_square({other, cn[0], v, cn[1]}, adj_list);
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
                            sols[canon_code] = solve_square({neigh_8[0], cn[0], neigh_8[1], cn[1]}, adj_list);
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
                    sols[canon_code] = solve_square(sqr, adj_list);
                }
            }
        }
    }
    std::cout << sols << "\n";
}
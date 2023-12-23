#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>


using namespace std;

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string join(const vector<string>& strings, char delimiter) {
    string result;
    for (int i = 0; i < strings.size(); i++) {
        result += strings[i];
        if (i != strings.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

vector<string> to_array(const string& state) {
    return split(state, ';');
}

string to_string(const vector<string>& state) {
    return join(state, ';');
}

int heuristic(const vector<string>& state, const vector<string>& goal_state) {
    int count = 0;
    for (int i = 0; i < state.size(); i++) {
        if (state[i] != goal_state[i]) {
            count++;
        }
    }
    return count;
}

vector<pair<string, vector<string>>> gen_neighbors(const vector<string>& state, const unordered_map<string, vector<int>>& moves) {
    vector<pair<string, vector<string>>> neighbors;
    for (const auto& move : moves) {
        const string& move_id = move.first;
        const vector<int>& move_values = move.second;
        vector<int> inv_move_values(move_values.size());
        for (int i = 0; i < move_values.size(); i++) {
            inv_move_values[move_values[i]] = i;
        }
        vector<string> inv_state(state.size());
        for (int i = 0; i < state.size(); i++) {
            inv_state[i] = state[inv_move_values[i]];
        }
        vector<string> perm_state(state.size());
        for (int i = 0; i < state.size(); i++) {
            perm_state[i] = state[move_values[i]];
        }
        neighbors.push_back(make_pair("-" + move_id, inv_state));
        neighbors.push_back(make_pair(move_id, perm_state));
    }
    return neighbors;
}

vector<string> a_star_solve(const vector<string>& goal_state, const vector<string>& init_state, const unordered_map<string, vector<int>>& moves) {
    priority_queue<pair<int, pair<vector<string>, vector<string>>>> priority_queue;
    unordered_map<string, int> state_cost;
    vector<string> track;
    state_cost[to_string(init_state)] = 0;
    priority_queue.push(make_pair(-(0 + heuristic(init_state, goal_state)), make_pair(init_state, track)));
    while (!priority_queue.empty()) {
        int current_f_cost = priority_queue.top().first;
        vector<string> current_state = priority_queue.top().second.first;
        track = priority_queue.top().second.second;
        priority_queue.pop();
        if (current_state == goal_state) {
            return track;
        }
        for (const auto& neighbor : gen_neighbors(current_state, moves)) {
            int new_g_cost = track.size() + 1;
            const string& neighbor_string = to_string(neighbor.second);
            if (state_cost.find(neighbor_string) == state_cost.end() || new_g_cost < state_cost[neighbor_string]) {
                state_cost[neighbor_string] = new_g_cost;
                int f_cost = -(new_g_cost + heuristic(neighbor.second, goal_state));
                vector<string> node_track = track;
                node_track.push_back(neighbor.first);
                priority_queue.push(make_pair(f_cost, make_pair(neighbor.second, node_track)));
            }
        }
    }
    return vector<string>();
}

vector<string> get_problem(int puzzle_id, const unordered_map<int, unordered_map<string, string>>& puzzles) {
    const unordered_map<string, string>& puzzle = puzzles.at(puzzle_id);
    const string& goal_state = puzzle.at("solution_state");
    const string& init_state = puzzle.at("initial_state");
    const string& puzzle_type = puzzle.at("puzzle_type");
    
    return {goal_state, init_state, puzzle_type};
}


int main() {
    ifstream puzzle_info_file("./input_data/transformed_puzzle_info.csv");
    ifstream puzzles_file("./input_data/puzzles.csv");

    // load puzzle info file into map
    string line;
    getline(puzzle_info_file, line); // Skip header

    unordered_map<string, unordered_map<string, vector<int>>> puzzle_info;

    while (getline(puzzle_info_file, line)) {
        stringstream ss(line);
        string ind, puzzle_type, move, number;
        getline(ss, ind, ',');
        getline(ss, puzzle_type, ',');
        getline(ss, move, ',');
        
        vector<int> numbers;
        while (getline(ss, number, ' ')) {
            numbers.push_back(stoi(number));
        }

        puzzle_info[puzzle_type][move] = numbers;
    }

    // load puzzle file into map
    unordered_map<int, unordered_map<string, string>> puzzles;

    // Assuming the first line is a header
    getline(puzzles_file, line);

    while (getline(puzzles_file, line)) {
        stringstream ss(line);
        string ind, puzzle_type, initial_state, solution_state, wildcards;

        // Parse the line. Adjust the indexes if necessary.
        getline(ss, ind, ',');
        getline(ss, puzzle_type, ',');
        getline(ss, initial_state, ',');
        getline(ss, solution_state, ',');
        getline(ss, wildcards, ',');

        // Store in the map
        puzzles[stoi(ind)] = {{"puzzle_type", puzzle_type}, {"initial_state", initial_state}, {"solution_state", solution_state}, {"wildcards", wildcards}};

    }

    auto t_all = chrono::steady_clock::now();
    for (int i = 0; i < 20; i++) {
        cout << "\nProblem: " << i << endl;
        vector<string> vars = get_problem(i, puzzles);
        const string& goal_state = vars[0];
        const string& init_state = vars[1];
        const string& puzzle_type = vars[2];

        const unordered_map<string, vector<int>>& allowed_moves = puzzle_info.at(puzzle_type);
        auto t = chrono::steady_clock::now();
        vector<string> solution = a_star_solve(to_array(goal_state), to_array(init_state), allowed_moves);
        for (const auto& step : solution) {
            cout << step << " ";
        }
        cout << endl;
        cout << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t).count() << " ms" << endl;
    }
    cout << "\nOverall time: " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t_all).count() << " ms" << endl;
    return 0;
}

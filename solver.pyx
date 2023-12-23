# distutils: language=c++
# cython: language_level=3
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from heapq import heappush, heappop
from libc.string cimport strcmp

cdef class State:
    cdef readonly vector[int] data

    cdef int compare(self, State other):
        cdef int i
        for i in range(self.data.size()):
            if self.data[i] != other.data[i]:
                return False
        return True

cdef str to_string(vector[str] state):
    cdef str result = ''
    cdef size_t i, n
    n = state.size()
    if n > 0:
        result += state[0]
    for i in range(1, n):
        result += ';' + state[i]
    return result

cdef int heuristic(vector[str] state, vector[str] goal_state):
    cdef int diff = 0, i
    for i in range(state.size()):
        if state[i] != goal_state[i]:
            diff += 1
    return diff

cdef vector[vector[str]] gen_neighbors(vector[str] state, unordered_map[str, vector[int]] moves):
    cdef vector[str] neighbors
    cdef vector[int] inv, move
    cdef int i, v

    for move_id, move in moves.items():
        inv.resize(move.size())
        for i in range(move.size()):
            inv[move[i]] = i

        cdef vector[str] neighbor
        for i in range(inv.size()):
            neighbor[i] = state[inv[i]]
        neighbors.push_back(neighbor)

        neighbor = State()
        neighbor.data = state.data
        for i in range(move.size()):
            neighbor.data[i] = state.data[move[i]]
        neighbors.push_back(neighbor)

    return neighbors

def solve(str goal_state_str, str init_state_str, dict moves_py):
    cdef unordered_map[int, vector[int]] moves
    # Convert Python dict to C++ unordered_map
    for key, value in moves_py.items():
        move = vector[int]()
        for v in value:
            move.push_back(v)
        moves[key] = move

    cdef vector priority_queue
    cdef list track
    cdef unordered_map[str, int] state_cost
    cdef State goal_state = to_state(goal_state_str)
    cdef State init_state = to_state(init_state_str)

    state_cost[to_string(init_state)] = 0
    heappush(priority_queue, (0 + heuristic(init_state, goal_state), init_state, track))

    while priority_queue:
        current_f_cost, current_state, track = heappop(priority_queue)

        if current_state.compare(goal_state):
            return track

        for neighbor in gen_neighbors(current_state, moves):
            new_g_cost = len(track) + 1
            neighbor_string = to_string(neighbor)
            if neighbor_string not in state_cost or new_g_cost < state_cost[neighbor_string]:
                state_cost[neighbor_string] = new_g_cost
                f_cost = new_g_cost + heuristic(neighbor, goal_state)
                node_track = track + [neighbor]
                heappush(priority_queue, (f_cost, neighbor_string, node_track))

    return None

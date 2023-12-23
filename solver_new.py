
import pandas as pd
import numpy as np
import heapq
import datetime

puzzle_info = pd.read_csv('./input_data/puzzle_info.csv')
puzzles = pd.read_csv('./input_data/puzzles.csv')


def to_array(state):
    return np.array(state.split(';'))

def to_string(state):
    return ";".join(state)

def heuristic(state, goal_state):
    return np.sum(np.where(state != goal_state, 1, 0))

def gen_neighbors(state, moves):
    neighbors = []
    for move_id, move in moves.items():
        inv = np.arange(len(move))
        for i, v in enumerate(move):
            inv[v] = i
        neighbors.append([f'-{move_id}', state[inv]])
        neighbors.append([move_id, state[move]])
    return neighbors

def a_star_solve(goal_state, init_state, moves):
    priority_queue = []
    track = []
    state_cost = {init_state: 0}

    goal_state_array = to_array(goal_state)
    init_state_array = to_array(init_state)

    heapq.heappush(priority_queue, (0 + heuristic(init_state_array, goal_state_array), init_state, track))

    while priority_queue:
        current_f_cost, current_state, track = heapq.heappop(priority_queue)
        current_state_array = to_array(current_state)

        if current_state == goal_state:
            return track
        
        for neighbor in gen_neighbors(current_state_array, moves):
            new_g_cost = len(track) + 1
            neighbor_string = to_string(neighbor[1])
            if neighbor_string not in state_cost or new_g_cost < state_cost[neighbor_string]:
                state_cost[neighbor_string] = new_g_cost
                f_cost = new_g_cost + heuristic(neighbor[1], goal_state_array)
                node_track = track + [neighbor[0]]
                heapq.heappush(priority_queue, (f_cost, neighbor_string, node_track))

    return None


def get_problem(puzzle_id):
    puzzle = puzzles.loc[puzzle_id]
    
    # dtype string
    goal_state = puzzle['solution_state']
    init_state = puzzle['initial_state']

    # dtype string
    puzzle_type = puzzle['puzzle_type']

    # eval to convert string to dict
    moves = eval(puzzle_info[puzzle_info['puzzle_type'] == puzzle_type]['allowed_moves'].item())
    moves = {move_id: np.array(move) for move_id, move in moves.items()}

    return [goal_state, init_state, moves]

t_all = datetime.datetime.now()
for i in range(0, 31):
    print(f'\nProblem: {i}')
    vars = get_problem(i)
    goal_state, init_state, moves = vars[0], vars[1], vars[2]
    t = datetime.datetime.now()
    solution = a_star_solve(goal_state, init_state, moves)
    print(solution)
    print((datetime.datetime.now() - t))
print(f'\nOverall time: {(datetime.datetime.now() - t_all)}')



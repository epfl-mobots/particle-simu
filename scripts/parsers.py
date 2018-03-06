params_parser = {
    ('simulation', 'time'): ['sim_time', int],
    ('dump', 'start'): ['dump_start', int],
    ('number', 'fish'): ['num_fish', int],
    ('number', 'robots'): ['num_robots', int],
    ('max', 'neighbors'): ['max_neighbors', int],
    ('number', 'cells'): ['num_cells', int],
    ('degrees', 'vision'): ['deg_vision', int],
    ('robot', 'heading'): ['heading_robot', str],
    ('probability', 'obey'): ['prob_obedience', float],
    ('probability', 'stay'): ['prob_stay', float],
    ('probability', 'move', 'robot'): ['prob_move', float]
}

group_params_parser = {
    ('group', 'threshold'): ['group_threshold', int],
    ('group', 'cells', 'forward'): ['group_cells_forward', int],
    ('group', 'cells', 'backward'): ['group_cells_backward', int],
    ('social', 'influence'): ['social_influence', float]
}


def parse_params(path, pdict):
    f = open(path)
    params_str = f.readlines()[1:]
    f.close()

    params = dict()
    for l in params_str:
        val = l.split(':')[1].strip()
        for kwrds, info in pdict.items():
            flag = True
            for k in kwrds:
                if k not in l.lower():
                    flag = False
                    break
            if flag:
                params[info[0]] = info[1](val)
    return params, params_str


def parse_fir_params(path):
    return parse_params(path + '/fish_in_ring_params.dat', params_parser)


def parse_group_params(path):
    return parse_params(path + '/group_params.dat', group_params_parser)

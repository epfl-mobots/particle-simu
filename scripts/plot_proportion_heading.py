#!/usr/bin/env python

from termcolor import colored
import os, errno
import argparse
import numpy as np
import matplotlib.pyplot as plt


Heading = {
    'clockwise': -1,
    'counterclockwise': 1}

params_parser = {
    ('simulation', 'time'): ['sim_time', int],
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


def parse_fir_params(path):
    f = open(path + '/fish_in_ring_params.dat')
    params_str = f.readlines()[1:]
    f.close()

    params = dict()
    for l in params_str:
        val = l.split(':')[1].strip()
        for kwrds, info in params_parser.items():
            flag = True
            for k in kwrds:
                if k not in l.lower():
                    flag = False
                    break
            if flag:
                params[info[0]] = info[1](val)
    return params, params_str


def read_data(path):
    data = dict()
    data['params'] = parse_fir_params(path)
    data['headings'] = np.loadtxt(path + '/headings.dat', skiprows=1)
    return data


def plot_proportion_heading(data, args):
    if args.png:
        pdirectory = args.path + '/plot'
        try:
            os.makedirs(pdirectory)
        except OSError as e:
            if e.errno == errno.EEXIST:
                print(colored('Skipping directory creation (already exists)', 'yellow'))

    params = data['params'][0]
    num_robots = params['num_robots']
    num_fish = params['num_fish']
    num_agents = num_robots + num_fish

    headings = data['headings'][:, 1:]
    proportion_hdg = np.float64(np.sum(np.where(headings == Heading[args.heading], 1, 0), 1)) / num_agents

    plt.figure(figsize=[8, 6], dpi=args.dpi)
    plt.plot(proportion_hdg, '.-')
    plt.ylim([0, 1])
    plt.xlabel('Time')
    plt.ylabel('Proportion ' + args.heading)
    if not args.hide_plot:
        plt.show()
    if args and args.png:
        plt.savefig(pdirectory + '/proportion_heading.png', dpi=args.dpi)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot the "Fish in Ring"')
    parser.add_argument('path',
                        type=str,
                        help='path to the folder containing the simulation results')
    parser.add_argument('--hide-plot',
                        action='store_true',
                        help='hide realtime plot while processing the input data')
    parser.add_argument('--dpi',
                        type=int,
                        default=100,
                        help='dpi for the plot and video output')
    parser.add_argument('--png',
                        action='store_true',
                        help='create a png for each frame of the experiment')
    parser.add_argument('--heading',
                        type=str,
                        default='CLOCKWISE',
                        help='heading for which you wish to see the proportion plot (CLOCKWISE or COUNTERCLOCKWISE)')
    args = parser.parse_args()

    if args.path:
        args.heading = args.heading.lower()
        data = read_data(args.path)
        plot_proportion_heading(data, args)

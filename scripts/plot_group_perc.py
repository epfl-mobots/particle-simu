#!/usr/bin/env python

from termcolor import colored
import os, errno, sys
import argparse
import numpy as np
import matplotlib.pyplot as plt
from parsers import parse_fir_params, parse_group_params
import seaborn as sns


markersize = 7
sns.set_style("darkgrid")


def read_data(path):
    data = dict()
    data['params'] = parse_fir_params(path)
    data['group_params'] = parse_group_params(path)
    data['positions'] = np.loadtxt(path + '/positions.dat', skiprows=1)
    data['headings'] = np.loadtxt(path + '/headings.dat', skiprows=1)
    data['polarities'] = np.loadtxt(path + '/polarities.dat', skiprows=1)
    data['group'] = np.loadtxt(path + '/group.dat', skiprows=1)
    return data


def plot_groups(data, args=[]):
    params = data['params'][0]
    group_params = data['group_params'][0]
    num_cells = params['num_cells']
    num_robots = params['num_robots']
    num_fish = params['num_fish']
    num_agents = num_robots + num_fish
    group_threshold = group_params['group_threshold']
    group_cells_forward = group_params['group_cells_forward']
    group_cells_backward = group_params['group_cells_backward']
    group_data = data['group']
    group_sizes = group_data[:, 1:]

    if args.png:
        pdirectory = args.path + '/plot'
        try:
            os.makedirs(pdirectory)
        except OSError as e:
            if e.errno == errno.EEXIST:
                print(colored('Skipping directory creation (already exists)', 'yellow'))

    perc_in_group = np.float64(np.sum(np.where(group_sizes >= group_threshold, 1, 0), 1)) / num_agents
    perc_in_group_per_fish = np.float64(np.sum(np.where(group_sizes >= group_threshold, 1, 0), 0)) / (params['sim_time'] - params['dump_start'])

    plt.figure(figsize=[8, 6], dpi=args.dpi)
    plt.plot(perc_in_group, '.-')
    plt.ylim([0, 1])
    plt.xlabel('Time')
    plt.ylabel('Percentage')
    if args and args.png:
        plt.savefig(pdirectory + '/group_percentages.png', dpi=args.dpi)

    fig = plt.figure(figsize=[8, 6], dpi=args.dpi)
    print(perc_in_group_per_fish)
    pgpf = sns.barplot(x=list(range(num_agents)), y=np.float64(perc_in_group_per_fish), palette="muted")
    pgpf.set_title('Percentage in group per fish')
    plt.ylim([0, 1])
    if args and args.png:
        plt.savefig(pdirectory + '/percentage_in_group_per_fish.png', dpi=args.dpi)
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot the percentage of time a fish spends in groups')
    parser.add_argument('path',
                        type=str,
                        help='path to the folder containing the simulation results')
    parser.add_argument('--dpi',
                        type=int,
                        default=100,
                        help='dpi for the plot and video output')
    parser.add_argument('--png',
                        action='store_true',
                        help='create a png for each frame of the experiment')
    args = parser.parse_args()

    if args.path:
        data = read_data(args.path)
        print('Plotting simulation results with parameters:\n')
        for l in data['params'][1]:
            sys.stdout.write(l)
        plot_groups(data, args)

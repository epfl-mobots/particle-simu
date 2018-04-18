#!/usr/bin/env python

from termcolor import colored
import os, errno
import argparse
import numpy as np
import matplotlib.pyplot as plt


def read_data(path):
    data = dict()
    data['polarities'] = np.loadtxt(path + '/polarities.dat', skiprows=1)
    return data


def plot_polarities(polarities, args):
    if args.png:
        pdirectory = args.path + '/plot'
        try:
            os.makedirs(pdirectory)
        except OSError as e:
            if e.errno == errno.EEXIST:
                print(colored('Skipping directory creation (already exists)', 'yellow'))

    plt.figure(figsize=[8, 6], dpi=args.dpi)
    plt.plot(polarities[:, 1:], '.--')
    plt.ylim([-2, 2])
    plt.xlabel('Time')
    plt.ylabel('Proportion Clockwise')
    if not args.hide_plot:
        plt.show()
    if args and args.png:
        plt.savefig(pdirectory + '/polarities.png', dpi=args.dpi)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot the polarities of the shoal per timestep')
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
    args = parser.parse_args()

    if args.path:
        data = read_data(args.path)
        plot_polarities(data['polarities'], args)
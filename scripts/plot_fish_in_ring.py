#!/usr/bin/env python

from termcolor import colored
import os, errno, sys
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as manimation
from plot_polarities import plot_polarities
from parsers import parse_fir_params


markersize = 7


def read_data(path):
    data = dict()
    data['params'] = parse_fir_params(path)
    data['positions'] = np.loadtxt(path + '/positions.dat', skiprows=1)
    data['headings'] = np.loadtxt(path + '/headings.dat', skiprows=1)
    data['polarities'] = np.loadtxt(path + '/polarities.dat', skiprows=1)
    return data


def plot_ring(data, args=[]):
    params = data['params'][0]
    num_cells = params['num_cells']
    num_robots = params['num_robots']
    num_fish = params['num_fish']
    num_agents = num_robots + num_fish
    cell_degree = 360 / num_cells
    positions = data['positions']
    polarities = data['polarities']

    th1 = np.linspace(0, 2 * np.pi, 100)
    r1 = 50 + np.zeros(len(th1))
    r2 = 50 + num_agents + 1 + np.zeros(len(th1))

    th2 = np.linspace(0 + (2 * np.pi) / (2 * num_cells),
                      2 * np.pi - (2 * np.pi) / (2 * num_cells),
                      num_cells)
    r3 = 50 + np.zeros(len(th2))
    r4 = 50 + num_agents + 1 + np.zeros(len(th2))

    if args.with_video or args.png:
        pdirectory = args.path + '/plot'
        try:
            os.makedirs(pdirectory)
        except OSError as e:
            if e.errno == errno.EEXIST:
                print(colored('Skipping directory creation (already exists)', 'yellow'))

    fig = plt.figure(figsize=[8, 6], dpi=args.dpi)
    if args and args.with_video:
        FFMpegWriter = manimation.writers['ffmpeg']
        metadata = dict(title='Fish In Ring Experiment', artist='LSRO',
                        comment='')
        vwriter = FFMpegWriter(fps=args.frame_rate, metadata=metadata)
        vwriter.setup(fig, pdirectory + "/fish_in_ring_exp.mp4", args.dpi)

    for i in range(np.shape(positions)[0]):
        os.system('cls' if os.name == 'nt' else 'clear')
        print('Time step: ' + str(i))
        plt.polar(th1, r1, 'b')
        plt.polar(th1, r2, 'b')
        plt.polar([th2, th2], [r3, r4], '--b')

        if num_robots:
            rpos_x = positions[i][1:num_robots + 1] * cell_degree
            rpos_y = 50 + np.array(range(1, num_robots + 1))
            fpos_x = positions[i][num_robots + 1:] * cell_degree
            fpos_y = 50 + (1 + np.array(range(1 + num_robots, num_agents + 1)))
            plt.polar(np.deg2rad(rpos_x), rpos_y, 'r.', markersize=markersize, marker='o', label='Robot')
            plt.polar(np.deg2rad(fpos_x), fpos_y, 'b.', markersize=markersize, marker='o', label='Fish')
        else:
            fpos_x = positions[i][1:] * cell_degree
            fpos_y = 50 + (1 + np.array(range(1, num_agents + 1)))
            plt.polar(np.deg2rad(fpos_x), fpos_y, 'b.', markersize=markersize, marker='o', label='Fish')

        ax = plt.gca()
        thetaticks = np.linspace(0, 360 - 360 / num_cells, num_cells)
        thetatick_labels = np.array(range(num_cells))
        ax.set_xticks(np.deg2rad(thetaticks))
        ax.set_xticklabels(thetatick_labels)
        ax.set_yticklabels([])
        plt.legend()
        if args and args.with_video:
            vwriter.grab_frame()
        if args and args.png:
            plt.savefig(pdirectory + '/' + str(i) + '.png', dpi=args.dpi)
            plt.savefig(pdirectory + '/current.png', dpi=args.dpi)
        if not args.hide_plot:
            plt.pause(1/args.frame_rate)
        plt.clf()
    if args and args.with_video:
        vwriter.finish()

    plot_polarities(polarities, args)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot the "Fish in Ring"')
    parser.add_argument('path',
                        type=str,
                        help='path to the folder containing the simulation results')
    parser.add_argument('--hide-plot',
                        action='store_true',
                        help='hide realtime plot while processing the input data')
    parser.add_argument('--with-video', '-v',
                        action='store_true',
                        help='create a video of the experiment')
    parser.add_argument('--frame-rate', '-f',
                        type=float,
                        default=5.0,
                        help='frame rate for the video output')
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
        plot_ring(data, args)

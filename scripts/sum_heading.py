#!/usr/bin/env python

import argparse
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy import stats, integrate
import seaborn as sns

markersize = 7
sns.set_style("darkgrid")


class PolarityWriter:
    def __init__(self):
        self.__ofile = open('sum_headings.dat', 'w')
        self.__ofile.write('#iteration polarity\n')

    def write(self, line):
        oline = str(line)
        oline = oline.replace('[', '')
        oline = oline.replace(']', '')
        oline = oline.replace(',', '')
        self.__ofile.write(oline + '\n')

    def __del__(self):
        self.__ofile.close()


def count_consecutive(values):
    iters = np.shape(values)[0]
    hist_vals = []
    sum = 1
    for i in range(1, iters):
        if np.sign(values[i, 1]) == np.sign(values[i-1, 1]):
            sum += 1
        else:
            hist_vals.append(sum)
            sum = 1
    return hist_vals


def plot_hist(args, sum_heading):
    hist_vals = count_consecutive(sum_heading)
    plt.figure(figsize=[8, 6], dpi=args.dpi)
    sns.distplot(hist_vals, kde=False, rug=False, norm_hist=False)
    # plt.yticks(np.arange(0, 1.2, 0.2))
    # plt.xticks([])
    # plt.legend()
    plt.show()


def calculate_sum_heading(args, data):
    positions = data[:, 1:]
    num_cells = args.num_cells
    sum_heading = np.int32(np.zeros([np.shape(positions)[0], 2]))
    sum_heading[0, 1] = np.sign(np.sum(positions[1] - positions[0]))

    if not args.only_plot:
        pw = PolarityWriter()

    for i in range(1, np.shape(positions)[0]):
        sum_heading[i, 0] = i

        dist = positions[i] - positions[i-1]
        for j in range(np.shape(positions)[1]):
            if dist[j] <= - (num_cells / 2) or dist[j] > num_cells / 2:
                dist[j] = np.sign(dist[j]) * (num_cells - np.abs(dist[j]))
        sum_heading[i, 1] = np.sum(dist)

        if not args.only_plot:
            pw.write(sum_heading[i])
    return sum_heading


def load_data(path):
    data = np.loadtxt(path, skiprows=1)
    return data


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Calculate or plot information about the headings of an experiment')
    parser.add_argument('path',
                        type=str,
                        help='path to the position file')
    parser.add_argument('--only-plot',
                        help='do not generate .dat files')
    parser.add_argument('--hist',
                        action='store_true',
                        help='plot a histogram for the consecutive polarity values')
    parser.add_argument('--dpi',
                        type=int,
                        default=100,
                        help='dpi for the plot/video output')
    parser.add_argument('--num-cells',
                        type=int,
                        default=40,
                        help='number of cells to divide the ring')

    parser.add_argument('--compare',
                        action='store_true',
                        help='compare two distributions')
    parser.add_argument('--against',
                        type=str,
                        help='file to compare against')
    args = parser.parse_args()

    if args.path:
        data = load_data(args.path)
        sum_heading = calculate_sum_heading(args, data)
        with open('mean_polarity.dat', 'w') as f:
            mean_p = np.mean(sum_heading[:, 1])
            f.write(str(mean_p) + '\n')
        if args.hist:
            plot_hist(args, sum_heading)
    if args.compare:
        sh1 = calculate_sum_heading(args, load_data(args.path))
        sh2 = calculate_sum_heading(args, load_data(args.against))

        hist1 = count_consecutive(sh1)
        hist2 = count_consecutive(sh2)

        print()
        ts = stats.ks_2samp(hist1, hist2)
        print(ts)

        ts = stats.mannwhitneyu(hist1, hist2)
        print(ts)

        dim = np.min([len(hist1), len(hist2)])
        ts = stats.wilcoxon(hist1[:dim], hist2[:dim])
        print(ts)

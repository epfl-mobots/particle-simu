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
        self.__ofile = open('polarities.dat', 'w')
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
        if values[i, 1] == values[i-1, 1]:
            sum += 1
        else:
            hist_vals.append(sum)
            sum = 1
    return hist_vals


def plot_hist(args, sum_heading):
    hist_vals = count_consecutive(sum_heading)
    plt.figure(figsize=[8, 6], dpi=args.dpi)
    sns.distplot(hist_vals, kde=True, rug=False, norm_hist=False)
    plt.yticks(np.arange(0, 1.2, 0.2))
    plt.xticks([])
    # plt.legend()
    plt.show()


def calculate_sum_heading(args, data):
    positions = data[:, 1:]
    sum_heading = np.int32(np.zeros([np.shape(positions)[0], 2]))
    sum_heading[0, 1] = np.sign(np.sum(positions[1] - positions[0]))

    if not args.only_plot:
        pw = PolarityWriter()

    for i in range(1, np.shape(positions)[0]):
        sum_heading[i, 0] = i
        sum_heading[i, 1] = np.sign(np.sum(positions[i] - positions[i-1]))
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
        sh1 = calculate_sum_heading(args, load_data(args.path))[:, 1:]
        sh2 = calculate_sum_heading(args, load_data(args.against))[:, 1:]
        # sh1 = sh1 / np.linalg.norm(sh1)
        sh1 = sh1.flatten()
        # sh2 = sh2 / np.linalg.norm(sh2)
        sh2 = sh2.flatten()

        print()
        ts = stats.ks_2samp(sh1, sh2)
        print(ts)

        ts = stats.mannwhitneyu(sh1, sh2)
        print(ts)

        ts = stats.wilcoxon(sh1[:1800], sh2[:1800])
        print(ts)

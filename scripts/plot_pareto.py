#!/usr/bin/env python
import argparse
import numpy as np
import matplotlib.pyplot as plt


def plot(pareto, args):
    plt.plot(pareto[:, 1], pareto[:, 0], 'x--', linewidth=2)
    plt.ylabel('Cohesion')
    plt.xlabel('Turns')
    plt.xlim([0, 0.6])
    plt.ylim([0, 1])
    plt.show()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('path',
                        type=str,
                        help='path to the folder containing the simulation results')
    args = parser.parse_args()

    if args.path:
        data = dict()
        plot(np.loadtxt(args.path), args)
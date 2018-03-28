#!/usr/bin/env python

import argparse
import numpy as np


def complete_circles(args, data):
    positions = data[:, 1:]
    num_cells = args.num_cells
    pos_placeholder = positions[0, :]
    travelled_dist = np.zeros([1, np.shape(positions)[1]])
    circles = np.zeros([1, np.shape(positions)[1]])

    for i in range(1, np.shape(positions)[0]):
        dist = positions[i, :] - positions[i-1, :]
        for j in range(np.shape(positions)[1]):
            if dist[j] <= - (num_cells / 2) or dist[j] > num_cells / 2:
                dist[j] = np.sign(dist[j]) * (num_cells - np.abs(dist[j]))
        travelled_dist += dist

        for j in range(np.shape(positions)[1]):
            if np.abs(travelled_dist[0, j]) >= num_cells:
                travelled_dist[0, j] = 0
                circles[0, j] += 1
    np.savetxt(args.path[:-len(args.path.split('/')[-1])] + 'circles.dat', circles)
    np.savetxt(args.path[:-len(args.path.split('/')[-1])] + 'circles.dat', np.mean(circles))
    print(circles)


def load_data(path):
    data = np.loadtxt(path, skiprows=1)
    return data


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Calculate or plot information about the headings of an experiment')
    parser.add_argument('path',
                        type=str,
                        help='path to the position file')

    parser.add_argument('--compare',
                        action='store_true',
                        help='compare two distributions')
    parser.add_argument('--against',
                        type=str,
                        help='file to compare against')
    parser.add_argument('--num-cells',
                        type=int,
                        default=40,
                        help='number of cells in the ring')


    args = parser.parse_args()

    if args.path:
        data = load_data(args.path)
        complete_circles(args, data)

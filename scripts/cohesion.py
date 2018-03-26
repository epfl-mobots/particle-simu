#!/usr/bin/env python

import argparse
import numpy as np
import cmath

def load_data(path):
    data = dict()
    data['positions'] = np.loadtxt(path)
    return data




def calc_cohesion(data):
    def cohexp(pos): return np.exp(2*np.pi*pos*1j / num_cells)

    positions = data['positions'][:, 1:]
    num_cells = data['num_cells']
    num_fish = np.shape(positions)[1]

    coh_per_iter = []
    for idx_r in range(np.shape(positions)[0]):
        sum = 0
        for idx_c in range(num_fish):
            sum += cohexp(positions[idx_r, idx_c])
        coh_per_iter.append(np.abs(sum)/num_fish)
    print(np.mean(coh_per_iter))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Calculate the cohesion of the group per iteration')
    parser.add_argument('path',
                        type=str,
                        help='path to position data')
    parser.add_argument('--num-cells', '-n',
                        type=int,
                        help='number of cells for the ring',
                        default=2)
    args = parser.parse_args()

    if args.path:
        data = load_data(args.path)
        data['num_cells'] = args.num_cells
        calc_cohesion(data)

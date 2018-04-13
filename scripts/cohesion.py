#!/usr/bin/env python

import argparse
import numpy as np


def load_data(path):
    data = dict()
    data['positions'] = np.loadtxt(path)
    return data


class ListFileWriter:
    def __init__(self, fname, title):
        self.__ofile = open(fname, 'w')
        self.__ofile.write(title)

    def write(self, line):
        oline = str(line)
        oline = oline.replace('[', '')
        oline = oline.replace(']', '')
        oline = oline.replace(',', '')
        self.__ofile.write(oline + '\n')

    def __del__(self):
        self.__ofile.close()


def calc_contribution(coh_per_fish, coh_per_iter, num_fish):
    contrib = []
    for i in range(len(coh_per_fish)):
        idcs = list(range(len(coh_per_fish)))
        idcs.remove(i)
        cpf = [coh_per_fish[ce] for ce in idcs]
        contrib.append(coh_per_iter - np.abs(np.sum(cpf))/num_fish)
    return contrib


def calc_cohesion(data):
    def cohexp(pos): return np.exp(2*np.pi*pos*1j / num_cells)

    positions = data['positions'][:, 1:]
    num_cells = data['num_cells']
    num_fish = np.shape(positions)[1]
    of = ListFileWriter('cohesion_contribution.dat', '#iteration fish1_contribution fish2_contribution ...\n')

    coh_per_iter = []
    for idx_r in range(np.shape(positions)[0]):
        coh_per_fish = []
        for idx_c in range(num_fish):
            coh_per_fish.append(cohexp(positions[idx_r, idx_c]))
        coh_per_iter.append(np.abs(np.sum(coh_per_fish))/num_fish)
        contrib_list = calc_contribution(coh_per_fish, coh_per_iter[idx_r], num_fish)
        contrib_list.insert(0, idx_r)
        of.write(contrib_list)
    print(np.mean(coh_per_iter))


def calc_avg_contrib():
    coh_per_fish = np.loadtxt('cohesion_contribution.dat')
    coh_per_fish = coh_per_fish[:, 1:]
    of = ListFileWriter('avg_cohesion_contribution.dat', 'fish1_avg_contribution fish2_avg_contribution ...\n')
    avg = []
    for j in range(np.shape(coh_per_fish)[1]):
        avg.append(np.mean(coh_per_fish[:, j]))
    of.write(avg)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Calculate the cohesion of the group per iteration')
    parser.add_argument('path',
                        type=str,
                        help='path to position data')
    parser.add_argument('--num-cells', '-n',
                        type=int,
                        help='number of cells for the ring',
                        default=40)
    parser.add_argument('--avg',
                        action='store_true',
                        help='compute the average contribution per fish')
    args = parser.parse_args()

    if args.path:
        data = load_data(args.path)
        data['num_cells'] = args.num_cells
        calc_cohesion(data)

    if args.avg:
        calc_avg_contrib()

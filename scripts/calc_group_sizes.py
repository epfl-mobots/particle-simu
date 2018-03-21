#!/usr/bin/env python

import argparse
import numpy as np


class GroupSizeWriter:
    def __init__(self):
        self.__ofile = open('group.dat', 'w')
        self.__ofile.write('#iteration fish1_group_size fish2_group_size ...\n')

    def write(self, line):
        oline = str(line)
        oline = oline.replace('[', '')
        oline = oline.replace(']', '')
        oline = oline.replace(',', '')
        self.__ofile.write(oline + '\n')

    def __del__(self):
        self.__ofile.close()


def load_data(path):
    data = dict()
    data['positions'] = np.loadtxt(path)
    return data


def get_group_size(positions, possible_neighs):
    size = 0
    for pn in possible_neighs:
        size += list(positions).count(pn)
    return size - 1  # need to remove itself from the count


def calc_group_sizes(data):
    positions = data['positions'][:, 1:]
    num_cells_forward = data['num_cells_forward']
    num_cells_backward = data['num_cells_backward']
    gsw = GroupSizeWriter()

    for i in range(np.shape(positions)[0]):
        group_size = [i]
        for j in range(np.shape(positions)[1]):
            current_fish_pos = np.int(positions[i][j])
            possible_neighs = list(range(current_fish_pos, current_fish_pos + num_cells_forward + 1))
            possible_neighs += list(range(current_fish_pos - 1, current_fish_pos - num_cells_backward - 1, -1))
            group_size.append(get_group_size(positions[i], possible_neighs))
        gsw.write(group_size)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Calculate the group sizes per iteration')
    parser.add_argument('path',
                        type=str,
                        help='path to position data')
    parser.add_argument('--group_threshold',
                        type=int,
                        help='threshold below which a fish gathering is not considered a group',
                        default=2)
    parser.add_argument('--num_cells_forward',
                        type=int,
                        help="number of cells in the forward direction that influence a fish's "
                             "decision to join a group",
                        default=3)
    parser.add_argument('--num_cells_backward',
                        type=int,
                        help="number of cells in the backward direction that influence a fish's "
                             "decision to join a group",
                        default=3)
    args = parser.parse_args()

    if args.path:
        data = load_data(args.path)
        data['group_threshold'] = args.group_threshold
        data['num_cells_forward'] = args.num_cells_forward
        data['num_cells_backward'] = args.num_cells_backward
        calc_group_sizes(data)

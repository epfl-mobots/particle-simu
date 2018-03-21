#!/usr/bin/env python

import scipy.io as sio
import argparse
import numpy as np


class CoordToCell:
    def __init__(self, num_cells, from_cell=False):
        self.__from_cell = from_cell
        if not self.__from_cell:
            self.__to_cell = self.__get_cell_map(num_cells)
        else:
            self.__to_cell = self.__get_cell_map(num_cells, ub=num_cells)

    def __get_cell_map(self, num_cells, lb=0, ub=360):
        to_cell = dict()
        lspace = np.linspace(lb, ub, num_cells)

        for i in range(np.shape(lspace)[0]):
            to_cell[str(lspace[i])] = i
        return to_cell

    def get_cell(self, val):
        for map_val, cell in self.__to_cell.items():
            if val < float(map_val):
                return self.__to_cell[map_val]


class PositionWriter:
    def __init__(self):
        self.__ofile = open('positions.dat', 'w')
        self.__ofile.write('#iteration fish1_position fish1_position ...\n')

    def write(self, line):
        oline = str(line)
        oline = oline.replace('[', '')
        oline = oline.replace(']', '')
        oline = oline.replace(',', '')
        self.__ofile.write(oline + '\n')

    def __del__(self):
        self.__ofile.close()


def load_cartesian_data(path):
    data = dict()
    raw_data = sio.loadmat(path)
    data['raw_trajectories'] = raw_data['trajectories']
    return data


def load_cell_data(path):
    data = dict()
    data['positions'] = np.loadtxt(path, skiprows=1)
    return data


def to_360(theta):
    if theta < 0:
        theta += 360
    return theta


def nanmean(mat):
    if len(np.shape(mat)) < 3:
        return mat
    else:
        return np.nanmean(mat, 0)


def from_cartesian_to_cell(data):
    fish_traj = data['raw_trajectories'] / data['resolution']
    ctc = CoordToCell(data['num_cells'])
    center = np.array([data['resolution']/2, data['resolution']/2]) / data['resolution']
    last_known_pos = np.empty([np.shape(fish_traj)[1], np.shape(fish_traj)[2]])
    pw = PositionWriter()
    fps = data['fps']

    j = 0
    for i in range(0, np.shape(fish_traj)[0] - fps + 1, fps):
        pos_per_iter = [int(i / fps)]
        if fps == 1:
            fish_mean_traj = nanmean(fish_traj[i, :, :])
        else:
            fish_mean_traj = nanmean(fish_traj[i:i+fps, :, :])
        for j in range(np.shape(fish_mean_traj)[0]):
            pos_fish_j = fish_mean_traj[j, :]
            if np.isnan(pos_fish_j)[0] or np.isnan(pos_fish_j)[1]:
                # pos_fish_j = last_known_pos[j]
                pos_per_iter.append(np.nan)
                continue
            else:
                last_known_pos[j] = pos_fish_j
            diff = pos_fish_j - center
            theta = to_360(np.arctan2(diff[1], diff[0]) * 180 / np.pi)
            cell = ctc.get_cell(theta)
            pos_per_iter.append(cell)
        all_nans = np.count_nonzero(~np.isnan([pos_per_iter[1:]]))
        if all_nans > 0:
            nm = np.nanmean(pos_per_iter[1:])
            for j in range(1, len(pos_per_iter)):
                if np.isnan(pos_per_iter[j]):
                    pos_per_iter[j] = int(nm)
        else:
            for j in range(1, len(pos_per_iter)):
                diff = last_known_pos[j-1] - center
                theta = to_360(np.arctan2(diff[1], diff[0]) * 180 / np.pi)
                cell = ctc.get_cell(theta)
                pos_per_iter[j] = cell
        pw.write(pos_per_iter)


def from_cell_to_cell(data):
    pw = PositionWriter()
    ctc = CoordToCell(data['num_cells'], True)
    positions = data['positions'][:, 1:]
    assert data['num_cells'] < np.max(positions[:, 1:]), 'You are attempting up-sample'

    div = (np.max(positions[:, 1:]) + 1) / data['num_cells']
    factor = 1 / div
    fps = int(div)
    for i in range(0, np.shape(positions)[0] - fps + 1, fps):
        fish_mean_traj = np.ceil(np.nanmean(positions[i:i + fps][:][:], 0))

        pos_per_iter = [int(i / fps)]
        for j in range(len(fish_mean_traj)):
            cell = ctc.get_cell(fish_mean_traj[j] * factor)
            pos_per_iter.append(cell)
        pw.write(pos_per_iter)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot the "Fish in Ring"')
    parser.add_argument('path',
                        type=str,
                        help='path to the raw .mat trajectories')
    parser.add_argument('--resolution', '-r',
                        type=int,
                        help='provide the camera resolution of the pictures',
                        default=2040)
    parser.add_argument('--num_cells', '-n',
                        type=int,
                        help='number of cells to use in the discretization process',
                        default=40)
    parser.add_argument('--fps', '-f',
                        type=int,
                        help='frames per second for the raw data',
                        default=15)
    parser.add_argument('--from-cell',
                        action='store_true',
                        help='discretize a set of cells')
    args = parser.parse_args()

    if args.path:
        if not args.from_cell:
            data = load_cartesian_data(args.path)
            data['num_cells'] = args.num_cells
            data['resolution'] = args.resolution
            data['fps'] = args.fps
            from_cartesian_to_cell(data)
        else:
            data = load_cell_data(args.path)
            data['num_cells'] = args.num_cells
            from_cell_to_cell(data)

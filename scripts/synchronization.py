#!/usr/bin/env python
from itertools import combinations
from termcolor import colored
from scipy import signal
import os, errno
import argparse
import numpy as np
import matplotlib.pyplot as plt
from pprint import pprint


def findLocalMaxima(ar):
    maxInd = np.zeros_like(ar)
    peakVar = -np.inf
    i = -1
    while i < len(ar)-1:
    #for i in range(len(ar)):
        i += 1
        if peakVar < ar[i]:
            peakVar = ar[i]
            for j in range(i,len(ar)):
                if peakVar < ar[j]:
                    break
                elif peakVar == ar[j]:
                    continue
                elif peakVar > ar[j]:
                    peakInd = i + np.floor(abs(i-j)/2)
                    maxInd[peakInd.astype(int)] = 1
                    i = j
                    break
        peakVar = ar[i]
    maxInd = np.where(maxInd)[0]
    return maxInd 


def compute_time_spent_heading(polarities):
    trajectories = []
    for col in range(np.shape(polarities)[1]):
        trajectories.append([polarities[0, col]])
    for col in range(np.shape(polarities)[1]):
        for row in range(1, np.shape(polarities)[0]):
            trajectories[col].append(trajectories[col][-1] + polarities[row, col])
    return trajectories


def compute(polarities, args):
    polarities = data['polarities'][:, 1:]
    tsh = compute_time_spent_heading(polarities)
    sync_data = dict()
    time = np.array(range(len(tsh[0])))

    cols = list(range(len(tsh)))
    for i in cols:
        points = np.array(tsh[i]) / 40
        lmax = findLocalMaxima(points)
        lmin = findLocalMaxima(1/points)
        loptima = list(lmax) + list(lmin)
        loptima = np.sort(loptima)
        sync_data[i] = {'points': points, 'lmax': lmax, 'lmin': lmin, 'loptima': loptima}
    
    plt.figure(figsize=[8, 6], dpi=100)
    for i in range(len(sync_data)):
        # p = plt.plot(sync_data[i]['points'], '--', linewidth=0.8)
        # plt.plot(sync_data[i]['points'],'^',  markevery=list(lmax), c=p[-1].get_color())
        # plt.plot(sync_data[i]['points'],'v',  markevery=list(lmin), c=p[-1].get_color())
        p = plt.plot(time, sync_data[i]['points'], '--', linewidth=1.5)
    plt.ylabel('Distance covered in direction CW/CCW')
    plt.xlabel('Time')
    plt.show()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('path',
                        type=str,
                        help='path to the folder containing the simulation results')
    parser.add_argument('--robot-idx', '-i',
                        type=int,
                        default=0,
                        help='index of the robot in the data')
    args = parser.parse_args()

    if args.path:
        data = dict()
        data['polarities'] = np.loadtxt(args.path, skiprows=1)
        compute(data, args)
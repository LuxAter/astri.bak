#!/usr/bin/env python3

import glob
import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import imread
from colormath.color_objects import sRGBColor, LabColor
from colormath.color_conversions import convert_color
from colormath.color_diff import delta_e_cie1976


def delta_e(a, b):
    return delta_e_cie1976(
        convert_color(sRGBColor(a[0], a[1], a[2], True), LabColor),
        convert_color(sRGBColor(b[0], b[1], b[2], True), LabColor))

def escape(color):
    return "\033[48;2;{};{};{}m".format(color[0], color[1], color[2])

def main():
    for file in glob.glob("./*.jpg"):
        avg = np.zeros(3)
        avg_high = np.zeros(3)
        img = imread(file)
        count = 0
        count_high = 0
        for row in img:
            for px in row:
                if delta_e(px, (0,0,0)) > 50:
                    avg += px
                    count += 1
                if delta_e(px, (0,0,0)) > 90:
                    avg_high += px
                    count_high += 1
        avg /= count
        avg = avg.astype('int')
        avg_high /= count_high
        avg_high = avg_high.astype('int')
        print("{:20} : {} / {}".format(file, avg, avg_high))
        print("{:20}   {}{}   {}{}\033[0m".format(' ', escape(avg), ' ' * 13, escape(avg_high), ' ' * 13))


if __name__ == "__main__":
    main()

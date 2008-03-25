#!/usr/bin/env python
"""
Prints a ordered list of attributes sorted by the highest gain of
information.
"""

import sys
import os
import csv

def work(args):
    for path in args:
        reader = csv.reader(open(path))
        columns = reader.next()

if __name__ == "__main__":
    work(sys.argv)

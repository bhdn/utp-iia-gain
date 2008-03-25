#!/usr/bin/env python
"""
Prints a ordered list of attributes sorted by the highest gain of
information.
"""

import sys
import os
import csv

def choose_ref_attr(cols):
    while True:
        print "Escolha o atributo de referencia:"
        for i, col in enumerate(cols):
            print "%d - %s" % (i, col)
        default = len(cols) # the last columns is the default
        rawchoice = raw_input("Qual destes? [%d]" % default)
        if rawchoice:
            try:
                choice = int(rawchoice)
            except ValueError:
                print "Escolha invalida!"
                continue
        else:
            choice = default
        break
    return choice - 1

def output_attrs(cols):
    pass

def sort_by_gain(cols, entries, ref):
    pass

def work(args):
    for path in args:
        print "Arquivo %s" % path
        reader = csv.reader(open(path))
        cols = reader.next()
        choice = choose_ref_attr(cols)
        sortedcols = sort_by_gain(cols, reader, choice)
        output_attrs(sortedcols)

if __name__ == "__main__":
    work(sys.argv[1:])

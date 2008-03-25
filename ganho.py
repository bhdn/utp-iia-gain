#!/usr/bin/env python
"""
Prints a ordered list of attributes sorted by the highest gain of
information.
"""

import sys
import os
import math
import csv

def choose_ref_attr(cols):
    while True:
        print "Escolha o atributo de referencia:"
        for i, col in enumerate(cols):
            print "%d - %s" % (i + 1, col)
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
    print "Atributos ordenados por entropia:"
    for entropy, attr in cols:
        print "\t%s - %.02f" % entropy

def sort_by_gain(attrcols, entries, ref):
    # a list of hash tables mapping each class with the occourrence inside
    # each attribute:
    allclasses = [{}] * len(attrcols)
    lines = 0
    for items in entries:
        lines += 1
        for idx, col in enumerate(items):
            allclasses[idx][col] = allclasses[idx].get(col, 0) + 1
    entropies = []
    lines = float(lines) # argh!
    for idx, classes in enumerate(allclasses):
        for class_, count in classes.iteritems():
            p = count / lines
            entropy += -p * math.log(p, 2)
        if idx == ref:
            # our reference attribute, doesn't need to be sorted
            refentropy = entropy
            continue
        entropies.append((entropy, attrcols[idx]))
    entropies.sort()
    return entropies
        
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

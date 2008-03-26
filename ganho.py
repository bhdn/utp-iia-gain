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
        print "\t%s - %f" % (attr, entropy)

def sort_by_gain(attrcols, entries, ref):
    # a list of hash tables mapping each class of the domain with the
    # occourrence inside each attribute:
    allclasses = [{} for i in xrange(len(attrcols))]
    lines = 0
    for items in entries:
        lines += 1
        for idx, col in enumerate(items):
            try:
                allclasses[idx][col][0] += 1
            except KeyError:
                allclasses[idx][col] = [1, [0]*len(attrcols)]
    entropies = []
    lines = float(lines) # argh!
    for idx, classes in enumerate(allclasses):
        #XXX FINISH, add the code to use refcount and separate the code
        # that handles the global entropy
        # this code should calculate the entropy of each class of each
        # attribute
        entropy = 0.0
        for class_, (count, refcount) in classes.iteritems():
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

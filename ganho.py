#!/usr/bin/env python
"""
Prints an ordered list of attributes sorted by the highest gain of
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
        print "%-25s %f" % (attr, entropy)

def collect_attributes(ncols, source, ref):
    # ["clima", "dinheiro", "quantos"] (column names)
    # [{"chuva": [N, {refcount}],
    #   "sol": [N, {refcount}],
    #   "frio": [N, {refcount}]},
    #  {"muito": [N, {refcount}],
    #   "pouco": [N, {refcount}]},
    #  {"so": [N, {refcount}],
    #   "dois": [N, {refcount}],
    #   "tres": [N, {refcount}],
    #   "um bando": [N, {refcount}]}]
    #
    # refcount counts how many times the class matched a given class in the
    # "reference" attribute. This is used to obtain the entropy for each
    # class.  Seems to not be a good representation.
    # 
    allclasses = [{} for i in xrange(ncols)]
    refclasses = {}
    lines = 0
    for items in source:
        refclass = items[ref]
        for idxattr, class_ in enumerate(items):
            if idxattr == ref:
                # the only useful information about the classes of the
                # reference attribute is their frequency
                refclasses[class_] = refclasses.setdefault(class_, 0) + 1
                continue
            try:
                counter = allclasses[idxattr][class_]
            except KeyError:
                counter = allclasses[idxattr][class_] = [0, {}]
            counter[0] += 1
            counter[1][refclass] = \
                    counter[1].setdefault(refclass, 0) + 1
        lines += 1
    return lines, refclasses, allclasses

def information_gain(attrcols, lines, refclasses, allclasses, ref):
    # obtain the reference attribute entropy:
    refentropy = 0.0
    lines = float(lines) # argh!
    for class_, count in refclasses.iteritems():
        p = count / lines
        refentropy += -p * math.log(p, 2)
    # calculate the gain for each attribute:
    for attridx, classes in enumerate(allclasses):
        if attridx == ref:
            continue
        attrgain = refentropy
        for class_, counter in classes.iteritems():
            classcount = float(counter[0])
            attrentropy = 0.0
            for refclass, refcount in counter[1].iteritems():
                p = refcount / classcount
                attrentropy += -p * math.log(p, 2)
            attrgain += -(classcount / lines) * attrentropy
        yield (attrgain, attrcols[attridx])

def sort_by_gain(attrcols, source, ref):
    lines, refclasses, allclasses = collect_attributes(len(attrcols),
            source, ref)
    gain = information_gain(attrcols, lines, refclasses, allclasses, ref)
    return sorted(gain, reverse=True)
        
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

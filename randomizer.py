#!/usr/bin/env python
"""
Creates random data to feed the information gain program.
"""
import sys
from random import shuffle, randint, choice

def words(wordfile):
    f = open(wordfile)
    lines = f.readlines()
    shuffle(lines)
    for line in lines:
        yield line.strip()
    n = 0
    while True:
        yield "exceededwordlist%d" % n
        n += 1

def randomize(out, wordfile):
    wordsrc = words(wordfile)
    nattribs = randint(2, 50)
    nlines = randint(2, 100000)
    domains = []
    names = []
    for na in xrange(nattribs):
        ndomain = randint(2, nattribs)
        domain = [wordsrc.next() for x in xrange(ndomain)]
        domains.append(domain)
        names.append(wordsrc.next())
    line = ",".join(names) + "\n"
    out.write(line)
    for nl in xrange(nlines):
        items = [choice(domains[na]) for na in xrange(nattribs)]
        line = ",".join(items) + "\n"
        out.write(line)

if __name__ == "__main__":
    if not len(sys.argv) >= 2:
        raise SystemExit, "you must supply a word list file!"
    randomize(sys.stdout, sys.argv[1])

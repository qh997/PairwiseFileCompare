#!/usr/bin/python

import sys
import getopt
import os
import time
import stat
import hashlib

HASH = False
DEBUG = 0

def usage():
    print """
USAGE:
    python pfc.py [-h|--hash] [-d|--debug]
"""

def init():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hd', ['hash', 'debug='])
    except getopt.GetoptError:
        usage()
        sys.exit()

    global HASH
    global DEBUG

    for opt, arg in opts:
        if opt == '--hash' or opt == '-h':
            HASH = True
        if opt == '-d':
            DEBUG = 1
        if opt == '--debug':
            DEBUG = int(arg)

    if not args:
        args.append(os.getcwd() + os.path.sep)

    return args

def debug(msg, level = 1):
    if level <= DEBUG:
        ori_time = time.time();
        crt_time = (time.strftime('%H:%M:%S',time.localtime(ori_time))
                 + '.' + str(ori_time - int(ori_time))[2:5])
        print "[DebugMessage %s %d] %s" % (crt_time, level, msg)

def getFileList(path):
    if os.path.exists(path):
        dirlist = []

        for name in os.listdir(path):
            fullname = os.path.join(path, name)
            st = os.lstat(fullname)
            if stat.S_ISDIR(st.st_mode):
                dirlist += getFileList(fullname)
            elif stat.S_ISREG(st.st_mode):
                    dirlist.append(fullname)

        return dirlist
    else:
        return []

def fileSHA1(filepath):
    with open(filepath,'rb') as f:
        sha1obj = hashlib.sha1()
        sha1obj.update(f.read())
        hash = sha1obj.hexdigest()
        return hash

if __name__ == '__main__':
    hash_files = {}

    for path in init():
        debug(path, 1)
        for filepath in getFileList(path):
            debug(filepath, 2)
            fSHA1 = fileSHA1(filepath)
            debug(fSHA1, 3)

            if fSHA1 not in hash_files.keys():
                hash_files[fSHA1] = []

            hash_files[fSHA1].append(filepath)

    results = {}
    for hash_code in hash_files.keys():
        if hash_files[hash_code][1:]:
            results[hash_code] = sorted(hash_files[hash_code])

    for group in sorted(results.iteritems(), key = lambda x: x[1][0]):
        if HASH: print group[0]
        for f in group[1]: print f
        print

# Cvelth <Cvelth.mail@gmail.com> © 2020
# version 0.1

import argparse
import os
from pathlib import Path
import re

parser = argparse.ArgumentParser(description='Runs gcov and uploads results to codecov.')
parser.add_argument('filename', help='input filename')
parser.add_argument('--output', '-o', help='target file name')
parser.add_argument('--limit', '-l', help='limits output to specified file(s)')

args = parser.parse_args()

gcda = list(Path('.').rglob(args.filename + '.gcda'))
gcno = list(Path('.').rglob(args.filename + '.gcno'))

if (args.limit):
    included_files = args.limit.split(';')

if (len(gcda) == 1):
    gcda = gcda[0]
else:
    print('Error: ' + args.filename + '.gcda file not found.')
    os._exit(2)

if (len(gcno) == 1):
    gcno = gcno[0]
else:
    print('Error: ' + args.filename + '.gcno file not found.')
    os._exit(2)

Path('build/gcov').mkdir(exist_ok=True)
os.chdir('build/gcov')
os.system('gcov ' + '../../' + str(gcda)[:str(gcda).rfind(".")] + '.cpp')

Path('../coverage').mkdir(exist_ok=True)

input = open(args.filename + '.gcov', "r")
if (args.output):
    output = open('../coverage/' + args.output + '.gcov', "w")
else:
    output = open('../coverage/' + args.filename + '.gcov', "w")

current_file = None
current_file_full = None
output_enabled = True
def to_output(line):
    if (args.limit):
        global output_enabled
        m = re.search('@to_single_cpp (?:include|source)_file_begin ([^ ]+) \((.*)\)', line)
        if (m):
            current_file = m.group(1)
            current_file_full = m.group(2)
            if (current_file in included_files or current_file_full in included_files):
                print ('Including ' + current_file + ' (' + current_file_full + ')')
                output_enabled = True
            else:
                print ('Ignoring ' + current_file + ' (' + current_file_full + ')')
                output_enabled = False

        return output_enabled
    else:
        return True

for line in input:
    if (to_output(line)):
        output.write(line)

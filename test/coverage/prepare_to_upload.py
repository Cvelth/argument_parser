# Cvelth <Cvelth.mail@gmail.com> © 2020
# version 0.2.0.1

import argparse
import os
from pathlib import Path
import re

parser = argparse.ArgumentParser(description='Runs gcov and uploads results to codecov.')
parser.add_argument('filename', help='input filename')

args = parser.parse_args()

gcda = list(Path('.').rglob(args.filename + '.gcda'))
gcno = list(Path('.').rglob(args.filename + '.gcno'))

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

Path('build').mkdir(exist_ok=True)
Path('build/test_coverage').mkdir(exist_ok=True)
Path('build/test_coverage/gcov').mkdir(exist_ok=True)
os.chdir('build/test_coverage/gcov')
os.system('gcov ' + '../../../' + str(gcda)[:str(gcda).rfind('.')] + '.cpp')

input = open(args.filename + '.gcov', 'r')

current_file = None
line_shift = 0
file_header = ''
for line in input:
    m = re.match('^(.*):([0-9 ]+):(.*)$', line)
    if (m):
        if (int(m.group(2)) == 0):
            mm = re.match('^([a-zA-Z]+):(.*)$', m.group(3))
            if (mm):
                if (mm.group(1) == "Source"):
                    file_header += m.group(1) + ':' + m.group(2) + ':' + mm.group(1) + ':{@current_full_path}\n'
                elif (mm.group(1) == "Graph"):
                    file_header += m.group(1) + ':' + m.group(2) + ':' + mm.group(1) + ':{@gcno_full_path}\n'
                elif (mm.group(1) == "Data"):
                    file_header += m.group(1) + ':' + m.group(2) + ':' + mm.group(1) + ':{@gcda_full_path}\n'
                else:
                    file_header += line
            else:
                file_header += line
        else:
            mm = re.match('^// @to_remove @(?:include|source)_file_begin ([^ ]+) \((.*)\)(?:.*)$', m.group(3))
            if (mm):
                print ("Processing " + mm.group(1) + '.gcov')
                current_file = open('../' + mm.group(1) + '.gcov', 'w')
                line_shift = int(m.group(2))

                temp = re.sub('{@current_full_path}', mm.group(2), file_header)
                temp = re.sub('{@gcno_full_path}', re.sub(args.filename, mm.group(1), str(gcno)), temp)
                temp = re.sub('{@gcda_full_path}', re.sub(args.filename, mm.group(1), str(gcda)), temp)
                current_file.write(temp)
            elif (re.search('@to_remove', m.group(3))):
                continue
            else:
                mm = re.match('^// @to_restore (.*)$', m.group(3))
                if (mm):
                    current_file.write(m.group(1) + ':' + str(int(m.group(2)) - line_shift).rjust(len(m.group(2))) + ':' + mm.group(1) + '\n')
                else:
                    current_file.write(m.group(1) + ':' + str(int(m.group(2)) - line_shift).rjust(len(m.group(2))) + ':' + m.group(3) + '\n')
    else:
        current_file.write(line)

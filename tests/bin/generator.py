#!/usr/bin/env python
import os
import sys
import string
import struct
import json
import random

from itertools import repeat

base_seed = 10000079
buffer_size = 1000000

def generate_matrix(n, p, m, path):
    with open(path, 'w') as f:
        f.write(str(n) + '\t' + str(p) + '\t' + str(m) + '\n')
        buffer = ''
        for i in range(n):
            for j in range(p):
                val = random.randint(-100, 100)
                buffer += str(val) + '\t'
                if len(buffer) > buffer_size:
                    f.write(buffer)
                    buffer = ''
            buffer += '\n'
        if buffer != '':
            f.write(buffer)
            buffer = ''
        for i in range(p):
            for j in range(m):
                val = random.randint(-100, 100)
                buffer += str(val) + '\t'
                if len(buffer) > buffer_size:
                    f.write(buffer)
                    buffer = ''
            buffer += '\n'
        if buffer != '':
            f.write(buffer)

def generate(matrixsize, file_path):
    n = random.randint(max(0, matrixsize - 200), matrixsize)
    p = random.randint(max(0, matrixsize - 200), matrixsize)
    m = random.randint(max(0, matrixsize - 200), matrixsize)
    matrix_path = os.path.join(file_path, 'matrix.txt')
    generate_matrix(n, p, m, matrix_path)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit('usage: generator.py <config_location> <output_dir>\n example: python generator.py 1.config tests-pmm')

    config_path = sys.argv[1]
    output_dir = sys.argv[2]
    with open(config_path, 'r') as f:
        config = json.load(f)
        file_path = str(config['filename'])
        if config.has_key('seed'):
            base_seed = config['seed']
        file_path = os.path.join(output_dir, file_path)
        if not os.path.exists(file_path):
            os.mkdir(file_path)
            print('Generate data based on ' + config_path)
            generate(config['matrixsize'], file_path)

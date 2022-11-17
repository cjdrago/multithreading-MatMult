#! /usr/bin/env python3

import argparse
import os
import signal
import subprocess
import sys
import time
import binascii
import json

#
# helper routines
#

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def read_file(f):
    with open(f, 'r') as content_file:
        content = content_file.read()
    return content

def write_file(f, data):
    with open(f, 'w') as content_file:
        content_file.write(data)

def print_failed(msg, expected, actual, test_passed, verbosity):
    if test_passed: # up til now, no one said that the test failed
        print('RESULT failed')
    if verbosity >= 0:
        print(msg)
        if verbosity < 2 and hasattr(expected, '__len__') and len(expected) > 1000:
            print('Showing first 500 bytes; use -vv to show full output')
            expected = expected[:500]
            actual = actual[:500]
        print('expected: [{}]'.format(expected))
        print('got:      [{}]\n'.format(actual))

# some code from:
# stackoverflow.com/questions/18404863/i-want-to-get-both-stdout-and-stderr-from-subprocess
def run_command(cmd, timeout_length, verbosity):
    if verbosity:
        print('Building Command\n\t', cmd)
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)

    try:
        if timeout_length == -1:
            std_output, std_error = proc.communicate()
        else:
            std_output, std_error = proc.communicate(timeout=timeout_length)
    except subprocess.TimeoutExpired:
        print('WARNING test timed out (i.e., it took too long)')
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        std_output, std_error = proc.communicate()
    return std_output.decode(), std_error.decode(), int(proc.returncode)

def handle_outcomes(stdout_expected, stdout_actual, stderr_expected, stderr_actual, rc_expected, rc_actual, verbosity, total_time):
    test_passed = True
    if verbosity != -1 and total_time is not None:
        print('Test finished in {:.3f} seconds'.format(total_time))
    if rc_actual != rc_expected:
        print_failed('return code does not match expected', rc_expected, rc_actual, test_passed, verbosity)
        test_passed = False
    stdout_actual = ' '.join(stdout_actual.replace('\t', ' ').replace('\n', ' ').strip().split())
    stdout_expected = ' '.join(stdout_expected.replace('\t', ' ').replace('\n', ' ').strip().split())
    if stdout_expected != stdout_actual:
        print_failed('standard output does not match expected', stdout_expected, stdout_actual, test_passed, verbosity)
        test_passed = False
    # if stderr_actual != stderr_expected:
    #     print_failed('standard error does not match expected', stderr_expected, stderr_actual, test_passed, verbosity)
    #     test_passed = False
    if test_passed:
        print(bcolors.OKGREEN + 'RESULT passed' + bcolors.ENDC)
        return True
    return False

def test_exists(test_number, input_dir):
    input_file = '{}/{}-0.in'.format(input_dir, test_number)
    input_dir = '{}/{}-0'.format(input_dir, test_number)
    return os.path.exists(input_file) or os.path.exists(input_dir)

def save_output(test_number, stdout_dir, stdout_actual, stderr_actual, rc_actual, total_time):
    write_file('{}/{}.out'.format(stdout_dir, test_number), str(stdout_actual))
    # write_file('{}/{}.err'.format(stdout_dir, test_number), str(stderr_actual))
    write_file('{}/{}.rc'.format(stdout_dir, test_number), str(rc_actual))
    print('Total time {:.3f} sec'.format(total_time))
    return True

def get_test_description(base_desc, matrixsize, timeout):
    base_desc = base_desc.replace('${timeout}', str(timeout)).replace("${matrixsize}", str(matrixsize))
    return base_desc

def test_one(test_number, test_path, config_dir, stdout_dir, binary_file, timeout_length, verbosity, show_timing, save):
    config = json.load(open(os.path.join(config_dir, '{}.json'.format(test_number)), 'r'))
    test_desc = get_test_description(config['description'], config['matrixsize'], config['timeout'])
    
    if not save:
        stdout_expected = read_file('{}/{}.out'.format(stdout_dir, test_number))
        stderr_expected = ''
        rc_expected = int(read_file('{}/{}.rc'.format(stdout_dir, test_number)))

    print('TEST {} - {}'.format(test_number, test_desc))
    if save:
        timeout_length = -1
    else:
        timeout_length = config['timeout']
    if show_timing:
        start_time = time.time()
    
    run_desc = './' + binary_file + ' < ' + test_path
    stdout_actual, stderr_actual, rc_actual = run_command(run_desc, timeout_length, verbosity)
    if show_timing:
        total_time = time.time() - start_time
    else:
        total_time = None

    if save:
        return save_output(test_number, stdout_dir, stdout_actual, stderr_actual, rc_actual, total_time)
    else:
        return handle_outcomes(stdout_expected, stdout_actual, stderr_expected, stderr_actual, rc_expected, rc_actual, verbosity, total_time)

#
# main test code
#
parser = argparse.ArgumentParser()
parser.add_argument('-c', '--continue', dest='continue_testing', help='continue testing even when a test fails', action='store_true') 
parser.add_argument('-C', '--config_dir', dest='config_directory', help='path to location of configs', type=str, default='')
parser.add_argument('-S', '--start_test', dest='start_test', help='start with this test number', type=int, default=1)
parser.add_argument('-E', '--end_test', dest='end_test', help='end with this test number; -1 means go until done', type=int, default=-1)
parser.add_argument('-s', '--source_file', dest='source_file', help='name of source file to test', type=str, default='')
parser.add_argument('-b', '--binary_file', dest='binary_file', help='name of binary to produce', type=str, default='a.out')
parser.add_argument('-B', '--build', dest='build', help='whether to build', type=int, default=0)
parser.add_argument('-t', '--test_dir', dest='test_dir', help='path to location of tests', type=str, default='')
parser.add_argument('-T', '--timeout', dest='timeout_length', help='length of timeout in seconds', type=int, default=30)
parser.add_argument('--timed', dest='show_timing', help='show time taken by each test in seconds', action='store_true', default=True)
parser.add_argument('-f', '--build_flags', help='extra build flags for gcc', type=str, default='')
parser.add_argument('-std', '--stdout_dir', dest='stdout_directory', help='path to location of standard outputs', type=str, default='')
parser.set_defaults(verbosity=0)
group = parser.add_mutually_exclusive_group()
group.add_argument('-q', '--quiet', dest='verbosity', help='only display test number and result', action='store_const', const=-1)
group.add_argument('-v', '--verbose', dest='verbosity', help='show command line so you can replicate for debugging, and also some other extra information', action='count')
args = parser.parse_args()

input_file = args.source_file
test_dir = args.test_dir
binary_file = args.binary_file
build = args.build
config_dir = args.config_directory
stdout_dir = args.stdout_directory
args.show_timing = True
if build == 1:
    print('TEST 0 - clean build (program should compile without errors or warnings)')
    if args.show_timing:
        start_time = time.time()
    stdout_actual, stderr_actual, rc_actual = run_command('gcc {} -o {} {}'.format(input_file, binary_file, args.build_flags), args.timeout_length, args.verbosity)
    if args.show_timing:
        total_time = time.time() - start_time
    else:
        total_time = None
    if handle_outcomes('', stdout_actual, '', stderr_actual, 0, rc_actual, args.verbosity, total_time) == False:
        exit(1)
    print('')

test_number = args.start_test
while True:
    test_path = os.path.join(test_dir, str(test_number) + '/matrix.txt')
    test_result = test_one(test_number, test_path, config_dir, stdout_dir, binary_file, args.timeout_length, args.verbosity, args.show_timing, False)
    print('')
    
    if not args.continue_testing and not test_result:
        print('TESTING HALTED (use -c or --continue to keep going if you like)')
        exit(1)

    test_number += 1
    if args.end_test != -1 and test_number > args.end_test:
        break

exit(0)



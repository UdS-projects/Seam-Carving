#!/usr/bin/env python2
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

from __future__ import print_function

if (__name__ == '__main__'):
    import argparse
    import os
    import re
    import subprocess
    import sys
    import threading

    class TimeoutError(Exception):
        pass

    class Command(object):
        def __init__(self, cmd):
            self.cmd = cmd
            self.process = None
            self.out = None
            self.err = None

        def run(self):
            def target():
                self.process = subprocess.Popen(self.cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                self.out, self.err = self.process.communicate()

            thread = threading.Thread(target=target)
            thread.start()
            thread.join(timeout=10)
            if thread.is_alive():
                self.process.terminate()
                thread.join()
                raise TimeoutError

            return (self.process.returncode, self.out, self.err)

    cwd = os.path.dirname(os.path.abspath(__file__))
    defaultTestDir = cwd

    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--filter', type=str, metavar='<regex>', help='only execute tests matching this regex')
    parser.add_argument('-l', '--list', action='store_true', help='only list tests, don\'t execute')

    args = parser.parse_args()

    test_filter = None
    if args.filter:
        test_filter = re.compile(args.filter)

    tests = {
        'pub.statistics_invalidinput.imgbroken1',
        'pub.statistics_invalidinput.imgbroken2',
        'pub.statistics.small1',
        'pub.statistics.small2',
        'pub.statistics.owl',
        'pub.min_path.small1',
        'pub.min_path.small2',
        'pub.min_path.owl',
        'pub.carve.small1_0',
        'pub.carve.small1_1',
        'pub.carve.small2_0',
        'pub.carve.small2_1',
    }

    all_tests = []

    for test in tests:
        if not test_filter or test_filter.match(test):
            all_tests.append(test)
    all_tests.sort()

    if args.list:
        for test in all_tests:
            print(test)
        exit(0)

    binary = "build/carve"

    if not (os.path.isfile(binary) and os.access(binary, os.X_OK)):
        print("'%s' is not a file or not executable" % binary)
        exit(1)

    def run(args):
        cmd = Command([binary] + args)
        return cmd.run()

    def test_statistics_invalidinput(case):
        args = ['-s', 'data/' + case + '.ppm']
        rc, out, err = run(args)
        if rc == 1:
            if err == "":
                return None
            else:
                return '\n' + err
        else:
            return 'application did not return EXIT_FAILURE\n' + err

    def test_statistics(case):
        args = ['-s', 'data/' + case + '.ppm']
        ref = 'test_data/' + case + '.statistics'
        rc, out, err = run(args)
        if rc == 0:
            if out == open(ref, 'r').read():
                return None
            else:
                return 'incorrect statistics'
        else:
            return 'application did not return EXIT_SUCCESS\n' + err

    def test_min_path(case):
        args = ['-p', 'data/' + case + '.ppm']
        ref = 'test_data/' + case + '.path'
        rc, out, err = run(args)
        if rc == 0:
            if out == open(ref, 'r').read():
                return None
            else:
                return 'incorrect minimal path'
        else:
            return 'application did not return EXIT_SUCCESS\n' + err

    def img_cmp(img1_name, img2_name):
        img1 = "".join(open(img1_name, 'r').read().split())
        img2 = "".join(open(img2_name, 'r').read().split())
        return img1 == img2

    def test_carve(case):
        if os.access('out.ppm', os.W_OK):
            os.remove('out.ppm')

        base, num = case.split('_', 1)
        src_img = 'data/' + base + '.ppm'
        ref = 'test_data/' + case + '.ppm'
        args = ['-n', num, src_img]
        rc, out, err = run(args)
        if rc == 0:
            if not os.access('out.ppm', os.R_OK):
                return 'no "out.ppm" produced\n' + err
            if img_cmp('out.ppm', ref):
                return None
            else:
                return 'incorrect output image'
        else:
            return 'application did not return EXIT_SUCCESS\n' + err

    def test(test):
        cat, ex, case = test.split('.', 2)
        if ex == 'statistics_invalidinput':
            return test_statistics_invalidinput(case)
        elif ex == 'statistics':
            return test_statistics(case)
        elif ex == 'min_path':
            return test_min_path(case)
        else:
            assert ex == 'carve'
            return test_carve(case)

    for t in all_tests:
        print("running test %s" % t)
        try:
            msg = test(t)
            if msg == None:
                print("PASS")
            else:
                print("FAIL: %s" % msg)
        except TimeoutError:
            print("FAIL: time out")
        print()

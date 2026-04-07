#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys

if sys.argv[1] != '-e':
    assert sys.argv[1] == '--solve'

equation_results = {
    '0+0': '0',
    'tan5': '0.087488664',
    '√π': '1.772453851',
    '1.23%+10': '10.0123',
}

if sys.argv[2] in equation_results:
    print(equation_results[sys.argv[2]])
else:
    sys.exit(1)

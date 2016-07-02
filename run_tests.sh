#! /bin/bash

(cd tests && cmake . && make && ./iterations_tests)

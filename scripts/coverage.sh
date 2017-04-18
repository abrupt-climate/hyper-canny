#!/bin/bash

cd build
ninja
lcov --directory . --capture --output-file ./code_coverage.info
ninja test
lcov --remove code_coverage.info "/usr/*" "test/*" -o code_coverage.info
genhtml code_coverage.info --output-directory ./code_coverage_report/
cd ..


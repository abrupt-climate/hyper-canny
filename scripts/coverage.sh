#!/bin/bash

if [ ! -d build-coverage ];
then
        meson build-coverage --buildtype debug --werror -Db_coverage=true
fi

cd build-coverage
ninja test
lcov --directory . --capture --output-file ./code-coverage.info
# ninja test
lcov --remove code-coverage.info "/usr/*" "test/*" -o code-coverage.info
genhtml code-coverage.info --output-directory ./coverage-report/
cd ..


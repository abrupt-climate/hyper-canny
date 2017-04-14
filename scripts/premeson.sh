#!/bin/bash

root=$(pwd)

create_meson_file() {
    cc_files=$(find . -name '*.cc' -printf "'%p',")
    d=$(pwd)
    if [ $# -eq 0 ]; then
        id=$(basename $d | sed -e "s/-/_/")
    else
        id="${1}_$(basename $d | sed -e "s/-/_/")"
    fi

    echo "${id}_files = files(${cc_files})" > meson.build
}

create_hierarchy() {
    cd ${1}
    echo -n "" > meson.build

    folders=$(find . -mindepth 1 -maxdepth 1 ${@:2} -type d)
    for d in ${folders}
    do
        echo "subdir('${d}')" >> meson.build
        cd ${d}
        create_meson_file ${1}
        cd ..
    done

    cd ..
}

create_hierarchy src
create_hierarchy test -not -path ./gtest

cd test/gtest/src
echo "gtest_src_files = files('./gtest-all.cc', './gtest_main.cc')" >> meson.build
cd ../..
echo "subdir('gtest/src')" >> meson.build

cd gmock/src
echo "gmock_src_files = files('./gmock-all.cc')" >> meson.build
cd ../..
echo "subdir('gmock/src')" >> meson.build

cd ${root}


#!/bin/bash

for d in intlib interpreter; do
	cd $d
	find . -iname '*.hpp' -o -iname '*.cpp' | xargs /usr/bin/clang-format-19 -style=file -i
	cd ..
done

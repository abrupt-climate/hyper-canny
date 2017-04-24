build/hyper-canny: build
	ninja -C build hyper-canny

build:
	meson build

.PHONY: clean test

clean:
	ninja -C build -t clean

test: build
	ninja -C build test

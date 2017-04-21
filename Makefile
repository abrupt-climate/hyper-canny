build/hyper-canny: build
	ninja -C build hyper-canny

build:
	meson build

.PHONY: clean test

clean:
	ninja -C build -t clean

test:
	ninja -C build test
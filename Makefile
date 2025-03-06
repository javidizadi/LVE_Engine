x11: build
	XDG_SESSION_TYPE=x11 ./build/LVE_Engine

run: build
	./build/LVE_Engine

build:
	cmake . -B build/  -DCMAKE_BUILD_TYPE=Debug
	make -C build/ -j`nproc`

test: build
	ctest --test-dir ./build

clean:
	rm -rf build/


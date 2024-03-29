.POSIX:

CC := clang

SOURCES := src/main.c src/dmm_string.c src/instance.c
TEST_SOURCES := test/instance_test.c test/main_test.c test/main.c

CINCLUDES := -Ibuild/deps/tinker/include -Iinclude

CFLAGS := ${CFLAGS} -std=c11 -pedantic-errors \
		  -Wall -Wextra -Wconversion -Wcast-qual

all: build/dmm-test

build-deps:
	mkdir -p build/deps/
	cd build/deps; test -d tinker || git clone https://github.com/awooos/tinker.git
	cd build/deps/tinker; git pull

build/dmm-test: $(SOURCES)
	$(MAKE) build-deps
	${CC} ${CFLAGS} ${CINCLUDES} ${SOURCES} build/deps/tinker/src/main.c ${TEST_SOURCES} -o $@

test: build/dmm-test
	./build/dmm-test

lint: build-deps
	clang-check ${SOURCES} -- ${CINCLUDES}

clean:
	rm -rf build

.PHONY: all clean test build-deps

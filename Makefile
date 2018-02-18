CC=g++
INCLUDES=-I. -Iexchange

TEST_SOURCES  = test/exchange_main.test.cpp
TEST_SOURCES += test/exchange_currency.test.cpp
TEST_SOURCES += test/exchange_market.test.cpp
TEST_SOURCES += test/exchange_arbitragedestroyer.test.cpp
TEST_SOURCES += test/exchange_fxconversions.test.cpp
TEST_SOURCES += test/exchange_fxconverter.test.cpp

CMAKE_GEN=
CMAKE_GEN=Ninja

all: fxbattle

.PHONY: test exchange fxbattle

exchange: test

fxbattle: test static
	cd build && cmake --build . --target FXBattle.exe -- -j3
	./build/FXBattle.exe config.json traded_pairs.json traders.json

static: build
	cd build && cmake --build . --target static -- -j3

test: build
	cd build && cmake --build . --target Test.exe -- -j3
	./build/Test.exe --reporter=spec

build:
	mkdir build
	mkdir build/templates
	cd build && cmake -DCMAKE_BUILD_TYPE=Release -G "${CMAKE_GEN}" ..

clean:
	rm -rf build

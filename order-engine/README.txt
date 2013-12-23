I've tried to keep dependencies to a minimum. It needs only the GNU make utility and a g++ 4.6 compiler.
The code has been tested on Ubuntu Linux 12.04.3 LTS.

This makefile project that has two targets.

1. all : Use "make all" or simply "make" in this directory to build the order-engine executable. Run it using ./order-engine

2. tests : Use "make tests" to build the test-engine executable. Run it using ./test-engine

To test the order-engine, pipe in the sample inputs to it by the following command :
$> cat testinputs_valid.json | ./order-engine

Use "make clean" to delete all object files and corresponding executables.

I have used the JSON cpp library from http://jsoncpp.sourceforge.net/
For testing I used the Google Test framework from https://code.google.com/p/googletest/

The order-engine target depends on lib json-cpp. The test-engine target depends on lib json-cpp as well as lib gtest.
The static linkable library files for both dependencies have been provided in the libs folder. Their respective include files are in the include folder.

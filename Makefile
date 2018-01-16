INCLUDE_PATH= ./eigen-3.3.4
CFLAGS=$(foreach d, $(INCLUDE_PATH), -I$d) -Wall 

test_fdcl_serial:  test_fdcl_serial.cpp fdcl_serial.h fdcl_serial.cpp
	g++ -o test_fdcl_serial test_fdcl_serial.cpp fdcl_serial.cpp $(CFLAGS) 


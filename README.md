# fdcl_serializlation

This library provides a C++ tool to save variables into a binary buffer or read variables from a binary buffer. 

In computer science, in the context of data storage, [serialization](https://en.wikipedia.org/wiki/Serialization) is the process of translating data structures or object state into a format that can be stored (for example, in a file or memory buffer) or transmitted (for example, across a network connection link) and reconstructed later (possibly in a different computer environment).

This library specifically deals with serialization of floating numbers into a binary buffer, intended to be used for wireless communications. Trasmitting floating numbers over the binary format is desirable as 1. the transmitted data size is smaller 2. there is no loss of floating number digits.

This library is based on [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/html/multi/advanced.html#serialization). A few errors are fixed, and it is wrapped with C++ for dynamic allocation, and also to support [Eigen](http://eigen.tuxfamily.org/) matrix types. 

## Installation

To test the example code provided in this packacge, simply run

```
make
./test_fdcl_serial
```

### Integration to your own project

This library is composed of a single header file, namely `fdcl_serial.h`. You just need to copy the file to your project source folder, and include the header file.

This library also requires the [Eigen library](http://eigen.tuxfamily.org/) as it suppors the eigen matrix types. For convenience, the eigen library is included in the package. The folder to the eigen library should be availalbe to the compiler. For example, the following makefile is used for the sample code.


```
INCLUDE_PATH= ./eigen-3.3.4
CFLAGS=$(foreach d, $(INCLUDE_PATH), -I$d) -Wall 

test_fdcl_serial:  test_fdcl_serial.cpp fdcl_serial.h
	g++ -o test_fdcl_serial test_fdcl_serial.cpp  $(CFLAGS) 
```



## Supported Variable Types

The followig types are suppored.

	1. bool (1 byte)
	2. integer (2 byets)
	3. float (4 byets)
	4. double (8 bytes)
	5. Eigen float or double matrix of an arbitrary size

## Usage

The usuage of the library is illustrated by the following sample code.

```
#include <iostream>
#include <iomanip> // for setprecision
#include "Eigen/Dense"
#include "fdcl_serial.h"

using namespace std;

int main(void)
{
	
	bool b=false;
	int i=-1;
	float f=0.987654321;
	double d=-0.123456789;
	Eigen::Matrix<double, 3, 1> vec(0.1,0.2,0.3);

	unsigned char* buf_received;
	
	// sender packing
	fdcl_serial buf_send;
	
	buf_send.pack(b); // 1 byte
	buf_send.pack(i);  // 2 byte
	buf_send.pack(f);  // 4 byte
	buf_send.pack(d);  // 8 byte
	buf_send.pack(vec); // 8x3 =24 bytes

	cout << "buf_send_size=" << buf_send.size() << " bytes" << endl;

	// transmitting "buf_send.data()" to unsigned char* of the receiver 	
	// this parts is done through a serial port or a wifi
	buf_received=buf_send.data();

	// clear variables to verify unpacking
	b=true;
	i=0;
	f=0.;
	d=0.;
	vec.setZero();
	cout << setprecision(10);

	// receiver unpacking
	fdcl_serial buf_recv(buf_received,39);

	buf_recv.unpack(b); 
	cout << "b=" << b << endl;
	buf_recv.unpack(i);
	cout << "i=" << i << endl;
	buf_recv.unpack(f);
	cout << "f=" << f << endl;
	buf_recv.unpack(d);
	cout << "d=" << d << endl;
	buf_recv.unpack(vec);
	cout << "vec=" << vec << endl;
}
```

### Sender

The library is used by a **sender** that packs data and send it over wifi or serial port, and a **receiver** that collects the binary data and unpack into varialbes. 

Sender initialize an empty buffer by 

```
fdcl_serial buf_send;
```
and pack data by the member function `pack()`. For example, a double variable `d` is packed by

```
buf_send.pack(d);
```
Then, the size of the buffer will be increased by 8 bytes. The above packing procedure can be repeated for other variales. 

Once the packing is completed, the size of the buffer and the data of the buffer can be accessed by `buf_send.size()` and `buf_send.data()` respectively. In particular, `buf_send.data()` returns a type `unsigned char*`, which can be transmitted to the sender via wifi or other communication protocols.


### Receiver

The receiver first recives the data from the sender, and save it to a variable with the type of `unsigned char*`. This procedure is completed via wifi or other communication protocols. 

Once the binary data is received, the receiver creates the class via

```
fdcl_serial buf_recv(buf_received,39);
```
NOte the constructor of the receiver class requires to inputs, namley the variable name of the `unsigned char*` received, and **the size of the data in bytes**. 

It is assumed that the sender and the receiver share the information about

1. the order of variables packed
2. the type of each variable packed
3. the total size of the buffer

When creating the receiver class, it is critial to have the exact size of the buffer in bytes, which can be accesed through `buf_send.size()` after the sender completed the packing.

Once the receiver buffer is created, the data can be unpaced by

```
	buf_recv.unpack(b); 
```



## REQUIREMENTS
* The order and the type of variables in packing must match with those of unpacking.
* The total size of the data packed must be manually specificed when unpacking.
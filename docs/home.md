# FDCL Serialization

This library provides a C++ tool to save variables into a binary buffer or read variables from a binary buffer. 

<a name="contents"></a>
## Contents
1. [Basic Introduction](#intro)
2. [Supported Variable Types](#supported-types) 
3. [Using the Class](#using-the-class)
4. [Sender](#sender)
5. [Receiver](#receiver)
6. [Using Eigen Matrices](#using-eigen)
7. [Requirements](#requirements)
8. [Contributing](#contributing)

<a name="intro"></a>
## Basic Introduction

In computer science, in the context of data storage, [serialization](https://en.wikipedia.org/wiki/Serialization) is the process of translating data structures or object state into a format that can be stored (for example, in a file or memory buffer) or transmitted (for example, across a network connection link) and reconstructed later (possibly in a different computer environment).

This library specifically deals with serialization of floating numbers into a binary buffer, intended to be used for wireless communications. Trasmitting floating numbers over the binary format is desirable as 1. the transmitted data size is smaller 2. there is no loss of floating number digits.

This library is based on [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/html/multi/advanced.html#serialization). A few errors are fixed, and it is wrapped with C++ for dynamic allocation, and also to support [Eigen](http://eigen.tuxfamily.org/) matrix types. 

To test the example code provided in this packacge, simply run
Below is an example on how to use this class.

```
#include <iostream>
#include <iomanip> // for setprecision
#include "Eigen/Dense"

#include "fdcl/serial.hpp"


int main(void)
{
	bool b0 = false;
	bool b1 = true;
	int i = -1;
	float f = 0.987654321;
	double d = -0.123456789;
	Eigen::Matrix<double, 3, 1> vec(0.1, 0.2, 0.3);

	unsigned char* buf_received;
	
	// sender packing
	fdcl::serial buf_send, buf_recv;
	
	//buf_send.clear();   required when repacking to the same buffer
	buf_send.reserve(40);// optional: reserve the buffer for 40 bytes // 
	buf_send.pack(b0); // 1 byte
	buf_send.pack(b1); // 1 byte
	buf_send.pack(i);  // 2 byte
	buf_send.pack(f);  // 4 byte
	buf_send.pack(d);  // 8 byte
	buf_send.pack(vec); // 8x3 =24 bytes
	
    std::cout << "buf_send_size = " << buf_send.size() << " bytes" 
              << std::endl;

	// transmitting "buf_send.data()" to unsigned char* of the receiver 	
	// this parts is done through a serial port or a wifi
	buf_received = buf_send.data();

	// clear variables to verify unpacking
	b0 = true;
	b1 = false;
	i = 0;
	f = 0.;
	d = 0.;
	vec.setZero();

    std::cout << std::setprecision(10);

	// receiver unpacking
	buf_recv.init(buf_received, 40);

	buf_recv.unpack(b0); 
    std::cout << "b0 = " << b0 << std::endl;

	buf_recv.unpack(b1); 
    std::cout << "b1 = " << b1 << std::endl;

	buf_recv.unpack(i);
    std::cout << "i = " << i << std::endl;

	buf_recv.unpack(f);
    std::cout << "f = " << f << std::endl;

	buf_recv.unpack(d);
    std::cout << "d = " << d << std::endl;

	buf_recv.unpack(vec);
    std::cout << "vec = " << vec << std::endl;
}
```

[back to contents](#contents)


<a name="supported-types"></a>
## Supported Variable Types

The following types are suppored.

1. bool (1 byte)
2. integer (2 byets)
3. float (4 byets)
4. double (8 bytes)
5. Eigen float or double matrix of an arbitrary size

[back to contents](#contents)


<a name="using-the-class"></a>
## Using the Class

To add this to class to your code, using CMake and cloning it as a git submodule is recommended. 
This instructions assumes that you are going to add your submodules to a directory named `libraries` in the main project directory.
If your submodule directory is different, make sure to change the path wherever it says `libraries`.
First, add this as a submodule in git.

```
cd 'main/project/dir'
git submodule add https://github.com/fdcl-gwu/fdcl_serialization.git ./libraries/fdcl_serialization
git submodule update --init --recursive
```

NOTE: Whenever you clone your main project, you must recursively update the submodules:
```
git submodule update --init --recursive
```

Now, in the main project's CMake file (CMakeLists.txt), do the followings:
```
include_directories(${PROJECT_SOURCE_DIR}/libraries/fdcl_serial/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/libraries/fdcl_serialization fdcl_serial)
```

Also, whenever you make a file that uses fdcl_serial class, add `fdcl_serial` to the linker:
```
target_link_libraries(name/of/the/library/or/exec
    PRIVATE fdcl_serial
)
```

Then, you can simply call `#include "fdcl/serial.hpp"` in your source/header files in the main directory.

[back to contents](#contents)


<a name="sender"></a>
## Sender

The library is used by a **sender** that packs data and send it over wifi or serial port, and a **receiver** that collects the binary data and unpack into varialbes. 

Sender initialize an empty buffer by 

```
fdcl::serial buf_send;
```
and pack data by the member function `pack()`. For example, a double variable `d` is packed by

```
buf_send.pack(d);
```
Then, the size of the buffer will be increased by 8 bytes. The above packing procedure can be repeated for other variales. 

Once the packing is completed, the size of the buffer and the data of the buffer can be accessed by `buf_send.size()` and `buf_send.data()` respectively. In particular, `buf_send.data()` returns a type `unsigned char*`, which can be transmitted to the sender via wifi or other communication protocols.

[back to contents](#contents)


<a name="receiver"></a>
## Receiver

The receiver first recives the data from the sender, and save it to a variable with the type of `unsigned char*`. This procedure is completed via wifi or other communication protocols. 

Once the binary data is received, the receiver creates the class via

```
fdcl::serial buf_recv(buf_received, 39);
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
[back to contents](#contents)


<a name="using-eigen"></a>
## Using Eigen Matrices

The package supports Eigen matrices for save and read functions, which are declared as template functions. Therefore, those functions must be explicitly instantiated according to the particular type of the Eigen matrices used. 

For example, the above example uses the Eigen marix type `Eigen::Matrix<double, 3, 1>`, and at the end of `serial.cpp` the following explicit istantiattion are included:

```
template void fdcl::serial::pack(Eigen::MatrixBase< Eigen::Matrix <double, 3,1> >& M);
template void fdcl::serial::unpack(Eigen::MatrixBase< Eigen::Matrix <double, 3,1> >& M);
```

When using other types or sizes of Eigen matrices, the corresponding instantiations must be included at the end of `fdcl_param.cpp`

[back to contents](#contents)


<a name="requirements"></a>
## Requirements
* The order and the type of variables in packing must match with those of unpacking.
* The total size of the data packed must be manually specificed when unpacking.

[back to contents](#contents)


<a name="contributing"></a>
## Contributing
* Anyone is welcome to contribute, but make sure you follow the existing coding style.
* Make sure to document all your changes/additions with Doxygen style comments.

### Generating the Documentation
Document generation is done with Doxygen
If you do not have Doxygen, install it first
```
sudo apt-get install -y doxygen graphviz
```

Use Doxygen to generate the documentation
```
cd docs/Doxygen
doxygen Doxygen
```

This will generate the documentation. 
Commit and push to update the online documentation.

[back to contents](#contents)

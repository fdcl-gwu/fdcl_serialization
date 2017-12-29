# fdcl_param

This library provides a C++ tool to read parameters from a text file, or save them to a text file.

## Installation

To test the example code provided in this packacge, simply run

```
make
./test_fdcl_param
```

### Integration to your own project

This library is composed of a single header file, namely `fdcl_param.h`. You just need to copy the file to your project source folder, and include the header file.

This library also requires the [Eigen library](http://eigen.tuxfamily.org/) as it suppors the eigen matrix types. For convenience, the eigen library is included in the package. The folder to the eigen library should be availalbe to the compiler. For example, the following makefile is used for the sample code.


```
INCLUDE_PATH= ./eigen-3.3.4
CFLAGS=$(foreach d, $(INCLUDE_PATH), -I$d) -Wall 

test_fdcl_param:  fdcl_param.h test_fdcl_param.cpp
	g++ -o test_fdcl_param test_fdcl_param.cpp  $(CFLAGS) 
```



## Configuration File

The format of the compatible text file is illustrated by the following sample file saved as `config.txt`

```
UAV:
	m: 2.5
	J: 3, 0, 0,  0, 2, 0,  0, 0, 1
Control:
	kR: 1.8
	kW: 2.0
IMU:
	dev: "/dev/tty"
I2C:
	dev: "/dev/i2c"
	addr: 50
GPS:
	available: 1
```

The name of the parameter is composed of the group name, and the variable name. For example, in the above file, `UAV` is the group name and `m` is the variable name.

More precisely, if the new line begins with letters without any space, the letters before `:` are considered as the **group**. Or, if the new line begins with `tab`, the the letters between `tab` and `:` are considered as the **variable**. 

The parameter name is constructed by concatenating **group** and **variable**. Note that the **variable** names can be repeated if they do not belong to the same **group**.

The followig types are suppored for the parameter value:

	1. bool (0: false, 1: true)
	2. double
	3. integer
	4. string (nested in " ")
	5. Eigen double matrix of an arbitrary size

Note the string parameter should be nested in ```" "```. For an eigen matrix, each row is concetanted in a single line, where each element is separated by `,  ` For example, in the above configuration file, the 3x3 matrix J is defined as

```
J = [ 3 2 1; 
      0 2 0;
      0 0 1];
```
in the matlab notation.

## Usage

The usuage of the library is illustrated by the following sample code.

```
#include <iostream>
#include <string>
#include "Eigen/Dense"
#include "fdcl_param.h"

using namespace std;

int main(void)
{
	double m;
	string dev;
	int addr;
	Eigen::Matrix<double, 3, 3> J;
	bool GPS;
	
	fdcl_param pfile;
	
	pfile.open("config.txt");

	pfile.read("UAV.m",m);
	cout << "m=" << m << endl;
	pfile.read("UAV.J",J);
	cout << "J=" << J << endl;
	pfile.read("IMU.dev",dev);
	cout << "dev=" << dev << endl;
	pfile.read("GPS.available",GPS);
	cout << "GPS=" << GPS << endl;

	pfile.save("I2C.addr",50);
	pfile.read("I2C.addr",addr);
	cout << "addr=" << addr << endl;
	pfile.save("GPS.available",true);
	pfile.read("GPS.available",GPS);
	cout << "GPS=" << GPS << endl;

	pfile.close();
}
```

First, the head file should be inlcuded.

```
#include "fdcl_param.h"
```

The class object is constructed and the configuration file is opened by

```
fdcl_param pfile;
pfile.open("config.txt");
```

To read a double valued parameter for the variable `m` under the group `UAV`, first a double varialbe `m` should be declared, and the member function `read` is called as follows.

```
pfile.read("UAV.m",m);
```
Note that the **group** name and the **variable** name are joined with `.` in between. 

Similarly, to save an integer valued parameter for `I2C.addr` can be saved by

```
pfile.save("I2C.addr",50);
```
The configuration file should be closed by
```
pfile.close();
```

## What it does NOT do
* Save function does NOT add a new entry to the configuration text file. When the save function is called, the parameter must alred exist in the text file.
* We are not able to add any comment to the configuration text file.
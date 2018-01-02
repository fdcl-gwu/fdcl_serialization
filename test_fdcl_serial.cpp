#include <iostream>
#include <iomanip> // for setprecision
#include "Eigen/Dense"
#include "fdcl_serial.h"

using namespace std;


int main(void)
{
	
	bool b=true;
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
	b=false;
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

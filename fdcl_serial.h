#ifndef _FDCL_SERIAL_H
#define _FDCL_SERIAL_H

#include <iomanip>
#include <iostream>
#include "Eigen/Dense"
#include <vector>
#include <typeinfo>
#include <stdio.h>

// macros for packing floats and doubles:
#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

using namespace std;


class fdcl_serial
{
public:
//	unsigned char* buf=new unsigned char[0];
	vector<unsigned char> buf;
	
	unsigned int loc;
	
	fdcl_serial()
	{
		loc=0;
	};
	fdcl_serial(unsigned char* buf_received, int size)
	{
		buf.insert(buf.end(),buf_received,buf_received+size);
	};
	~fdcl_serial()
	{
//		delete[] buf;
	};
	void pack(double&);
	void pack(int&);
	void pack(float&);
	void pack(bool&);
	template<typename Derived>
	void pack(Eigen::MatrixBase<Derived>& M);
	
	void unpack(double&);
	void unpack(int&);
	void unpack(float&);
	void unpack(bool&);
	template<typename Derived>
	void unpack(Eigen::MatrixBase<Derived>& M);
	
	int size();
	unsigned char* data();

	// the following functions are copied from http://beej.us/guide/bgnet/html/multi/advanced.html#serialization
private:
	unsigned long long int pack754(long double f, unsigned bits, unsigned expbits);
	long double unpack754(unsigned long long int i, unsigned bits, unsigned expbits);
	void packi8(unsigned char *buf, unsigned int i);
	void packi16(unsigned char *buf, unsigned int i);
	void packi32(unsigned char *buf, unsigned long int i);
	void packi64(unsigned char *buf, unsigned long long int i);
	unsigned int unpacku8(unsigned char *buf);
	int unpacki16(unsigned char *buf);
	unsigned int unpacku16(unsigned char *buf);
	long int unpacki32(unsigned char *buf);
	unsigned long int unpacku32(unsigned char *buf);
	long long int unpacki64(unsigned char *buf);
	unsigned long long int unpacku64(unsigned char *buf);	
};


int fdcl_serial::size()
{
	return buf.size();
}

unsigned char* fdcl_serial::data()
{
	return buf.data();
}

void fdcl_serial::pack(double& d)
{
	unsigned long long int i;
	unsigned char buf_double[8];

	i = pack754_64(d); // convert to IEEE 754	
	packi64(buf_double,i);
	
	buf.insert(buf.end(),buf_double,buf_double+8);
}

void fdcl_serial::pack(float& f)
{
	unsigned long long int i;
	unsigned char buf_float[4];

	i = pack754_32(f); // convert to IEEE 754	
	packi32(buf_float,i);
	
	buf.insert(buf.end(),buf_float,buf_float+4);
}

void fdcl_serial::pack(int& i)
{
	unsigned char buf_int[2];

	packi16(buf_int, i);
	buf.insert(buf.end(),buf_int,buf_int+2);
}

void fdcl_serial::pack(bool& b)
{
	unsigned char buf_bool[1];
	unsigned int f=0;
	unsigned int t=1;
	
//	printf("f= %d, %c\n",f,f);
//	printf("t= %d, %c\n",t,t);
		
	
	if(b==false)
	{	//buf_bool=(unsiged char) "0";
		packi8(buf_bool, f);
	}
	else
	{
		packi8(buf_bool, t);
	}
	
//	printf("buf_bool=%d\n", buf_bool[0]);
	
	buf.insert(buf.end(),buf_bool,buf_bool+1);
}


template<typename Derived>
void fdcl_serial::pack(Eigen::MatrixBase<Derived>& M)
{
	int i,j;
	unsigned char buf_double[8], buf_float[4];
	unsigned long long int ii;

	typedef typename Eigen::MatrixBase<Derived>::Scalar type;

	switch(*typeid(type).name())
	{
		case 'd': // double
		for(i=0;i<M.rows();i++)
		{
			for(j=0;j<M.cols();j++)
			{		
				ii = pack754_64(M(i,j)); // convert to IEEE 754	
				packi64(buf_double,ii);	
				buf.insert(buf.end(),buf_double,buf_double+8);
			}
		}
		break;
			
		case 'f': // float
		for(i=0;i<M.rows();i++)
		{
			for(j=0;j<M.cols();j++)
			{			
				ii = pack754_32(M(i,j)); // convert to IEEE 754	
				packi32(buf_float,ii);	
				buf.insert(buf.end(),buf_float,buf_float+4);
			}
		}
		break;
	}
}



void fdcl_serial::unpack(double& d)
{
	unsigned long long int i;
	unsigned char buf_double[8];
	
	copy(&buf[loc],&buf[loc+8],buf_double);
	i = unpacku64(buf_double);
	d = unpack754_64(i);
	loc+=8;
}
void fdcl_serial::unpack(float& f)
{
	unsigned long long int i;
	unsigned char buf_float[4];
	
	copy(&buf[loc],&buf[loc+4],buf_float);
	i = unpacku32(buf_float);
	f = unpack754_32(i);
	loc+=4;
}

void fdcl_serial::unpack(int& i)
{
	unsigned char buf_int[2];
	
	copy(&buf[loc],&buf[loc+2],buf_int);
	i = unpacki16(buf_int);
	loc+=2;
}

void fdcl_serial::unpack(bool& b)
{
	unsigned char buf_bool[1];
	unsigned int bb;


	copy(&buf[loc],&buf[loc+1],buf_bool);
	bb= unpacku8(buf_bool);

//	printf("bb=%d\n",bb);
	if(bb==0)
		b=false;
	else if(bb==256)	
		b=true;
	
	loc+=1;
}
 
template<typename Derived>
void fdcl_serial::unpack(Eigen::MatrixBase<Derived>& M)
{
	int i,j;
	unsigned char buf_double[8], buf_float[4];
	unsigned long long int ii;

	typedef typename Eigen::MatrixBase<Derived>::Scalar type;

	switch(*typeid(type).name())
	{
		case 'd': // double
		for(i=0;i<M.rows();i++)
		{
			for(j=0;j<M.cols();j++)
			{		
				copy(&buf[loc],&buf[loc+8],buf_double);
				ii = unpacku64(buf_double);
				M(i,j) = unpack754_64(ii);
				loc+=8;
			}
		}
		break;
			
		case 'f': // float
		for(i=0;i<M.rows();i++)
		{
			for(j=0;j<M.cols();j++)
			{			
				copy(&buf[loc],&buf[loc+4],buf_float);
				ii = unpacku32(buf_float);
				M(i,j) = unpack754_32(ii);
				loc+=4;
			}
		}
		break;
	}
}

unsigned long long int fdcl_serial::pack754(long double f, unsigned bits, unsigned expbits)
{
	// pack a floating numbere to IEEE754 format
	long double fnorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (f == 0.0) return 0; // get this special case out of the way

	// check sign and begin normalization
	if (f < 0) { sign = 1; fnorm = -f; }
	else { sign = 0; fnorm = f; }

	// get the normalized form of f and track the exponent
	shift = 0;
	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
	fnorm = fnorm - 1.0;

	// calculate the binary form (non-float) of the significand data
	significand = fnorm * ((1LL<<significandbits) + 0.5f);

	// get the biased exponent
	exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

	// return the final answer
	return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double fdcl_serial::unpack754(unsigned long long int i, unsigned bits, unsigned expbits)
{
	// convert IEEE754 format to a floating number
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (i == 0) return 0.0;

	// pull the significand
	result = (i&((1LL<<significandbits)-1)); // mask
	result /= (1LL<<significandbits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	bias = (1<<(expbits-1)) - 1;
	shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	// sign it
	result *= (i>>(bits-1))&1? -1.0: 1.0;

	return result;
}



// store integer into unsigned char buffer
void fdcl_serial::packi8(unsigned char *buf, unsigned int i)
{
	*buf++ = i;
}
void fdcl_serial::packi16(unsigned char *buf, unsigned int i)
{
	*buf++ = i>>8; *buf++ = i;
}
void fdcl_serial::packi32(unsigned char *buf, unsigned long int i)
{
	*buf++ = i>>24; *buf++ = i>>16;
	*buf++ = i>>8;  *buf++ = i;
}
void fdcl_serial::packi64(unsigned char *buf, unsigned long long int i)
{
	*buf++ = i>>56; *buf++ = i>>48;
	*buf++ = i>>40; *buf++ = i>>32;
	*buf++ = i>>24; *buf++ = i>>16;
	*buf++ = i>>8;  *buf++ = i;
}





// unpack unsiged char buffer to integer
int fdcl_serial::unpacki16(unsigned char *buf)
{
	unsigned int i2 = ((unsigned int)buf[0]<<8) | buf[1];
	int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffu) { i = i2; }
	else { i = -1 - (unsigned int)(0xffffu - i2); }

	return i;
}
unsigned int fdcl_serial::unpacku8(unsigned char *buf)
{
	return ((unsigned int)buf[0]<<8);
}
unsigned int fdcl_serial::unpacku16(unsigned char *buf)
{
	return ((unsigned int)buf[0]<<8) | buf[1];
}
long int fdcl_serial::unpacki32(unsigned char *buf)
{
	unsigned long int i2 = ((unsigned long int)buf[0]<<24) |
		((unsigned long int)buf[1]<<16) |
			((unsigned long int)buf[2]<<8)  |
				buf[3];
	long int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffffffu) { i = i2; }
	else { i = -1 - (long int)(0xffffffffu - i2); }

	return i;
}
unsigned long int fdcl_serial::unpacku32(unsigned char *buf)
{
	return ((unsigned long int)buf[0]<<24) |
		((unsigned long int)buf[1]<<16) |
			((unsigned long int)buf[2]<<8)  |
				buf[3];
}
long long int fdcl_serial::unpacki64(unsigned char *buf)
{
	unsigned long long int i2 = ((unsigned long long int)buf[0]<<56) |
		((unsigned long long int)buf[1]<<48) |
			((unsigned long long int)buf[2]<<40) |
				((unsigned long long int)buf[3]<<32) |
					((unsigned long long int)buf[4]<<24) |
						((unsigned long long int)buf[5]<<16) |
							((unsigned long long int)buf[6]<<8)  |
								buf[7];
	long long int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffffffffffffffu) { i = i2; }
	else { i = -1 -(long long int)(0xffffffffffffffffu - i2); }

	return i;
}
unsigned long long int fdcl_serial::unpacku64(unsigned char *buf)
{
	return ((unsigned long long int)buf[0]<<56) |
		((unsigned long long int)buf[1]<<48) |
			((unsigned long long int)buf[2]<<40) |
				((unsigned long long int)buf[3]<<32) |
					((unsigned long long int)buf[4]<<24) |
						((unsigned long long int)buf[5]<<16) |
							((unsigned long long int)buf[6]<<8)  |
								buf[7];
}


#endif

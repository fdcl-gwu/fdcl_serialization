#ifndef FDCL_SERIAL_HPP
#define FDCL_SERIAL_HPP

#include <iomanip>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <stdio.h>

#include "Eigen/Dense"

#define MAX_BUFFER_RECV_SIZE 8192

namespace fdcl 
{

/** \brief serialization library
*
*  This library provides a tool to save variables into a binary buffer or
*  read variables from a binary buffer.
*/
class serial
{
public:
    serial();
    serial(unsigned char* buf_received, int size);
    ~serial();

    std::vector<unsigned char> buf; /**< buffer in which the serialized data is
                                     *  save in
                                     */
    unsigned int loc; /**< current location in the buffer */


    /** \fn void clear()
     * Clears the buffer
     */
    void clear();


    /** \fn void init(unsigned char* buf_received, int size)
     * Initializes the buffer
     * @param buf_received received buffer
     * @param size         size of the received buffer
     */
    void init(unsigned char* buf_received, int size);


    /** \fn void reserve(int size)
     * Reserves size
     * @param size amount of size required to reserve
     */
    void reserve(int size);


    /** \fn int size()
     * Returns the size of the buffer
     * @return size of the buffer
     */
    int size();


    /** \fn unsigned char* data()
     * Returns the buffer data
     * @return buffer data
     */
    unsigned char* data();


    /** \fn void pack(int &i)
    * Packs an int into the buffer
    * @param i int to be packed
    */
    void pack(int &i);


    /** \fn void pack(double &d)
     * Packs a double into the buffer
     * @param d double to be packed
     */
    void pack(double &d);


    /** \fn void pack(float &f)
     * Packs a float into the buffer
     * @param f float to be packed
     */
    void pack(float &f);


    /** \fn void pack(bool &b)
     * Packs a bool into the buffer
     * @param b bool to be packed
     */
    void pack(bool &b);


    /** \fn void pack(Eigen::MatrixBase<Derived> &M)
     * Packs an Eigen vector into the buffer
     * @param M Eigen::MatrixBase<Derived> to be packed
     */
    template<typename Derived>
    void pack(Eigen::MatrixBase<Derived> &M);


    /** \fn void pack(Eigen::MatrixBase<Derived> &M)
     * Packs an Eigen vector into the buffer as float, even if their original
     * accuracy is set to double. This helps to keep the buffer size smaller
     * when large Eigen vectors are being sent.
     * @param M Eigen::MatrixBase<Derived> to be packed
     */
    template<typename Derived>
    void pack_as_float(Eigen::MatrixBase<Derived> &M);


    /** \fn void unpack(int &i)
    * Unpacks an int from the buffer
    * @param i int to be unpacked
    */
    void unpack(int &i);


    /** \fn void unpack(double &d)
    * Unpacks an double from the buffer
    * @param d double to be unpacked
    */
    void unpack(double &d);


    /** \fn void unpack(float &f)
    * Unpacks an float from the buffer
    * @param f float to be unpacked
    */
    void unpack(float &f);


    /** \fn void unpack(bool &b)
    * Unpacks an bool from the buffer
    * @param b bool to be unpacked
    */
    void unpack(bool &b);


    /** \fn void unpack(Eigen::MatrixBase<Derived> &d)
    * Unpacks an Eigen vector from the buffer
    * @param d Eigen::MatrixBase<Derived> to be unpacked
    */
    template<typename Derived>
    void unpack(Eigen::MatrixBase<Derived>& M);


    /** \fn void unpack(double &d)
    * Unpacks an Eigen vector from the buffer as a double, even if the original
    * values is a float.
    * @param M Eigen::MatrixBase<Derived> to be unpacked
    */
    template<typename Derived>
    void unpack_as_double(Eigen::MatrixBase<Derived>& M);


private:
    // the following functions are copied from
    // http://beej.us/guide/bgnet/html/multi/advanced.html#serialization
    unsigned long long int pack754(long double f, unsigned bits,
        unsigned expbits);
    long double unpack754(unsigned long long int i, unsigned bits,
        unsigned expbits);
    void packi16(unsigned char *buf, unsigned int i);
    void packi32(unsigned char *buf, unsigned long int i);
    void packi64(unsigned char *buf, unsigned long long int i);
    int unpacki16(unsigned char *buf);
    unsigned int unpacku16(unsigned char *buf);
    long int unpacki32(unsigned char *buf);
    unsigned long int unpacku32(unsigned char *buf);
    long long int unpacki64(unsigned char *buf);
    unsigned long long int unpacku64(unsigned char *buf);
};  // end of serial class

}  // end of namespace fdcl
#endif

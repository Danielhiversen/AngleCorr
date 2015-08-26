#ifndef HELPERS_HPP
#define HELPERS_HPP
#include <cmath>
/**
 * @return -1 if val is negative
 * 0 if val is 0
 * 1 if positive
 */
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


/**
 * Calculate the length of a 3D vector
 * @param a the 3d vector
 * @return the length
 */
template <typename T> T length3d(T const a[3])
{
  T ret = 0.0;
  for(int i = 0; i < 3; i++)
  {
    ret += a[i]*a[i];
  }
  return sqrt(ret);
}

/**
 * Normalize a 3d vector
 * @param normalized the normalized vector is stored here
 * @param toNormalize the vector to normalize
 */
template <typename T> void normalize3d(T normalized[3], T const toNormalize[3] )
{
  T normalization = length3d(toNormalize);
   for(int i = 0; i < 3; i++)
  {
    normalized[i] = toNormalize[i]/normalization;
  }
}
/**
 * Calculate the inner product of two vectors
 * @param one the first vector
 * @param two the second vector
 * @return the inner product of one and two
 */
template <typename T> T innerProduct(T const one[3], T const two[3])
{
  T sum = 0;
  for(int i = 0; i < 3; i++)
  {
    sum += one[i]*two[i];
  }
  return sum;
}

#define radToDeg(x) (180.0*x/M_PI)
#define degToRad(x) (M_PI*x/180.0)

#endif

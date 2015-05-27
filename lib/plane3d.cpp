#include "plane3d.hpp"
#include "helpers.hpp"
void Plane3D::projectOnto(double projected[3], double const pt[3] ) const
{
  // move pt along the vector described by m_coeffs[0-2] for a 
  // distance given by getDistance
  // FIXME: Normalization should be cached?

  double normalized[3];
  double dist;
  // Yes, m_coeffs is longer than normalized, 
  // but the normalize3d function only uses the 3 first coefficients
  normalize3d(normalized, m_coeffs);

  dist = getDistance(pt);
   
  for(int i = 0; i < 3; i++)
  {
    projected[i] = pt[i] - normalized[i]*dist;
  }

}

#ifndef PLANE3D_HPP
#define PLANE3D_HPP
#include <Eigen/Dense>

//#define DEBUG_PLANE


/** Representation of the plane equation
* ax + by + cz + d = dist 
*/
class Plane3D
{
public:

  /** 
   * Coefficient constructor - simply copy the given coefficients
   * @param coeffs Coefficients {a,b,c,d}
   */
  Plane3D(const double coeffs[4])
    {
      for(int i = 0; i < 4; i++)
      {
	m_coeffs[i] = coeffs[i];
      }
    }
  
  /**
   * Matrix constructor - treat the z axis of the rotation part of the matrix as a normal vector
   * @param coeffs Matrix to construct from
   */ 
  Plane3D(const Eigen::Matrix4f coeffs)
    {
      for(int i = 0; i < 3; i++)
      {
	m_coeffs[i] = coeffs(i,2);
      }
      m_coeffs[3] = 0.0;
      for(int i = 0; i < 3; i++)
      {
	
	m_coeffs[3] -= m_coeffs[i]*coeffs(i,3);
      }
    }

  /**
   * Matrix constructor - treat the z axis of the rotation part of the matrix as a normal vector
   * @param coeffs Matrix to construct from
   */
  Plane3D(const Eigen::Matrix4d coeffs)
    {
      for(int i = 0; i < 3; i++)
      {
	m_coeffs[i] = coeffs(i,2);
      }
      m_coeffs[3] = 0.0;
      for(int i = 0; i < 3; i++)
      {
	
	m_coeffs[3] -= m_coeffs[i]*coeffs(i,3);
      }
    }
  
  /**
   * Zero constructor. Initialize coefficients to zero
   */
  Plane3D() 
    { 
      for(int i = 0; i < 4; i++) 
      { 
	m_coeffs[i] = 0.0; 
      } 
    };
  ~Plane3D() { }

  /**
   * Set a coefficient
   * @param i coefficient to set
   * @param c value to set it to
   */
  inline void setCoefficient(const int i, const double c)
    {
      m_coeffs[i] = c;
    }
  /**
   * Get a coefficient
   * @param i coefficient to get
   * @return coefficient
   */
  inline double getCoefficient(const int i)
    {
      return m_coeffs[i];
    }
  /**
   * Get distance from the plane to a point
   * @param pt point to find distance to
   * @return distance to point
   */
  inline double getDistance(double const pt[3]) const
    {
      double ret = 0.0;
      for(int i = 0; i < 3; i++)
      {
	ret += pt[i]*m_coeffs[i];
      }
      ret += m_coeffs[3];
#ifdef DEBUG_PLANE
      cerr << "Distance: " << ret << endl;
#endif
      return ret;
    }
  
  /**
   * Project a point onto the plane
   * @param projected Projected point will be returned here
   * @param pt Point to project
   */
  void projectOnto(double projected[3], double const pt[3] ) const;
      
protected:
  /**
   * The plane equation coefficients a,b,c,d
   */
  double m_coeffs[4];
  
};

#endif // PLANE3D_HPP

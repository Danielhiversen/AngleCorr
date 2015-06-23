#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <algorithm>
#include <functional>
#include <numeric>
#include "spline3d.hpp"
#include "metaimage.hpp"

template<typename T>
class Spline3D;

/**
 * Class to represent an intersection between an image and a spline curve
 */
template<typename T>
class Intersection 
{
public:

  /**
   * Initialize everything to 0, false or NULL
   */
  Intersection()
  {
    m_spline = NULL;
    m_intersection_pos = 0.0;
    m_cosTheta = 0.0;
    m_origAvgValue = 0.0;
    m_points = vector<T>();
    m_valid = false;
    m_img = NULL;
  }      
  /**
   * Retrieve the intersecting spline curve 
   * @return pointer to the spline curve
   */
  inline const Spline3D<T> 
  *getSpline() const
  {
    return m_spline;
  }
  
  /**
   * Set the intersecting spline curve
   * @param spline Pointer to the intersecting spline curve
   */
  inline void 
  setSpline(const Spline3D<T> *spline)
  {
    m_spline = spline;
  }
  
  /**
   * Get the spline curve parameter position of the intersection
   * @return the parameter position
   */
  inline T 
  getParameterPosition() const
  {
    return m_intersection_pos;
  }

  /**
   * Set the parameter position of the intersection
   * @return the paramters position
   */
  inline void 
  setParameterPosition(T t)
  {
    m_intersection_pos = t;
  }

  /**
   * Get the cosTheta value of the intersection (cos(theta) where theta is the angle
   * between the acquisition direction and the derivative of the curve
   * @return cos(theta)
   */
  inline T 
  getCosTheta() const 
  {
    return m_cosTheta;
  }

  /**
   * Set the cosTheta value of the intersection
   * @parameter t the cosTheta value
   */
  inline void 
  setCosTheta(T t)
  {
    m_cosTheta = t;
  }

  /**
   * Set the points associated with this intersection (i.e. the region-grown points)
   * @param points the points
   */
  inline void
  setPoints(const vector<T>& points)
  {
    m_points = points;
    m_avg_computed = false;
  }
 
  /**
   * Set the points associated with this intersection (i.e. the region-grown points)
   * @param points the points
   */
  inline void
  setPoints(vector<T>&& points)
  {
    m_avg_computed = false;
    m_points = std::move(points);
  }

  /**
   * Set the points associated with this intersection (i.e. the region-grown points)
   * @return  the points
   */
  inline vector<T>& 
  getPoints()
  {
    return m_points;
  }

  /**
   * Get the (cached) average of the region growed points
   */
  inline T 
  getAverage()
  {
    if(!m_avg_computed)
      __computeAverage();
    return m_avgValue;
  }

  /**
   * Set whether this intersection is valid
   * @param valid 
   */
  inline void 
  setValid(bool valid)
  {
    m_valid = valid;
  }
  
  /** 
   * Check if this intersection is valid
   * @return true if valid, false otherwise
   */
  inline bool 
  isValid() const
  {
    return m_valid;
  }
  
  /**
   * Get pointer to the meta image the spline intersects
   * @return pointer to meta image
   */
  inline const MetaImage<inData_t>* 
  getMetaImage() const
  {
    return m_img;
  }

  /**
   * Set the meta image of this intersection
   * @param img image 
   */
  inline void 
  setMetaImage(const MetaImage<inData_t>* img)
  {
    m_img = img;
  }
  
  /**
   * Convenience method to evaluate the spline
   * \sa Spline3D::evaluateSingle
   */
  inline void 
  evaluate(T p[3]) const
  {
    if(isValid())
    {
      m_spline->evaluateSingle(m_intersection_pos,p);
    }
  }

  /**
   * Get original average value (pre aliasing correction)
   * @return average value before the aliasing correction
   */
  inline T 
  getOrigAvgValue()
  {
    return m_origAvgValue;
  }
  
  /**
   * Perform aliasing correction on the image data of this intersection
   * @param direction Direction (relative to parameter of spline) the blood is assumed to flow
   * @param Vnyq the nyquist velocity
   */
  inline void 
  correctAliasing(T direction,T Vnyq)
  {

    m_avgValue = 0.0;
    for(auto it = m_points.begin(); it != m_points.end(); it++)
    {
      bool sign = sgn(direction) == sgn(m_cosTheta);
      if(*it < 0 && sign)
      {
    	  *it += 2*Vnyq;
      }
      else if(*it > 0 && !sign)
      {
    	  *it -= 2*Vnyq;
      }
      m_avgValue += *it;
    }
    if (m_points.size()==0){
    	m_avgValue =0.0;
    	m_valid = false;
    }else{
    	m_avgValue = m_avgValue/m_points.size();
    }
  }
 
  /**
   * Compute the sample weight for the image in this intersection
   * @param A The factor with which to multiply the sample weighting function
   * @param a lower abs(cosTheta) cutoff
   * @param b upper abs(cosTheta) cutoff
   * @return the sample weight
   */
  inline T 
  sampleWeight(const T A, const T a, const T b) const
  {
    int positive;
    int negative;
    positive = std::accumulate(m_points.begin(), m_points.end(), 0, 
			       [](int k, T l){ if(l > 0) return k + 1; return k; }
      );

    negative = m_points.size()-positive;
  
    T pos_weight = (T)(positive - negative)/(double)(positive + negative);
    pos_weight = pos_weight * pos_weight;
    T ret = A*pos_weight;
    if(abs(m_cosTheta) > a && abs(m_cosTheta) < b)
    {
      ret += 1;
    }
    return ret;
  }
      
  /** 
   * Return the simple velocity estimation for this image
   * @return velocity estimate
   */
  inline T 
  estimateVelocitySimple() 
  {
    if(isValid())
    {
      return getAverage()/m_cosTheta;
    }
    else 
      return 0.0;
  }
      
  /**
   * Region grow the meta image
   */
  inline void 
  regionGrow()
  {
    if(!isValid()) return;
    T p[3];
    evaluate(p);
    T img_x, img_y;
    m_img->toImgCoords(img_x, img_y, p);
    if(m_img->inImage(img_x, img_y))
    {
      m_img->regionGrow(m_points,(int)img_x, (int)img_y);
    }
  }


private:
  void __computeAverage()
    {
      m_avgValue = std::accumulate(m_points.begin(), m_points.end(), 0.0, plus<T>());
      //m_avgValue = m_avgValue/(T)m_points.size();
      m_origAvgValue = m_avgValue;
      m_avg_computed = true;

      if (m_points.size()==0){
      	m_avgValue =0.0;
      	m_valid = false;
      }else{
    	  m_avgValue = m_avgValue/(T)m_points.size();
      }
    }
  
  const Spline3D<T> *m_spline;
  T m_intersection_pos;
  T m_cosTheta;
  T m_avgValue;
  const MetaImage<inData_t> *m_img;
  vector<T> m_points;
  bool m_valid;
  bool m_avg_computed;
  T m_origAvgValue;
};
  

#endif //INTERSECTION_HPP

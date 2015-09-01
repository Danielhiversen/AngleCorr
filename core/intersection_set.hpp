#ifndef INTERSECTION_SET_HPP
#define INTERSECTION_SET_HPP
#include "intersection.hpp"

#include <vector>

/**
 * Represents a set of intersections.
 * Typically, an instance of this object will only contain intersections with the same spline.
 * Implements the algorithms relevant for multiple instersections.
 */
template<typename T>
class IntersectionSet : public std::vector<Intersection<T> >
{
public:

  /**
   * Initialize everything to zero except the algorithm parameters:
   * For direction estimation:
   *     cosTheta cutoff will be 0.07 < abs(cosTheta) < 0.9
   *     A will be 10.0
   * For velocity estimation:
   *     cosTheta cutoff will be 0.17 < abs(cosTheta) < 1.0
   */
  IntersectionSet()
  {
    m_direction = 0.0;
    m_have_direction = false;
    m_have_velocity_ls = false;
    m_velocity_ls = 0.0;
    m_dir_a = 0.07;
    m_dir_b = 0.9;
    m_dir_A = 10.0;
    m_vel_a = 0.17;
    m_vel_b = 1.0;
  }
public:
  /**
   * Estimate the flow direction, assuming all intersections belong to the same curve
   * Parameters can be set with setDirectionEstimationParameters()
   */
  void 
  estimateDirection()
  {
    std::pair<T,T> vel_weight;
    struct 
    {
      T A;
      T a;
      T b;
      std::pair<T,T>& operator()(std::pair<T,T>& ret, Intersection<T>& i)
      {
	   
		T weight = i.sampleWeight(A, a,b);
		T tmp = i.getAverage()*i.getCosTheta();
		tmp = weight*tmp/abs(tmp);

		if(!std::isnan(tmp))
		{
		  ret.first += tmp;
		  ret.second += weight;

		}
		return ret;

	  }
    } weight_accum;
    
    weight_accum.a = m_dir_a;
    weight_accum.b = m_dir_b;
    weight_accum.A = m_dir_A;
    vel_weight = std::accumulate(this->begin(), this->end(), std::make_pair(0.0, 0.0),
				 weight_accum);
    m_direction = vel_weight.first/vel_weight.second;
    m_have_direction = true;
    if(std::isnan(m_direction))
    {
    	m_direction = 0.0;
    }
  }
  
  /**
   * Perform least-squares velocity estimation
   * Parameters can be set with setVelocityEstimationCutoff
   */
  void 
  estimateVelocityLS()
  {
    std::pair<T,T> top_bottom;
      
    struct
    {
      T a;
      T b;
      std::pair<T,T>& operator()(std::pair<T,T> &ret, Intersection<T> &i)
      {
        if(abs(i.getCosTheta()) < a || abs(i.getCosTheta()) > b){
		  return ret;
        }
		double tmp1, tmp2;

		tmp1 = i.getAverage()*i.getCosTheta();
		tmp2 = i.getCosTheta()*i.getCosTheta();
		if(!std::isnan(tmp1) && !std::isnan(tmp2))
		{
		  ret.first += tmp1;
		  ret.second += tmp2;
		}
		return ret;
      }
    } accum;
	
    accum.a = m_vel_a;
    accum.b = m_vel_b;
    top_bottom = std::accumulate(this->begin(), this->end(), 
				 std::make_pair(0.0,0.0), accum);
    m_velocity_ls = top_bottom.first/top_bottom.second;
    m_have_velocity_ls = true;

    if(std::isnan(m_velocity_ls ))
    {
    	m_velocity_ls = 0.0;
    }
  }
  
  /**
   * Perform aliasing correction all images belonging to this intersection set
   * @param Vnyq the Nyquist velocity
   */
  void 
  correctAliasing(T Vnyq)
  {
    for(auto &intersection : *this)
    {
      intersection.correctAliasing(m_direction,Vnyq);
    }
  }

  /**
   * Set parameters for direction estimation algorithm
   * @param A The factor with which to multiply the sample weighting function
   * @param lower lower abs(cosTheta) cutoff
   * @param upper upper abs(cosTheta) cutoff
   */
  void setDirectionEstimationParameters(T A, T lower, T upper)
    {
      m_dir_A = A;
      m_dir_a = lower;
      m_dir_b = upper;
    }

  /**
   * Set parameters for velocity estimation algorithm
   * @param lower lower abs(cosTheta) cutoff
   * @param upper upper abs(cosTheta) cutoff
   */
  void setVelocityEstimationCutoff(T lower, T upper)
    {
      m_vel_a = lower;
      m_vel_b = upper;
    }

  /**
   * Get cached estimated direction, if computed.
   * If not computed, compute it
   */
  T getEstimatedDirection()
    {
      if(!m_have_direction)
    	  estimateDirection();
      return m_direction;
    }

  /**
   * Get cached estimated velocity, if computed.
   * If not computed, compute it
   */
  T getEstimatedVelocity()
    {
      if(!m_have_velocity_ls)
    	  estimateVelocityLS();
      return m_velocity_ls;
    }
	
  

private:
  T m_direction;
  bool m_have_direction;
  bool m_have_velocity_ls;
  T m_velocity_ls;
  T m_dir_a, m_dir_b;
  T m_vel_a, m_vel_b;
  T m_dir_A;
  
};




#endif

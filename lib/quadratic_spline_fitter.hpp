#ifndef SPLINE_FITTER_HPP
#define SPLINE_FITTER_HPP
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/SuperLUSupport>
#include <Eigen/Dense>


/**
 * Quadratic B-spline curve fitter
 * This class computes the control points necessary to make a quadratic B-spline curve 
 * interpolating a known set of points.
 *
 */
template<typename T>
class QuadraticSplineFitter 
{
public:

  /**
   * Default constructor
   */
  QuadraticSplineFitter() : QuadraticSplineFitter({})
  {
  };

  /**
   * Constructor
   * @param points Points to interpolate
   */
  QuadraticSplineFitter(std::vector<T>& points){
    setPoints(points);
  }
  
  /**
   * Set points to interpolate
   * @param points points to interpolate 
   */
  inline void
  setPoints(std::vector<T>& points)
  {
    m_points = points;
  }
  
 
  /**
   * Compute the control points necessary to interpolate the points set by setPoints
   * @return the control points
   */
  std::vector<T> 
  compute_control_points() const
  {
    // Here:
    // Build a NxN matrix such that
    // matrix[row][row] =  matrix[row][row+1] = 1/2
    // rest = 0
    // build B such that B = [ep_start points ep_stop]
    // Solve Ax = B
    // x are the knots
    int n = m_points.size()+2;
    Eigen::SparseMatrix<T> A(n,n);
      
    Eigen::Matrix<T, Eigen::Dynamic, 1> b(n);
    std::vector<Eigen::Triplet<T> > triplets;
    triplets.reserve((n)*2);
      
    // Initialize coefficients for the points to reach
    
    for(int i = 1; i < n-1; i++)
    {
      triplets.push_back(Eigen::Triplet<T>(i,i-1,0.125));
      triplets.push_back(Eigen::Triplet<T>(i,i,0.75));
      triplets.push_back(Eigen::Triplet<T>(i,i+1,0.125));
      b(i) = m_points[i-1];
    }
      
    __insertStartpointCoeffs(triplets,b,0);
    __insertEndpointCoeffs(triplets,b,n-1);
    A.setFromTriplets(triplets.begin(), triplets.end());
     
    // Now solve the linear system using linear least squares
    Eigen::Matrix<T, Eigen::Dynamic, 1> x(n);
    Eigen::SuperLU<Eigen::SparseMatrix<T> > solver;
    solver.compute(A);
    x = solver.solve(b);

      

    //     printVector(x,n);
    // Copy back to std::vector
    std::vector<T> ret;
      
    ret.reserve(n);
    for(int i = 0; i < n; i++)
    {
      ret.push_back(x(i));
    }
    return ret;
      
  }

	  
  
private:
  inline void 
  __insertStartpointCoeffs( std::vector<Eigen::Triplet<T> >& triplets, 
			    Eigen::Matrix<T, Eigen::Dynamic, 1>& b, 
			    const int i) const
  {
    triplets.push_back(Eigen::Triplet<T>(i,i,-1));
    triplets.push_back(Eigen::Triplet<T>(i,i+1,1));
    b(i) = 0.0;
  }

  inline void 
  __insertEndpointCoeffs( std::vector<Eigen::Triplet<T> >& triplets, 
			  Eigen::Matrix<T, Eigen::Dynamic, 1>& b, 
			  const int i) const
  {
    triplets.push_back(Eigen::Triplet<T>(i,i-1,-1));
    triplets.push_back(Eigen::Triplet<T>(i,i,1));
    b(i) = 0.0;
  }
	
  std::vector<T> m_points;

};

#endif

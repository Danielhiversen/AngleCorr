#ifndef __SPLINE_H
#define __SPLINE_H

#include <stdint.h>
#include <vector>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include "plane3d.hpp"
#include "quadratic_spline_fitter.hpp"
#include "adjlist.hpp"
#include "helpers.hpp"
#include <unordered_map>
#include "matrix.hpp"
#include "intersection.hpp"
#include "intersection_set.hpp"
#include "metaimage.hpp"

using namespace std;

/**
 * A class representing a quadratic 3D B-spline curve.
 * It is templated: the parameter T is the type of the values it interpolates as well
 *  as the type of the parameter */
template<typename T>
class Spline3D {
public:

 //   typedef  QuadraticSplineFitter<T> QSP_t;

    /**
   * Constructor
   * Initialize a 3D spline  with n_points number of breakpoints
   * @param npoints the number of points
   */
    Spline3D(size_t npoints)
    {
        for(int i = 0; i < 3; i++)
        {
            m_points[i] = vector<T>(npoints);
        }
        m_initialized = false;
        m_axis = 1;
        m_transform = true;
    }

    /**
   * Default constructor - initialize to 10 points
   */
    Spline3D() { Spline3D(0); }

    /**
   * Destructor
   * Frees the splines and does cleanup
   */
    ~Spline3D() { };

    /**
   * Set a point to interpolate
   * @param idx Index to set
   * @param pt Point to set
   */
    inline void
    setPoint(int idx, T const pt[3])
    {
        for(int i = 0; i < 3; i++)
        {
            m_points[i][idx] = pt[i];
        }
        m_initialized = false;
    }

    /**
   * Set whether to transform the plane matrix when retrieving the
   * vector to use for angle recovery
   * @b true to transform, false otherwise
   */
    inline void
    setTransform(bool b)
    {
        m_transform = b;
    }

    /**
   * Return whether plane matrix will be transformed when retrieving
   * the vector to use for angle recovery
   */
    inline bool
    getTransform() const
    {
        return m_transform;
    }

    /**
   * Set which axis of plane matrix to use for angle recovery
   * @param axis: 0 for X axis, 1 for Y axis, 2 for Z axis
   */
    inline void
    setAxis(int axis)
    {
        m_axis = axis;
    }

    /**
   * Retrieve which axis of plane matrix is to be used for angle recovery
   * @return 0 for X axis, 1 for Y axis, 2 for Z axis
   */
    inline int
    getAxis() const
    {
        return m_axis;
    }

    /**
   * Get the length of the spline
   */
    inline size_t
    getLength() const { return m_points[0].size(); }

    /**
   * Set all points in this spline
   * @param x X coordinates
   * @param y Y coordinates
   * @param z Z coordinates
   */
    inline  void
    setPoints(vector<T> const &x,
              vector<T> const &y,
              vector<T> const &z)
    {
        m_points[0] = x;
        m_points[1] = y;
        m_points[2] = z;
    }

    /**
   * Apply an 1d spatial convolution to the points to interpolate
   * @mask Mask to use for convolution - must be of length 3.
   */
    void
    applyConvolution(vector<T> const &mask)
    {
        assert(mask.size() == 3);
        for(int j = 0; j < 3; j++)
        {
            vector<T> newpoints(m_points[j].size());
            int end = m_points[j].size()-1;

            // Reflexive behaviour
            newpoints[0] = 2*mask[0] * m_points[j][1] +	mask[1]*m_points[j][0];
            newpoints[end] = 2*mask[0] * m_points[j][end-1] +	mask[1]*m_points[j][end];
            for(unsigned int i = 1; i < m_points[j].size()-1; i++)
            {
                newpoints[i] = mask[0]*m_points[j][i-1]
                        + mask[1]*m_points[j][i]
                        + mask[2]*m_points[j][i+1];
            }
            m_points[j] = newpoints;
        }
        m_initialized = false;
    }

    /**
   * Initializes the underlying spline structure with the provided data.
   * Needs to be called after adding points to the spline before evaluating it.
   * If it isn't, it will be done automatically
   */
    void
    compute()
    {
        // Build splines
        for(int i = 0; i < 3; i++)
        {
            QuadraticSplineFitter<T> fitter(m_points[i]);
            m_cpoints[i] = fitter.compute_control_points();
        }
        m_initialized = true;
    }


    /**
   * Get number of points to interpolate
   * (and thus, how far the parameter may move)
   * @return Number of points to interpolate
   */
    inline int
    length() const
    {
        return m_points[0].size();
    }





    /**
   * Find an intersection between this curve and a plane.
   * The method is to first find two adjacent points in m_points (the points to interpolate)
   * that is on different sides of the plane. Then we call findRoots() to find the exact location between those two points.
   * @param t The position to start at. Will contain the intersection position after
   * @param plane The plane to intersect with
   * @param pointOnPlane will contain a point that is both on the curve and on the plane.
   * @return true if an intersection was found, false otherwise.
   */
    bool
    intersect(T& t, Plane3D& plane, T pointOnPlane[3]) const
    {
        // 1. Find two neighbouring points for which the the plane distance function has opposite signs
        double pt[3];
        for(int i = 0; i < 3; i++)
        {
            pt[i] = m_points[i][0];
        }
        int sign;
        int prevsign = sgn(plane.getDistance(pt));
        int pos = -1;
        for(unsigned int i = 1; i < m_points[0].size(); i++)
        {
            for(int j = 0; j < 3; j++)
            {
                pt[j] = m_points[j][i];
            }
            sign = sgn(plane.getDistance(pt));
            if(prevsign != sign){
                // We found the points:
                // Points(i-1 and i)
                pos = i-1;
                break;
            }
            prevsign = sign;
        }
        // Return false if we didn't find an intersection
        if(pos == -1){
            return false;
        }

        T roots[2];
        int nroots;
        nroots = findRoots(pos,plane, roots);
        // Rounding errors may cause us to miss the 0-1 interval just slightly, try both
        if(nroots == 0)
            nroots = findRoots(++pos,plane,roots);
        // Add the position of p0 to the roots
        roots[0] += pos;
        roots[1] += pos;

        switch(nroots)
        {
        case 0:
            return false;
        case 1:
        case 2:
            // For now, return only one intersection. May have to fix this.
            // Subtracting 0.5 because the spline is translated by 0.5 along the t axis.
            // This way the spline is aligned with the interpolation points (spline(0) == point 0)
            t = roots[0] - 0.5;
            //	t = roots[0];
            evaluateSingle(t,pointOnPlane);

            return true;
        default:
            assert(1 == 0);
            return false;
        }
    }

    /**
   * Get the derivative of the curve at position t
   * @param t The position to find the derivative for
   * @param point The gradient vector at t=t
   */
    void
    derivativeSingle(T t, T point[3]) const
    {
        // Align with m_points (curve(0) == m_points[0])
        t += 1.5;
        int pos = t;
        t = t - pos;

        for(int i = 0; i < 3; i++)
        {
            point[i]
                    = t*(m_cpoints[i][pos+1]-m_cpoints[i][pos])
                    + (1-t)*(m_cpoints[i][pos]-m_cpoints[i][pos-1]);
        }
    }

    /**
   * Evaluate the curve at position t
   * @param t the parameter position to evaluate at
   * @param point The spatial position at parameter position t
   */
    void
    evaluateSingle(T t, T point[3]) const
    {
        if(!m_initialized )
        {
            reportError("ERROR: Spline3d not initialized");
            return;
        }

        // Align with m_points (curve(0) == m_points[0])
        t += 1.5;
        int pos = t;
        t = t - pos;

        for(int i = 0; i < 3; i++)
        {
            point[i] = (t*t*0.5)*m_cpoints[i][pos+1]
                    + (-(t*t)+t + 0.5) * m_cpoints[i][pos]
                    + ((1-t)*(1-t)*0.5)*m_cpoints[i][pos-1];
        }
    }



    /**
   * Contstruct a set of splines representing the centerline described as a set of lines in *data.
   * The algorithm will detect when centerlines split and make separate spline for the branch.
   * @param data A set of points with lines between them
   * @return a set of 3D splines
   */
    static vector<Spline3D<T> >*
    build(vtkPolyData *data)
    {
        // Algorithm:
        // 1. Make a data structure for each point that holds what line it belongs to (adjacency list)
        // 2. Iterate over the lines in data, building adjacencies, as well as the rank of each point
        // 3. Find all points with rank 1 and push it onto point stack
        // 4. Do (multistart) depth first search
        //    For every discoevered node, add it to the current spline.
        //    When a leaf node is found and there are still points on the stack
        //    make a new spline, adding a few points from before the junction


        // Step 1
        AdjList list(data->GetNumberOfPoints());
        vector<Spline3D<T> > *ret = new vector<Spline3D<T> >();

        // Step 2: Get the lines from the polyData structure and put them in the AdjList
        vtkCellArray *lines = data->GetLines();

        vtkIdType n_ids;
        vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();

        while(lines->GetNextCell(idlist))
        {
            n_ids = idlist->GetNumberOfIds();
            assert(n_ids == 2);
            list.adjacent(idlist->GetId(0), idlist->GetId(1));
        }

        // Initialize node stack
        vector<int> stack = vector<int>();
        // Point stack - this is where the discovered points go.
        // The array is one for each axis, and there
        // is one vector per set of control points.
        // I.e ptstack[2][0] is the set of z coordinates of the control points for curve 0
        vector<vector<T> > ptstack[3];

        for(int i = 0; i < 3; i++)
        {
            ptstack[i] = vector<vector<T> >();
            ptstack[i].push_back(vector<T>());
        }

        double pt[3];
        vector<int> children;

        // When we hit the end of a curve, we will need to know which point in which control point set
        // comes before the new set of control points we will be about to make.
        // The idea is then to keep a stack of "parents", and move it along with the node stack, so that this stack
        // will have the parent information on its head for the point at the
        // stacks head.
        // The pair is <spline, idx>
        vector<pair<int,int> > parents = vector<pair<int,int> >();


        // Build the splines depth first
        int curspline = 0;
        int curnode = 0;

        vector<int> firstNodes = list.findAllFirst();

        for(int i = firstNodes.size()-1; i >= 0; i--)
        {
            // The initial points have no parents
            parents.push_back(pair<int,int>(-1, -1));
            stack.push_back(firstNodes[i]);
        }

        // Do depth first search
        while(!stack.empty())
        {

            // Pop stack
            curnode = stack.back();
            parents.pop_back();
            stack.pop_back();

            // Get the point data for this node
            data->GetPoint(curnode,pt);

            // Assign point to current spline
            for(int i = 0; i < 3; i++)
            {
                ptstack[i][curspline].push_back(pt[i]);
            }

            list.visit(curnode);

            // Find all children, add to stack
            children = list.findAllNext(curnode);
            if(children.size()!=1 && !stack.empty() )
            {
                curspline++;
                int parentspline;
                int parentidx;
                parentspline = parents.back().first;
                parentidx = parents.back().second;

                // Leaf node, we're gonna need a new spline now
                for(int i = 0; i < 3; i++)
                {
                    ptstack[i].push_back(vector<T>());


                    // Add old points to the spline. This is needed so that the branches remain continuous
                    //   ptstack[i][curspline].push_back(ptstack[i][parentspline][parentidx-1]);
                    if(parentidx != -1)
                        ptstack[i][curspline].push_back(ptstack[i][parentspline][parentidx]);

                }

            }
            stack.insert(stack.end(), children.begin(), children.end());
            // Push the parents of all the children we just pushed to the node stack
            // to the parent stack
            for(unsigned int i = 0; i < children.size(); i++)
            {
                parents.push_back(pair<int,int>(curspline,ptstack[0][curspline].size()-1));
            }
        }

        // Put all the generated data into the return spline3d structure
        for(unsigned int i = 0; i < ptstack[0].size(); i++)
        {
            if(ptstack[0][i].size() > 1) {
                Spline3D<T> newspline = Spline3D<T>(ptstack[0][i].size());
                newspline.setPoints(ptstack[0][i], ptstack[1][i], ptstack[2][i]);
                ret->push_back(newspline);      }
        }

        return ret;
    }

    /**
   * Find an intersection with a MetaImage
   *
   * @param img The image to find an intersection with
   *
   * @return Intersection instance where the image intersects with the
   *         curve. If no intersection was found, the isValid() method of the
   *         returned instance will return false.
   *
   * @see Spline3D<T>::intersect()
   */
    Intersection<T>
    findIntersection(const MetaImage<inData_t> *img) const
    {
        Plane3D plane(img->getTransform());
        T t = 0.0;
        T pt[3];
        Intersection<T> intersection = Intersection<T>();
        if(intersect(t,plane,pt))
        {
            intersection.setSpline(this);
            intersection.setParameterPosition(t);
            intersection.setValid(true);
            intersection.setMetaImage(img);

            // Find cos(theta).
            // cos(theta) = plane Y axis dot spline derivative normalized

            T spline_deriv[3];

            derivativeSingle(t,spline_deriv);
            T y_axis[3];

            if(m_transform)
            {
                for(int i = 0; i < 3; i++)
                {
                    y_axis[i] = -img->getTransform()(i,m_axis);
                }
            } else {
                for(int i = 0; i < 3; i++)
                {
                    y_axis[i] = -img->getTransform()(m_axis,i);
                }
            }
            double length_y = length3d(y_axis);
            double length_splinederiv = length3d(spline_deriv);

            T cosTheta = innerProduct(spline_deriv, y_axis)/(length_y*length_splinederiv);
            intersection.setCosTheta(cosTheta);
        }
        return intersection;
    }
    
    /**
   * Find all intersections for a set of images. The result can be retrieved by getIntersections()
   * and getConstIntersections
   *
   * @param imgs Vector of images to intersect with the curve
   */
    void
    findAllIntersections(const vector<MetaImage<inData_t> >& imgs)
    {
        Intersection<T> intersection;


        for(unsigned int i = 0; i < imgs.size(); i++)
        {
            intersection = findIntersection(&imgs[i]);
            if(intersection.isValid())
            {

                m_intersections.push_back(intersection);

            }
        }
    }

    /**
   * Get the set of intersections that was found with findAllIntersections()
   * @return The set of intersections found (empty if none)
   */
    IntersectionSet<T>& getIntersections()
    {
        return m_intersections;
    }

    /**
   * Get the set of intersections that was found with findAllIntersections()
   * @return The set of intersections found (empty if none)
   */
    const IntersectionSet<T>& getConstIntersections() const
    {
        return m_intersections;
    }



protected:

    /**
   * Find the roots of the quadratic equation that can be built by inserting the basis functions into the plane equation
   *
   * @param p The position in m_points such that m_points[p] and m_points[p+1] are on opposite sides of the plane
   * @param plane The plane (from which we get the coefficients)
   * @param roots The roots will be returned here
   * @return the number of roots found
   */
    int findRoots(int p, Plane3D &plane, T roots[2]) const
    {
        assert(m_initialized == true);

        // p0_idx is an index into the interpolation point array,
        // we need to use control points to find the roots.
        p++;
        T a = 0.0;
        T b = 0.0;
        T c = plane.getCoefficient(3);;

        // Use the equation xS_x(t) + yS_y(t) + zS_z(t) - d = 0 to build
        // a 2nd order equation (we are using quadratic splines, so this exists)
        // See equation 3.29, 3.30 and 3.31 in the report.
        // We need to build the equation on the form
        // at^2 + bt + c = 0.

        for(int dim = 0; dim < 3; dim++)
        {
            T a_tmp = 0.0;
            T b_tmp = 0.0;
            T c_tmp = 0.0;


            a_tmp
                    = m_cpoints[dim][p-1]*0.5
                    - m_cpoints[dim][p]
                    + m_cpoints[dim][p+1]*0.5;
            a_tmp *= plane.getCoefficient(dim);

            a += a_tmp;

            b_tmp
                    = - m_cpoints[dim][p-1]
                    + m_cpoints[dim][p];
            b_tmp *= plane.getCoefficient(dim);

            b += b_tmp;

            c_tmp
                    = m_cpoints[dim][p-1]*0.5
                    + m_cpoints[dim][p]*0.5;
            c_tmp *= plane.getCoefficient(dim);

            c += c_tmp;
        }
        // Now we have a, b and c, and we can solve it using the standard formula for solving 2nd order equations
        // (-b +- sqrt(b^2-4ac))/2a

        T d = b*b - 4*a*c;
        // Roots are complex, we are not interested in those..
        // Actually this shouldn't happen
        if(d >= -0.01 && d < 0.0) d = 0.0;

        if(d < 0) return 0;

        if(d == 0)
        {
            roots[0] = -b/(2.0*a);
            // The root can only be a number between 0 and 1
            if(roots[0] >= 0.0 && roots[0] <= 1.0)
                return 1;
            else
                return 0;
        }
        else {
            T r1, r2;
            r1 = (-b + sqrt(d))/(2.0*a);
            r2 = (-b - sqrt(d))/(2.0*a);
            int nroots = 0;
            if(r1 >= 0.0 && r1 <= 1.0)
            {
                roots[nroots] = r1;
                nroots++;
            }
            if(r2 >= 0.0 && r2 <= 1.0)
            {
                roots[nroots] = r2;
                nroots++;
            }
            return nroots;
        }


    }
    /// The points to interpolate
    std::vector<T> m_points[3];
    /// The control points
    std::vector<T> m_cpoints[3];

    /// The intersections (as filled by findAllIntersections)
    IntersectionSet<T> m_intersections;
    /// True if m_cpoints is valid
    bool m_initialized;

    /// Whether to transform when finding vector from plane matrix
    bool m_transform;
    /// Which axis to use from plane matrix
    int m_axis;
};


#endif

#ifndef ANGLE_CORRECTION_IMPL_H
#define ANGLE_CORRECTION_IMPL_H

#include "spline3d.hpp"


typedef vector<Spline3D<double> > vectorSpline3dDouble;

vectorSpline3dDouble angle_correction_impl(vtkPolyData *vpd_centerline, const  char* image_prefix , double Vnyq, double cutoff,  int nConvolutions);
vtkSmartPointer<vtkPolyData> flowDirection( vector<Spline3D<double> > splines, double uncertainty_limit, double minArrowDist);

#endif /* ANGLE_CORRECTION_IMPL_H */

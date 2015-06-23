#include <vector>
#include <vtkSmartPointer.h>
#include "angle_correction_impl.cpp"
#include <vtkPolyDataWriter.h>
#include <lib/writeToFile.cpp>

using namespace std;


/**
 * Write to vtk file
 * This writes the flow direction
 * alias corrected velocities and acquisition direction vector and cosTheta, all annotated to the point representing representing the intersection position
 * for each intersection to a VTK file.
 * @param filename Filename to write to
 * @param splines The spline curves to write
 */


  

int main(int argc, char *argv[])
{
  if(argc != 7)
  {
    cerr << "Usage: " << argv[0] << " centerline.vtk image_prefix Vnyq cutoff nConvolutions dir_uncertainty\n";
    exit(1);
  }
  int argidx = 1;
  const char* centerline = argv[argidx++];
  const char* image_prefix = argv[argidx++];
  double Vnyq = atof(argv[argidx++]);
  double cutoff = atof(argv[argidx++]);
  int nConvolutions = atoi(argv[argidx++]);
  double uncertainty_limit = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vectorSpline3dDouble splines = angle_correction_impl(centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  // Write output files
  writeDirectionToVtkFile("output_flowdirection.vtk", splines,uncertainty_limit);

}

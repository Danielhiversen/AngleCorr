/**
* Main for AngleCorrection
*
* \author Daniel Hoyer Iversen
*
*/

#include "AngleCorrection.h"

using namespace std;


/**
 * Write to vtk file
 * This writes the flow direction
 * alias corrected velocities and acquisition direction vector and cosTheta, all annotated to the point representing representing the intersection position
 * for each intersection to a VTK file.
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

  AngleCorrection angleCorr = AngleCorrection();
  angleCorr.setInput(centerline, image_prefix, Vnyq, cutoff, nConvolutions, nConvolutions,uncertainty_limit);
  if(angleCorr.calculate()){
      // Write output files
      angleCorr.writeDirectionToVtkFile("output_flowdirection.vtk");
   }

}

#include <vector>


void writeDirectionToVtkFile(const char* filename,
		vtkSmartPointer<vtkPolyData> polydata )
{

	  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	  writer->SetFileName(filename);
	  #if VTK_MAJOR_VERSION <= 5
	  	  writer->SetInput(polydata);
	  #else
	  	  writer->SetInputData(polydata);
	  #endif
	  writer->Write();
}

void writeDirectionToVtkFile(const char* filename,
		    vector<Spline3D<D> > *splines, double uncertainty_limit, double minArrowDist)
{
	  vtkSmartPointer<vtkPolyData> polydata = flowDirection(splines, uncertainty_limit, minArrowDist);
	  writeDirectionToVtkFile(filename,	polydata);
}


void writeDirectionToVtkFile(const char* filename,
		    vector<Spline3D<D> > *splines, double uncertainty_limit)
{
	  writeDirectionToVtkFile(filename,	splines,uncertainty_limit,0.3);
}

#include <vector>
#include "ErrorHandler.hpp"

typedef vector<Spline3D<double> > vectorSpline3dDouble;

void writeDirectionToVtkFile(const char* filename,
		vtkSmartPointer<vtkPolyData> polydata )
{
      vtkSmartPointer<ErrorObserver>  errorObserver =  vtkSmartPointer<ErrorObserver>::New();
	  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	  writer->AddObserver(vtkCommand::ErrorEvent,errorObserver);
	  writer->AddObserver(vtkCommand::WarningEvent,errorObserver);
	
	  writer->SetFileName(filename);
	  #if VTK_MAJOR_VERSION <= 5
	  	  writer->SetInput(polydata);
	  #else
	  	  writer->SetInputData(polydata);
	  #endif
	  writer->Write();


    if (errorObserver->GetError())
	{
		reportError("ERROR: Could not write file to disk \n"+ errorObserver->GetErrorMessage());
	}
	if (errorObserver->GetWarning()){
	   cerr << "Caught warning while not writing file to disk! \n " << errorObserver->GetWarningMessage();
	}


}

void writeDirectionToVtkFile(const char* filename,
            vectorSpline3dDouble splines, double uncertainty_limit, double minArrowDist)
{
	  vtkSmartPointer<vtkPolyData> polydata = flowDirection(splines, uncertainty_limit, minArrowDist);
	  writeDirectionToVtkFile(filename,	polydata);
}


void writeDirectionToVtkFile(const char* filename,
            vectorSpline3dDouble splines, double uncertainty_limit)
{
	  writeDirectionToVtkFile(filename,	splines,uncertainty_limit,1.0);
}

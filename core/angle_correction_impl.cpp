#include "angle_correction_impl.h"

#include "spline3d.hpp"
#include "ErrorHandler.hpp"
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>

using namespace std;

/**
* EstimateAngleCorrectedFlowDirection
* @param centerline - centerline of the blood vessels
* @param images - 2D velocity frames
* @param Vnyq - Nyquist velocity
*
* @param minAbsCosThetaCutoff - lower abs(cosTheta) cutoff
* @param splineSmoothing - smoothning of the blood vessel spline
* @param uncertainty_limit - lower value for reject vessel segment
* @param minArrowDist - min distance between visualization arrows
*/

AngleCorrection::AngleCorrection(){
    mOutput = NULL;
    mValidInput= false;
    mClSplinesPtr = new vectorSpline3dDouble();
    mVelDataPtr = new vector<MetaImage<inData_t>>();
    mClData=vtkSmartPointer<vtkPolyData>::New();
    mVelImagePrefix="";
    mIntersections =  0;
    mBloodVessels = 0;
    mNumOfStepsRan=0;

}


AngleCorrection::~AngleCorrection(){
    mOutput = NULL;
    mClSplinesPtr->clear();
    mVelDataPtr->clear();
}


void AngleCorrection::setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, vector<MetaImage<inData_t> >* velData, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    cerr << vpd_centerline->GetNumberOfPoints() <<endl;
    mValidInput= false;
    if (uncertainty_limit < 0.0) reportError("ERROR: uncertainty_limit must be positive ");
    if (minArrowDist < 0.0) reportError("ERROR: minArrowDist must be positive ");
    if (Vnyq < 0.0) reportError("ERROR: vNyquist must be positive ");
    if (nConvolutions < 0.0) reportError("ERROR: nConvolutions must be positive ");
    if (vpd_centerline->GetNumberOfPoints()<=0) reportError("ERROR: No points found in the center line ");
    if (vpd_centerline->GetNumberOfLines()<=0) reportError("ERROR: No lines found in the center line, the center line must be a linked list ");

    if((velData->size() > 0 && mVelDataPtr!=velData) || mVelImagePrefix.size()==0)
    {
        mVelDataPtr->clear();
        mVelDataPtr=velData;
        mUpdate1=true;
    }

    if(mVnyq!=Vnyq ||
            mCutoff!=cutoff ||
            mnConvolutions!=nConvolutions ||
            !EqualVtkPolyData(mClData,vpd_centerline))
    {
        mClData->DeepCopy(vpd_centerline);
        mVnyq=Vnyq;
        mCutoff=cutoff;
        mnConvolutions=nConvolutions;
        mUpdate1=true;
    }

    if(mUncertainty_limit!=uncertainty_limit ||
            mMinArrowDist!=minArrowDist)
    {
        mUncertainty_limit=uncertainty_limit;
        mMinArrowDist=minArrowDist;
        mUpdate2=true;
    }
    mValidInput= true;
    cerr << "Successfully set params" << endl;
    cerr << mClData->GetNumberOfPoints() <<endl;
}


void AngleCorrection::setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, const  char* velImagePrefix , double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    cerr << vpd_centerline->GetNumberOfPoints() <<endl;

    if(mVelImagePrefix!=std::string(velImagePrefix))
    {
        mVelImagePrefix=std::string(velImagePrefix);
        mVelDataPtr->clear();
        mUpdate1=true;
    }

    std::string filename=std::string(velImagePrefix);
    filename.append("0.mhd");
    if(access( filename.c_str(), F_OK ) == -1)
    {
        reportError("ERROR: Could not read velocity data \n");
    }

    setInput(vpd_centerline,  new vector<MetaImage<inData_t>>(),  Vnyq, cutoff, nConvolutions, uncertainty_limit, minArrowDist);
}


void AngleCorrection::setInput(const char* centerline,const char* image_prefix, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    vtkSmartPointer<vtkPolyDataReader> clReader = vtkSmartPointer<vtkPolyDataReader>::New();

    vtkSmartPointer<ErrorObserver>  errorObserver =  vtkSmartPointer<ErrorObserver>::New();
    clReader->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    clReader->AddObserver(vtkCommand::WarningEvent,errorObserver);

    clReader->SetFileName(centerline);
    clReader->Update();
    if (errorObserver->GetError())
    {
        reportError("ERROR: Could not read center line data \n"+ errorObserver->GetErrorMessage());

    }
    if (errorObserver->GetWarning()){
        cerr << "Caught warning while reading center line data \n! " << errorObserver->GetWarningMessage();
    }

    vtkSmartPointer<vtkPolyData> vpd_centerline = clReader->GetOutput();

    setInput(vpd_centerline,  image_prefix,  Vnyq, cutoff, nConvolutions, uncertainty_limit, minArrowDist);
}




bool AngleCorrection::calculate()
{
    cerr << mClData->GetNumberOfPoints() <<endl;

    if(!mValidInput)
    {
        mOutput = NULL;
        return false;
    }
    mValidInput=false;

    cerr << mClData->GetNumberOfPoints() <<endl;

    if(mVelDataPtr->size() == 0)
    {
        cerr << "Loading data " << endl;
        mVelDataPtr->clear();
        mVelDataPtr = MetaImage<inData_t>::readImages(mVelImagePrefix);
    }

    mNumOfStepsRan=0;
    if(mUpdate1)
    {
        mNumOfStepsRan++;
        cerr << "started step 1 of 2 " << mnConvolutions<< endl;
        angle_correction_impl(mClData, mVelDataPtr, mVnyq, mCutoff, mnConvolutions);
    }

    if(mUpdate1 || mUpdate2)
    {
        mNumOfStepsRan++;
        cerr << "started step 2 of 2" << endl;
        mOutput= computeVtkPolyData(mClSplinesPtr, mUncertainty_limit, mMinArrowDist);
    }

    mUpdate1=false;
    mUpdate2=false;
    return true;
}

vtkSmartPointer<vtkPolyData>  AngleCorrection::getOutput()
{
    return mOutput;
}


vectorSpline3dDouble AngleCorrection::getClSpline()
{
    return *mClSplinesPtr;
}



void AngleCorrection::writeDirectionToVtkFile(const char* filename)
{

    vtkSmartPointer<vtkPolyData> polydata = getOutput();
    if(!polydata) polydata=vtkSmartPointer<vtkPolyData>::New();

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

void AngleCorrection::angle_correction_impl(vtkSmartPointer<vtkPolyData> vpd_centerline, vector<MetaImage<inData_t> >* images , double Vnyq, double cutoff,  int nConvolutions)
{
    bool verbose = true;

    mClSplinesPtr->clear();
    mClSplinesPtr = Spline3D<double>::build(vpd_centerline);

    for(auto &spline: *mClSplinesPtr)
    {
        mBloodVessels++;
        // Smooth the splines
        for(int j = 0; j < nConvolutions; j++)
        {
            spline.applyConvolution({0.25, 0.50, 0.25 });
        }

        // Compute control points for splines
        spline.compute();

        // Find all the intersections
        spline.findAllIntersections(*images);
        spline.getIntersections().setVelocityEstimationCutoff(cutoff,1.0);
        mIntersections += spline.getIntersections().size();

        // Now that we know the intersection points,
        // we can go through all the image planes and do the region growing.
        for_each(spline.getIntersections().begin(), spline.getIntersections().end(),[](Intersection<double> &it){it.regionGrow();});

        // We may now do the direction vector estimation
        // Using the default parameters set in IntersectionSet constructor
        spline.getIntersections().estimateDirection();

        // Do aliasing correction
        if (Vnyq > 0){
            spline.getIntersections().correctAliasing(Vnyq);
        }

        // Least squares velocity estimates
        spline.getIntersections().estimateVelocityLS();

        // Output direction and LS velocity
        if (verbose)
        {
            cerr << "Spline " << mBloodVessels << " gave direction "
                 << spline.getIntersections().getEstimatedDirection()
                 << " LS velocity " << spline.getIntersections().getEstimatedVelocity() << endl;
        }
    }
    if (verbose)
    {
        cerr << "Found " << mIntersections << " intersections in " << mBloodVessels << " splines.\n";
    }
}



vtkSmartPointer<vtkPolyData> AngleCorrection::computeVtkPolyData( vectorSpline3dDoublePtr splines, double uncertainty_limit, double minArrowDist)
{
    vtkSmartPointer<vtkPoints> pointarray = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkDoubleArray> flowdirection = vtkSmartPointer<vtkDoubleArray>::New();
    flowdirection->SetName("Flow direction");
    flowdirection->SetNumberOfComponents(3);
    vtkSmartPointer<vtkDoubleArray> dir_uncertainty = vtkSmartPointer<vtkDoubleArray>::New();
    dir_uncertainty->SetName("FLow direction uncertainty");
    vtkSmartPointer<vtkDoubleArray> velocitydata = vtkSmartPointer<vtkDoubleArray>::New();
    velocitydata->SetName("Vessel velocity");

    double p[3];
    double p_prev[3];
    double p_temp[3];
    double flow_vector[3];
    double flow_vector_n[3];
    double flow_direction;
    double abs_dir;
    double abs_vessel_vel;
    int num_uncertainty_limit = 0;
    for(auto &spline: *splines)
    {

        flow_direction = spline.getIntersections().getEstimatedDirection();
        abs_dir = flow_direction*sgn(flow_direction);
        abs_vessel_vel = spline.getIntersections().getEstimatedVelocity();
        abs_vessel_vel = abs_vessel_vel*sgn(abs_vessel_vel);


        if( abs_dir< uncertainty_limit){
            num_uncertainty_limit++;
            continue;
        }
        if(std::isnan(abs_dir) || std::isnan(abs_vessel_vel) ){
            continue;
        }

        spline.evaluateSingle(0,p_prev);
        for(double t = 0; t < (spline.length()-1); t+=0.3){
            spline.evaluateSingle(t,p);
            if(std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2])  ){
                continue;
            }

            p_temp[0]=p[0]-p_prev[0];
            p_temp[1]=p[2]-p_prev[1];
            p_temp[1]=p[2]-p_prev[2];

            if(length3d(p_temp)<minArrowDist){
                continue;
            }
            p_prev[0]=p[0];
            p_prev[1]=p[1];
            p_prev[2]=p[2];

            spline.derivativeSingle(t,flow_vector);

            flow_vector[0]=flow_direction*flow_vector[0];
            flow_vector[1]=flow_direction*flow_vector[1];
            flow_vector[2]=flow_direction*flow_vector[2];

            normalize3d(flow_vector_n, flow_vector);
            if(std::isnan(flow_vector_n[0]) || std::isnan(flow_vector_n[1]) || std::isnan(flow_vector_n[2]) ){
                continue;
            }

            pointarray->InsertNextPoint(p);
            flowdirection->InsertNextTupleValue(flow_vector_n);
            dir_uncertainty->InsertNextTupleValue(&abs_dir);
            velocitydata->InsertNextTupleValue(&abs_vessel_vel);
        }
    }
    // Put it all inside a vtkPolyData
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPointData> pointdata = polydata->GetPointData();
    polydata->SetPoints(pointarray);

    pointdata->AddArray(flowdirection);
    pointdata->AddArray(dir_uncertainty);
    pointdata->AddArray(velocitydata);
    if (num_uncertainty_limit){
        cerr << "Removed " << num_uncertainty_limit << " segment(s) due to an uncertainty limit of " << uncertainty_limit << "\n";
    }
    return polydata;
}



bool AngleCorrection::EqualVtkPolyData( vtkSmartPointer<vtkPolyData> leftHandSide, vtkSmartPointer<vtkPolyData> rightHandSide)
{
    if( leftHandSide->GetNumberOfCells()!=rightHandSide->GetNumberOfCells()) return false;
    if( leftHandSide->GetNumberOfVerts()!=rightHandSide->GetNumberOfVerts()) return false;
    if( leftHandSide->GetNumberOfLines()!=rightHandSide->GetNumberOfLines()) return false;
    if( leftHandSide->GetNumberOfPolys()!=rightHandSide->GetNumberOfPolys()) return false;
    if( leftHandSide->GetNumberOfStrips()!=rightHandSide->GetNumberOfStrips()) return false;
    unsigned int numberOfPointsRight = rightHandSide->GetNumberOfPoints();
    unsigned int numberOfPointsLeft = leftHandSide->GetNumberOfPoints();
    if(numberOfPointsLeft!=numberOfPointsRight) return false;

    double pointOne[3];
    double pointTwo[3];
    for( unsigned int i( 0 ); i < numberOfPointsRight; i++ )
    {
        rightHandSide->GetPoint(i, pointOne);
        leftHandSide->GetPoint(i, pointTwo);
        double x = pointOne[0] - pointTwo[0];
        double y = pointOne[1] - pointTwo[1];
        double z = pointOne[2] - pointTwo[2];
        double distance = x*x + y*y + z*z;
        if( distance > 0.001 ) return false;
    }
     return true;
}

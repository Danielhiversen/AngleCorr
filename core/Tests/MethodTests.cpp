/**
* Tests for AngleCorrection
*
* \author Daniel Hoyer Iversen
*
*
*/

#include <vector>
#include <vtkSmartPointer.h>
#include "AngleCorrection.h"
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <cstdio>
#include <time.h>

#include "catch.hpp"


using namespace std;


char * appendTestFolder(const char * filename){
    char anglecorrection_test_data_dir[]=ANGLECORRECTION_TEST_DATA_DIR;
    char * newArray = new char[std::strlen(anglecorrection_test_data_dir)+std::strlen(filename)+1];
    std::strcpy(newArray,anglecorrection_test_data_dir);
    std::strcat(newArray,filename);

    return newArray;
}

void validateFlowDirection_FlowVel(vectorSpline3dDouble splines, double *true_flow){
    double flow_direction;
    double flow_vel;

    int k=0;
    for(auto &spline: splines)
    {
        flow_vel = spline.getIntersections().getEstimatedVelocity();
        flow_direction = spline.getIntersections().getEstimatedDirection();
        CHECK( (flow_vel) == Approx(true_flow[k]).epsilon(0.005));
        CHECK(sgn(flow_direction) == sgn(true_flow[k++]));
    }
}

void validateFiles(const char* filename_a,const char* filename_b, bool shouldBeEqual = true){
    vtkSmartPointer<ErrorObserver>  errorObserver =  vtkSmartPointer<ErrorObserver>::New();

    vtkSmartPointer<vtkPolyDataReader> reader2 = vtkSmartPointer<vtkPolyDataReader>::New();
    reader2->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    reader2->SetFileName(filename_b);
    reader2->Update();

    vtkSmartPointer<vtkPolyDataReader> reader1 = vtkSmartPointer<vtkPolyDataReader>::New();
    reader1->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    reader1->SetFileName(filename_a);
    reader1->Update();

   bool readFilesSuccesfully = true;
   if (errorObserver->GetError() ||errorObserver->GetWarning()){
         readFilesSuccesfully = false;

   }
   REQUIRE(readFilesSuccesfully);

    vtkSmartPointer<vtkPolyData> leftHandSide = reader1->GetOutput();
    vtkSmartPointer<vtkPolyData> rightHandSide = reader2->GetOutput();

    unsigned int numberOfPointsRight = rightHandSide->GetNumberOfPoints();
    unsigned int numberOfPointsLeft = leftHandSide->GetNumberOfPoints();
    if(shouldBeEqual){
        REQUIRE(numberOfPointsLeft==numberOfPointsRight);
    }
    if(!shouldBeEqual && numberOfPointsLeft!=numberOfPointsRight){
        return;
    }

    bool all_equal = true;
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
        if( distance > 0.001 ) all_equal=false;
    }

    unsigned int numberOfArraysRight = rightHandSide->GetPointData()->GetNumberOfArrays();
    unsigned int numberOfArraysLeft = leftHandSide->GetPointData()->GetNumberOfArrays();
    if(shouldBeEqual){
        REQUIRE(numberOfArraysLeft==numberOfArraysRight);
    }
    if(!shouldBeEqual && numberOfArraysLeft!=numberOfArraysRight){
        return;
    }


    double pointRight;
    double pointLeft;
    for(int k=0; k <  numberOfArraysRight; k++)
    {
        unsigned int numberRight = rightHandSide->GetPointData()->GetArray(k)->GetDataSize()/ rightHandSide->GetPointData()->GetArray(k)->GetNumberOfComponents();
        unsigned int numberLeft =  leftHandSide->GetPointData()->GetArray(k)->GetDataSize()/ leftHandSide->GetPointData()->GetArray(k)->GetNumberOfComponents();
        if(shouldBeEqual){
            REQUIRE(numberLeft==numberRight);
        }
        if(!shouldBeEqual && numberLeft!=numberRight){
            return;
        }


        for(int m=0; m < rightHandSide->GetPointData()->GetArray(k)->GetNumberOfComponents() ; m++){
            for(int l=0; l < numberRight ; l++){
                pointRight= rightHandSide->GetPointData()->GetArray(k)->GetComponent(l,m);
                pointLeft=   leftHandSide->GetPointData()->GetArray(k)->GetComponent(l,m);
                if( abs(pointRight-pointLeft) > 0.001 ) all_equal=false;
            }
        }
    }
    REQUIRE(all_equal== shouldBeEqual);
}


void testFlow(char centerline[], char image_prefix[], double Vnyq, double cutoff, int nConvolutions,double *true_flow, char true_output[]){
    const char testFile[] = "/flowdirection_test_1.vtk";

    AngleCorrection angleCorr = AngleCorrection();
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions));
    bool res = false;
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    vectorSpline3dDouble splines = angleCorr.getClSpline();
   // validateFlowDirection_FlowVel(splines,true_flow);

    CHECK_NOTHROW(angleCorr.writeDirectionToVtkFile(appendTestFolder(testFile)));
    validateFiles(appendTestFolder(testFile), appendTestFolder(true_output));
  //  std::remove(appendTestFolder(testFile));
}

TEST_CASE("AngleCorrection: Test flow direction estimation 1", "[angle_correction][unit][flow_dirA][")
    {
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.465};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_1.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}



TEST_CASE("AngleCorrection: Test flow direction estimation 2", "[angle_correction][flow_dir][unit]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150527T125751_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751/US-Acq_02_20150527T125751_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.557};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_2.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 3", "[angle_correction][integration][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_03_20150527T130026_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026/US-Acq_03_20150527T130026_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.534};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_3.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 4", "[angle_correction][integration][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_04_20150527T130043_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_04_20150527T130043/US-Acq_04_20150527T130043_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.577};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_4.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 5", "[angle_correction][integration][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_05_20150527T130229_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_05_20150527T130229/US-Acq_05_20150527T130229_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [2]={-0.933,0.239};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_5.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 6", "[angle_correction][integration][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329/US-Acq_06_20150527T130329_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [2]={0.651,-2.50};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_6.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 7, aliasing", "[angle_correction][unit][aliasing]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_07_20150527T130532_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532/US-Acq_07_20150527T130532_Velocity_";
    
    double Vnyq =  0.156;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.314};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_7.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 8, aliasing", "[angle_correction][integration][aliasing]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_08_20150527T130558_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_08_20150527T130558/US-Acq_08_20150527T130558_Velocity_";
    
    double Vnyq =  0.156;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={0.403};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_8.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 9, cross movement", "[angle_correction][integration]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_09_20150527T131009_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_09_20150527T131009/US-Acq_09_20150527T131009_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.625};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_9.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test flow direction estimation 10, cross movement", "[angle_correction][unit]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={0.5847};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("AngleCorrection: Test Invalid parameters", "[angle_correction][unit]")
{
    
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/NonExisting.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0.5;
    double minArrowDist = 1.0;
    
    bool res = false;

    AngleCorrection angleCorr = AngleCorrection();
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions,uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);

    char centerline2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-NonExisting";
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline2), appendTestFolder(image_prefix2), Vnyq, cutoff, nConvolutions,  uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);
    
    char centerline3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    Vnyq =  -0.312;
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);

    Vnyq =  0.0;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    nConvolutions = -1;
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);

    nConvolutions = 2;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    uncertainty_limit = 0;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    uncertainty_limit = -0.5;
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);

    uncertainty_limit = 0.5;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    minArrowDist = -0.5;
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);

    minArrowDist = 1.5;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
}


TEST_CASE("AngleCorrection: Test several runs", "[angle_correction][integration]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0.5;
    double minArrowDist = 1.0;
    
    
    char centerline2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
    char image_prefix2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329/US-Acq_06_20150527T130329_Velocity_";
    
    double Vnyq2 =  0.312;
    double cutoff2 = 0.18;
    int nConvolutions2 = 6;
    
    const char* filename_a ="/flowdirection_test_11_a.vtk";
    const char* filename_b ="/flowdirection_test_11_b.vtk";
    
    bool res = false;
    AngleCorrection angleCorr = AngleCorrection();

    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);

    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    std::remove(appendTestFolder(filename_b));

    angleCorr.setInput(appendTestFolder(centerline2), appendTestFolder(image_prefix2), Vnyq2, cutoff2, nConvolutions2);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_6.vtk"));
  
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));

    std::remove(appendTestFolder(filename_a));
    std::remove(appendTestFolder(filename_b));

    cutoff = 1;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));


    cutoff = 0.18;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    nConvolutions = 50;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    nConvolutions = 6;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    minArrowDist = 5.0;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    minArrowDist = 1.0;
    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

}


TEST_CASE("AngleCorrection: Test several runs cl pointer input", "[angle_correction][unit]")
{
    
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0.5;
    double minArrowDist = 1.0;
    
    vtkSmartPointer<vtkPolyDataReader> clReader1 = vtkSmartPointer<vtkPolyDataReader>::New();
    clReader1->SetFileName(appendTestFolder(centerline));
    clReader1->Update();
    vtkSmartPointer<vtkPolyData> vpd_centerline1 = clReader1->GetOutput();
    const char* filename_a ="/flowdirection_test_11_a.vtk";
    const char* filename_b ="/flowdirection_test_11_b.vtk";
    
    bool res = false;
    AngleCorrection angleCorr = AngleCorrection();

    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);

    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    std::remove(appendTestFolder(filename_b));

    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder(filename_a));

    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));

    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));

    std::remove(appendTestFolder(filename_a));
    std::remove(appendTestFolder(filename_b));

    cutoff = 1;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    cutoff = 0.18;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    nConvolutions = 50;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    nConvolutions = 6;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    minArrowDist = 5.0;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    minArrowDist = 1.0;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    vpd_centerline1=vtkSmartPointer<vtkPolyData>::New();
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    vpd_centerline1=vtkSmartPointer<vtkPolyData>::New();
    CHECK_THROWS(angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    res = angleCorr.calculate();
    REQUIRE(!res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_b));
}



TEST_CASE("AngleCorrection: Benchmark", "[angle_correction][Benchmark]")
{
    clock_t start, stop;
    double run_time = 0.0;
    start = clock();

    bool res;

    char centerline1[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
    char image_prefix1[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity_";
    double Vnyq1 =  0.312;
    double cutoff1 = 0.18;
    int nConvolutions1 = 6;

    AngleCorrection angleCorr = AngleCorrection();
    angleCorr.setInput(appendTestFolder(centerline1), appendTestFolder(image_prefix1), Vnyq1, cutoff1, nConvolutions1);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150527T125751_Angio_1_tsf_cl1.vtk";
    char image_prefix2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751/US-Acq_02_20150527T125751_Velocity_";
    double Vnyq2 =  0.312;
    double cutoff2 = 0.18;
    int nConvolutions2 = 6;

    angleCorr.setInput(appendTestFolder(centerline2), appendTestFolder(image_prefix2), Vnyq2, cutoff2, nConvolutions2);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_03_20150527T130026_Angio_1_tsf_cl1.vtk";
    char image_prefix3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026/US-Acq_03_20150527T130026_Velocity_";
    double Vnyq3 =  0.312;
    double cutoff3 = 0.18;
    int nConvolutions3 = 6;

    angleCorr.setInput(appendTestFolder(centerline3), appendTestFolder(image_prefix3), Vnyq3, cutoff3, nConvolutions3);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline4[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_04_20150527T130043_Angio_1_tsf_cl1.vtk";
    char image_prefix4[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_04_20150527T130043/US-Acq_04_20150527T130043_Velocity_";
    double Vnyq4 =  0.312;
    double cutoff4 = 0.18;
    int nConvolutions4 = 6;

    angleCorr.setInput(appendTestFolder(centerline4), appendTestFolder(image_prefix4), Vnyq4, cutoff4, nConvolutions4);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline5[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_05_20150527T130229_Angio_1_tsf_cl1.vtk";
    char image_prefix5[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_05_20150527T130229/US-Acq_05_20150527T130229_Velocity_";
    double Vnyq5 =  0.312;
    double cutoff5 = 0.18;
    int nConvolutions5 = 6;

    angleCorr.setInput(appendTestFolder(centerline5), appendTestFolder(image_prefix5), Vnyq5, cutoff5, nConvolutions5);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline6[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
    char image_prefix6[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329/US-Acq_06_20150527T130329_Velocity_";
    double Vnyq6 =  0.312;
    double cutoff6 = 0.18;
    int nConvolutions6 = 6;

    angleCorr.setInput(appendTestFolder(centerline6), appendTestFolder(image_prefix6), Vnyq6, cutoff6, nConvolutions6);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline7[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_07_20150527T130532_Angio_1_tsf_cl1.vtk";
    char image_prefix7[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532/US-Acq_07_20150527T130532_Velocity_";
    double Vnyq7 =  0.156;
    double cutoff7 = 0.18;
    int nConvolutions7 = 6;


    angleCorr.setInput(appendTestFolder(centerline7), appendTestFolder(image_prefix7), Vnyq7, cutoff7, nConvolutions7);
    res = angleCorr.calculate();
    REQUIRE(res);

    char centerline8[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_08_20150527T130558_Angio_1_tsf_cl1.vtk";
    char image_prefix8[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_08_20150527T130558/US-Acq_08_20150527T130558_Velocity_";
    double Vnyq8 =  0.156;
    double cutoff8 = 0.18;
    int nConvolutions8 = 6;


    angleCorr.setInput(appendTestFolder(centerline8), appendTestFolder(image_prefix8), Vnyq8, cutoff8, nConvolutions8);
    res = angleCorr.calculate();
    REQUIRE(res);

    char centerline9[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_09_20150527T131009_Angio_1_tsf_cl1.vtk";
    char image_prefix9[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_09_20150527T131009/US-Acq_09_20150527T131009_Velocity_";
    double Vnyq9 =  0.312;
    double cutoff9 = 0.18;
    int nConvolutions9 = 6;

    angleCorr = AngleCorrection();
    angleCorr.setInput(appendTestFolder(centerline9), appendTestFolder(image_prefix9), Vnyq9, cutoff9, nConvolutions9);
    res = angleCorr.calculate();
    REQUIRE(res);


    char centerline10[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix10[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    double Vnyq10 =  0.312;
    double cutoff10 = 0.18;
    int nConvolutions10 = 6;


    angleCorr.setInput(appendTestFolder(centerline10), appendTestFolder(image_prefix10), Vnyq10, cutoff10, nConvolutions10);
    res = angleCorr.calculate();
    REQUIRE(res);

    stop = clock();
    run_time = (double) (stop-start)/CLOCKS_PER_SEC;
    REQUIRE(run_time<15);

    cerr << "Run time: " <<  run_time <<"\n";
}




TEST_CASE("AngleCorrection: Test several runs cl pointer input simple", "[angle_correction][integration][simple]")
{
    
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0.5;
    double minArrowDist = 1.0;
    
    vtkSmartPointer<vtkPolyDataReader> clReader1 = vtkSmartPointer<vtkPolyDataReader>::New();
    clReader1->SetFileName(appendTestFolder(centerline));
    clReader1->Update();
    vtkSmartPointer<vtkPolyData> vpd_centerline1 = clReader1->GetOutput();
    const char* filename_a ="/flowdirection_test_11_a.vtk";
    const char* filename_b ="/flowdirection_test_11_b.vtk";
    
    bool res = false;
    AngleCorrection angleCorr = AngleCorrection();

    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);

    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    std::remove(appendTestFolder(filename_b));

    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder(filename_a));

    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));

    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));

    std::remove(appendTestFolder(filename_a));
    std::remove(appendTestFolder(filename_b));

    cutoff = 1;
    angleCorr.setInput(vpd_centerline1, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist);
    res = angleCorr.calculate();
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    std::remove(appendTestFolder(filename_a));
}




TEST_CASE("AngleCorrection: Test flow direction estimation tumour data", "[angle_correction][unit]")
    {
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150625T105554_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150625T105554/US-Acq_02_20150625T105554_Velocity_";

    double Vnyq =  0.0;
    double cutoff = 0;
    int nConvolutions = 5;

    double true_flow [4]={0.082484, 0.146338,  0.119538, 0.322488};
    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_tumour.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}

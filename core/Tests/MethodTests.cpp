#include <vector>
#include <vtkSmartPointer.h>
#include "angle_correction_impl.h"
#include <vtkPolyDataWriter.h>
#include <cstdio>
#include <time.h>

#include "catch.hpp"


using namespace std;


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
    std::ifstream file_a(filename_a);
    std::ifstream file_b(filename_b);
    std::string line_a,line_b;
    bool file_a_exist = false;
    bool all_equal = true;
    while (std::getline(file_a, line_a))
    {
        std::getline(file_b, line_b);
        if( line_a != line_b){
            all_equal = false;
        }
        file_a_exist = true;
    }
    REQUIRE(file_a_exist);
    REQUIRE(all_equal== shouldBeEqual);
}

char * appendTestFolder(const char * filename){
    char anglecorrection_test_data_dir[]=ANGLECORRECTION_TEST_DATA_DIR;
    char * newArray = new char[std::strlen(anglecorrection_test_data_dir)+std::strlen(filename)+1];
    std::strcpy(newArray,anglecorrection_test_data_dir);
    std::strcat(newArray,filename);
    
    return newArray;
}


void testFlow(char centerline[], char image_prefix[], double Vnyq, double cutoff, int nConvolutions,double *true_flow, char true_output[]){
    const char testFile[] = "/testOut/flowdirection_test_1.vtk";

    AngleCorrection angleCorr = AngleCorrection();
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions));
    bool res = false;
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    vectorSpline3dDouble splines = angleCorr.getClSpline();
    validateFlowDirection_FlowVel(splines,true_flow);

    CHECK_NOTHROW( angleCorr.writeDirectionToVtkFile(appendTestFolder(testFile)));
    validateFiles(appendTestFolder(testFile), appendTestFolder(true_output));
    std::remove(appendTestFolder(testFile));
}


TEST_CASE("Test flow direction estimation 1", "[angle_correction][flow_dirA]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.465};
    char true_output[] ="/outPutFiles/output_flowdirection_test_1.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}



TEST_CASE("Test flow direction estimation 2", "[angle_correction][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150527T125751_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751_raw/US-Acq_02_20150527T125751_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.557};
    char true_output[] ="/outPutFiles/output_flowdirection_test_2.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 3", "[angle_correction][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_03_20150527T130026_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026_raw/US-Acq_03_20150527T130026_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [1]={-0.534};
    char true_output[] ="/outPutFiles/output_flowdirection_test_3.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 4", "[angle_correction][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_04_20150527T130043_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_04_20150527T130043_raw/US-Acq_04_20150527T130043_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.577};
    char true_output[] ="/outPutFiles/output_flowdirection_test_4.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 5", "[angle_correction][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_05_20150527T130229_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_05_20150527T130229_raw/US-Acq_05_20150527T130229_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [2]={-0.933,0.239};
    char true_output[] ="/outPutFiles/output_flowdirection_test_5.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 6", "[angle_correction][flow_dir]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329_raw/US-Acq_06_20150527T130329_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    double true_flow [2]={0.651,-2.50};
    char true_output[] ="/outPutFiles/output_flowdirection_test_6.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 7, aliasing", "[angle_correction][aliasing]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_07_20150527T130532_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532_raw/US-Acq_07_20150527T130532_Velocity_";
    
    double Vnyq =  0.156;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.314};
    char true_output[] ="/outPutFiles/output_flowdirection_test_7.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 8, aliasing", "[angle_correction][aliasing]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_08_20150527T130558_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_08_20150527T130558_raw/US-Acq_08_20150527T130558_Velocity_";
    
    double Vnyq =  0.156;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={0.403};
    char true_output[] ="/outPutFiles/output_flowdirection_test_8.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 9, cross movement", "[angle_correction]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_09_20150527T131009_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_09_20150527T131009_raw/US-Acq_09_20150527T131009_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={-0.625};
    char true_output[] ="/outPutFiles/output_flowdirection_test_9.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test flow direction estimation 10, cross movement", "[angle_correction]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    
    double true_flow [1]={0.5847};
    char true_output[] ="/outPutFiles/output_flowdirection_test_10.vtk";

    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions, true_flow, true_output);
}


TEST_CASE("Test Invalid parameters", "[angle_correction]")
{
    
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/NonExisting.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";
    
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
    char image_prefix2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-NonExisting";
    CHECK_THROWS(angleCorr.setInput(appendTestFolder(centerline2), appendTestFolder(image_prefix2), Vnyq, cutoff, nConvolutions,  uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(!res);
    
    char centerline3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix3[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";
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


TEST_CASE("Test several runs", "[angle_correction]")
{
    
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0.5;
    double minArrowDist = 1.0;
    
    
    char centerline2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
    char image_prefix2[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329_raw/US-Acq_06_20150527T130329_Velocity_";
    
    double Vnyq2 =  0.312;
    double cutoff2 = 0.18;
    int nConvolutions2 = 6;
    
    const char* filename_a ="/testOut/flowdirection_test_11_a.vtk";
    const char* filename_b ="/testOut/flowdirection_test_11_b.vtk";
    
    bool res = false;
    AngleCorrection angleCorr = AngleCorrection();

    cerr << "test1\n" ;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);

    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    std::remove(appendTestFolder(filename_b));

    cerr << "test2\n" ;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline2), appendTestFolder(image_prefix2), Vnyq2, cutoff2, nConvolutions2));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/outPutFiles/output_flowdirection_test_6.vtk"));
    cerr << "test3\n" ;

    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    
    validateFiles(appendTestFolder(filename_a), appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"));

    std::remove(appendTestFolder(filename_a));
    std::remove(appendTestFolder(filename_b));

    cutoff = 1;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));


    cutoff = 0.18;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    nConvolutions = 50;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    nConvolutions = 6;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

    minArrowDist = 5.0;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_a));
    validateFiles(appendTestFolder(filename_a), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"),false);
    std::remove(appendTestFolder(filename_a));

    minArrowDist = 1.0;
    CHECK_NOTHROW(angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit,minArrowDist));
    CHECK_NOTHROW(res = angleCorr.calculate());
    REQUIRE(res);
    angleCorr.writeDirectionToVtkFile(appendTestFolder(filename_b));
    validateFiles(appendTestFolder(filename_b), appendTestFolder("/outPutFiles/output_flowdirection_test_10.vtk"));
    std::remove(appendTestFolder(filename_b));

}




TEST_CASE("Timing", "[angle_correction_time]")
{
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_03_20150527T130026_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026_raw/US-Acq_03_20150527T130026_Velocity_";

    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;

    clock_t start, stop;
    double run_time = 0.0;
    start = clock();

    AngleCorrection angleCorr = AngleCorrection();


    angleCorr.setInput(appendTestFolder(centerline), appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions);
    bool res = angleCorr.calculate();

    stop = clock();
    run_time = (double) (stop-start)/CLOCKS_PER_SEC;
    REQUIRE(run_time<10);

    printf("Run time: %f\n", run_time);
}

#include <vector>
#include <vtkSmartPointer.h>
#include "angle_correction_impl.cpp"
#include <vtkPolyDataWriter.h>
#include <lib/writeToFile.cpp>

#include "catch.hpp"

using namespace std;


TEST_CASE("Test for nan in output", "[angle_correction]")
{

	cerr << std::string(ANGLECORRECTION_TEST_DATA_DIR) << endl;
  char toolpositions[] =  "../Testdata/2015-01-27_Tumor.cx3/US_Acq/US-Acq_02_20150127T110442_raw/US-Acq_02_20150127T110442_Velocity.fp";
  char centerline[] = "../Testdata/2015-01-27_Tumor.cx3/Images/US_02_20150127T110442_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-01-27_Tumor.cx3/US_Acq/US-Acq_02_20150127T110442_raw/US-Acq_02_20150127T110442_Velocity_";
  double Vnyq = 0.2925;
  double cutoff = 0.0;
  int nConvolutions = 3;

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D p[3];
  D abs_dir;
  D abs_vessel_vel;
  D flow_direction;

  for(auto &spline: *splines)
  {

	flow_direction = spline.getIntersections().getEstimatedDirection();
	abs_dir = flow_direction*sgn(flow_direction);
	abs_vessel_vel = spline.getIntersections().getEstimatedVelocity();
	abs_vessel_vel = abs_vessel_vel*sgn(abs_vessel_vel);


	REQUIRE(!std::isnan(abs_dir));
	REQUIRE(!std::isnan(abs_vessel_vel));
	  
	for(double t = 0; t < (spline.length()-1); t+=0.3){

		spline.evaluateSingle(t,p);
		REQUIRE(!std::isnan(p[0]));
		REQUIRE(!std::isnan(p[1]));
		REQUIRE(!std::isnan(p[2]));
		
	}
  } 
}
  


TEST_CASE("Test flow direction estimation 1", "[angle_correction][flow_dir]")
{
  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;

  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) ==-1);
  }
  
  writeDirectionToVtkFile("output_flowdirection_test_1.vtk", splines,0.0);
}



TEST_CASE("Test flow direction estimation 2", "[angle_correction][flow_dir]")
{
  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751_raw/US-Acq_02_20150527T125751_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150527T125751_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751_raw/US-Acq_02_20150527T125751_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;

  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) ==-1);
  }

  writeDirectionToVtkFile("output_flowdirection_test_2.vtk", splines,0.0);
}
  
TEST_CASE("Test flow direction estimation 3", "[angle_correction][flow_dir]")
{
  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026_raw/US-Acq_03_20150527T130026_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_03_20150527T130026_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_03_20150527T130026_raw/US-Acq_03_20150527T130026_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;

  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) ==-1);
  }
  writeDirectionToVtkFile("output_flowdirection_test_3.vtk", splines,0.0);
}


TEST_CASE("Test flow direction estimation 4", "[angle_correction][flow_dir]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_04_20150527T130043_raw/US-Acq_04_20150527T130043_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_04_20150527T130043_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_04_20150527T130043_raw/US-Acq_04_20150527T130043_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;

  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) ==-1);
  }
  writeDirectionToVtkFile("output_flowdirection_test_4.vtk", splines,0.0);
}


TEST_CASE("Test flow direction estimation 5", "[angle_correction][flow_dir]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_05_20150527T130229_raw/US-Acq_05_20150527T130229_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_05_20150527T130229_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_05_20150527T130229_raw/US-Acq_05_20150527T130229_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={-1,1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_5.vtk", splines,0.0);
}


TEST_CASE("Test flow direction estimation 6", "[angle_correction][flow_dir]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329_raw/US-Acq_06_20150527T130329_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_06_20150527T130329_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_06_20150527T130329_raw/US-Acq_06_20150527T130329_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={1,-1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_6.vtk", splines,0.0);
}


TEST_CASE("Test flow direction estimation 7", "[angle_correction][aliasing]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532_raw/US-Acq_07_20150527T130532_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_07_20150527T130532_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532_raw/US-Acq_07_20150527T130532_Velocity_";

  double Vnyq =  0.156;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={-1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_7.vtk", splines,0.0);
}


TEST_CASE("Test flow direction estimation 8", "[angle_correction][aliasing]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_08_20150527T130558_raw/US-Acq_08_20150527T130558_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_08_20150527T130558_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_08_20150527T130558_raw/US-Acq_08_20150527T130558_Velocity_";

  double Vnyq =  0.156;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_8.vtk", splines,0.0);
}





TEST_CASE("Test flow direction estimation 9", "[angle_correction]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_09_20150527T131009_raw/US-Acq_09_20150527T131009_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_09_20150527T131009_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_09_20150527T131009_raw/US-Acq_09_20150527T131009_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={-1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_9.vtk", splines,0.0);
}



TEST_CASE("Test flow direction estimation 10", "[angle_correction]")
{

  char toolpositions[] =  "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Angio.fp";
  char centerline[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "../Testdata/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";

  double Vnyq =  0.312;
  double cutoff = 0.0;
  int nConvolutions = 3;

  //double 0.0 = atof(argv[argidx++]);//Hide flow direction with uncertainty under given value [0,1]

  vector<Spline3D<D> > *splines = angle_correction_impl(toolpositions, centerline, image_prefix, Vnyq, cutoff, nConvolutions);

  D flow_direction;
  double true_flow_direction [2]={1};
  int k=0;
  for(auto &spline: *splines)
  {
	flow_direction = spline.getIntersections().getEstimatedDirection();
	CHECK(sgn(flow_direction) == true_flow_direction[k++]);
  }
  writeDirectionToVtkFile("output_flowdirection_test_10.vtk", splines,0.0);
}


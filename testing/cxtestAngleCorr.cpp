/**
* Tests for AngleCorrection
*
* \author Daniel Hoyer Iversen
*
*
*/

#include <vector>
#include "cxAngleCorrectionExecuter.h"

#include <vtkPolyDataReader.h>
#include <vtkPointData.h>
#include <cstdio>
#include <time.h>

#include "catch.hpp"


using namespace std;


void validateFiles(const char* filename_a,const char* filename_b, bool shouldBeEqual = true){
    vtkSmartPointer<ErrorObserver>  errorObserver =  vtkSmartPointer<ErrorObserver>::New();

    vtkSmartPointer<vtkPolyDataReader> reader2 = vtkSmartPointer<vtkPolyDataReader>::New();
    reader2->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    reader2->AddObserver(vtkCommand::WarningEvent,errorObserver);
    reader2->SetFileName(filename_b);
    reader2->Update();

    vtkSmartPointer<vtkPolyDataReader> reader1 = vtkSmartPointer<vtkPolyDataReader>::New();
    reader1->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    reader1->AddObserver(vtkCommand::WarningEvent,errorObserver);
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

    unsigned int numberRight = 0;
    unsigned int numberLeft = 0;
    double pointRight;
    double pointLeft;
    for(int k=0; k <  numberOfArraysRight; k++)
    {
        numberRight = rightHandSide->GetPointData()->GetArray(k)->GetDataSize()/ rightHandSide->GetPointData()->GetArray(k)->GetNumberOfComponents();
        numberLeft =  leftHandSide->GetPointData()->GetArray(k)->GetDataSize()/ leftHandSide->GetPointData()->GetArray(k)->GetNumberOfComponents();
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


char * appendTestFolder(const char * filename){
    char anglecorrection_test_data_dir[]=ANGLECORRECTION_TEST_DATA_DIR;
    char * newArray = new char[std::strlen(anglecorrection_test_data_dir)+std::strlen(filename)+1];
    std::strcpy(newArray,anglecorrection_test_data_dir);
    std::strcat(newArray,filename);
    
    return newArray;
}

void testFlow(char centerline[], char image_prefix[], double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist, char true_output[]){
    const char testFile[] = "/flowdirection_test_1.vtk";

    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(appendTestFolder(centerline));
    reader->Update();
    vtkSmartPointer<vtkPolyData> clData = reader->GetOutput();

    cx::AngleCorrectionExecuterPtr executer;
    executer.reset(new cx::AngleCorrectionExecuter());
    CHECK_NOTHROW(executer->setInput(clData, appendTestFolder(image_prefix), Vnyq, cutoff, nConvolutions, uncertainty_limit, minArrowDist));

    executer->execute();
    while (executer->isRunning() ){
        #ifdef WIN32
          Sleep(100);
        #else
          sleep(1);
        #endif
    }

    executer->writeDirectionToVtkFile(appendTestFolder(testFile));
    validateFiles(appendTestFolder(testFile), appendTestFolder(true_output));
    std::remove(appendTestFolder(testFile));
}



TEST_CASE("AngleCorrection: Test flow direction estimation from gui 1", "[aa][angle_correction][unit]")
    {
    char centerline[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
    char image_prefix[] = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity_";
    
    double Vnyq =  0.312;
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit= 0.0;
    double minArrowDist = 1.0;

    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_1.vtk";


    testFlow(centerline, image_prefix,  Vnyq,  cutoff,  nConvolutions,  uncertainty_limit, minArrowDist, true_output);
}


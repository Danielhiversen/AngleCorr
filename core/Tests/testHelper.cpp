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

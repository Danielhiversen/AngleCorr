
#include "catch.hpp"

#include "cxtestAngleCorr.h"

#include "cxAngleCorrectionWidget.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxMainWindow.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxMesh.h"
#include <QTimer>
#include "cxUtilHelpers.h"
#include "cxForwardDeclarations.h"

#include <vtkPolyDataReader.h>
#include <vtkPointData.h>
#include "cxtestUtilities.h"
#include "cxtestDummyDataManager.h"
#include "cxSessionStorageService.h"
#include "cxReporter.h"
#include "cxVisServices.h"



namespace cxtest
{

TestAngleCorrFixture::TestAngleCorrFixture() : mw(NULL), angleCorrWidget(NULL)
{

}

void TestAngleCorrFixture::setupInsideMainWindow()
{
    cx::DataLocations::setTestMode();

    cx::ApplicationComponentPtr mainwindow(new cx::MainWindowApplicationComponent<cx::MainWindow>());
    cx::LogicManager::initialize(mainwindow);

    mw = dynamic_cast<cx::MainWindowApplicationComponent<cx::MainWindow>*>(mainwindow.get())->mMainWindow;
    REQUIRE(mw!=NULL);


    angleCorrWidget = mw->findChild<cx::AngleCorrectionWidget*>("AngleCorrectionWidget");
    REQUIRE(angleCorrWidget!=NULL);
    angleCorrWidget->forcePrePaint(); // explicitly populate angleCorrWidget


}

void TestAngleCorrFixture::runApp(int milliseconds)
{
//    QTimer::singleShot(milliseconds, qApp, SLOT(quit()));
    qApp->exec();
}


void TestAngleCorrFixture::shutdown()
{
    cx::LogicManager::shutdown();
}

} // namespace cxtest


void validateData( vtkSmartPointer<vtkPolyData> leftHandSide,const char* filename_b, bool shouldBeEqual = true){
    vtkSmartPointer<ErrorObserver>  errorObserver =  vtkSmartPointer<ErrorObserver>::New();

    vtkSmartPointer<vtkPolyDataReader> reader2 = vtkSmartPointer<vtkPolyDataReader>::New();
    reader2->AddObserver(vtkCommand::ErrorEvent,errorObserver);
    reader2->AddObserver(vtkCommand::WarningEvent,errorObserver);
    reader2->SetFileName(filename_b);
    reader2->Update();

   bool readFilesSuccesfully = true;
   if (errorObserver->GetError() ||errorObserver->GetWarning()){
         readFilesSuccesfully = false;

   }
   REQUIRE(readFilesSuccesfully);

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


void setInput(cxtest::TestAngleCorrFixture fixture, QString centerline, QString velData, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    fixture.angleCorrWidget->setClSmoothing(nConvolutions);
    fixture.angleCorrWidget->setMaxThetaCutoff(cutoff);
    fixture.angleCorrWidget->setMinArrowDist(minArrowDist);
    fixture.angleCorrWidget->setUncertaintyLimit(uncertainty_limit);

    cx::VisServicesPtr visServices = fixture.angleCorrWidget->getVisServicesPtr();


    QString dummy;

    QString filename = cx::DataLocations::getLargeTestDataPath()+"/testing"+centerline;
    cx::DataPtr data = visServices->getPatientService()->importData(filename, dummy);
    REQUIRE(data);
    fixture.angleCorrWidget->setClData(data->getUid());
    fixture.angleCorrWidget->selectVelData(cx::DataLocations::getLargeTestDataPath()+ "/testing"+velData);


}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

TEST_CASE("anglecorr", "[cc]")
{
    REQUIRE(true);
    cxtest::TestAngleCorrFixture fixture;
    fixture.setupInsideMainWindow();
    cx::sessionStorageService()->load(cx::DataLocations::getTestDataPath()+ "/temp/angleCorr/");


    QString centerline = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150527T125751_Angio_1_tsf_cl1.vtk";
    QString velData = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150527T125751/US-Acq_02_20150527T125751_Velocity.fts";
    double cutoff = 0.18;
    int nConvolutions = 6;
    double uncertainty_limit = 0;
    double minArrowDist = 1;

    setInput(fixture, centerline, velData, cutoff, nConvolutions, uncertainty_limit, minArrowDist);
    fixture.angleCorrWidget->runAngleCorection();
//    REQUIRE(outData!=NULL);

    char true_output[] ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_tumour.vtk";

    while( fixture.angleCorrWidget->isRunning())
    {
        cx::sleep_ms(500);
    }

    cx::sleep_ms(500);

    cx::MeshPtr outData = fixture.angleCorrWidget->getOutData();

  // outData->save(cx::DataLocations::getTestDataPath()+ "/temp/angleCorr/");
   // validateData(outData->getVtkPolyData(),appendTestFolder(true_output));


    fixture.shutdown();
}

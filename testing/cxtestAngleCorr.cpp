
#include "catch.hpp"
#include "cxtestAngleCorr.h"

#include "cxAngleCorrectionWidget.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxMainWindow.h"
#include "cxMesh.h"
#include "cxSessionStorageService.h"
#include "cxtestUtilities.h"
#include "cxVisServices.h"

#include <vtkPolyDataReader.h>
#include <vtkPointData.h>
#include <QTimer>

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
    QTimer::singleShot(milliseconds, qApp, SLOT(quit()));
    qApp->exec();
}


void TestAngleCorrFixture::shutdown()
{
    cx::LogicManager::shutdown();
}

} // namespace cxtest


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
        std:cerr << "Failed to read: " << filename_a << " or " << filename_b << std::endl;
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
        else if(!shouldBeEqual && numberLeft!=numberRight){
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



void testFlow(cxtest::TestAngleCorrFixture fixture, QString centerline, QString velData, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist, double vNyq, QString true_output)
{
    fixture.angleCorrWidget->setClSmoothing(nConvolutions);
    fixture.angleCorrWidget->setMaxThetaCutoff(cutoff);
    fixture.angleCorrWidget->setMinArrowDist(minArrowDist);
    fixture.angleCorrWidget->setUncertaintyLimit(uncertainty_limit);
    fixture.angleCorrWidget->setVNyq(vNyq);


    cx::VisServicesPtr visServices = fixture.angleCorrWidget->getVisServicesPtr();


    QString dummy;

    QString filename = cx::DataLocations::getLargeTestDataPath()+"/testing"+centerline;
    cx::DataPtr data = visServices->getPatientService()->importData(filename, dummy);
    REQUIRE(data);
    CHECK_NOTHROW(fixture.angleCorrWidget->setClData(data->getUid()));
    CHECK_NOTHROW(fixture.angleCorrWidget->selectVelData(cx::DataLocations::getLargeTestDataPath()+ "/testing"+velData));

    CHECK_NOTHROW(fixture.angleCorrWidget->runAngleCorection());
    while(fixture.angleCorrWidget->isRunning())
    {
        cxtest::Utilities::sleep_sec(0.5);
    }

    fixture.runApp(300);

    if(fixture.angleCorrWidget->getOutData()==NULL) REQUIRE(false); //REQUIRE(fixture.angleCorrWidget->getOutData()!=NULL);

    CHECK_NOTHROW(fixture.angleCorrWidget->toggleDetailsSlot());

    QString outpuFilepath = visServices->getPatientService()->getActivePatientFolder() +"/"+fixture.angleCorrWidget->getOutData()->getFilename();
    validateFiles(outpuFilepath.toStdString().c_str(),appendTestFolder(true_output.toStdString().c_str()));
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

TEST_CASE("AngleCorrection: Test gui plugin with several runs", "[angle_correction][integration]")
{
    REQUIRE(true);
    cxtest::TestAngleCorrFixture fixture;
    fixture.setupInsideMainWindow();
    cx::sessionStorageService()->load(cx::DataLocations::getTestDataPath()+ "/temp/angleCorr/");


    CHECK_NOTHROW(fixture.angleCorrWidget->runAngleCorection());


    QString centerline = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_01_20150527T125724_Angio_1_tsf_cl1.vtk";
    QString velData = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_01_20150527T125724_raw/US-Acq_01_20150527T125724_Velocity.fts";
    QString true_output ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_1.vtk";
    double cutoff = 79.6302402; // = acos(0.18)
    int nConvolutions = 6;
    double uncertainty_limit = 0;
    double minArrowDist = 1;
    double vNyq = 0.312;
    CHECK_NOTHROW(testFlow(fixture, centerline, velData, cutoff, nConvolutions, uncertainty_limit, minArrowDist,vNyq,true_output ));


    centerline = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150625T105554_Angio_1_tsf_cl1.vtk";
    velData= "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150625T105554/US-Acq_02_20150625T105554_Velocity.fts";
    true_output ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_tumour.vtk";
    vNyq =  0.0;
    cutoff = 90; //acos(0)
    nConvolutions = 5;
    CHECK_NOTHROW(testFlow(fixture, centerline, velData, cutoff, nConvolutions, uncertainty_limit, minArrowDist,vNyq,true_output ));


    centerline = "/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_07_20150527T130532_Angio_1_tsf_cl1.vtk";
    velData = "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_07_20150527T130532/US-Acq_07_20150527T130532_Velocity.fts";
    true_output ="/2015-05-27_12-02_AngelCorr_tets.cx3/trueOutputAngleCorr/output_flowdirection_test_7.vtk";
    vNyq =  0.156;
    cutoff = 79.6302402; // = acos(0.18)
    nConvolutions = 6;
    CHECK_NOTHROW(testFlow(fixture, centerline, velData, cutoff, nConvolutions, uncertainty_limit, minArrowDist,vNyq,true_output ));


    fixture.shutdown();
}

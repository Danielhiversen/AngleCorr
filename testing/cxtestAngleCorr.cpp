
#include "catch.hpp"
#include "cxtestAngleCorr.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxMainWindow.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxAngleCorrectionWidget.h"
#include <QTimer>
#include "cxUtilHelpers.h"

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

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

TEST_CASE("anglecorr", "[cc]")
{
    REQUIRE(true);
    cxtest::TestAngleCorrFixture fixture;
    fixture.setupInsideMainWindow();
    fixture.angleCorrWidget->setClSmoothing(5);
    fixture.angleCorrWidget->setMaxThetaCutoff(0);
    fixture.angleCorrWidget->setMinArrowDist(1);
    fixture.angleCorrWidget->setUncertaintyLimit(0);


    fixture.angleCorrWidget->setClData(cx::DataLocations::getLargeTestDataPath()+"/2015-05-27_12-02_AngelCorr_tets.cx3/Images/US_02_20150625T105554_Angio_1_tsf_cl1.vtk");
    fixture.angleCorrWidget->selectVelData(cx::DataLocations::getLargeTestDataPath()+ "/2015-05-27_12-02_AngelCorr_tets.cx3/US_Acq/US-Acq_02_20150625T105554/US-Acq_02_20150625T105554_Velocity.fts");

    fixture.angleCorrWidget->runAngleCorection();

    cx::MeshPtr outData = fixture.angleCorrWidget->getOutData();

    fixture.shutdown();
}

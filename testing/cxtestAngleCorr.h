#ifndef CXTESTANGLECORR_H
#define CXTESTANGLECORR_H

#include "cxMessageListener.h"
#include "cxLogicManager.h"


namespace cx
{
class MainWindow;
class AngleCorrectionWidget;
}

namespace cxtest
{

class TestAngleCorrFixture
{
public:
    TestAngleCorrFixture();
    void runApp(int milliseconds = 300);
    bool logContains(QString testString);
    void clearLog();
    QString getActivePatientFolder();
    cx::DataPtr importData(QString fileName);
    void shutdown();

    cx::MainWindow* mw;
    cx::AngleCorrectionWidget* angleCorrWidget;
    cx::MessageListenerPtr logger;
};

}

#endif // CXTESTANGLECORR_H




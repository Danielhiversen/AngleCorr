#ifndef CXTESTANGLECORR_H
#define CXTESTANGLECORR_H

#include "cxMessageListener.h"


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
    void setupInsideMainWindow();
    void runApp(int milliseconds = 300);
    bool logContains(QString testString);
    void clearLog();
    void shutdown();

    cx::MainWindow* mw;
    cx::AngleCorrectionWidget* angleCorrWidget;
    cx::MessageListenerPtr listener;
};

}

#endif // CXTESTANGLECORR_H




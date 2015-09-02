#ifndef CXTESTANGLECORR_H
#define CXTESTANGLECORR_H

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
    void shutdown();

    cx::MainWindow* mw;
    cx::AngleCorrectionWidget* angleCorrWidget;
};

}

#endif // CXTESTANGLECORR_H




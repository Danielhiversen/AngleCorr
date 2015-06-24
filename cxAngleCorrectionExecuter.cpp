#include "cxAngleCorrectionExecuter.h"
#include "cxLogger.h"

#include "angle_correction_impl.h"
namespace cx
{

/**
 *
 * \ingroup org_custusx_AngleCorrection
 *
 * \date 2015-06-14
 * \author Daniel Hoyer Iversen
 */

AngleCorrectionExecuter::AngleCorrectionExecuter() :
    ThreadedTimedAlgorithm<bool>("Angle correction", 5)
{
    mOutput = NULL;
    mUseDefaultMessages = false;
    mClData=NULL;
    mDataFilename="";
    mVnyq=0;
    mCutoff=0;
    mnConvolutions=0;
    mUncertainty_limit=0;
    mMinArrowDist=0;
    mUpdate1 = true;
    mUpdate2 = true;
    mValidInput= false;
}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
}

void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    //mDataFilename.toStdString().c_str()
}

vtkSmartPointer<vtkPolyData>  AngleCorrectionExecuter::getOutput()
{
    if(!this->isFinished()) return NULL;
    if(!this->getResult()) return NULL;

    return mOutput;
}




} /* namespace cx */

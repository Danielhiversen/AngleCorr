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
    mClSplines = NULL;
    mUpdate1 = true;
    mUpdate2 = true;
    mValidInput= false;
}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
    mClSplines->clear();
}


void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    if(mClData!=clData ||
            mDataFilename!=dataFilename ||
            mVnyq!=Vnyq ||
            mCutoff!=cutoff ||
            mnConvolutions!=nConvolutions)
    {
        mClData=clData;
        mDataFilename=dataFilename;
        mVnyq=Vnyq;
        mCutoff=cutoff;
        mnConvolutions=nConvolutions;
        mUpdate1=true;
    }

    if(mUncertainty_limit!=uncertainty_limit ||
            mMinArrowDist!=minArrowDist)
    {
        mUncertainty_limit=uncertainty_limit;
        mMinArrowDist=minArrowDist;
        mUpdate2=true;
    }
    mValidInput= true;

}

void AngleCorrectionExecuter::postProcessingSlot()
{
    mUpdate1=false;
    mUpdate2=false;
    mValidInput=false;
}

bool AngleCorrectionExecuter::calculate()
{
    if(!mValidInput) return false;
    report(QString("Algorithm Angle correction started"));

    if(mUpdate1)
    {
        try {
            if(mClSplines!=NULL) mClSplines->clear();
            mClSplines = angle_correction_impl(mClData, mDataFilename.toStdString().c_str(), mVnyq, mCutoff, mnConvolutions);
        } catch (std::exception& e){
            reportError("std::exception in angle correction algorithm  step 1: "+qstring_cast(e.what()));
            return false;
        } catch (...){
            reportError("Angle correction algorithm threw a unknown exception in step 1.");
            return false;
        }
    }
    report(QString("Finished step 1 of 2 for angle correction"));

    if(mUpdate2 || mUpdate2)
    {
        try {
            mOutput= flowDirection(mClSplines, mUncertainty_limit, mMinArrowDist);
        } catch (std::exception& e){
            reportError("std::exception in angle correction algorithm  step 2: "+qstring_cast(e.what()));
            return false;
        } catch (...){
            reportError("Angle correction algorithm threw a unknown exception in step 2.");
            return false;
        }
    }

    reportSuccess(QString("Algorithm Angle correction complete [%1s]").arg(this->getSecondsPassedAsString()));
    return true;
}


vtkSmartPointer<vtkPolyData>  AngleCorrectionExecuter::getOutput()
{
    if(!this->isFinished()) return NULL;
    if(!this->getResult()) return NULL;

    return mOutput;
}




} /* namespace cx */

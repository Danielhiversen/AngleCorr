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
}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
   // mClSplines->clear();
}


void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
  mClData=clData;
  mDataFilename=dataFilename;
  mVnyq=Vnyq;
  mCutoff=cutoff;
  mnConvolutions=nConvolutions;
  mUncertainty_limit=uncertainty_limit;
  mMinArrowDist=minArrowDist;
}

void AngleCorrectionExecuter::postProcessingSlot()
{

}

bool AngleCorrectionExecuter::calculate()
{
  
    report(QString("Algorithm Angle correction started"));
    vectorSpline3dDouble *mClSplines;
    try {    
        mClSplines = angle_correction_impl(mClData, mDataFilename.toStdString().c_str(), mVnyq, mCutoff, mnConvolutions);
    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 1: "+qstring_cast(e.what()));
        return false;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 1.");
        return false;
    }
    report(QString("Finished step 1 of 2 for angle correction"));

    try {    
         mOutput= flowDirection(mClSplines, mUncertainty_limit, mMinArrowDist);
    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 2: "+qstring_cast(e.what()));
        return false;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 2.");
        return false;
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

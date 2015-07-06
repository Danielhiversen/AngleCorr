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
    ThreadedTimedAlgorithm<bool>("Angle correction", 5),
    AngleCorrection()
{
  //  mAngleCorrPtr=new AngleCorrection();
    mUseDefaultMessages = false;
}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
    //angleCorr.~AngleCorrection();
}

void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    try {
        AngleCorrection::setInput(clData,  dataFilename.toStdString().c_str(),  Vnyq,  cutoff,  nConvolutions,  uncertainty_limit,  minArrowDist);
    } catch (std::exception& e){
        reportError("std::exception in angle correction algorithm during setting parameters: "+qstring_cast(e.what()));
    } catch (...){
        reportError("Angle correction algorithm threw a unknown exception during setting parameters.");
    }
}


bool AngleCorrectionExecuter::calculate()
{
    report(QString("Algorithm Angle correction started."));
    bool res= false;
    try {
        res=AngleCorrection::calculate();
    } catch (std::exception& e){
        reportError("std::exception in angle correction algorithm: "+qstring_cast(e.what()));
    } catch (...){
        reportError("Angle correction algorithm threw a unknown exception.");
    }
    if(res){
        reportSuccess(QString("Algorithm Angle correction complete [%1s].").arg(this->getSecondsPassedAsString()));
    }else{
        QString text =QString("Algorithm Angle correction failed [%1s].").arg(this->getSecondsPassedAsString());
        if(getBloodVessels() <1) text.append("\n Found %1 blood vessels. Maybe <<Max angle cut off>> should be lower?").arg(QString(getBloodVessels()));
        if(getIntersections() <1) text.append("\n Found %1 blood vessels. Maybe <<Max angle cut off>> should be lower?").arg(QString(getIntersections()));
        reportError(text);
    }
    return res;
}


void AngleCorrectionExecuter::postProcessingSlot()
{
}

vtkSmartPointer<vtkPolyData>  AngleCorrectionExecuter::getOutput()
{
    if(!this->isFinished()) return NULL;
    if(!this->getResult()) return NULL;
    return AngleCorrection::getOutput();
}


} /* namespace cx */

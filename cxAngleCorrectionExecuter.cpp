#include "cxAngleCorrectionExecuter.h"
#include "cxLogger.h"

#include "cxForwardDeclarations.h"

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
    mUseDefaultMessages = false;
}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
}

void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    try {
        AngleCorrection::setInput(clData,  dataFilename.toStdString().c_str(),  Vnyq,  cutoff,  nConvolutions, uncertainty_limit, minArrowDist);
    } catch (std::exception& e){
        reportError("std::exception in angle correction algorithm during setting parameters: "+qstring_cast(e.what()));
    } catch (...){
        reportError("Angle correction algorithm threw a unknown exception during setting parameters.");
    }
}


bool AngleCorrectionExecuter::calculate(bool reportOutSuccess)
{
    if(reportOutSuccess) report(QString("Algorithm Angle correction started."));
    bool res= false;
    try {
        res=AngleCorrection::calculate();
    } catch (std::exception& e){
        reportError("std::exception in angle correction algorithm: "+qstring_cast(e.what()));
    } catch (...){
        reportError("Angle correction algorithm threw a unknown exception.");
    }
    if(res){
        QString text =QString("Algorithm Angle correction complete [%1s].").arg(this->getSecondsPassedAsString());
        if(getBloodVessels() <1) text.append(QString("\n Found %1 blood vessels. Maybe <<Velocity certainty cut off>> should be higher?").arg(getBloodVessels()));
        if(getIntersections() <1) text.append(QString("\n Found %1 interesections. Maybe <<Max angle cut off>> should be lower?").arg(getIntersections()));
        if(getNumOfStepsRan() <1) text.append("\n Same input as previous. No new data generated.");
        if(reportOutSuccess) reportSuccess(text);
    }else{
        QString text =QString("Algorithm Angle correction failed [%1s].").arg(this->getSecondsPassedAsString());
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

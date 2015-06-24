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
    ThreadedTimedAlgorithm<bool>("Angle correction", 5), AngleCorrection()
{

}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{
}

void AngleCorrectionExecuter::setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist)
{
    AngleCorrection::setInput(clData,  dataFilename.toStdString().c_str(),  Vnyq,  cutoff,  nConvolutions,  uncertainty_limit,  minArrowDist);
}


bool AngleCorrectionExecuter::calculate()
{
     return  AngleCorrection::calculate();
}


void AngleCorrectionExecuter::postProcessingSlot()
{

}
vtkSmartPointer<vtkPolyData>  AngleCorrectionExecuter::getOutput()
{
    if(!this->isFinished()) return NULL;
    if(!this->getResult()) return NULL;

    return  AngleCorrection::getOutput();
}


} /* namespace cx */

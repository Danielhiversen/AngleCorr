#ifndef CXANGLECORRECTIONEXECUTER_H
#define CXANGLECORRECTIONEXECUTER_H

#include "cxMathBase.h"
#include "org_custusx_anglecorrection_Export.h"
#include "cxThreadedTimedAlgorithm.h"

#include "AngleCorrection.h"

namespace cx
{

/**
 *
 * \ingroup org_custusx_AngleCorrection
 *
 * \date 2015-06-14
 * \author Daniel Hoyer Iversen
 */


typedef boost::shared_ptr<class AngleCorrectionExecuter> AngleCorrectionExecuterPtr;


class org_custusx_anglecorrection_EXPORT AngleCorrectionExecuter : public ThreadedTimedAlgorithm<bool>, private AngleCorrection
{
  Q_OBJECT
public:
  AngleCorrectionExecuter();
  virtual ~AngleCorrectionExecuter();
  void setInput(QString clFilename, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist);
  vtkSmartPointer<vtkPolyData> getOutput();
  int getNumOfStepsRan(){return AngleCorrection::getNumOfStepsRan();}
  virtual bool calculate(bool reportOutSuccess);
  virtual bool calculate(){return calculate(true);}
private slots:
  virtual void postProcessingSlot();

private:

};

}//namespace
#endif /* CXANGLECORRECTIONEXECUTER_H */

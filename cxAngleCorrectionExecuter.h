#ifndef CXANGLECORRECTIONEXECUTER_H
#define CXANGLECORRECTIONEXECUTER_H

#include "org_custusx_anglecorrection_Export.h"

#include "cxForwardDeclarations.h"
#include "cxThreadedTimedAlgorithm.h"

#include "cxMathBase.h"
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

class org_custusx_anglecorrection_EXPORT AngleCorrectionExecuter : public ThreadedTimedAlgorithm<bool>
{
  Q_OBJECT
public:
  AngleCorrectionExecuter();
  virtual ~AngleCorrectionExecuter();
  void setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist);
  vtkSmartPointer<vtkPolyData> getOutput();

private slots:
  virtual void postProcessingSlot();

private:
  virtual bool calculate();

  vtkSmartPointer<vtkPolyData> mClData;
  QString mDataFilename;
  double mVnyq;
  double mCutoff;
  int mnConvolutions;
  double mUncertainty_limit;
  double mMinArrowDist;

  vtkSmartPointer<vtkPolyData> mOutput;

  vectorSpline3dDouble *mClSplines;
  bool mUpdate1;
  bool mUpdate2;
  bool mValidInput;
};

}//namespace
#endif /* CXANGLECORRECTIONEXECUTER_H */

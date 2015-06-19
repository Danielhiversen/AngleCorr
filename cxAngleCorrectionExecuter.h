//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * \ingroup org_custusx_AngleCorrection
 *
 * \date 2015-06-14
 * \author Daniel Hoyer Iversen
 */
#ifndef CXANGLECORRECTIONEXECUTER_H_
#define CXANGLECORRECTIONEXECUTER_H_

#include "cxResourceExport.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "cxTimedAlgorithm.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

namespace cx
{

class AngleCorrectionExecuter : public ThreadedTimedAlgorithm<QString>
{
  Q_OBJECT
public:
  AngleCorrectionExecuter();
  void setInput(vtkSmartPointer<vtkPolyData> clData, QString dataFilename, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit, double minArrowDist);
  virtual ~AngleCorrectionExecuter();
  vtkSmartPointer<vtkPolyData> getOutput();

private slots:
  virtual void postProcessingSlot();

private:
  virtual QString calculate();
  vtkSmartPointer<vtkPolyData> mClData;
  QString mDataFilename;
  double mVnyq;
  double mCutoff;
  int mnConvolutions;
  double mUncertainty_limit;
  double mMinArrowDist;
};

}//namespace


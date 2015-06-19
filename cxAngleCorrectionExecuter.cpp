//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * \ingroup org_custusx_AngleCorrection
 *
 * \date 2015-06-14
 * \author Daniel Hoyer Iversen
 */

AngleCorrectionExecuter::AngleCorrectionExecuter() :
    ThreadedTimedAlgorithm<QString>("Angle correction", 1)
{

}

AngleCorrectionExecuter::~AngleCorrectionExecuter()
{}


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
  emit finished();
}

vtkSmartPointer<vtkPolyData> AngleCorrectionExecuter::calculate()
{
  
    vector<Spline3D<D> > *mClSplines;
    try {    
        mClSplines = angle_correction_impl(mClData, mDataFilename.toStdString().c_str(), mVnyq, mCutoff, mnConvolutions);

    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 1:"+qstring_cast(e.what()));
        return false;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 1.");
        return false;
    }


    vtkSmartPointer<vtkPolyData> output;
    try {    
         output= flowDirection(mClSplines, mUncertainty_limit, mMinArrowDist);
    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 2:"+qstring_cast(e.what()));
        return false;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 2.");
        return false;
    }
}

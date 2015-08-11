#ifndef ANGLE_CORRECTION_IMPL_H
#define ANGLE_CORRECTION_IMPL_H

#include "spline3d.hpp"


typedef vector<Spline3D<double> > vectorSpline3dDouble;
typedef vectorSpline3dDouble* vectorSpline3dDoublePtr;



class AngleCorrection
{
public:
    AngleCorrection();
    ~AngleCorrection();
    void setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, const  char* image_prefix , double Vnyq, double cutoff,  int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    void setInput(const char* centerline,const char* image_prefix, double Vnyq, double cutoff,int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    bool calculate();
    vtkSmartPointer<vtkPolyData> getOutput();
    vectorSpline3dDouble getClSpline();
    void writeDirectionToVtkFile(const char* filename);

    int getIntersections(){return mIntersections;}
    int getBloodVessels(){return mBloodVessels;}
    int getNumOfStepsRan(){return mNumOfStepsRan;}
private:
    // not tested as public:
    void setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, vector<MetaImage<inData_t> >* velData, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    void angle_correction_impl(vtkSmartPointer<vtkPolyData> vpd_centerline, vector<MetaImage<inData_t> >* images , double Vnyq, double cutoff,  int nConvolutions);
    vtkSmartPointer<vtkPolyData> computeVtkPolyData( vectorSpline3dDoublePtr splines, double uncertainty_limit, double minArrowDist);
    bool EqualVtkPolyData( vtkSmartPointer<vtkPolyData> leftHandSide, vtkSmartPointer<vtkPolyData> rightHandSide);

    vtkSmartPointer<vtkPolyData> mClData;
    vector<MetaImage<inData_t> > * mVelDataPtr;
    std::string mVelImagePrefix;
    double mVnyq;
    double mCutoff;
    int mnConvolutions;
    double mUncertainty_limit;
    double mMinArrowDist;

    vtkSmartPointer<vtkPolyData> mOutput;

    vectorSpline3dDoublePtr mClSplinesPtr;
    bool mUpdate1;
    bool mUpdate2;
    bool mValidInput;

    int mIntersections;
    int mBloodVessels;
    int mNumOfStepsRan;

};
#endif /* ANGLE_CORRECTION_IMPL_H */

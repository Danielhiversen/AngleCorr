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
    void setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, vector<MetaImage<inData_t> >* velData, double Vnyq, double cutoff, int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    void setInput(vtkSmartPointer<vtkPolyData> vpd_centerline, const  char* image_prefix , double Vnyq, double cutoff,  int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    void setInput(const char* centerline,const char* image_prefix, double Vnyq, double cutoff,int nConvolutions, double uncertainty_limit=0.0, double minArrowDist= 1.0);
    bool calculate();
    vtkSmartPointer<vtkPolyData> getOutput();
    vectorSpline3dDouble getClSpline();
    void writeDirectionToVtkFile(const char* filename);
private:
    vtkSmartPointer<vtkPolyData> flowDirection( vectorSpline3dDoublePtr splines, double uncertainty_limit, double minArrowDist);
    vectorSpline3dDoublePtr angle_correction_impl(vtkPolyData *vpd_centerline, vector<MetaImage<inData_t> >* images , double Vnyq, double cutoff,  int nConvolutions);

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
};
#endif /* ANGLE_CORRECTION_IMPL_H */

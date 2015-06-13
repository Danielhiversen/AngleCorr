/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxAngleCorrectionWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include "Exceptions.hpp"

#include "cxLogger.h"
#include "cxPatientModelServiceProxy.h"
#include "cxAcquisitionServiceProxy.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"
#include "cxSettings.h"
#include "cxDoubleProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxTime.h"
#include "cxMesh.h"


#include <vector>
#include "angle_correction_impl.cpp"
#include <vtkPolyDataWriter.h>
#include <lib/writeToFile.cpp>

///

namespace cx
{

AngleCorrectionWidget::AngleCorrectionWidget(VisServicesPtr visServices, AcquisitionServicePtr acquisitionService, QWidget* parent) :
    BaseWidget(parent, "AngleCorrectionWidget", "Angle Correction"),
    mVerticalLayout(new QVBoxLayout(this)),
    mVelFileSelectWidget( new FileSelectWidget(this)),
    mClFileSelectWidget( new FileSelectWidget(this)),
    mVisServices(visServices)
{
//    mVisServices = visServices;
    mAcquisitionService = acquisitionService;
    mSettings = profile()->getXmlSettings().descend("angelCorr");

    connect(mVisServices->getPatientService().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
    connect(mAcquisitionService.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(selectVelData(QString)));
    this->setWhatsThis(this->defaultWhatsThis());

    QLabel* velLabel = new QLabel("Select velocity data:");
    mVerticalLayout->addWidget(velLabel);
	connect(mVelFileSelectWidget, &FileSelectWidget::fileSelected, this,&AngleCorrectionWidget::selectVelData);
	mVelFileSelectWidget->setNameFilter(QStringList() << "*_Velocity.fts");
    mVerticalLayout->addWidget(mVelFileSelectWidget);

    QLabel* clLabel = new QLabel("Select centerline:");
    mVerticalLayout->addWidget(clLabel);
	connect(mClFileSelectWidget, &FileSelectWidget::fileSelected, this,&AngleCorrectionWidget::selectClData);
	mClFileSelectWidget->setNameFilter(QStringList() << "*_tsf_cl*.vtk");
    mVerticalLayout->addWidget(mClFileSelectWidget);    


    mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("AngleCorr/AngleCorrShowDetails").toBool());
    mVerticalLayout->addWidget(mOptionsWidget);    

	this->createAction(this,
		  QIcon(":/icons/open_icon_library/system-run-5.png"),
		  "Details", "Show angle correction settings",
		  SLOT(toggleDetailsSlot()),
		  mVerticalLayout);


    mRunAngleCorrButton = new QPushButton("Angle correct", this);
	connect(mRunAngleCorrButton, &QPushButton::clicked, this, &AngleCorrectionWidget::runAngleCorection);
	mVerticalLayout->addWidget(mRunAngleCorrButton);


	mVerticalLayout->addStretch();

    this->patientChangedSlot();
    mOptionsWidget->setVisible(false);

}

AngleCorrectionWidget::~AngleCorrectionWidget()
{
}

QString AngleCorrectionWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>AngleCorrection plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}

void AngleCorrectionWidget::patientChangedSlot()
{
    QString dataFilename = mVisServices->getPatientService()->getActivePatientFolder() + "/US_Acq/";
    mVelFileSelectWidget->setPath(dataFilename);

    QString clFilename = mVisServices->getPatientService()->getActivePatientFolder() + "/Images/";
    mClFileSelectWidget->setPath(clFilename);
}


void AngleCorrectionWidget::selectVelData(QString filename)
{
	if (filename.isEmpty())
	{
		reportWarning("no velocity file selected");
		return;
	}

	  mVelFileSelectWidget->setFilename(filename);
}


void AngleCorrectionWidget::selectClData(QString filename)
{
	if (filename.isEmpty())
	{
		reportWarning("no centerline file selected");
		return;
	}

	  mClFileSelectWidget->setFilename(filename);
}


void AngleCorrectionWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("AngleCorr/AngleCorrShowDetails", mOptionsWidget->isVisible());
}

QWidget* AngleCorrectionWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 3);
	++line;

    layout->addWidget(new QLabel("Centerline smoothing:", this), line, 0);
    mClSmoothing = DoubleProperty::initialize("ClSmoothing", " ", "Smoothing of the centerline", 5, DoubleRange(0, 100, 1), 0, mSettings.getElement());
    mClSmoothing->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    layout->addWidget(createDataWidget(mVisServices->visualizationService, mVisServices->patientModelService, this, mClSmoothing), line, 1);
	++line;

    layout->addWidget(new QLabel("Max angle cut off [deg]:", this), line, 0);
    mMaxThetaCutoff = DoubleProperty::initialize("maxThetaCutoff", " ", "Data from steeper angle will be ignored", 70.0, DoubleRange(0, 90, 1), 0, mSettings.getElement());
    mMaxThetaCutoff->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    layout->addWidget(createDataWidget(mVisServices->visualizationService, mVisServices->patientModelService, this, mMaxThetaCutoff), line, 1);
	++line;

    layout->addWidget(new QLabel("Velocity certainty cut off:", this), line, 0);    
    mUncertaintyLimit = DoubleProperty::initialize("uncertaintyLimit", " ", "Semgents with lower certainty will be ignored", 0.0, DoubleRange(0, 1, 0.1), 1, mSettings.getElement());
    mUncertaintyLimit->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    layout->addWidget(createDataWidget(mVisServices->visualizationService, mVisServices->patientModelService, this, mUncertaintyLimit), line, 1);
	++line;

    layout->addWidget(new QLabel("Min arrow[mm]:", this), line, 0);
    mMinArrowDist = DoubleProperty::initialize("minArrowDist", " ", "Min dist between visualization arrows [mm]", 0.3, DoubleRange(0, 1, 0.1), 1, mSettings.getElement());
    mMinArrowDist->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    layout->addWidget(createDataWidget(mVisServices->visualizationService, mVisServices->patientModelService, this, mMinArrowDist), line, 1);
	++line;

	return retval;
}

void AngleCorrectionWidget::runAngleCorection()
{

    QString dataFilename = mVelFileSelectWidget->getFilename();
    if(dataFilename.length() ==0){
        reportError("No velocity data selected");
        return;
    }
    dataFilename.replace(".fts","_");


    QString clFilename = mClFileSelectWidget->getFilename();
    if(clFilename.length() ==0){
        reportError("No centerline selected");
        return;
    }
    double Vnyq = 0.0;
    double cutoff = cos(mMaxThetaCutoff->getValue()/180.0*M_PI);
    int nConvolutions = (int) mClSmoothing->getValue();
    double uncertainty_limit = mUncertaintyLimit->getValue();
    double minArrowDist = mMinArrowDist->getValue();

    try {    
        report(dataFilename);
        report(clFilename);
        //.toStdString()
        //EstimateAngleCorrectedFlowDirection(const char* centerline,const  char* image_prefix, double Vnyq, double cutoff,  int nConvolutions, double uncertainty_limit, double minArrowDist)7
    	//vector<Spline3D<D> > *mVelSplines = angle_correction_impl(centerline, image_prefix , Vnyq, cutoff, nConvolutions);

    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 1:"+qstring_cast(e.what()));
        return;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 1.");
        return;
    }


   QString outputFilename = mVisServices->getPatientService()->getActivePatientFolder() + "/Images/"+QFileInfo(dataFilename).baseName()
                            +QFileInfo(clFilename).baseName()+"_"+QDateTime::currentDateTime().toString(timestampSecondsFormat())+"_angleCorr.vtk";
    try {    
        report(outputFilename);
    	//	outputCenterline->setVtkPolyData(flowDirection( mVelSplines, uncertainty_limit,minArrowDist));
        sleep(4);
    } catch (std::exception& e){
		reportError("std::exception in angle correction algorithm  step 2:"+qstring_cast(e.what()));
        return;
    } catch (...){
		reportError("Angle correction algorithm threw a unknown exception in step 2.");
        return;
    }


    //Display data in cx

  char centerline[] = "/home/dahoiv/Dokumenter/postDoc/AngleCorr/Testdata/2015-05-27_12-02_AngelCorr_tets (copy).cx3/Images/US_10_20150527T131055_Angio_1_tsf_cl1.vtk";
  char image_prefix[] = "/home/dahoiv/Dokumenter/postDoc/AngleCorr/Testdata/2015-05-27_12-02_AngelCorr_tets (copy).cx3/US_Acq/US-Acq_10_20150527T131055_raw/US-Acq_10_20150527T131055_Velocity_";

  Vnyq =  0.312;
  cutoff = 0.18;
  nConvolutions = 6;
  vector<Spline3D<D> > *splines = angle_correction_impl(centerline, image_prefix, Vnyq, cutoff, nConvolutions);


  const char testFile[] = "output_flowdirection_test_10.vtk";
  //writeDirectionToVtkFile(testFile, splines,0.0);




	reportSuccess(QString("Completed angle correction"));
}



} /* namespace cx */

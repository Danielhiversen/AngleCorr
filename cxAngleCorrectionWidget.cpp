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
#include "cxDoubleProperty.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"
#include "cxMesh.h"
#include "cxProfile.h"
#include "cxRegistrationTransform.h"
#include "cxSelectDataStringProperty.h"
#include "cxSettings.h"
#include "cxTime.h"
#include "cxViewGroupData.h"
#include "cxViewService.h"
#include "cxVisServices.h"
#include "Exceptions.hpp"
#include <QDir>
#include <QLabel>
#include <QVBoxLayout>




///

namespace cx
{

AngleCorrectionWidget::AngleCorrectionWidget(VisServicesPtr visServices, QWidget* parent) :
    BaseWidget(parent, "AngleCorrectionWidget", "Angle Correction"),
    mVerticalLayout(new QVBoxLayout(this)),
    mVelFileSelectWidget( new FileSelectWidget(this)),
    mVisServices(visServices)
{
    mSettings = profile()->getXmlSettings().descend("angleCorr");
    connect(mVisServices->patient().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
    this->setWhatsThis(this->defaultWhatsThis());
	
    mClDataSelectWidget =   StringPropertySelectMesh::New(mVisServices->patient());
    mClDataSelectWidget->setUidRegexp("tsf_cl(?!.*angleCorr).*"); 
    mClDataSelectWidget->setValueName("Centerline: ");
    mVerticalLayout->addWidget(new DataSelectWidget(mVisServices->view(), mVisServices->patient(), this, mClDataSelectWidget));
    connect(mClDataSelectWidget.get(), SIGNAL(changed()),          this, SLOT(cLDataChangedSlot()));
    connect(mClDataSelectWidget.get(), SIGNAL(changed()),          this, SLOT(step1ParamChangedSlot()));


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

    mOutDataSelectWidget =   StringPropertySelectMesh::New(mVisServices->patient());
    mOutDataSelectWidget->setUidRegexp("angleCorr"); 
	mOutDataSelectWidget->setValueName("Output: ");
    mVerticalLayout->addWidget(new DataSelectWidget(mVisServices->view(), mVisServices->patient(), this, mOutDataSelectWidget));

    mExecuter.reset(new AngleCorrectionExecuter());
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinished()));
	connect(mExecuter.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessExecuter()));
    mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mExecuter);
	mVerticalLayout->addWidget(mTimedAlgorithmProgressBar);

	mVerticalLayout->addStretch();

    this->patientChangedSlot();
    this->cLDataChangedSlot();
    mOptionsWidget->setVisible(false);

    mUid="";
    mName="";
    mStep1ParamChanged=true;

    mVNyq = 0.0;

}

AngleCorrectionWidget::~AngleCorrectionWidget()
{

}

QString AngleCorrectionWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>AngleCorrection plugin.</h3>"
      "<p>Angle correct velocities from ultrasound acquisitons</p>"
      "</html>";
}

void AngleCorrectionWidget::patientChangedSlot()
{
    QString dataFilename = mVisServices->patient()->getActivePatientFolder() + "/US_Acq/";
    mVelFileSelectWidget->setPath(dataFilename);
}

void AngleCorrectionWidget::cLDataChangedSlot()
{
    if(!mClDataSelectWidget->getMesh()){
        return;
    }
    QString clUid = mClDataSelectWidget->getMesh()->getUid().section("_",1,2 );
    QStringList files=mVelFileSelectWidget->getAllFiles();
    for (int i = 0; i < files.size(); ++i)
    {
        if(files.at(i).contains(clUid))
        {
        	  mVelFileSelectWidget->setFilename(files.at(i));
              return;
        }
    }

    // No velocity data found => open advanced settings
    mOptionsWidget->setVisible(true);
}


void AngleCorrectionWidget::step1ParamChangedSlot()
{
    mStep1ParamChanged=true;
}

void AngleCorrectionWidget::step2ParamChangedSlot()
{
    if(mStep1ParamChanged) return;
    setInput();
    if(!mExecuter->calculate(false)) return;
    vtkSmartPointer<vtkPolyData> output = mExecuter->getOutput();
    mOutData->setVtkPolyData(output);
    report(QString("Angle correction updated"));
}

void AngleCorrectionWidget::selectVelData(QString filename)
{
    if (filename.isEmpty())
    {
        reportWarning("No velocity file selected");
        return;
    }
    mVelFileSelectWidget->setFilename(filename);
    step1ParamChangedSlot();
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

    QLabel* velLabel = new QLabel("Velocity data:");
    layout->addWidget(velLabel,line,0);
	connect(mVelFileSelectWidget, &FileSelectWidget::fileSelected, this,&AngleCorrectionWidget::selectVelData);
  //  connect(mVelFileSelectWidget, &FileSelectWidget::fileSelected, this,SLOT(Step1ParamChangedSlot()));

	mVelFileSelectWidget->setNameFilter(QStringList() << "*Velocity.fts");
    layout->addWidget(mVelFileSelectWidget,line,1);
	++line;

    layout->addWidget(new QLabel("Centerline smoothing:", this), line, 0);
    mClSmoothing = DoubleProperty::initialize("ClSmoothing", " ", "Smoothing of the centerline", 5, DoubleRange(0, 100, 1), 0, mSettings.getElement());
    mClSmoothing->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    connect(mClSmoothing.get(), SIGNAL(changed()),          this, SLOT(step1ParamChangedSlot()));
    layout->addWidget(createDataWidget(mVisServices->view(), mVisServices->patient(), this, mClSmoothing), line, 1);
	++line;

    layout->addWidget(new QLabel("Max angle cut off [deg]:", this), line, 0);
    mMaxThetaCutoff = DoubleProperty::initialize("maxThetaCutoff", " ", "Data from steeper angle will be ignored", 70.0, DoubleRange(0, 90, 1), 0, mSettings.getElement());
    mMaxThetaCutoff->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    connect(mMaxThetaCutoff.get(), SIGNAL(changed()),          this, SLOT(step1ParamChangedSlot()));
    layout->addWidget(createDataWidget(mVisServices->view(), mVisServices->patient(), this, mMaxThetaCutoff), line, 1);
	++line;

    layout->addWidget(new QLabel("FLow direction certainty cut off:", this), line, 0);
    mUncertaintyLimit = DoubleProperty::initialize("uncertaintyLimit", " ", "Semgents with lower certainty will be ignored", 0.0, DoubleRange(0, 1, 0.1), 1, mSettings.getElement());
    mUncertaintyLimit->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    connect(mUncertaintyLimit.get(), SIGNAL(changed()),          this, SLOT(step2ParamChangedSlot()));
    layout->addWidget(createDataWidget(mVisServices->view(), mVisServices->patient(), this, mUncertaintyLimit), line, 1);
	++line;

    layout->addWidget(new QLabel("Min arrow dist. [mm]:", this), line, 0);
    mMinArrowDist = DoubleProperty::initialize("minArrowDist", " ", "Min dist between visualization arrows [mm]", 0.3, DoubleRange(0, 10, 0.1), 1, mSettings.getElement());
    mMinArrowDist->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    connect(mMinArrowDist.get(), SIGNAL(changed()),          this, SLOT(step2ParamChangedSlot()));
    layout->addWidget(createDataWidget(mVisServices->view(), mVisServices->patient(), this, mMinArrowDist), line, 1);
	++line;

	return retval;
}

MeshPtr AngleCorrectionWidget::getOutData() const
{
    return mOutData;

}

void AngleCorrectionWidget::setMinArrowDist(double value)
{
    mMinArrowDist->setValue(value);
}

void AngleCorrectionWidget::setUncertaintyLimit(double value)
{
    mUncertaintyLimit->setValue(value);
}

void AngleCorrectionWidget::setMaxThetaCutoff(double value)
{
    mMaxThetaCutoff->setValue(value);
}

void AngleCorrectionWidget::setClSmoothing(double value)
{
    mClSmoothing->setValue(value);
}

void AngleCorrectionWidget::setVNyq(double value)
{
    mVNyq = value;
}

void AngleCorrectionWidget::setClData(QString value)
{
    mClDataSelectWidget->setValue(value);
}

bool AngleCorrectionWidget::isRunning(){
	if(mExecuter)
	{
		return mExecuter->isRunning();
	}
	return false;
}

void AngleCorrectionWidget::setInput()
{
    if(!mClDataSelectWidget->getMesh()){
        reportError("No centerline selected");
        return;
    }
    QString clFilename =QDir(mVisServices->patient()->getActivePatientFolder()).filePath(mClDataSelectWidget->getMesh()->getFilename());

    QString dataFilename = mVelFileSelectWidget->getFilename();
    if(dataFilename.length() ==0){
        reportError("No velocity data selected");
         mOptionsWidget->setVisible(true);
        return;
    }
    dataFilename.replace(".fts","_");
    double cutoff = cos(mMaxThetaCutoff->getValue()/180.0*M_PI);
    int nConvolutions = (int) mClSmoothing->getValue();
    double uncertainty_limit = mUncertaintyLimit->getValue();
    double minArrowDist = mMinArrowDist->getValue();

    mExecuter->setInput(clFilename, dataFilename, mVNyq, cutoff, nConvolutions, uncertainty_limit, minArrowDist);
}

void AngleCorrectionWidget::preprocessExecuter()
{
    setInput();
    mRunAngleCorrButton->setEnabled(false);
}

void AngleCorrectionWidget::runAngleCorection()
{
    mExecuter->execute();
}

void AngleCorrectionWidget::executionFinished()
{
    mRunAngleCorrButton->setEnabled(true);
    vtkSmartPointer<vtkPolyData> output = mExecuter->getOutput();
    if(output==NULL)
    {
        reportError("Invalid output from anglecorrection algorithm");
        return;
    }
    if (mExecuter->getNumOfStepsRan() > 1)
    {
        mUid = mClDataSelectWidget->getMesh()->getUid() + "_angleCorr%1";
        mName = mClDataSelectWidget->getMesh()->getName()+" angleCorr%1";
        mOutData = mVisServices->patient()->createSpecificData<Mesh>(mUid, mName);
        mOutData->get_rMd_History()->setParentSpace(mClDataSelectWidget->getMesh()->getUid());
    }

	mOutData->setVtkPolyData(output);

    mVisServices->patient()->insertData(mOutData);
    mVisServices->view()->autoShowData(mOutData);
    mOutDataSelectWidget->setValue(mOutData->getUid());
    mStep1ParamChanged=false;
}

} /* namespace cx */

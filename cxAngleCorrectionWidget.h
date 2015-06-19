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

#ifndef CXANGLECORRECTIONWIDGET_H_
#define CXANGLECORRECTIONWIDGET_H_


#include "cxBaseWidget.h"
#include <QPushButton>
#include "cxFileSelectWidget.h"
#include "cxDataSelectWidget.h"
#include "cxUSAcqusitionWidget.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxPatientModelServiceProxy.h"
#include "cxXmlOptionItem.h"
#include "cxSelectDataStringProperty.h"


class QVBoxLayout;

namespace cx
{

/**
 * Widget for use in the plugin AngleCorrection
 *
 * \ingroup org_custusx_AngleCorrection
 *
 * \date 2015-06-14
 * \author Daniel Hoyer Iversen
 */
class AngleCorrectionWidget : public BaseWidget
{
	Q_OBJECT
public:
    AngleCorrectionWidget(VisServicesPtr visServices, QWidget* parent) ;
	virtual ~AngleCorrectionWidget();
public slots:
	void runAngleCorection();
    void patientChangedSlot();    
    void selectVelData(QString filename);
	void toggleDetailsSlot();
    void cLDataChangedSlot();
    void toggleShowOutputData();
private:

    bool execute();
	XmlOptionFile mSettings;

    VisServicesPtr mVisServices;
    FileSelectWidget* mVelFileSelectWidget;
    StringPropertySelectMeshPtr mClDataSelectWidget;

	QString defaultWhatsThis() const;
	QVBoxLayout*  mVerticalLayout;
    QPushButton* mRunAngleCorrButton;
    QWidget* createOptionsWidget();
    QWidget* mOptionsWidget;


    DoublePropertyPtr mClSmoothing;
    DoublePropertyPtr mMaxThetaCutoff;
    DoublePropertyPtr mUncertaintyLimit;
    DoublePropertyPtr mMinArrowDist;
    MeshPtr mOutData;

    StringPropertySelectMeshPtr mOutDataSelectWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

};

} /* namespace cx */

#endif /* CXAngleCorrectionWIDGET_H_ */

#include "fswebcam.h"
#include "ui_fscontrolpanel.h"
#include "fscontroller.h"
#include <QProcess>
#include <QCameraViewfinder>
#include <QObject>

#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

using namespace cv;



FSWebCam::FSWebCam()
{
    info.portName = "";
    info.friendlyName = "";
    info.sizeX = CAM_IMAGE_WIDTH;
    info.sizeY = CAM_IMAGE_HEIGHT;
    isCapturingImage=false;
    camera=0;
    imageCapture=0;

	 
	 VideoCapture cap(0); // open the default camera
 //   if(!cap.isOpened())  // check if we succeeded
   //     return -1;

	 imageCaptureCv = cap;

	 
	imageCaptureCv.set( CV_CAP_PROP_FPS, 15);
    imageCaptureCv.set( CV_CAP_PROP_FRAME_WIDTH, 1280);
      imageCaptureCv.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
}

FSWebCam::~FSWebCam()
{
    delete imageCapture;
    delete camera;
}

void FSWebCam::StartX()
{
	 QFuture<void> future = QtConcurrent::run(this, &FSWebCam::StartX2);
}

void FSWebCam::StartX2()
{ 
	Mat NN;
	while(true)
	{
		 
			imageCaptureCv.read(NN);

			  frameTaken = false;
			  QThread::msleep(1);
			frame=NN.clone();
			QThread::msleep(1);
			   frameTaken = true;
		QThread::msleep(10);

	}
}

cv::Mat FSWebCam::getFrame()
{
 //   frameTaken = false;
 //   isCapturingImage = true;
 //   imageCapture->capture("./");
    //qDebug() << "preparing to take frame";
    //wait until camera has taken picture, then return
    while(!frameTaken){
		QThread::msleep(1);

        qApp->processEvents();
    }
    //qDebug() << "received frame";
	
	//for(int i=0;i<2;i++)
	{
		// qApp->processEvents();
	}

	//imageCaptureCv.grab();
	//imageCaptureCv.grab();
	//imageCaptureCv.retrieve(frame);
//	imageCaptureCv.read(frame);
	
 
//
//Mat m(frame);
//Mat mask=Mat::zeros(m.size(), CV_8UC1);
//
// 
//cv::rectangle(mask, cv::Point(100,100) , cv::Point(1000,960) , cv::Scalar(255,255,255),-1,8,0); // draws the circle around your point.
// 
//
//Mat output = Mat::zeros(m.size(),m.type()); // output starts with a black background.
//m.copyTo(output, mask); 
//
//		 

	


	 Mat ONOFF = frame.clone();
	  cv::imshow("extractedlaserLine1",ONOFF);waitKey(1) ;

	  
    return ONOFF;
}

FSPoint FSWebCam::getPosition()
{
    return FSMakePoint(CAM_POS_X, CAM_POS_Y, CAM_POS_Z);
}

void FSWebCam::setCamera(const QByteArray &cameraDevice)
{
	/*
    delete camera;
    delete imageCapture;
    if (cameraDevice.isEmpty()){
        camera = new QCamera;
    }else{
        camera = new QCamera(cameraDevice);
    }
    imageCapture = new QCameraImageCapture(camera);
    QImageEncoderSettings imageSettings = imageCapture->encodingSettings();
        qDebug() << imageCapture->supportedResolutions();
        qDebug() << imageSettings.resolution();
    //QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(1280, 960);
    imageCapture->setEncodingSettings(imageSettings);

    camera->setViewfinder(FSController::getInstance()->controlPanel->ui->viewfinder );
    FSController::getInstance()->controlPanel->ui->cameraLabel->setStyleSheet("border-style: solid; border-color: black; border-width: 3px 1px 3px 1px;");
    FSController::getInstance()->controlPanel->ui->cameraLabel->setText("");
    camera->setCaptureMode(QCamera::CaptureStillImage);

    //connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));

    camera->start();
    QStringList res = imageCapture->supportedImageCodecs();
    //QList<QSize> res = imageCapture->supportedResolutions();
    //QSize r;
    QString r;
    foreach(r,res){
        qDebug() << "Supported Resolutions:"<<r;
    }
    qDebug() << "set camera:" << cameraDevice;
	*/
}

void FSWebCam::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    Q_UNUSED(img);
    //qDebug("processing captured frame");
    //remove alpha channel
    /*QImage img2 = img.convertToFormat(QImage::Format_RGB888);
    qDebug() << img2.height() << img2.width();
    cv::Mat mat(img2.height(),
                img2.width(),
                CV_8UC3,
                (uchar*)img2.bits(), img2.bytesPerLine());
    frame = mat;
    cv::cvtColor(mat,frame, CV_RGB2BGR);
    frameTaken = true;*/
}

void FSWebCam::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    Q_UNUSED(fileName);
    QImage img = QImage(fileName);
    QFile::remove(fileName);
    QImage img2 = img.convertToFormat(QImage::Format_RGB888);
    //qDebug() << img2.height() << img2.width();
    cv::Mat mat(img2.height(),
                img2.width(),
                CV_8UC3,
                (uchar*)img2.bits(), img2.bytesPerLine());
    frame = mat;
    cv::cvtColor(mat,frame, CV_RGB2BGR);
    frameTaken = true;
    isCapturingImage = false;
    //if (applicationExiting)
        //close();
}

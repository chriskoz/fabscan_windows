#include "fscontroller.h"
#include "fsdialog.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;

FSController* FSController::singleton=0;

FSConfiguration* FSController::config = new FSConfiguration();

FSController::FSController()
{
    //Software
    geometries = new GeometryEngine();
    model = new FSModel();
    //Harware
    serial = new FSSerial();
    webcam = new FSWebCam();
    turntable = new FSTurntable();
    laser = new FSLaser();
    vision = new FSVision();

    scanning = false;
    //threshold = 40;
    //all in degrees; (only when stepper is attached to laser)
    laserSwipeMin = 30; //18
    laserSwipeMax = 45; //50
    meshComputed = false;
    ////old = true;
    //webcam->StartX();
}

FSController* FSController::getInstance()
{
    if (singleton == 0){
        singleton = new FSController();
    }
    return singleton;
}

void FSController::destroy()
{
    if (singleton != 0) {
        delete singleton;
        singleton = 0;
    }
}

void FSController::fetchFrame()
{
    if(webcam->info.portName.isEmpty()){
        mainwindow->showDialog("No webcam selected!");
        return;
    }

    cv::Mat frame;
    frame = FSController::getInstance()->webcam->getFrame();
    //cv::imshow("Extracted Frame",frame);
    //cv::waitKey(0);
    if(frame.cols!=1280)
        cv::resize(frame,frame,cv::Size(1280,960));
    cv::Mat result = vision->drawHelperLinesToFrame(frame);
    cv::resize(result,result,cv::Size(800,600)); //this is the resolution of the preview
    cv::imshow("Extracted Frame",result);
    cv::waitKey(0);
    cvDestroyWindow("Extracted Frame");
}

void FSController::hideFrame()
{
    cvDestroyWindow("Extracted Frame");
}

void FSController::scan()
{
    if(webcam->info.portName.isEmpty()){
        mainwindow->showDialog("No webcam selected!");
        return;
    }
    QFuture<void> future = QtConcurrent::run(this, &FSController::scanThread);
}

void FSController::scanThread()
{
    //check if camera is connected
    if(webcam->info.portName.isEmpty()){
        mainwindow->showDialog("No webcam selected!");
        return;
    }
    //detect laser line
    this->detectLaserLine();
    //turn off stepper (if available)
//    this->laser->disable();

    scanning = true; //start scanning, if false, scan stops
    FSFloat stepDegrees = turntableStepSize;

//    laser->turnOn();
    turntable->setDirection(FS_DIRECTION_CCW);
    turntable->enable();

    //iterate over a complete turn of the turntable
    for(FSFloat i=0; i<360 && scanning==true; i+=stepDegrees){
        qDebug() << "Scan step #" << i;

		//take picture without laser
        laser->turnOff();
        QThread::msleep(200);
		//waitForController();
        cv::Mat laserOff = webcam->getFrame();
		laserOff = webcam->getFrame();
     //     cv::imshow("extractedlaserLine",laserOff);
		
        if(laserOff.cols!=1280)
            cv::resize( laserOff,laserOff,cv::Size(1280,960) );

        //take picture with laser
        laser->turnOn();
        QThread::msleep(200);
		//waitForController();
        cv::Mat laserOn = webcam->getFrame();
		laserOn = webcam->getFrame();

	//	cv::imshow("extractedlaserLine",laserOn);

        if(laserOn.cols!=1280)
            cv::resize( laserOn,laserOn,cv::Size(1280,960) );

        /*cv::namedWindow("extracted laserLine");
        cv::imshow("extracted laserLine",laserOff);
        cv::waitKey(0);
        cv::imshow("extracted laserLine",laserOn);
        cv::waitKey(0);
        cvDestroyWindow("extracted laserLine");*/

		  // cv::namedWindow("extractedlaserLineoff",0);
       // cv::imshow("extractedlaserLineoff",laserOn);
        
      //  cv::imshow("extractedlaserLineon",laserOff);
      //  cv::waitKey(0);
       // cvDestroyWindow("extractedlaserLineon"); 

		 

		//cv::resizeWindow("extractedlaserLine", 500,380);
	//	cv::moveWindow("extractedlaserLine", 50,80);

        //here the magic happens
        vision->putPointsFromFrameToCloud(laserOff, laserOn, yDpi, 0);
        //update gui
        geometries->setPointCloudTo(model->pointCloud);
        mainwindow->redraw();
        //turn turntable a step
        turntable->turnNumberOfDegrees(stepDegrees);
        QThread::msleep(  300+stepDegrees*100);
		qDebug() << "scan step #" << i << "done";
//		cv::waitKey(0);

//		cv::circle (laserOn,
//			laser->mpx,10,
//			CV_RGB( 255,255,0 ),4,8,0);

//		cv::namedWindow("extractedlaserLine1",0);	
//		cv::imshow("extractedlaserLine1",laserOn);

//		cv::waitKey( 100+stepDegrees*100);

//		QThread::sleep(1);
		waitForController();
    }
    if(scanning) mainwindow->doneScanning();
    scanning = false; //stop scanning
}


/*
void FSController::scanThread()
{
	//check if camera is connected
	if(webcam->info.portName.isEmpty()){
		//   mainwindow->showDialog("No webcam selected!");
		//   return;
	}
	//detec laser line
	this->detectLaserLine();
	//turn of stepper (if available)
	this->laser->disable();

	scanning = true; //start scanning, if false, scan stops
	double stepDegrees = turntableStepSize;

	laser->turnOn();
	turntable->setDirection(FS_DIRECTION_CCW);
	turntable->enable();


	laser->turnOff();       		
	QThread::msleep(600);
	laser->turnOn();
    cv::Mat laserOff = webcam->getFrame();
	//iterate over a complete turn of the turntable
	for(double i=0; i<360 ; i+=stepDegrees){
		laser->turnOn();
     if( scanning==false)
		 break;

	    
		//detectLaserLine(); //每次 重新计算一下点
		cv::Mat laserOn = webcam->getFrame();

		//turn turntable a step
		turntable->turnNumberOfDegrees(stepDegrees);

		//here magic happens
		vision->putPointsFromFrameToCloud(laserOff, laserOn, yDpi, 0);
		//update gui
		geometries->setPointCloudTo(model->pointCloud);

		mainwindow->redraw();


		cv::circle (laserOn,
			laser->mpx,10,
			CV_RGB( 255,255,0 ),4,8,0);

		cv::namedWindow("extractedlaserLine1",0);	
		cv::imshow("extractedlaserLine1",laserOn);

		cv::waitKey( 50);//+stepDegrees*100);
		QThread::msleep(  50);

	}

	laser->turnOff();

	if(scanning) mainwindow->doneScanning();
	scanning = false; //stop scanning
}

*/

void FSController::scanThread2()
{
    if(webcam->info.portName.isEmpty()){
        mainwindow->showDialog("No webcam selected!");
        return;
    }
    scanning = true; //start scanning

    qDebug() << "done with turn to angle";
    //laser->turnNumberOfDegrees( laser->getRotation().y - LASER_SWIPE_MIN );
    turntable->setDirection(FS_DIRECTION_CW);
    for(FSFloat j=0; j<360 && scanning==true; j+=turntableStepSize){

		detectLaserLine(); //每次 重新计算一下点

		turntable->disable();
		laser->turnOn();
		laser->enable();
		laser->turnToAngle(laserSwipeMin);
		QThread::msleep(2500);
		laser->setDirection(FS_DIRECTION_CCW);
		for(double i=laserSwipeMin; i<laserSwipeMax && scanning==true; i+=laserStepSize){
			qDebug() << i;
			laser->turnOff();
			QThread::msleep(200);
			cv::Mat laserOff = webcam->getFrame();

			if(laserOff.cols!=1280)
				cv::resize( laserOff,laserOff,cv::Size(1280,960) );

			laser->turnOn();
			QThread::msleep(200);
			cv::Mat laserOn = webcam->getFrame();

			if(laserOn.cols!=1280)
				cv::resize( laserOn,laserOn,cv::Size(1280,960) );

			vision->putPointsFromFrameToCloud(laserOff, laserOn, 5, 0);
			geometries->setPointCloudTo(model->pointCloud);
			mainwindow->redraw();
			laser->turnNumberOfDegrees(laserStepSize);





			QThread::msleep(laserStepSize*200);
		}
		laser->disable();
		turntable->enable();
		turntable->turnNumberOfDegrees(turntableStepSize);
		std::string name;
		name.append(boost::lexical_cast<std::string>(j));
		name.append(".ply");
		//model->savePointCloudAsPLY(name);
		//model->pointCloud->clear();
		QThread::msleep(turntableStepSize*100);

		/*	cv::circle (laserOn,
		laser->mpx,10,
		CV_RGB( 255,255,0 ),4,8,0);
		cv::imshow("extractedlaserLine2",laserOn);
		cv::waitKey( 0);*/

	}
	if(scanning) mainwindow->doneScanning();
	scanning = false; //stop scanning
}

/*
cv::Mat FSController::subLaser()
{
	laser->turnOff();
	QThread::msleep(200);
	cv::Mat laserOff = webcam->getFrame();

	if(laserOff.cols!=1280)
		cv::resize( laserOff,laserOff,cv::Size(1280,960) );

	laser->turnOn();
	QThread::msleep(200);
	cv::Mat laserOn = webcam->getFrame();

	if(laserOn.cols!=1280)
		cv::resize( laserOn,laserOn,cv::Size(1280,960) );

	return vision->subLaser(laserOff,laserOn,threshold);
}
*/

cv::Mat FSController::diffImage()
{
    laser->turnOff();
    QThread::msleep(200);
    cv::Mat laserOff = webcam->getFrame();

    if(laserOff.cols!=1280)
        cv::resize( laserOff,laserOff,cv::Size(1280,960) );

    laser->turnOn();
    QThread::msleep(200);
    cv::Mat laserOn = webcam->getFrame();

    if(laserOn.cols!=1280)
        cv::resize( laserOn,laserOn,cv::Size(1280,960) );
//    laser->turnOff();
    return vision->diffImage(laserOff,laserOn);
}

bool FSController::waitForController()
{
	return serial->heatbeat();
}

bool FSController::detectLaserLine()
{
    unsigned int threshold = 40;
    laser->turnOff();
    QThread::msleep(400);
    cv::Mat laserOffFrame = webcam->getFrame();
    laserOffFrame = webcam->getFrame();
    cv::imshow("extractedlaserLine1",laserOffFrame);
    cv::waitKey(800);
    laser->turnOn();
    QThread::msleep(400);
    cv::Mat laserOnFrame = webcam->getFrame();
    laserOnFrame = webcam->getFrame();
    cv::imshow("extractedlaserLine2",laserOffFrame);
    cv::waitKey(1);
//    QThread::msleep(200);
//    laser->turnOff();
    if(laserOnFrame.cols!=1280)
        cv::resize( laserOnFrame,laserOnFrame,cv::Size(1280,960) );

	if(laserOffFrame.cols!=1280)
		cv::resize( laserOffFrame,laserOffFrame,cv::Size(1280,960) );

	// cv::imshow("extracted laserLine1",laserOnFrame);
	// cv::waitKey(10);
	// cv::imshow("extracted laserLine2",laserOffFrame);
	// cv::waitKey(10);
	//cvDestroyWindow("extracted laserLine");

    qDebug("images loaded, now detecting...");
    FSPoint p = vision->detectLaserLine( laserOffFrame, laserOnFrame, threshold );
    if(p.x == 0.0){return false;}
    laser->setLaserPointPosition(p);
    return true;
}

void FSController::computeSurfaceMesh()
{
    if(FSController::getInstance()->model->pointCloud->empty()){
        return;
    }
    //model->convertPointCloudToSurfaceMesh();
    //geometries->setSurfaceMeshTo(model->surfaceMesh,model->pointCloud);
    model->convertPointCloudToSurfaceMesh2();
    geometries->setSurfaceMeshTo(model->surfaceMeshPoisson,model->pointCloudPoisson);

    mainwindow->redraw();
}

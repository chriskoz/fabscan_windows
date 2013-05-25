#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fscontroller.h"
#include "fsdialog.h"

#include <QBasicTimer>
#include <QDialogButtonBox>
#include <QFuture>
#include <QtCore>
//#include <QtConcurrentRun>
#include <QtConcurrent/QtConcurrentRun>
#include <QCamera>
#include <QSound>

#include <boost/bind.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    hwTimer(new QBasicTimer),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setupMenu();
    this->enumerateSerialPorts();
    this->enumerateWebCams();
    hwTimer->start(400, this);
    dialog = new FSDialog(this);
    controlPanel = new FSControlPanel(this);
    FSController::getInstance()->mainwindow=this;
    FSController::getInstance()->controlPanel=controlPanel;
    ui->widget->setStyleSheet("border: 1px solid black;");
    applyState(POINT_CLOUD);
    //resolution: Good
    FSController::getInstance()->turntableStepSize =0.9;//0.84706;// 9*FSController::getInstance()->turntable->degreesPerStep;

	double xx= FSController::getInstance()->turntableStepSize;

	xx=xx+1;

    FSController::getInstance()->yDpi = 1;
}

	//cv::Rect selectx;
 //bool select_flag=false;
 //cv::Point origin;


	 /************************************************************************************************************************/
 /****                            如果采用这个onMouse()函数的话，则可以画出鼠标拖动矩形框的4种情形                        ****/
 /************************************************************************************************************************/
 //void onMouse(int event,int x,int y,int,void*)
 //{
 //    //Point origin;//不能在这个地方进行定义，因为这是基于消息响应的函数，执行完后origin就释放了，所以达不到效果。
 //    if(select_flag)
 //    {
	//	 selectx.x=MIN(origin.x,x);//不一定要等鼠标弹起才计算矩形框，而应该在鼠标按下开始到弹起这段时间实时计算所选矩形框
 //        selectx.y=MIN(origin.y,y);
 //        selectx.width=abs(x-origin.x);//算矩形宽度和高度
 //        selectx.height=abs(y-origin.y);
 //       // select&=cv::Rect(0,0,frame.cols,frame.rows);//保证所选矩形框在视频显示区域之内
 //    }
 //    if(event==CV_EVENT_LBUTTONDOWN)
 //    {
 //        select_flag=true;//鼠标按下的标志赋真值
 //        origin=cv::Point(x,y);//保存下来单击是捕捉到的点
 //        selectx=cv::Rect(x,y,0,0);//这里一定要初始化，宽和高为(0,0)是因为在opencv中Rect矩形框类内的点是包含左上角那个点的，但是不含右下角那个点
 //    }
 //    else if(event==CV_EVENT_LBUTTONUP)
 //    {
 //        select_flag=false;
 //    }
 //}


MainWindow::~MainWindow()
{
    delete ui;
    FSController::getInstance()->scanning=false;
}

void MainWindow::setupMenu()
{
    QAction* newPointCloudAction = new QAction("New", this);
    newPointCloudAction->setShortcuts(QKeySequence::New);
    connect(newPointCloudAction,SIGNAL(triggered()),this, SLOT(newPointCloud()));
    ui->menuFile->addAction(newPointCloudAction);

    QAction* openPointCloudAction = new QAction("Open PointCloud...", this);
    openPointCloudAction->setShortcuts(QKeySequence::Open);
    connect(openPointCloudAction,SIGNAL(triggered()),this, SLOT(openPointCloud()));
    ui->menuFile->addAction(openPointCloudAction);

    QAction* savePointCloudAction = new QAction("Save PointCloud...", this);
    savePointCloudAction->setShortcuts(QKeySequence::Save);
    connect(savePointCloudAction,SIGNAL(triggered()),this, SLOT(savePointCloud()));
    ui->menuFile->addAction(savePointCloudAction);

    QAction* showControlPanelAction = new QAction("Control Panel...", this);
    showControlPanelAction->setShortcuts(QKeySequence::Preferences);
    connect(showControlPanelAction,SIGNAL(triggered()),this, SLOT(showControlPanel()));
    ui->menuFile->addAction(showControlPanelAction);
}

void MainWindow::showDialog(QString dialogText)
{
    dialog->setStandardButtons(QDialogButtonBox::Ok);
    dialog->setText(dialogText);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();


	 cv::namedWindow("extractedlaserLine1",0); //显示实时采集	
	  cv::namedWindow("extractedlaserLine2",0);	//显示处理结果

	   //捕捉鼠标
   //  cv::setMouseCallback("extractedlaserLine1",onMouse,0);

	  	   	   	 HWND hWnd0 = (HWND)cvGetWindowHandle("FabScan");
 HWND hRawWnd0 = ::GetParent(hWnd0);
 if (hRawWnd0 != NULL) {
  BOOL bRet = ::SetWindowPos(hRawWnd0, HWND_TOPMOST, 100, 100, 0, 0, SWP_NOSIZE );
  assert(bRet);

 }

	   	   	 HWND hWnd = (HWND)cvGetWindowHandle("extractedlaserLine1");
 HWND hRawWnd = ::GetParent(hWnd);
 if (hRawWnd != NULL) {
  BOOL bRet = ::SetWindowPos(hRawWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE |SWP_NOMOVE);
  assert(bRet);

 }

 	   	   	 HWND hWnd2 = (HWND)cvGetWindowHandle("extractedlaserLine2");
 HWND hRawWnd2 = ::GetParent(hWnd2);
 if (hRawWnd2 != NULL) {
  BOOL bRet = ::SetWindowPos(hRawWnd2, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE |SWP_NOMOVE);
  assert(bRet);

 }


	 cv::resizeWindow("extractedlaserLine1", FSController::getInstance()->mainwindow->height()*3/4,FSController::getInstance()->mainwindow->height()/2-19);
	  cv::resizeWindow("extractedlaserLine2", FSController::getInstance()->mainwindow->height()*3/4,FSController::getInstance()->mainwindow->height()/2-19);
	 cv::moveWindow("extractedlaserLine1", FSController::getInstance()->mainwindow->x() +FSController::getInstance()->mainwindow->width()+15,FSController::getInstance()->mainwindow->y());
	  cv::moveWindow("extractedlaserLine2", FSController::getInstance()->mainwindow->x() +FSController::getInstance()->mainwindow->width()+15,FSController::getInstance()->mainwindow->y()+FSController::getInstance()->mainwindow->height()/2+19);

}

//===========================================
// Action Methods
//===========================================

void MainWindow::on_toggleViewButton_clicked()
{
    FSController::getInstance()->mainwindow->setCursor(Qt::BusyCursor);
    char currentDrawState = ui->widget->drawState;
    ui->widget->drawState = 1-currentDrawState;
    ui->widget->updateGL();
    if(ui->widget->drawState==0){
        state=POINT_CLOUD;
    }else{
        state=SURFACE_MESH;
        if(!FSController::getInstance()->meshComputed){
            qDebug() << "computing mesh";
            FSController::getInstance()->computeSurfaceMesh();
        }

    }
    applyState(state);
    FSController::getInstance()->mainwindow->setCursor(Qt::ArrowCursor);
}

void MainWindow::showControlPanel()
{

			 //set new path
    FSController::getInstance()->serial->serialPortPath->clear();
    FSController::getInstance()->serial->serialPortPath->append("COM11");
   // this->enumerateSerialPorts();
    FSController::getInstance()->serial->connectToSerialPort();

    controlPanel->show();
    controlPanel->raise();
    controlPanel->activateWindow();

	
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    //this->enumerateSerialPorts();
    //this->enumerateWebCams();

	 cv::namedWindow("extractedlaserLine1",0);	

	 cv::namedWindow("extractedlaserLine2",0);	



	 cv::resizeWindow("extractedlaserLine1", FSController::getInstance()->mainwindow->height()*3/4,FSController::getInstance()->mainwindow->height()/2-19);
	  cv::resizeWindow("extractedlaserLine2", FSController::getInstance()->mainwindow->height()*3/4,FSController::getInstance()->mainwindow->height()/2-19);
	 cv::moveWindow("extractedlaserLine1", FSController::getInstance()->mainwindow->x() +FSController::getInstance()->mainwindow->width()+15,FSController::getInstance()->mainwindow->y());
	  cv::moveWindow("extractedlaserLine2", FSController::getInstance()->mainwindow->x() +FSController::getInstance()->mainwindow->width()+15,FSController::getInstance()->mainwindow->y()+FSController::getInstance()->mainwindow->height()/2+19);


	    ;
	//	FSController::getInstance()->laser->mpx = cv::Point( this->ui->spinBox_x->value(), 240);

}

//===========================================
// Menu Methods
//===========================================

void MainWindow::onSelectSerialPort()
{
    QAction* action=qobject_cast<QAction*>(sender());
    if(!action) return;
    //set new path
    FSController::getInstance()->serial->serialPortPath->clear();
    FSController::getInstance()->serial->serialPortPath->append(action->iconText());
    this->enumerateSerialPorts();
    FSController::getInstance()->serial->connectToSerialPort();
}

void MainWindow::onSelectWebCam()
{
    QAction* action=qobject_cast<QAction*>(sender());
    if(!action) return;
    FSController::getInstance()->webcam->info.portName=action->iconText(); //eigentlich doppelt gemoppelt, das hier kann weg muss jedoch gekukt werden
    FSController::getInstance()->webcam->setCamera(action->data().toByteArray());
    this->enumerateWebCams();
}

void MainWindow::openPointCloud()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File","","Files (*.pcd) ;; PLY (*.ply)");
    if(fileName.isEmpty() ) return;
    if(fileName.endsWith(".pcd", Qt::CaseInsensitive) ){
        FSController::getInstance()->model->loadPointCloudFromPCD(fileName.toStdString());
    }else if(fileName.endsWith(".ply", Qt::CaseInsensitive) ){
        FSController::getInstance()->model->loadPointCloudFromPLY(fileName.toStdString());
    }
    ui->widget->drawState = 0;
    ui->widget->updateGL();
    applyState(POINT_CLOUD);
    FSController::getInstance()->meshComputed=false;
}

void MainWindow::savePointCloud()
{
    QFileDialog d(this, "Save File","","PCD (*.pcd) ;; PLY (*.ply)");
    d.setAcceptMode(QFileDialog::AcceptSave);
    if(d.exec()){
        QString fileName = d.selectedFiles()[0];
        //fileName.append(d.selectedNameFilter());
        if(fileName.isEmpty() ) return;
        qDebug() << fileName;
        if(fileName.endsWith(".pcd", Qt::CaseInsensitive) ){
            qDebug() << "Save as pcd file.";
            FSController::getInstance()->model->savePointCloudAsPCD(fileName.toStdString());
        }else if(fileName.endsWith(".ply", Qt::CaseInsensitive) ){
            qDebug() << "Save as ply file.";
            FSController::getInstance()->model->savePointCloudAsPLY(fileName.toStdString());
        }
    }

    ui->widget->drawState = 0;
    ui->widget->updateGL();
}

void MainWindow::newPointCloud()
{
    FSController::getInstance()->model->pointCloud->clear();
    FSController::getInstance()->model->surfaceMesh.polygons.clear();
    ui->widget->updateGL();
    applyState(POINT_CLOUD);
    FSController::getInstance()->meshComputed=false;
}

void MainWindow::enumerateSerialPorts()
{

	 ui->menuSerialPort->clear();

	 for(int i=0;i<14;i++)
	 {
		  QAction* ac = new QAction("COM0" , this);
		 switch(i)
		 {
		 case 0:
	          ac = new QAction("COM0" , this);break;
		 case 1:
	         ac = new QAction("COM1" , this);break;
		 case 2:
	          ac = new QAction("COM2" , this);break;
		 case 3:
	         ac = new QAction("COM3" , this);break;
		 case 4:
	           ac = new QAction("COM4" , this);break;
		 case 5:
	          ac = new QAction("COM5" , this);break;
		 case 6:
	           ac = new QAction("COM6" , this);break;
		 case 7:
	          ac = new QAction("COM7" , this);break;
		 case 8:
	          ac = new QAction("COM8" , this);break;
		 case 9:
	          ac = new QAction("COM9" , this);break;
			 		 case 10:
	           ac = new QAction("COM10" , this);break;
		 case 11:
	          ac = new QAction("COM11" , this);break;
		 case 12:
	          ac = new QAction("COM12" , this);break;
		 case 13:
	          ac = new QAction("COM13" , this);break;
		 case 14:
	          ac = new QAction("COM14" , this);break;
		 }


            ac->setCheckable(true);              
            ui->menuSerialPort->addAction(ac);


	 }
	/*
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    ui->menuSerialPort->clear();

    foreach (QextPortInfo info, ports) {
        if(!info.portName.isEmpty() && !info.portName.startsWith("ttyS")){
            QAction* ac = new QAction(info.portName, this);
            ac->setCheckable(true);
            connect(ac,SIGNAL(triggered()),this, SLOT(onSelectSerialPort()));
            if(FSController::getInstance()->serial->serialPortPath->compare(info.portName)==0){
                ac->setChecked(true);
            }
            ui->menuSerialPort->addAction(ac);
        }
		
        //qDebug() << "port name:"       << info.portName;
        //qDebug() << "friendly name:"   << info.friendName;
        //qDebug() << "physical name:"   << info.physName;
        //qDebug() << "enumerator name:" << info.enumName;
        //qDebug() << "vendor ID:"       << info.vendorID;
        //qDebug() << "product ID:"      << info.productID;
        //qDebug() << "===================================";
    }*/
}

void MainWindow::enumerateWebCams()
{
    if(QCamera::availableDevices().size()==0){
   
		  QAction* a = new QAction("camera", this);
	//	QAction* a = new QAction("No camera found.", this);
    
       ui->menuCamera->clear();

		      a->setCheckable(true);  

   a->setEnabled(true);
	      a->setChecked(true);
       ui->menuCamera->addAction(a);


	    a->setEnabled(true);
	      a->setChecked(true);

       return;
    }

    QByteArray cameraDevice;
    QCamera* camera;
    ui->menuCamera->clear();
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
        QString description = camera->deviceDescription(deviceName);
        QAction *videoDeviceAction = new QAction(description, this);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant(deviceName));
        connect(videoDeviceAction,SIGNAL(triggered()),this, SLOT(onSelectWebCam()));

        /*if(description.contains("Logitech")){
            FSController::getInstance()->webcam->info.portName=description; //eigentlich doppelt gemoppelt, das hier kann weg muss jedoch gekukt werden
            FSController::getInstance()->webcam->setCamera(videoDeviceAction->data().toByteArray());
        }*/

        if (FSController::getInstance()->webcam->info.portName.compare(description)==0) {
            //cameraDevice = deviceName;
            videoDeviceAction->setChecked(true);
        }
        ui->menuCamera->addAction(videoDeviceAction);
    }
}

void MainWindow::on_scanButton_clicked()
{
    //doneScanning();
    //QFuture<void> future = QtConcurrent::run(FSController::getInstance(), &FSController::scanThread);
    bool s = FSController::getInstance()->scanning;
    if (s==false){





		 //set new path
    FSController::getInstance()->serial->serialPortPath->clear();
    FSController::getInstance()->serial->serialPortPath->append("COM11");
   // this->enumerateSerialPorts();
    FSController::getInstance()->serial->connectToSerialPort();


        applyState(SCANNING);
        FSController::getInstance()->scanThread();
    }else{
        applyState(POINT_CLOUD);
        this->ui->scanButton->setText("Start Scan");
        FSController::getInstance()->scanning = false;
    }

}

void MainWindow::doneScanning()
{
    QSound::play("done.wav");
    this->ui->scanButton->setText("Start Scan");
    FSController::getInstance()->laser->disable();
    FSController::getInstance()->turntable->disable();
    applyState(POINT_CLOUD);
}

void MainWindow::redraw()
{
    //ui->widget->drawState = 0;
    ui->widget->updateGL();
}

void MainWindow::applyState(FSState s)
{
    state = s;
    switch(state){
    case SCANNING:
        this->ui->widget->drawState=0;
        this->ui->scanButton->setText("Stop Scan");
        break;
    case POINT_CLOUD:
        this->ui->scanButton->setText("Start Scan");
        if(FSController::getInstance()->meshComputed){
            this->ui->toggleViewButton->setText("Show SurfaceMesh");
        }else{
            this->ui->toggleViewButton->setText("Compute SurfaceMesh");
        }
        break;
    case SURFACE_MESH:
        this->ui->scanButton->setText("Start Scan");
        this->ui->toggleViewButton->setText("Show PointCloud");
        break;
    }
}

void MainWindow::on_resolutionComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("Best")==0){
        //laserStepSize = 2*laser->degreesPerStep;
        FSController::getInstance()->turntableStepSize = FSController::getInstance()->turntable->degreesPerStep;
        FSController::getInstance()->yDpi = 1;
    }
    if(arg1.compare("Good")==0){
        FSController::getInstance()->turntableStepSize = 8*FSController::getInstance()->turntable->degreesPerStep;
        FSController::getInstance()->yDpi = 1;
    }
    if(arg1.compare("Normal")==0){
        FSController::getInstance()->turntableStepSize = 2*16*FSController::getInstance()->turntable->degreesPerStep;
        FSController::getInstance()->yDpi = 5;
    }
    if(arg1.compare("Poor")==0){
        FSController::getInstance()->turntableStepSize = 10*16*FSController::getInstance()->turntable->degreesPerStep;
        FSController::getInstance()->yDpi = 10;
    }

	   FSController::getInstance()->turntableStepSize =0.9;
	    FSController::getInstance()->yDpi = 1;
}

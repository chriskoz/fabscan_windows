#include "fsserial.h"
#include <QThread>
#include "staticHeaders.h"

FSSerial::FSSerial()
{
    serialPortPath = new QString();
	recievedPing = false;
}

bool FSSerial::connectToSerialPort()
{
    this->serialPort = new QextSerialPort(*serialPortPath, QextSerialPort::EventDriven);
//    serialPort->setBaudRate(BAUD9600);
    serialPort->setBaudRate(BAUD57600);
    serialPort->setFlowControl(FLOW_OFF);
    serialPort->setParity(PAR_NONE);
    serialPort->setDataBits(DATA_8);
    serialPort->setStopBits(STOP_2);

    if (serialPort->open(QIODevice::ReadWrite) == true) {
        connect(serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()) );
        connect(serialPort, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)) );
        if (!(serialPort->lineStatus() & LS_DSR)){
            //qDebug() << "warning: device is not turned on";
            return false;
        }
        //qDebug() << "listening for data on" << serialPort->portName();
        return true;
    }else{
        //qDebug() << "device failed to open:" << serialPort->errorString();
        return true;
    }
}

void FSSerial::onReadyRead()
{
	recievedPing = false;
    QByteArray bytes;
    int a = serialPort->bytesAvailable();
    bytes.resize(a);
    serialPort->read(bytes.data(), bytes.size());
//    qDebug() << "#"<<bytes.size() <<"bytes=" << bytes.data();
	char *tmpData = bytes.data();
	unsigned char *tmpData2 = (unsigned char *)tmpData;
	for (int i = 0 ; i < a ; i++ ) {
		//qDebug() << " "<< (int)tmpData2[i];
        if (bytes.data()[i] == (char)MC_FABSCAN_PONG) {
            recievedPing = true;
            qDebug("got 'PONG' from controller");
        }
	}
}

void FSSerial::onDsrChanged(bool)
{
    //qDebug("onDsrChanged");
}

void FSSerial::writeChar(char c)
{
//    cout << "writing to serial port: " << (int)((unsigned char)c);
    if( serialPortPath->isEmpty() ) return;
    if( !serialPort->isOpen() ) return;
    if( serialPort->isWritable() ){
        //qDebug("is writable");
        //usleep(100000);
        serialPort->write(&c, 1);
    }else{
       // qDebug("is not writable");
    }
}

void FSSerial::writeChars(char* c)
{
    if( serialPortPath->isEmpty() ) return;
    if( !serialPort->isOpen() ) return;
    if( serialPort->isWritable() ){
        //qDebug("is writable");
        //usleep(100000);
        serialPort->write(c);
    }else{
        //qDebug("is not writable");
    }
}

bool FSSerial::heatbeat()
{
	recievedPing = false;
	writeChar((char)MC_FABSCAN_PING);
	QThread::msleep(200);
//	while (recievedPing == false) {
//		QThread::msleep(200);
//		QThread::yieldCurrentThread();
//		qDebug("heatbeat... waiting");
//		cv::waitKey(1000);
//	}
	return true;
}


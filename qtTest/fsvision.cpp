#include "fsvision.h"
#include "fslaser.h"
#include "fswebcam.h"
#include "fsturntable.h"
#include "fscontroller.h"

#include <assert.h>
#include <QDebug>



class FSLaser;
class FSController;




//****************************************************************************//
// Copyright (C) 2012, Michele (MikyMouse) Martinelli                      //
//****************************************************************************//
// This file is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//


int first_subiteration(cv::Mat imgMat){
    int changed=0;
    
    //for each non-zero pixel p1
    for(int i=2; i<imgMat.rows-2; i++)
            for(int j=2; j<imgMat.cols-2; j++){
                unsigned char p1 = imgMat.at<int>(i,j)>0?1:0;
                if(p1==0) 
                    continue;
                //compute pixel surrounding p1
                unsigned char p2 = imgMat.at<int>(i-1,j)>0?1:0;
                unsigned char p3 = imgMat.at<int>(i-1,j+1)>0?1:0;
                unsigned char p4 = imgMat.at<int>(i,j+1)>0?1:0;
                unsigned char p5 = imgMat.at<int>(i+1,j+1)>0?1:0;
                unsigned char p6 = imgMat.at<int>(i+1,j)>0?1:0;
                unsigned char p7 = imgMat.at<int>(i+1,j-1)>0?1:0;
                unsigned char p8 = imgMat.at<int>(i,j-1)>0?1:0;
                unsigned char p9 = imgMat.at<int>(i-1,j-1)>0?1:0;
                
                //compute B(p1)
                int Bp1=p2+p3+p4+p5+p6+p7+p8+p9;
                
                //compute A(p1)
                unsigned char points[10]={p2,p3,p4,p5,p6,p7,p8,p9,p2};
                unsigned char Ap1=0;
                for(int ind=0;ind<9;ind++)
                    if(points[ind]==0 && points[ind+1]==1)
                        Ap1++;
                
                //conditions as reported in the article
                if ( Bp1>=2 && Bp1<=6 )
                    if (Ap1==1)
                        if (p2*p4*p6==0)
                            if(p4*p6*p8==0){
                                imgMat.at<int>(i,j)=0;
                                changed++;    
                            }
            }
        return changed;
}

int second_subiteration(cv::Mat imgMat){
    int changed=0;
    
    for(int i=2; i<imgMat.rows-2; i++)
            for(int j=2; j<imgMat.cols-2; j++){
                unsigned char p1 = imgMat.at<int>(i,j)>0?1:0;
                if(p1==0) 
                    continue;
                unsigned char p2 = imgMat.at<int>(i-1,j)>0?1:0;
                unsigned char p3 = imgMat.at<int>(i-1,j+1)>0?1:0;
                unsigned char p4 = imgMat.at<int>(i,j+1)>0?1:0;
                unsigned char p5 = imgMat.at<int>(i+1,j+1)>0?1:0;
                unsigned char p6 = imgMat.at<int>(i+1,j)>0?1:0;
                unsigned char p7 = imgMat.at<int>(i+1,j-1)>0?1:0;
                unsigned char p8 = imgMat.at<int>(i,j-1)>0?1:0;
                unsigned char p9 = imgMat.at<int>(i-1,j-1)>0?1:0;
                
                int Bp1=p2+p3+p4+p5+p6+p7+p8+p9;
                unsigned char points[10]={p2,p3,p4,p5,p6,p7,p8,p9,p2};
                unsigned char Ap1=0;
                for(int ind=0;ind<9;ind++)
                    if(points[ind]==0 && points[ind+1]==1)
                        Ap1++;
                
                if ( Bp1>=2 && Bp1<=6 )
                    
                    if (Ap1==1)if (p2*p4*p8==0)
                            if(p2*p6*p8==0){
                                imgMat.at<int>(i,j)=0;
                                changed++;
                                }
            }
    
    return changed;        
}


int skeletonize(cv::Mat img){
    int iter;
    for (iter=0;first_subiteration(img)+second_subiteration(img)>0;iter++);
    
    //this return the number of iteration made
    return iter;
}


FSVision::FSVision()
{
	IsFindLine=false;

	 
}

 

FSPoint FSVision::convertCvPointToFSPoint(CvPoint cvPoint)
{
	CvSize cvImageSize = cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT);
	FSSize fsImageSize = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH*(CAM_IMAGE_HEIGHT/CAM_IMAGE_WIDTH), 0.0f);

	//here we define the origin of the cvImage, we place it in the middle of the frame and in the corner of the two perpendiculair planes
	CvPoint origin;
	origin.x = cvImageSize.width/2.0f;
	origin.y = cvImageSize.height*ORIGIN_Y;

	FSPoint fsPoint;
	//translate
	cvPoint.x -= origin.x;
	cvPoint.y -= origin.y;
	//scale
	fsPoint.x = cvPoint.x*fsImageSize.width/cvImageSize.width;
	fsPoint.y = -cvPoint.y*fsImageSize.height/cvImageSize.height;
	fsPoint.z = 0.0f;

	return fsPoint;
}

CvPoint FSVision::convertFSPointToCvPoint(FSPoint fsPoint)
{
	CvSize cvImageSize = cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT);
	FSSize fsImageSize = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH*(CAM_IMAGE_HEIGHT/CAM_IMAGE_WIDTH), 0.0f);
	CvPoint origin;
	origin.x = cvImageSize.width/2.0f;
	origin.y = cvImageSize.height*ORIGIN_Y;

	CvPoint cvPoint;

	cvPoint.x = fsPoint.x*cvImageSize.width/fsImageSize.width;
	cvPoint.y = -fsPoint.y*cvImageSize.height/fsImageSize.height;

	//translate
	cvPoint.x += origin.x;
	cvPoint.y += origin.y;

	return cvPoint;
}

cv::Mat FSVision::diffImage(cv::Mat &laserOff, cv::Mat &laserOn)
{
	unsigned int cols = laserOff.cols;
	unsigned int rows = laserOff.rows;
	cv::Mat bwLaserOff( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat bwLaserOn( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat diffImage( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat result( cols,rows,CV_8UC3,cv::Scalar(100) );
	cv::cvtColor(laserOff, bwLaserOff, CV_RGB2GRAY); //convert to grayscale
	cv::cvtColor(laserOn, bwLaserOn, CV_RGB2GRAY); //convert to grayscale
	cv::subtract(bwLaserOn,bwLaserOff,diffImage); //subtract both grayscales
	cv::cvtColor(diffImage, result, CV_GRAY2RGB); //convert back ro rgb
	return result;
}

cv::Mat FSVision::subLaser(cv::Mat &laserOff, cv::Mat &laserOn, double threshold)
{
	unsigned int cols = laserOff.cols;
	unsigned int rows = laserOff.rows;
	cv::Mat bwLaserOff( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat bwLaserOn( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat diffImage( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat treshImage( cols,rows,CV_8U,cv::Scalar(100) );
	cv::Mat result( cols,rows,CV_8UC3,cv::Scalar(100) );

	cv::cvtColor(laserOff, bwLaserOff, CV_RGB2GRAY); //convert to grayscale
	cv::cvtColor(laserOn, bwLaserOn, CV_RGB2GRAY); //convert to grayscale
	cv::subtract(bwLaserOn,bwLaserOff,diffImage); //subtract both grayscales
	cv::GaussianBlur(diffImage,diffImage,cv::Size(5,5),3); //gaussian filter
	cv::threshold(diffImage,treshImage,threshold,255,cv::THRESH_BINARY); //apply threshold
	cv::Mat element5(3,3,CV_8U,cv::Scalar(1));
	cv::morphologyEx(treshImage,treshImage,cv::MORPH_OPEN,element5);

	cv::cvtColor(treshImage, result, CV_GRAY2RGB); //convert back ro rgb
	/*cv::namedWindow("laserLine");
	cv::imshow("laserLine", result);
	cv::waitKey(0);
	cv::destroyWindow("laserLine");*/
	return result;
}

cv::Mat FSVision::histogram(cv::Mat &img)
{
	/// Separate the image in 3 places ( B, G and R )

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	cv::Mat hist;

	/// Compute the histograms:
	//cv::calcHist()
	cv::calcHist(&img, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

	// Draw the histograms for B, G and R
	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar(0,0,0) );

	/// Normalize the result to [ 0, histImage.rows ]
	cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	/// Draw for each channel
	for( int i = 1; i < histSize; i++ )
	{
		cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
			cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
			cv::Scalar( 255, 0, 0), 2, 8, 0  );
	}
	return histImage;
}

cv::Mat FSVision::subLaser2(cv::Mat &laserOff, cv::Mat &laserOn,int JD)
{
	unsigned int cols = laserOff.cols;
	unsigned int rows = laserOff.rows;
	cv::Mat bwLaserOff( rows,cols,CV_8U,cv::Scalar(100) );
	cv::Mat bwLaserOn( rows,cols,CV_8U,cv::Scalar(100) );
	cv::Mat tresh2Image( rows,cols,CV_8U,cv::Scalar(0) );
	cv::Mat diffImage( rows,cols,CV_8U,cv::Scalar(100) );
	cv::Mat gaussImage( rows,cols,CV_8U,cv::Scalar(0) );
	cv::Mat laserImage( rows,cols,CV_8U,cv::Scalar(0) );
	cv::Mat result( rows,cols,CV_8UC3,cv::Scalar(0) );

	std::vector<cv::Mat> rgbChannelsOff(3);
	cv::split(laserOff, rgbChannelsOff);

	// cv::cvtColor(laserOff, bwLaserOff, CV_RGB2GRAY);//convert to grayscale
	bwLaserOff=rgbChannelsOff[0];
	//cv::imshow("laserLineOff", bwLaserOff);  cv::waitKey(10);

	std::vector<cv::Mat> rgbChannelsOn(3);
	cv::split(laserOn, rgbChannelsOn);




	// cv::cvtColor(laserOn, bwLaserOn, CV_RGB2GRAY); //convert to grayscale
	bwLaserOn = rgbChannelsOn[0];



	//  





	cv::subtract(bwLaserOn,bwLaserOff,diffImage); //subtract both grayscales

	// cv::cvtColor(laserOn, diffImage, CV_RGB2GRAY); 


	tresh2Image = diffImage.clone();
	//cv::imshow("tresh2Image1", tresh2Image);  cv::waitKey(10);
	//细化测试
                	cv::Mat img=tresh2Image.clone();

	//cv::equalizeHist(img,img);

	 cv::blur(img,img, cv::Size(5,5));
	//cv::imshow("tresh2Image1_2", img);  cv::waitKey(10);
	




	cv::threshold(img, img,40, 255, cv::THRESH_BINARY); 
	 cv::blur(img,img, cv::Size(3,3));
	 	char buffer [256];
   
	if(JD%4==0)
	{
		JD=JD/4;
   sprintf (buffer, "D:\\img\\img1\\sd_spline-%d.jpg",JD);
   cv::imwrite(buffer,img);
      sprintf (buffer, "D:\\img\\img2\\tex_image-%d.jpg",JD);
   cv::imwrite(buffer,laserOff);

	}

normalizeLetter(img,img);  //很好的细化
//	cv::imshow("tresh2Image2", img);  cv::waitKey(10);
//		img.convertTo(img,CV_32FC1);


	//	skeletonize(img);  //很好的细化
 
 //cv::blur(img,img, cv::Size(3,3));
// cv::imshow("tresh2Image3", img);  cv::waitKey(0);
	/*

	 cv::blur(img,img, cv::Size(3,3));

	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));


	cv::erode(img, img, element); // cv::imshow("tresh2Image3", img);  cv::waitKey(1000);
	 cv::dilate(img, img, element);
 cv::dilate(img, img, element);
	//  cv::dilate(img, img, element);
	 	 cv::blur(img,img, cv::Size(5,5));
		
		 cv::imshow("tresh2Image2_2", img);  cv::waitKey(10);


	

	 cv::imshow("tresh2Image3", img);  cv::waitKey(10);
	// cv::blur(img,img, cv::Size(3,3));

	 cv::Mat element2 = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	 cv::dilate(img, img, element2);//  cv::dilate(img, img, element);
	 cv::blur(img,img, cv::Size(3,3));
	// cv::erode(img, img, element2);

	  cv::imshow("tresh2Image4", img);  cv::waitKey(0);

	 //	atfx atf ;// = new atfx();
		//        atf.setInputSketch(1-img);

		  //    cv::Mat thinned_image = atf.getATFImage();
	 
	// cv::imshow("tresh2Image4", img);  cv::waitKey(10);
	// skeletonize(img);  //很好的细化

 //cv::imshow("tresh2Image5", img);  cv::waitKey(10);
 // cv::dilate(img, img, element2);

 // cv::imshow("tresh2Image6", img);  cv::waitKey(0);
	////
	//cv::namedWindow("tresh2Image2",0);
	//cv::imshow("tresh2Image2", img);  //cv::waitKey(10);

	//diffImage =img.clone();

	//	   cv::cvtColor(laserImage, result, CV_GRAY2RGB); //convert back ro rgb
	//return result;




 

	//细化
	//void cvThin (IplImage* src, IplImage* dst, int iterations)
	//cvThreshold


	//	cv::blur(tresh2Image,tresh2Image, cv::Size(3,3));

 

	// Apply the specified morphology operation
	//cv::imshow("laserLine", FSVision::histogram(diffImage));
	//cv::waitKey(0);
	 
	//cv::imshow("laserLine", diffImage);
	//cv::waitKey(0);
	cv::GaussianBlur(diffImage,gaussImage,cv::Size(15,15),12,12);
	diffImage = diffImage-gaussImage;
	//cv::imshow("laserLine", diffImage);
	//cv::waitKey(0);
	double threshold = 10;
	cv::threshold(diffImage,diffImage,threshold,255,cv::THRESH_TOZERO); //apply threshold
	//cv::imshow("laserLine", FSVision::histogram(diffImage));
	//cv::waitKey(0);
	//cv::equalizeHist(diffImage,diffImage);
	//cv::imshow("laserLine", FSVision::histogram(diffImage));
	//cv::waitKey(0);
	cv::erode(diffImage,diffImage,cv::Mat(3,3,CV_8U,cv::Scalar(1)) );
	//cv::imshow("laserLine", diffImage);
	//cv::waitKey(0);

	//cv::Mat element5(3,3,CV_8U,cv::Scalar(1));
	//cv::morphologyEx(diffImage,diffImage,cv::MORPH_OPEN,element5);
	//cv::imshow("laserLine", diffImage);
	//cv::waitKey(0);
	cv::Canny(diffImage,diffImage,20,50);
	//cv::imshow("laserLine", diffImage);
	//cv::waitKey(0);
	//cv::imshow("laserLine", treshImage+diffImage);
	//cv::waitKey(0);
	//cv::destroyWindow("laserLine");




	int *edges;

	edges=new int[cols]; //contains the cols index of the detected edges per row

	for(unsigned int y = 0; y <rows; y++){
		//reset the detected edges
		for(unsigned int j=0; j<cols; j++){ edges[j]=-1; }
		int j=0;
		for(unsigned int x = 0; x<cols; x++){
			if(diffImage.at<uchar>(y,x)>250){
				edges[j]=x;
				j++;
			}
		}
		//iterate over detected edges, take middle of two edges
		for(unsigned int j=0; j<cols-1; j+=2){
			if(edges[j]>=0 && edges[j+1]>=0 && edges[j+1]-edges[j]<40){
				int middle = (int)(edges[j]+edges[j+1])/2;
				//qDebug() << cols << rows << y << middle;
				laserImage.at<uchar>(y,middle) = 255;
			}
		}
	}

	*/

		for(unsigned int y = 0; y <img.rows ; y++)
		{
			for(unsigned int x = 0; x<img.cols; x++)
		   {

			if(img.at<int>(y,x)>99){
				laserImage.at<uchar>(y,x) = 255;
			}
			else
			{
				laserImage.at<uchar>(y,x) = 0;
			}
			}
		}




//	cv::namedWindow("laserLine",0);
	//cv::imshow("laserLine", diffImage);

//	cv::imshow("laserLine", laserImage);







	//cv::waitKey(0);
	//  cv::imshow("laserLine", laser+treshImage);
	//   cv::waitKey(0);
	//cv::destroyAllWindows();


	cv::cvtColor(laserImage, result, CV_GRAY2RGB); //convert back ro rgb
	return result;
}

cv::Mat FSVision::drawHelperLinesToFrame(cv::Mat &frame)
{
	//artifical horizont
	cv::line(frame,
		cv::Point(0,frame.rows*ORIGIN_Y),
		cv::Point(frame.cols,frame.rows*ORIGIN_Y),
		CV_RGB( 0,0,255 ),
		2);

	//two lines for center of frame
	cv::line(frame,
		cv::Point(frame.cols*0.5f,0),
		cv::Point(frame.cols*0.5f,frame.rows),
		CV_RGB( 255,255,0 ),
		1);
	cv::line(frame,
		cv::Point(0,frame.rows*0.5f),
		cv::Point(frame.cols,frame.rows*0.5f),
		CV_RGB( 255,255,0 ),
		1);

	//line showing the upper limit where analyzing starts
	cv::line(frame,
		cv::Point(0,frame.rows-LOWER_ANALYZING_FRAME_LIMIT),
		cv::Point(frame.cols,frame.rows-LOWER_ANALYZING_FRAME_LIMIT),
		CV_RGB( 255,255,0 ),
		1);

	//line showing the lower limit where analyzing stops
	cv::line(frame,
		cv::Point(0,UPPER_ANALYZING_FRAME_LIMIT),
		cv::Point(frame.cols,UPPER_ANALYZING_FRAME_LIMIT),
		CV_RGB( 255,255,0 ),
		1);
	frame = drawLaserLineToFrame(frame);
	return frame;
}

cv::Mat FSVision::drawLaserLineToFrame(cv::Mat &frame)
{
	FSLaser* laser = FSController::getInstance()->laser;
	CvPoint cvLaserPoint = convertFSPointToCvPoint(laser->getLaserPointPosition());

	double vertical    = cvLaserPoint.x;
	double horizontal  = convertFSPointToCvPoint(FSMakePoint(0,0,0)).y;

	cv::Point p1 = cv::Point(vertical, 0);          //top of laser line
	cv::Point p2 = cv::Point(vertical, horizontal); //bottom of laser line
	cv::line(frame, p1, p2, CV_RGB( 255,0,0 ),6);   //draw laser line
	return frame;
}

void FSVision::putPointsFromFrameToCloud(
	cv::Mat &laserOff,
	cv::Mat &laserOn,
	int dpiVertical,    //step between vertical points
	double lowerLimit, int JD) //remove points below this limit
{
	//qDebug() << "putPointsFromFrameToCloud";
	//the following lines are just to make to code more readable
	FSModel* model = FSController::getInstance()->model;
	FSLaser* laser = FSController::getInstance()->laser;
	FSTurntable* turntable = FSController::getInstance()->turntable;
	FSWebCam* webcam = FSController::getInstance()->webcam;

	//extract laser line from the two images
	//cv::Mat laserLine = subLaser(laserOff,laserOn,threshold);
	cv::Mat laserLine = subLaser2(laserOff,laserOn,JD);


	//	cv::imshow("extractedlaserLine2",laserLine);

	//	  cv::waitKey(300) ;

	//calculate position of laser in cv frame
	//position of the laser line on the back plane in frame/image coordinates
	FSPoint fsLaserLinePosition = laser->getLaserPointPosition();
	//position of the laser line on the back plane in world coordinates
	CvPoint cvLaserLinePosition = convertFSPointToCvPoint(fsLaserLinePosition);

	double laserPos = cvLaserLinePosition.x; //const over all y

	//laserLine is result of subLaser2, is in RGB
	unsigned int cols = laserLine.cols;
	unsigned int rows = laserLine.rows;
	//create new image in black&white
	cv::Mat bwImage( cols,rows,CV_8U,cv::Scalar(100) );
	//qDebug("still works here");
	//convert from rgb to b&w
	cv::cvtColor(laserLine, bwImage, CV_RGB2GRAY); //convert to grayscale


	cv::imshow("extractedlaserLine2",bwImage);

	cv::waitKey(1) ;

	//now iterating from top to bottom over bwLaserLine frame
	//no bear outside of these limits :) cutting of top and bottom of frame
	for(int y = UPPER_ANALYZING_FRAME_LIMIT;
		y < bwImage.rows-(LOWER_ANALYZING_FRAME_LIMIT);
		y+=dpiVertical )
	{
		//qDebug() << "checking point at line " << y << laserPos+ANALYZING_LASER_OFFSET;
		//ANALYZING_LASER_OFFSET is the offset where we stop looking for a reflected laser, cos we might catch the non reflected
		//now iteratinf from right to left over bwLaserLine frame
		for(int x = bwImage.cols-1;
			x >= laserPos+ANALYZING_LASER_OFFSET;
			x -= 1){
				//qDebug() << "Pixel value: " << bwImage.at<uchar>(y,x);
				if(bwImage.at<uchar>(y,x)==255){ //check if white=laser-reflection
					//qDebug() << "found point at x=" << x;
					//if (row[x] > 200){
					//we have a white point in the grayscale image, so one edge laser line found
					//no we should continue to look for the other edge and then take the middle of those two points
					//to take the width of the laser line into account

					//position of the reflected laser line on the image coord
					CvPoint cvNewPoint;
					cvNewPoint.x = x;
					cvNewPoint.y = y;

					//convert to world coordinates withouth depth
					FSPoint fsNewPoint = FSVision::convertCvPointToFSPoint(cvNewPoint);
					FSLine l1 = computeLineFromPoints(webcam->getPosition(), fsNewPoint);
					FSLine l2 = computeLineFromPoints(laser->getPosition(), laser->getLaserPointPosition());

					FSPoint i = computeIntersectionOfLines(l1, l2);
					fsNewPoint.x = i.x;
					fsNewPoint.z = i.z;

					//At this point we know the depth=z. Now we need to consider the scaling depending on the depth.
					//First we move our point to a camera centered cartesion system.
					fsNewPoint.y -= (webcam->getPosition()).y;
					fsNewPoint.y *= ((webcam->getPosition()).z - fsNewPoint.z)/(webcam->getPosition()).z;
					//Redo the translation to the box centered cartesion system.
					fsNewPoint.y += (webcam->getPosition()).y;

					//get color from picture without laser
					FSUChar r = laserOff.at<cv::Vec3b>(y,x)[2];
					FSUChar g = laserOff.at<cv::Vec3b>(y,x)[1];
					FSUChar b = laserOff.at<cv::Vec3b>(y,x)[0];
					fsNewPoint.color = FSMakeColor(r, g, b);

					//turning new point according to current angle of turntable
					//translate coordinate system to the middle of the turntable
					fsNewPoint.z -= TURNTABLE_POS_Z; //7cm radius of turntbale plus 5mm offset from back plane
					FSPoint alphaDelta = turntable->getRotation();
					double alphaOld = (float)atan(fsNewPoint.z/fsNewPoint.x);
					double alphaNew = alphaOld+alphaDelta.y*(M_PI/180.0f);
					double hypotenuse = (float)sqrt(fsNewPoint.x*fsNewPoint.x + fsNewPoint.z*fsNewPoint.z);

					if(fsNewPoint.z < 0 && fsNewPoint.x < 0){
						alphaNew += M_PI;
					}else if(fsNewPoint.z > 0 && fsNewPoint.x < 0){
						alphaNew -= M_PI;
					}
					fsNewPoint.z = (float)sin(alphaNew)*hypotenuse;
					fsNewPoint.x = (float)cos(alphaNew)*hypotenuse;

					if(fsNewPoint.y>lowerLimit+0.5 && hypotenuse < 7){ //eliminate points from the grounds, that are not part of the model
						//qDebug("adding point");
						model->addPointToPointCloud(fsNewPoint);
					}
					break;
				}
		}
	}
}



FSPoint FSVision::detectLaserLine( cv::Mat &laserOff, cv::Mat &laserOn, unsigned int threshold, cv::Point *mP )
{

	IsFindLine=false;

	unsigned int cols = laserOff.cols;
	unsigned int rows = laserOff.rows;
	cv::Mat laserLine = subLaser2(laserOff, laserOn,1);
	std::vector<cv::Vec4i> lines;
	double deltaRho = 1;
	double deltaTheta = M_PI/2;
	int minVote = 20;
	double minLength = 50;
	double maxGap = 10;
	cv::Mat laserLineBW( cols, rows, CV_8U, cv::Scalar(0) );
	cv::cvtColor(laserLine, laserLineBW, CV_RGB2GRAY); //convert to grayscale

	cv::Mat mask( cols/2, rows/2-100, CV_8U, cv::Scalar(0) );
 
	 
 cv::Rect rect(0, 0,cols/2, rows/2 - 100);
 laserLineBW(rect).copyTo(mask);

	
	//cv::imshow("extractedlaserLine3",mask);
	//cv::waitKey(100);

//	cv::namedWindow("Detected Lines with HoughP");

	//cvDestroyWindow("Detected Lines with HoughP");

	cv::HoughLinesP( mask,
		lines,
		deltaRho,
		deltaTheta,
		minVote,
		minLength,
		maxGap );


	cv::putText(mask,"",cv::Point(10,10),6,6,cv::Scalar(255,0,0),1,8,false);
	cv::imshow("extractedlaserLine2",mask);
	//cv::waitKey(0);

	//should at least detect the laser line
	qDebug() << "detected"<<lines.size()<<"lines";
	if(lines.size()==0){
		qDebug("Did not detect any laser line, did you select a SerialPort form the menu?");
		FSPoint p = FSMakePoint(0.0,0.0,0.0);
		
		mP[0].x =0;
		mP[0].y =0;

		return(p);
	}
	//assert(lines.size()>0);
	//for(int i=0;i<lines.size();i++){
	cv::Point p1;
	cv::Point p2;
	for(int i=0;i<lines.size();i++){
		qDebug() << "drawing line "<<lines[i][0]<<lines[i][1]<<lines[i][2]<<lines[i][3];
		//int i = 0;
		p1.x = lines[i][0];
		p1.y = lines[i][1];
		p2.x = lines[i][2];
		p2.y = lines[i][3];
		cv::line(laserLine, p1, p2, CV_RGB( 255,0,0 ),1);   //draw laser line

		
		

		IsFindLine=true;
	}

	 mP[0].x =p1.x;
	 mP[0].y =p1.y;


	/*cv::namedWindow("Detected Lines with HoughP");
	cv::imshow("Detected Lines with HoughP",laserLine);
	cv::waitKey(100);
	cvDestroyWindow("Detected Lines with HoughP");
*/
	FSPoint p = convertCvPointToFSPoint(p1);
	return p;
}

 


////////////////////////////////////////////////////////////////test
void ThinSubiteration1(cv::Mat &pSrc, cv::Mat &pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo(pDst);
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			if(pSrc.at<float>(i, j) == 1.0f) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int) pSrc.at<float>( i-1, j-1);
				int neighbor1 = (int) pSrc.at<float>( i-1, j);
				int neighbor2 = (int) pSrc.at<float>( i-1, j+1);
				int neighbor3 = (int) pSrc.at<float>( i, j+1);
				int neighbor4 = (int) pSrc.at<float>( i+1, j+1);
				int neighbor5 = (int) pSrc.at<float>( i+1, j);
				int neighbor6 = (int) pSrc.at<float>( i+1, j-1);
				int neighbor7 = (int) pSrc.at<float>( i, j-1);
				int C = int(~neighbor1 & ( neighbor2 | neighbor3)) +
					int(~neighbor3 & ( neighbor4 | neighbor5)) +
					int(~neighbor5 & ( neighbor6 | neighbor7)) +
					int(~neighbor7 & ( neighbor0 | neighbor1));
				if(C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = std::min(N1,N2);
					if ((N == 2) || (N == 3)) {
						/// calculate criteria 3
						int c3 = ( neighbor1 | neighbor2 | ~neighbor4) & neighbor3;
						if(c3 == 0) {
							pDst.at<float>( i, j) = 0.0f;
						}
					}
				}
			}
		}
	}
}


void ThinSubiteration2(cv::Mat & pSrc, cv::Mat & pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo( pDst);
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			if (pSrc.at<float>( i, j) == 1.0f) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int) pSrc.at<float>( i-1, j-1);
				int neighbor1 = (int) pSrc.at<float>( i-1, j);
				int neighbor2 = (int) pSrc.at<float>( i-1, j+1);
				int neighbor3 = (int) pSrc.at<float>( i, j+1);
				int neighbor4 = (int) pSrc.at<float>( i+1, j+1);
				int neighbor5 = (int) pSrc.at<float>( i+1, j);
				int neighbor6 = (int) pSrc.at<float>( i+1, j-1);
				int neighbor7 = (int) pSrc.at<float>( i, j-1);
				int C = int(~neighbor1 & ( neighbor2 | neighbor3)) +
					int(~neighbor3 & ( neighbor4 | neighbor5)) +
					int(~neighbor5 & ( neighbor6 | neighbor7)) +
					int(~neighbor7 & ( neighbor0 | neighbor1));
				if(C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = std::min(N1,N2);
					if((N == 2) || (N == 3)) {
						int E = (neighbor5 | neighbor6 | ~neighbor0) & neighbor7;
						if(E == 0) {
							pDst.at<float>(i, j) = 0.0f;
						}
					}
				}
			}
		}
	}
}


void FSVision::normalizeLetter(cv::Mat &inputarray, cv::Mat &outputarray) {
	bool bDone = false;
	int rows = inputarray.rows;
	int cols = inputarray.cols;

	inputarray.convertTo(inputarray,CV_32FC1);

	inputarray.copyTo(outputarray);

	outputarray.convertTo(outputarray,CV_32FC1);

	/// pad source
	cv::Mat p_enlarged_src = cv::Mat(rows + 2, cols + 2, CV_32FC1);
	for(int i = 0; i < (rows+2); i++) {
		p_enlarged_src.at<float>(i, 0) = 0.0f;
		p_enlarged_src.at<float>( i, cols+1) = 0.0f;
	}
	for(int j = 0; j < (cols+2); j++) {
		p_enlarged_src.at<float>(0, j) = 0.0f;
		p_enlarged_src.at<float>(rows+1, j) = 0.0f;
	}
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			if (inputarray.at<float>(i, j) >= 20.0f) {
				p_enlarged_src.at<float>( i+1, j+1) = 1.0f;
			}
			else
				p_enlarged_src.at<float>( i+1, j+1) = 0.0f;
		}
	}

	/// start to thin
	cv::Mat p_thinMat1 = cv::Mat::zeros(rows + 2, cols + 2, CV_32FC1);
	cv::Mat p_thinMat2 = cv::Mat::zeros(rows + 2, cols + 2, CV_32FC1);
	cv::Mat p_cmp = cv::Mat::zeros(rows + 2, cols + 2, CV_8UC1);

	while (bDone != true) {
		/// sub-iteration 1
		ThinSubiteration1(p_enlarged_src, p_thinMat1);
		/// sub-iteration 2
		ThinSubiteration2(p_thinMat1, p_thinMat2);
		/// compare
		compare(p_enlarged_src, p_thinMat2, p_cmp, CV_CMP_EQ);
		/// check
		int num_non_zero = countNonZero(p_cmp);
		if(num_non_zero == (rows + 2) * (cols + 2)) {
			bDone = true;
		}
		/// copy
		p_thinMat2.copyTo(p_enlarged_src);
	}
	// copy result
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			outputarray.at<float>( i, j) = p_enlarged_src.at<float>( i+1, j+1);
		}
	}
}
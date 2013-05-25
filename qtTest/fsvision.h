#ifndef FSVISION_H
#define FSVISION_H

#include <opencv2/core/core.hpp>

#include "fsgeometries.h"

 class FSVision
{
public:
    FSVision();
   static  FSPoint convertCvPointToFSPoint(CvPoint cvPoint);
  static   CvPoint convertFSPointToCvPoint(FSPoint fsPoint);

  static   cv::Mat subLaser(cv::Mat &laserOff, cv::Mat &laserOn, double threshold);
  static   cv::Mat subLaser2(cv::Mat &laserOff, cv::Mat &laserOn,int JD);
  static   cv::Mat drawHelperLinesToFrame(cv::Mat &frame);
 static    cv::Mat drawLaserLineToFrame(cv::Mat &frame);
  static   cv::Mat diffImage(cv::Mat &laserOff, cv::Mat &laserOn);

   
 
  bool IsFindLine;


   static  void normalizeLetter(cv::Mat &inputarray, cv::Mat &outputarray) ;
     void putPointsFromFrameToCloud(
            cv::Mat &laserOff,
            cv::Mat &laserOn,
            int dpiVertical,
            double lowerLimit,int JD);

     FSPoint detectLaserLine(cv::Mat &laserOff, cv::Mat &laserOn, unsigned int threshold, cv::Point *mP);
     cv::Mat histogram(cv::Mat &img);
};

#endif // FSVISION_H

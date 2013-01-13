// ObjectTracking.cpp : Define the entry point for console app.  
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "BlobResult.h"  
#define CNT_LENGTH 12
#define CNT_RESET 6

// Get thresholded image in HSV format  
IplImage* GetThresholdedImageHSV_Green( IplImage* img )  
{  
    // Create an HSV format image from image passed  
    IplImage* imgHSV = cvCreateImage( cvGetSize( img ),   
                                      8,   
                                      3 );     
  
    cvCvtColor( img, imgHSV, CV_BGR2HSV );  
  
    // Create binary thresholded image acc. to max/min HSV ranges  
    // For detecting blue gloves in "MOV.MPG - HSV mode  
    IplImage* imgThresh = cvCreateImage( cvGetSize( img ),   
                                         8,   
                                         1 );             
  
    cvInRangeS( imgHSV,  
                cvScalar(29, 65, 20 ),  
                cvScalar(50, 255, 85 ),  
                imgThresh );  
  
    // Tidy up and return thresholded image  
    cvReleaseImage( &imgHSV );  
    return imgThresh;  
}  

IplImage* GetThresholdedImageHSV_White( IplImage* img )  
{  
    // Create an HSV format image from image passed  
    IplImage* imgHSV = cvCreateImage( cvGetSize( img ),   
                                      8,   
                                      3 );     
  
    cvCvtColor( img, imgHSV, CV_BGR2HSV );  
  
    // Create binary thresholded image acc. to max/min HSV ranges  
    // For detecting blue gloves in "MOV.MPG - HSV mode  
    IplImage* imgThresh = cvCreateImage( cvGetSize( img ),   
                                         8,   
                                         1 );             
  
    cvInRangeS( imgHSV,  
                cvScalar(16, 8, 128 ),  
                cvScalar(29, 57, 255 ),  
                imgThresh );  
  
    // Tidy up and return thresholded image  
    cvReleaseImage( &imgHSV );  
    return imgThresh;  
}  


IplImage* GetThresholdedImageHSV_Red( IplImage* img )  
{  
    // Create an HSV format image from image passed  
    IplImage* imgHSV = cvCreateImage( cvGetSize( img ),   
                                      8,   
                                      3 );     
  
    cvCvtColor( img, imgHSV, CV_BGR2HSV );  
  
    // Create binary thresholded image acc. to max/min HSV ranges  
    // For detecting blue gloves in "MOV.MPG - HSV mode  
    IplImage* imgThresh = cvCreateImage( cvGetSize( img ),   
                                         8,   
                                         1 );             
  
    cvInRangeS( imgHSV,  
                cvScalar(170, 205, 115 ),  
                cvScalar(179, 255, 255 ),  
                imgThresh );  
  
    // Tidy up and return thresholded image  
    cvReleaseImage( &imgHSV );  
    return imgThresh;  
}  


int main()  
{ 
	int width [CNT_LENGTH];
	int height [CNT_LENGTH];
	int x[CNT_LENGTH];
	int y[CNT_LENGTH];
	int cnt = 0;
	int cnt2 = 0;

	for(int i=0;i<CNT_LENGTH;i++)
	{
		width[i] = 0;
		height[i] = 0;
		x[i] = 0;
		y[i] = 0;
	}

    CBlobResult blobs;    
    CBlob *currentBlob;   

    CBlobResult blobs_red;    
    CBlob *currentBlob_red;  

    CvPoint pt1, pt2;  
    CvRect cvRect;  

	CvPoint pt1_red, pt2_red, pt3_red, pt4_red;  
    CvRect cvRect_red;  

    int key = 0;  
    IplImage* frame = 0;

	IplImage* imgThresh0 = 0;
	IplImage* imgThresh1 = 0;
	CvArr* imgThreshArr = 0;
	IplImage* imgThresh = 0;

	IplImage* imgThresh_Red = 0;
	IplImage* imgThresh_White = 0;

	IplConvKernel *element = cvCreateStructuringElementEx(2,2,1,1,CV_SHAPE_RECT);
  

	frame = cvLoadImage("calibr.png", 1 );
	cvSmooth(frame, frame, CV_GAUSSIAN,3,3); 

	imgThresh1 = GetThresholdedImageHSV_Green( frame );   

	cvErode( imgThresh1, imgThresh1, element, 2);
	cvDilate( imgThresh1, imgThresh1, element, 2);

	imgThresh_Red = GetThresholdedImageHSV_Red( frame ); 

    // Detect the white blobs from the black background  
    blobs = CBlobResult( imgThresh1, NULL, 0 );    

    blobs_red = CBlobResult( imgThresh_Red, NULL, 0 );  

    // Exclude white blobs smaller than the given value (10)    
    // The bigger the last parameter, the bigger the blobs need  
	// to be for inclusion    
    blobs.Filter( blobs,  
                      B_EXCLUDE,  
                      CBlobGetArea(),  
                      B_LESS,  
                      300 );    

    blobs_red.Filter( blobs_red,  
                      B_EXCLUDE,  
                      CBlobGetArea(),  
                      B_LESS,  
                      300 );   
  
  // Attach a bounding rectangle for each blob discovered  
		int num_blobs = blobs.GetNumBlobs();  
  
        if(num_blobs > 0)         {
				currentBlob = blobs.GetBlob( 0 );              
                cvRect = currentBlob->GetBoundingBox(); 
				pt1.x = cvRect.x + 0.1*cvRect.width;  
				pt1.y = cvRect.y + 0.05*cvRect.height; 
				pt2.x =  pt1.x + 0.8*cvRect.width;  
				pt2.y= pt1.y + 0.85*cvRect.height;  
            // Attach bounding rect to blob in orginal video input  
           /* cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  */
        }  

		int num_blobs_red = blobs_red.GetNumBlobs();  
  
        if(num_blobs_red > 1)         {
			    currentBlob_red = blobs_red.GetBlob( 0 );              
                cvRect_red = currentBlob_red->GetBoundingBox(); 
				pt1_red.x = cvRect_red.x;  
				pt1_red.y = cvRect_red.y;  
				pt2_red.x = cvRect_red.x + cvRect_red.width;  
				pt2_red.y = cvRect_red.y + cvRect_red.height;  

			    currentBlob_red = blobs_red.GetBlob( 1 );              
                cvRect_red = currentBlob_red->GetBoundingBox(); 
				pt3_red.x = cvRect_red.x;
				pt3_red.y = cvRect_red.y; 
				pt4_red.x = cvRect_red.x + cvRect_red.width;  
				pt4_red.y = cvRect_red.y + cvRect_red.height;  

				printf("pt1.x %d  pt1.y %d\n", pt1.x, pt1.y);
				printf("pt2.x %d  pt2.y %d\n\n", pt2.x, pt2.y);
				printf("RED pt1.x %d  pt1.y %d\n", pt1_red.x, pt1_red.y);
				printf("RED pt2.x %d  pt2.y %d\n", pt2_red.x, pt2_red.y);
				printf("RED pt3.x %d  pt3.y %d\n", pt3_red.x, pt3_red.y);
				printf("RED pt4.x %d  pt4.y %d\n", pt4_red.x, pt4_red.y);
            // Attach bounding rect to blob in orginal video input  
        }  

		double pt1xB = double(pt1_red.x - cvRect.x)/double(cvRect.width*cvRect.height);
		double pt1yB = double(pt1_red.y - cvRect.y)/double(cvRect.width*cvRect.height);

		double widthA = double((pt4_red.x-pt1_red.x))/double(cvRect.width);
		double heightA = double((pt4_red.y-pt1_red.y))/double(cvRect.height);
				pt1.x = cvRect.x + int(pt1xB*cvRect.width*cvRect.height); 
				pt1.y = cvRect.y + int(pt1yB*cvRect.width*cvRect.height); 
				pt2.x =  pt1.x + int(widthA*cvRect.width);  
				pt2.y= pt1.y + int(heightA*cvRect.height);  
				
				cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  


        // Add the black and white and original images  
		cvNamedWindow( "video" );  
		cvNamedWindow( "thresh" );        
		cvNamedWindow( "thresh_red" );   

        cvShowImage( "thresh", imgThresh1 );  
        cvShowImage( "video", frame );  
		cvShowImage( "thresh_red", imgThresh_Red );  
		 
		key = cvWaitKey( 2000 ); 


    // Initialize capturing live feed from video file or camera  
    CvCapture* capture = cvCaptureFromFile( "ujecie1.wmv" );  
  
    // Get the frames per second  
    int fps = ( int )cvGetCaptureProperty( capture,  
                                           CV_CAP_PROP_FPS );    
  
    // Can't get device? Complain and quit  
    if( !capture )  
    {  
        printf( "Could not initialize capturing...\n" );  
        return -1;  
    }  
  
    // Windows used to display input video with bounding rectangles  
    // and the thresholded video  
    cvNamedWindow( "video" );  
    cvNamedWindow( "thresh" );        

    frame = cvQueryFrame( capture );   
	cvSmooth(frame, frame, CV_GAUSSIAN,3,3); 
    imgThresh1 = GetThresholdedImageHSV_Green( frame );   
	imgThresh0 = GetThresholdedImageHSV_Green( frame );   
	imgThresh = GetThresholdedImageHSV_Green( frame );   
	cvErode( imgThresh1, imgThresh1, element, 1);
	cvDilate( imgThresh1, imgThresh1, element, 1);

    // An infinite loop  
    while( key != 'x' ) 
    { 
        // If we couldn't grab a frame... quit  
        if( !( frame = cvQueryFrame( capture ) ) )  
            break;        
        // Get object's thresholded image (blue = white, rest = black)  

		cvSmooth(frame, frame, CV_GAUSSIAN,3,3); 

		imgThresh0 = cvCloneImage(imgThresh1);

		imgThresh1 = GetThresholdedImageHSV_Green( frame );   

		cvErode( imgThresh1, imgThresh1, element, 1);
		cvDilate( imgThresh1, imgThresh1, element, 1);

		imgThresh = cvCloneImage(imgThresh1);

		cvOr(imgThresh0, imgThresh, imgThresh);

        // Detect the white blobs from the black background  
        blobs = CBlobResult( imgThresh, NULL, 0 );    
  
        // Exclude white blobs smaller than the given value (10)    
        // The bigger the last parameter, the bigger the blobs need    
        // to be for inclusion    
        blobs.Filter( blobs,  
                      B_EXCLUDE,  
                      CBlobGetArea(),  
                      B_LESS,  
                      300 );           
  
        // Attach a bounding rectangle for each blob discovered  
        int num_blobs = blobs.GetNumBlobs();  
  
        if(num_blobs > 0)         {
			cnt2 = 0;
			currentBlob = blobs.GetBlob( 0 );              
            cvRect = currentBlob->GetBoundingBox(); 
			
			if(cnt<=CNT_LENGTH-1)	{
				x[cnt] = cvRect.x;  
				y[cnt] = cvRect.y;  
				width[cnt] =  cvRect.width;  
				height[cnt]= cvRect.height;  
				cnt++;
			}
			else if(cnt==CNT_LENGTH)	{
				for(int i=0; i<CNT_LENGTH-1;i++)	{
					x[i] = x[i+1];  
					y[i] = y[i+1];
					width[i] =  width[i+1];
					height[i] = height[i+1];
				}
				x[cnt-1] = cvRect.x;  
				y[cnt-1] = cvRect.y;  
				width[cnt-1] =  cvRect.width;  
				height[cnt-1]= cvRect.height;  
			}

			int xAvg = 0;
			int yAvg = 0;
			int widthAvg = 0;
			int heightAvg =0;
  
			for(int i=0; i<cnt; i++)	{
				xAvg = xAvg + x[i];
				yAvg = yAvg + y[i];
				widthAvg = widthAvg + width[i];
				heightAvg = heightAvg + height[i];
			}
			xAvg = xAvg/cnt;
			yAvg = yAvg/cnt;
			widthAvg = widthAvg/cnt;
			heightAvg = heightAvg/cnt;

            pt1.x = xAvg;
            pt1.y = yAvg; 
            pt2.x = pt1.x + widthAvg;
            pt2.y = pt1.y + heightAvg;
  

				pt1_red.x = xAvg + int(pt1xB*cvRect.width*cvRect.height); 
				pt1_red.y = yAvg + 2 * int(pt1yB*cvRect.width*cvRect.height); 
				pt2_red.x =  pt1_red.x + int(widthA*widthAvg);  
				pt2_red.y= pt1_red.y + int(heightA*heightAvg);  
  
            // Attach bounding rect to blob in orginal video input  
            cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  
            cvRectangle( frame,  
                         pt1_red,   
                         pt2_red,  
                         cvScalar(170, 170, 170, 170),  
                         1,  
                         8,  
                         0 ); 
			 //printf("i = %d\n", i);
        }  else	{
			cnt2++;
			if(cnt2 >= CNT_RESET)	{
				cnt = 0;
				for(int i=0;i<CNT_LENGTH;i++)
				{
					width[i] = 0;
					height[i] = 0;
					x[i] = 0;
					y[i] = 0;
				}
			}
		}

  
        // Add the black and white and original images  
        cvShowImage( "thresh", imgThresh );  
        cvShowImage( "video", frame );  
  
        // Optional - used to slow up the display of frames  
        key = cvWaitKey( 2000 / fps );  
  
        // Prevent memory leaks by releasing thresholded image  
        cvReleaseImage( &imgThresh );        
    }  
  
    // We're through with using camera.   
    cvReleaseCapture( &capture );  

    return 0;  
}  
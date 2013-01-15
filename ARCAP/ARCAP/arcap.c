/* Program umo¿liwiaj¹cy robienie zdjêæ za pomoc¹ kamery umieszczonej na g³owie i ramki s³u¿¹cej do wskazywania interesuj¹cego obszaru. */
#include "arcap.h"


CvPoint pt1_calibrate, pt2_calibrate, pt3_calibrate, pt4_calibrate;
int drag = 0;
CvCapture *capture = 0;
int key = 0;
CvRect rect_calibrate;
IplImage* frame, * img1;

/* Metoda s³u¿¹ca do obs³ugi zaznaczania obszaru, który widoczny jest w ramce w trakcie kalibracji.  */
void areaSelector(int event, int x, int y, int flags, void* param)
{
    /* wciœniêcie lewego przycisku myszki  */
    if (event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        pt1_calibrate = cvPoint(x, y);
        drag = 1;
    }
    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE && drag)
    {
        img1 = cvCloneImage(frame);
        cvRectangle(img1,pt1_calibrate,cvPoint(x, y),CV_RGB(255, 0, 0),1,8,0);
        cvShowImage("video", img1);
    }
    /* user release left button */
    if (event == CV_EVENT_LBUTTONUP && drag)
    {
		pt2_calibrate = cvPoint(x,y);
        drag = 0;
    }

    /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP)
    {
        drag = 0;
    }
}

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
	cvNamedWindow( "video" );  
	cvNamedWindow( "thresh" );        
	cvNamedWindow( "thresh_red" );  
	
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
    CvRect rect;  

    int key = 0;  

	IplImage* imgThresh0 = 0;
	IplImage* imgThresh1 = 0;
	CvArr* imgThreshArr = 0;
	IplImage* imgThresh = 0;

	IplImage* imgThresh_Red = 0;
	IplImage* imgThresh_White = 0;

	IplConvKernel *element = cvCreateStructuringElementEx(2,2,1,1,CV_SHAPE_RECT);
  

	frame = cvLoadImage("calibr1.png", 1 );
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
                rect = currentBlob->GetBoundingBox(); 
				pt1.x = rect.x + 0.1*rect.width;  
				pt1.y = rect.y + 0.05*rect.height; 
				pt2.x =  pt1.x + 0.8*rect.width;  
				pt2.y= pt1.y + 0.85*rect.height;  
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
                rect_calibrate = currentBlob_red->GetBoundingBox(); 
				pt1_calibrate.x = rect_calibrate.x;  
				pt1_calibrate.y = rect_calibrate.y;  
				pt2_calibrate.x = rect_calibrate.x + rect_calibrate.width;  
				pt2_calibrate.y = rect_calibrate.y + rect_calibrate.height;  

			    currentBlob_red = blobs_red.GetBlob( 1 );              
                rect_calibrate = currentBlob_red->GetBoundingBox(); 
				pt3_calibrate.x = rect_calibrate.x;
				pt3_calibrate.y = rect_calibrate.y; 
				pt4_calibrate.x = rect_calibrate.x + rect_calibrate.width;  
				pt4_calibrate.y = rect_calibrate.y + rect_calibrate.height;  

				printf("pt1.x %d  pt1.y %d\n", pt1.x, pt1.y);
				printf("pt2.x %d  pt2.y %d\n\n", pt2.x, pt2.y);
				printf("CALIBRATE pt1.x %d  pt1.y %d\n", pt1_calibrate.x, pt1_calibrate.y);
				printf("CALIBRATE pt2.x %d  pt2.y %d\n", pt2_calibrate.x, pt2_calibrate.y);
				printf("CALIBRATE pt3.x %d  pt3.y %d\n", pt3_calibrate.x, pt3_calibrate.y);
				printf("CALIBRATE pt4.x %d  pt4.y %d\n", pt4_calibrate.x, pt4_calibrate.y);
            // Attach bounding rect to blob in orginal video input  
        }  

		double pt1xB = double(pt1_calibrate.x - rect.x)/sqrt(double(rect.width*rect.height));
		double pt1yB = double(pt1_calibrate.y - rect.y)/sqrt(double(rect.width*rect.height));

		/*int pt1xB = pt1_red.x - cvRect.x;
		int pt1yB = pt1_red.y - cvRect.y;*/

		double widthA = double((pt4_calibrate.x-pt1_calibrate.x))/double(rect.width);
		//double heightA = double((pt4_red.y-pt1_red.y))/double(cvRect.height);
		double heightA = widthA; 
				
				printf("pt1xB %f  pt1yB %f\n", pt1xB, pt1yB);
				printf("widthA %f  heightA %f\n\n", widthA, heightA);

				pt1_calibrate.x = rect.x + int(pt1xB * sqrt(double(rect.width*rect.height))-3.2); 
				pt1_calibrate.y = rect.y + int(pt1yB * sqrt(double(rect.width*rect.height))-2.4); 

				pt2_calibrate.x =  pt1_calibrate.x + int(widthA*rect.width+6.4);  
				pt2_calibrate.y= pt1_calibrate.y + int(heightA*rect.height+4.8);  
				
				cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  

				cvRectangle( frame,  
                         pt1_calibrate,   
                         pt2_calibrate,  
                         cvScalar(170, 170, 170, 0),  
                         1,  
                         8,  
                         0 );


        // Add the black and white and original images   
        cvShowImage( "thresh", imgThresh1 );  
		cvShowImage( "thresh_red", imgThresh_Red );  

		while( key != 'c' )
		{
			cvShowImage("video", frame);
			key = cvWaitKey(10);
		}

		while( key != 'q' )
		{
			//frame = cvQueryFrame( capture );
			//if (rect.width>0)
			//	cvSetImageROI(frame,rect);
			cvSetMouseCallback("video", mouseHandler, NULL);
			key = cvWaitKey(10);
			//if( (char) key== 'r' ){ rect = cvRect(0,0,0,0); cvResetImageROI(frame);}
			//cvShowImage("video", frame);
		}
		pt1xB = double(pt1_calibrate.x - rect.x)/sqrt(double(rect.width*rect.height));
		pt1yB = double(pt1_calibrate.y - rect.y)/sqrt(double(rect.width*rect.height));

		/*int pt1xB = pt1_red.x - cvRect.x;
		int pt1yB = pt1_red.y - cvRect.y;*/

		widthA = double((pt4_calibrate.x-pt1_calibrate.x))/double(rect.width);
		//double heightA = double((pt4_red.y-pt1_red.y))/double(cvRect.height);
		heightA = widthA; 
				
				printf("pt1xB %f  pt1yB %f\n", pt1xB, pt1yB);
				printf("widthA %f  heightA %f\n\n", widthA, heightA);
		 
		key = cvWaitKey( 10000 ); 


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
            rect = currentBlob->GetBoundingBox(); 
			
			if(cnt<=CNT_LENGTH-1)	{
				x[cnt] = rect.x;  
				y[cnt] = rect.y;  
				width[cnt] =  rect.width;  
				height[cnt]= rect.height;  
				cnt++;
			}
			else if(cnt==CNT_LENGTH)	{
				for(int i=0; i<CNT_LENGTH-1;i++)	{
					x[i] = x[i+1];  
					y[i] = y[i+1];
					width[i] =  width[i+1];
					height[i] = height[i+1];
				}
				x[cnt-1] = rect.x;  
				y[cnt-1] = rect.y;  
				width[cnt-1] =  rect.width;  
				height[cnt-1]= rect.height;  
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
  

				pt1_calibrate.x = xAvg + int(pt1xB * sqrt(double(rect.width*rect.height))-3.2); 
				pt1_calibrate.y = yAvg + int(pt1yB * sqrt(double(rect.width*rect.height))-2.4); 
				/*pt1_calibrate.x = xAvg + pt1xB;
				pt1_calibrate.y = yAvg +  pt1yB;*/
				pt2_calibrate.x =  pt1_calibrate.x + int(widthA*widthAvg+6.4);  
				pt2_calibrate.y= pt1_calibrate.y + int(heightA*heightAvg+4.8);  
  
            // Attach bounding rect to blob in orginal video input  
            cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  
            cvRectangle( frame,  
                         pt1_calibrate,   
                         pt2_calibrate,  
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
/* Program umo¿liwiaj¹cy robienie zdjêæ za pomoc¹ kamery umieszczonej na g³owie i ramki s³u¿¹cej do wskazywania interesuj¹cego obszaru. */
#include "arcap.h"


CvPoint pt1, pt2, pt1_calibrate, pt2_calibrate, pt3_calibrate, pt4_calibrate;
int drag = 0;
CvCapture *capture = 0;
int fps;
int key = 0;
CvRect rect, rect_calibrate;
IplImage* frame, *frame0, * frame_gaussian, * frame_thresholded, * frame_thresholded0, * frame_thresholded1, *frame_calibrate, *photo;
CBlobResult blobs;    
CBlob *currentBlob;   
IplConvKernel *element = cvCreateStructuringElementEx(2,2,1,1,CV_SHAPE_RECT);

/* zmienne kalibracji HSV */
int lowestH=27;
int lowestS=65;
int lowestV=20;
int highestH=48;
int highestS=255;
int highestV=115;

/*zmienne kalibracji po³o¿enia ramki */
double pt1xC = 0.0;
double pt1yC = 0.0;
double widthB = 0.0;
double heightB = 0.0;
				

/* zmienne wykorzystywane przy obliczaniu uœrednionego po³o¿enia ramki */
int width [CNT_LENGTH];
int height [CNT_LENGTH];
int x[CNT_LENGTH];
int y[CNT_LENGTH];
int cnt = 0;
int cnt2 = 0;

char picture_filename[24];
time_t now;


/* Metoda s³u¿¹ca do obs³ugi zaznaczania obszaru, który widoczny jest w ramce w trakcie kalibracji.  */
void areaSelector(int event, int x, int y, int flags, void* param)
{
    /* wciœniêcie lewego przycisku myszki  */
    if (event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        pt1_calibrate = cvPoint(x, y);
        drag = 1;
    }
    /* w trakcie zaznaczania */
    if (event == CV_EVENT_MOUSEMOVE && drag)
    {
        frame_calibrate = cvCloneImage(frame);
        cvRectangle(frame_calibrate,pt1_calibrate,cvPoint(x, y),CV_RGB(255, 0, 0),1,8,0);
        cvShowImage("VideoOut", frame_calibrate);
    }
    /* puszczeie lewego przycisku myszki */
    if (event == CV_EVENT_LBUTTONUP && drag)
    {
		pt2_calibrate = cvPoint(x,y);
        drag = 0;
    }

    /* restetowanie zaznaczenia (prawy przycisk myszy) */
    if (event == CV_EVENT_RBUTTONUP)
    {
        drag = 0;
    }
}

// Get thresholded image in HSV format  
IplImage* GetThresholdedImageHSV( IplImage* img )  
{  
	frame_gaussian = cvCloneImage(img);
	cvSmooth(frame_gaussian, frame_gaussian, CV_GAUSSIAN,3,3); 
    // Create an HSV format image from image passed  
    IplImage* imgHSV = cvCreateImage( cvGetSize( frame_gaussian ),   
                                      8,   
                                      3 );     
  
    cvCvtColor( frame_gaussian, imgHSV, CV_BGR2HSV );  
    // Create binary thresholded image acc. to max/min HSV ranges  
    // For detecting blue gloves in "MOV.MPG - HSV mode  
    IplImage* imgThresholded = cvCreateImage( cvGetSize( frame_gaussian ),   
                                         8,   
                                         1 );             
  
	cvInRangeS(imgHSV, cvScalar(lowestH,lowestS,lowestV), cvScalar(highestH,highestS,highestV), imgThresholded); 
	cvErode( imgThresholded, imgThresholded, element, 2);
	cvDilate( imgThresholded, imgThresholded, element, 2);
    // Tidy up and return thresholded image  
	cvReleaseImage( &frame_gaussian ); 
    cvReleaseImage( &imgHSV );  
    return imgThresholded;  
}  

void setWindowsHSVCalibration(){
 cvNamedWindow("Trackbars");
 cvNamedWindow("Thresholded");
 
 cvCreateTrackbar("Dolna granica H", "Trackbars", &lowestH, 180, NULL);
        cvCreateTrackbar("Górna granica H", "Trackbars", &highestH, 180, NULL);

  cvCreateTrackbar("Dolna granica S", "Trackbars", &lowestS, 256, NULL);
        cvCreateTrackbar("Górna granica S", "Trackbars", &highestS, 256, NULL);

  cvCreateTrackbar("Dolna granica V", "Trackbars", &lowestV, 256, NULL);
        cvCreateTrackbar("Górna granica V", "Trackbars", &highestV, 256, NULL); 
}



int start()	{
	cvNamedWindow( "VideoOut" );  
	for(int i=0;i<CNT_LENGTH;i++)
	{
		width[i] = 0;
		height[i] = 0;
		x[i] = 0;
		y[i] = 0;
	}

    // Initialize capturing live feed from video file or camera  
    //capture = cvCaptureFromFile( "ujecie1.wmv" );  
    capture = cvCaptureFromCAM( 1 );


    // Get the frames per second  
    /*fps = ( int )cvGetCaptureProperty( capture,  
                                           CV_CAP_PROP_FPS );    */
  
    // Can't get device? Complain and quit  
    if( !capture )  
    {  
        printf( "Nie udalo sie zanicjalizowac pobierania wideo...\n" );  
        return -1;  
    }  

	
	printf("Wcisnij 'c', gdy ramka zostanie ustawiona w polozeniu kalibracyjnym.\n\n");
	
	frame = cvQueryFrame( capture );
	while( key != 'c' )
	{
		frame0 = cvCloneImage(frame);
		frame = cvQueryFrame( capture );   
		cvShowImage("VideoOut", frame);
		key = cvWaitKey(100);
		//key = cvWaitKey( 600 / fps );  
	}


	return 0;
}


int calibrate()	{
	printf("Ustaw suwakami odpowiednie wartosci koloru do wykrywania ramki i wcisnij 'x' by kontynuowac kalibracje.\n\n");
	setWindowsHSVCalibration();
	while( key != 'x' )
	{
		frame_thresholded0 = GetThresholdedImageHSV(frame);
		frame_thresholded = GetThresholdedImageHSV(frame);
		cvOr(frame_thresholded0, frame_thresholded, frame_thresholded);
		cvShowImage("Thresholded", frame_thresholded);
		key = cvWaitKey(10);
	}

	//cvDestroyWindow("Trackbars");

    // Detect the white blobs from the black background  
    blobs = CBlobResult( frame_thresholded, NULL, 0 );    

    // Exclude white blobs smaller than the given value (10)    
    // The bigger the last parameter, the bigger the blobs need  
	// to be for inclusion    
    blobs.Filter( blobs,  
                      B_EXCLUDE,  
                      CBlobGetArea(),  
                      B_LESS,  
                      300 );    
 
	int num_blobs = blobs.GetNumBlobs();  
  
    if(num_blobs > 0)         {
				currentBlob = blobs.GetBlob( 0 );              
                rect = currentBlob->GetBoundingBox(); 



				pt1.x = rect.x + 0.1*rect.width;  
				pt1.y = rect.y + 0.05*rect.height; 
				pt2.x =  pt1.x + 0.8*rect.width;  
				pt2.y= pt1.y + 0.9*rect.height;  
               cvRectangle( frame,  
                         pt1,   
                         pt2,  
                         cvScalar(0, 0, 0, 0),  
                         1,  
                         8,  
                         0 );  
        } 
	else	{
		printf("Nie wykryto ramki.\n");
		return -1;
	}

	printf("Zaznacz obszar, ktory byl widoczny w ramce w momencie rozpoczecia kalibracji i wcisnij 'c' by kontynuowac kalibracje.\n");

	while( key != 'c' )
	{
		cvSetMouseCallback("VideoOut", areaSelector, NULL);
		key = cvWaitKey(10);
	}


	pt1xC = double(pt1_calibrate.x - pt1.x)/sqrt(double(rect.width*rect.height));
	pt1yC = double(pt1_calibrate.y - pt1.y)/sqrt(double(rect.width*rect.height));

	widthB = double((pt2_calibrate.x-pt1_calibrate.x))/double(rect.width);
	heightB = widthB; 

	printf("pt1xC %f  pt1yC %f\n", pt1xC, pt1yC);
	printf("widthB %f  heightB %f\n\n", widthB, heightB);
		 
	key = cvWaitKey( 1000 ); 

	return 0;

}

int process()	{
	if( !( frame = cvQueryFrame( capture ) ) )  {
		return -1;
	}
	frame = cvQueryFrame( capture );
    frame_thresholded1 = GetThresholdedImageHSV( frame );   


    // An infinite loop  
    while( key != 'x' ) 
    { 
        // If we couldn't grab a frame... quit  
        if( !( frame = cvQueryFrame( capture ) ) )	{ 
			break;        
		}
        // Get object's thresholded image (blue = white, rest = black)  

		cvReleaseImage(&frame_thresholded0);
		frame_thresholded0 = cvCloneImage(frame_thresholded1);

		cvReleaseImage(&frame_thresholded1);
		frame_thresholded1 = GetThresholdedImageHSV(frame);   

		cvReleaseImage(&frame_thresholded);
		frame_thresholded = cvCloneImage(frame_thresholded1);


		cvOr(frame_thresholded0, frame_thresholded, frame_thresholded);

        // Detect the white blobs from the black background  
        blobs = CBlobResult( frame_thresholded, NULL, 0 );    

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

            pt1.x = xAvg + 0.1*widthAvg;
            pt1.y = yAvg + 0.05*heightAvg; 
            pt2.x = pt1.x + 0.8*widthAvg;
            pt2.y = pt1.y + 0.9*heightAvg;
  

			pt1_calibrate.x = pt1.x + int(pt1xC * sqrt(double(widthAvg*heightAvg))-3.2); 
			pt1_calibrate.y = pt1.y + int(pt1yC * sqrt(double(widthAvg*heightAvg))-2.4); 
			/*pt1_calibrate.x = xAvg + pt1xB;
			pt1_calibrate.y = yAvg +  pt1yB;*/
			pt2_calibrate.x =  pt1_calibrate.x + int(widthB*widthAvg+6.4);  
			pt2_calibrate.y= pt1_calibrate.y + int(heightB*heightAvg+4.8);  
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
        }  else if(cnt==CNT_LENGTH)	{
			cnt2++;
			if(cnt2 >= CNT_RESET)	{
				now = time(NULL);
				strftime(picture_filename, 24, "%Y%m%d%H%M%S", localtime(&now));
				strcat(picture_filename,".jpg");
				printf(picture_filename);

				cvSetImageROI(frame,  cvRect( pt1_calibrate.x, pt1_calibrate.y,pt2_calibrate.x, pt2_calibrate.y) ); 
				photo = cvCloneImage(frame); 
				cvResetImageROI( frame ); 

				cvSaveImage(picture_filename, photo);

				cvReleaseImage(&photo);
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
        cvShowImage( "Thresholded", frame_thresholded );  
        cvShowImage( "VideoOut", frame );  
  
        // Optional - used to slow up the display of frames  
        //key = cvWaitKey( 1000 / fps ); 
		key = cvWaitKey( 100);
    }  
	return 0;
}

int main()  
{ 


	if(start()==-1)	{
		return -1;
	}

	if(calibrate()==-1)	{
		return -1;
	}

	if(process()==-1)	{
		return -1;
	}
       

  
    // We're through with using camera.   
    cvReleaseCapture( &capture );  

    return 0;  
}  
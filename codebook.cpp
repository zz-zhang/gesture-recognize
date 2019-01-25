#include "codebook.h"
#include "mousecolor.h"
/**********************************************************************************/


/**********************************************************************************/


/**********************************************************************************/
// int updateCodeBook( uchar* p, codeBook &c, unsigned* cbBounds, int numChannels )
// Updates the codebook entry with a new data point
//
// p			Pointer to a YUV pixel
// c			Codebook for this pixel
// cbBounds		Learning bounds for codebook (Rule of thumb: 10)
// numChannels	Number of color channels we're learning
//
// NOTES		cbBounds must be of size cbBounds[numChannels]
//		
// RETURN		codebook index
/**********************************************************************************/
int cvupdateCodeBook(uchar* p, codeBook &c, unsigned* cbBounds, int numChannels)
{
	if (c.numEntries == 0)					// �뱾����ԪΪ��ʱ����main������һ��ʼΪ��
		c.t = 0;							// ��ʼ��ʱ��Ϊ0	
	c.t += 1;								// Record learning event
	// �뱾ʱ�䣬��¼ѧϰ�Ĵ�����ÿ����һ�μ�һ����ÿһ֡ͼ���һ	
	/*SET HIGH AND LOW BOUNDS*/
	int n;
	unsigned int high[3], low[3];
	for (n = 0; n<numChannels; n++)			//������ͨ��
	{
		high[n] = *(p + n) + *(cbBounds + n);	// *(p+n) �� p[n] ����ȼۣ�������*(p+n)�ٶȸ���
		if (high[n]>255)					// high[n] = p[n] + cbBounds[n]��������ֵ
			high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);		// low[n] = p[n] - cbBounds[n]��������ֵ
		if (low[n]<0)
			low[n] = 0;						// ��p ��ָ����ͨ������,�Ӽ�cbBonds����ֵ,��Ϊ�����ط�ֵ��������	
	}
	/*SEE IF THIS FITS AN EXISTING CODEWORD*/
	int matchChannel;						// ����p������Ԫ��ͨ����
	int i;									// �뱾����Ԫ������
	for (i = 0; i<c.numEntries; i++)			// �������뱾ÿ����Ԫ,����p�����Ƿ���������֮һ
	{
		matchChannel = 0;
		for (n = 0; n<numChannels; n++)		// ����ÿ��ͨ��			
		{
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n]))	// Found an entry for this channel
				// �뱾c�ĵ�i����Ԫ��learnlow[n] <= p[n] <= �뱾c�ĵ�i����Ԫ��learnhigh[n]
				// �����p����ͨ�������ڸ���Ԫ��ֵ������֮��
				matchChannel++;														// ���ÿ��ͨ�������ϣ���matchChannel = numChannels
		}
		if (matchChannel == numChannels)	// If an entry was found over all channels
			// ���p ���ظ�ͨ����������������
		{
			c.cb[i]->t_last_update = c.t;	// ���¸���Ԫʱ��Ϊ�뱾ʱ�䣬����ǰʱ��
			for (n = 0; n<numChannels; n++)			//��ÿһͨ������������Ԫ�����Сֵ		
			{
				if (c.cb[i]->max[n] < *(p + n))		//�������p������Ԫ��max������Ԫ��max��ֵΪp
					c.cb[i]->max[n] = *(p + n);
				else if (c.cb[i]->min[n] > *(p + n))	//�������pС����Ԫ��min������Ԫ��min��ֵΪp
					c.cb[i]->min[n] = *(p + n);
			}
			break;							// �������������뱾ÿ����Ԫ�����ѭ����������p��ͨ���������뱾��ĳһ��Ԫ�����ñ������µ���Ԫ
		}									// ��ʱ��i<c.numEntries
	}
	/*ENTER A NEW CODE WORD IF NEEDED*/
	if (i == c.numEntries)					// No existing code word found, make a new one
		// p ���ز�������뱾���κ�һ����Ԫ,���洴��һ������Ԫ
	{
		code_element **foo = new code_element*[c.numEntries + 1];	// Ϊc.numEntries+1��ָ����Ԫ�����ָ�����ռ䣬��ԭ�뱾����Ԫ������1��								
		for (int ii = 0; ii<c.numEntries; ii++)
			foo[ii] = c.cb[ii];										// ��ԭ�뱾����Ԫ��������Ԫ����ǰc.numEntries��ָ��ָ���·����ÿ����Ԫ		
		foo[c.numEntries] = new code_element;						// Ϊ���һ������Ԫ����ռ�	
		if (c.numEntries)
			delete[] c.cb;					// ɾ��c.cb ָ�����飨ע�⣺delete[]��new[]���Ӧʹ�ã�		
		c.cb = foo;							// ��foo ͷָ�븳��c.cb	
		for (n = 0; n<numChannels; n++)		// ��������Ԫ��ͨ������		
		{
			c.cb[c.numEntries]->learnHigh[n] = high[n];		// ����Ԫ��learnhighΪ������ֵ
			c.cb[c.numEntries]->learnLow[n] = low[n];		// learnlowΪ������ֵ
			c.cb[c.numEntries]->max[n] = *(p + n);			// max��minΪ����p��ֵ
			c.cb[c.numEntries]->min[n] = *(p + n);
		}
		c.cb[c.numEntries]->t_last_update = c.t;			// ����Ԫʱ������Ϊ�뱾ʱ��
		c.cb[c.numEntries]->stale = 0;
		c.numEntries += 1;									// ������ı���Ԫ����
	}
	/*OVERHEAD TO TRACK POTENTIAL STALE ENTRIES*/
	for (int s = 0; s<c.numEntries; s++)
	{
		int negRun = c.t - c.cb[s]->t_last_update;	// This garbage is to track which codebook entries are going stale
		// �������Ԫ�Ĳ�����ʱ��
		if (c.cb[s]->stale < negRun)
			c.cb[s]->stale = negRun;
	}
	/*SLOWLY ADJUST LEARNING BOUNDS*/
	for (n = 0; n<numChannels; n++)					// �������ͨ�������ڸߵͷ�ֵ��Χ��,������Ԫ��ֵ֮��,������������Ԫѧϰ����		
	{
		if (c.cb[i]->learnHigh[n] < high[n])
			c.cb[i]->learnHigh[n] += 1;
		if (c.cb[i]->learnLow[n] > low[n])
			c.cb[i]->learnLow[n] -= 1;
	}

	return(i);
}


/**********************************************************************************/
// uchar cvbackgroundDiff( uchar* p, codeBook &c, int minMod, int maxMod )
// Given a pixel and a code book, determine if the pixel is covered by the codebook
//
// p			pixel pointer (YUV interleaved)
// c			codebook reference
// numChannels  Number of channels we are testing
// maxMod		Add this (possibly negative) number onto max level when code_element determining if new pixel is foreground
// minMod		Subract this (possible negative) number from min level code_element when determining if pixel is foreground
//
// NOTES		minMod and maxMod must have length numChannels, e.g. 3 channels => minMod[3], maxMod[3].
// 
// Return		0 => background, 255 => foreground
/**********************************************************************************/
uchar cvbackgroundDiff(uchar* p, codeBook &c, int numChannels, int* minMod, int* maxMod)
{
	int matchChannel;							// ���沽��ͱ���ѧϰ�в�����Ԫ���һ��
	/*SEE IF THIS FITS AN EXISTING CODEWORD*/
	int i;
	for (i = 0; i<c.numEntries; i++)
	{
		matchChannel = 0;
		for (int n = 0; n<numChannels; n++)
		{
			if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->max[n] + maxMod[n]))
				matchChannel++;					// Found an entry for this channel
			else
				break;							// �����һͨ�������ϣ�������forѭ��
		}
		if (matchChannel == numChannels)		// �����i����Ԫ����ͨ��������(�������Ԫ���ü����)��������forѭ������ʱi<c.numEntries
			break;								// Found an entry that matched all channels
	}
	if (i == c.numEntries)						// ��ʱû��һ����Ԫ���ϣ���֤����ǰ��������255����ɫ��							
		return(255);
	return(0);
}


//UTILITES//////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
// int clearStaleEntries( codeBook &c )
// After you've learned for some period of time, periodically call this to clear 
// out stale codebook entries
//
// c			Codebook to clean up
//
// Return		number of entries cleared
/**********************************************************************************/
int cvclearStaleEntries(codeBook &c)
{
	int staleThresh = c.t >> 1;				// �趨ˢ��ʱ��
	int* keep = new int[c.numEntries];		// ����һ��������飬����Ԫ����ĿΪ�뱾����Ԫ�ĸ���
	int keepCnt = 0;						// ��¼��ɾ����Ԫ��Ŀ
	// SEE WHICH CODEBOOK ENTRIES ARE TOO STALE
	for (int i = 0; i<c.numEntries; i++)		// �����뱾��ÿ����Ԫ		
	{
		if (c.cb[i]->stale > staleThresh)	// ����Ԫ�еĲ�����ʱ������趨��ˢ��ʱ��,����Ϊɾ��
			keep[i] = 0;					// Mark for destruction�����
		else
		{
			keep[i] = 1;					// Mark to keep
			keepCnt += 1;					// ��¼��ɾ����Ԫ��Ŀ
		}
	}
	/*KEEP ONLY THE GOOD*/
	c.t = 0;								// Full reset on stale tracking
	// �뱾ʱ������
	code_element **foo = new code_element*[keepCnt];	// �����СΪkeepCnt ����Ԫָ������
	int k = 0;
	for (int ii = 0; ii<c.numEntries; ii++)
	{
		if (keep[ii])						// ���keep[ii] = 0�򲻽��룬��ӦҪɾ������Ԫ
		{
			foo[k] = c.cb[ii];
			foo[k]->stale = 0;				// We have to refresh these entries for next clearStale
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	/*CLEAN UP*/
	delete[] keep;
	delete[] c.cb;
	c.cb = foo;								// ��foo ͷָ���ַ����c.cb 
	int numCleared = c.numEntries - keepCnt;// ���������Ԫ����
	c.numEntries = keepCnt;					// ʣ�����Ԫ����
	delete keep;
	return(numCleared);						// ���ر��������Ԫ����
}


/**********************************************************************************/
// void cvconnectedComponents( IplImage* mask, int poly1_hull0, float perimScale, int* num, CvRect* bbs, CvPoint* centers )
// This cleans up the forground segmentation mask derived from calls to cvbackgroundDiff
//
// mask			Is a grayscale (8 bit depth) "raw" mask image which will be cleaned up
//
// OPTIONAL PARAMETERS:
// poly1_hull0	If set, approximate connected component by (DEFAULT) polygon, or else convex hull (0)
// perimScale 	Len = image (width+height)/perimScale.  If contour len < this, delete that contour (DEFAULT: 4)
// num			Maximum number of rectangles and/or centers to return, on return, will contain number filled (DEFAULT: NULL)
// bbs			Pointer to bounding box rectangle vector of length num.  (DEFAULT SETTING: NULL)
// centers		Pointer to contour centers vectore of length num (DEFULT: NULL)
/**********************************************************************************/
void cvconnectedComponents(IplImage* mask, int poly1_hull0, float perimScale, int* num, CvRect* bbs, CvPoint* centers)
{
	static CvMemStorage* mem_storage = NULL;
	static CvSeq* contours = NULL;
	/*CLEAN UP RAW MASK*/
	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_OPEN, CVCLOSE_ITR);	// ��mask���п����㣨���������Ĺ����㣩
	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_CLOSE, CVCLOSE_ITR);// ��mask���б����㣨���������Ĺ����㣩

	//morphologyEx(mask, mask, MORPH_OPEN, NULL);	// ��mask���п����㣨���������Ĺ����㣩
	//morphologyEx(mask, mask, MORPH_CLOSE, NULL);// ��mask���б����㣨���������Ĺ����㣩


	/*FIND CONTOURS AROUND ONLY BIGGER REGIONS*/
	if (mem_storage == NULL)
		mem_storage = cvCreateMemStorage(0);
	else
		cvClearMemStorage(mem_storage);



	CvContourScanner scanner = cvStartFindContours(mask,
		mem_storage,
		sizeof(CvContour),
		CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_SIMPLE);	// �ú���ÿ�η���һ������
	CvSeq* c;
	int numCont = 0;
	while ((c = cvFindNextContour(scanner)) != NULL)						// ����ʣ��������һֱѭ����ֱ��Ϊ��
	{
		double len = cvContourPerimeter(c);								// �����������ܳ�
		double q = (mask->height + mask->width) / perimScale;				// calculate perimeter len threshold
		// ���������ܳ�����ֵ
		if (len<q)														// Get rid of blob if it's perimeter is too small
			cvSubstituteContour(scanner, NULL);						// ���������ܳ���С������
		else															// Smooth it's edges if it's large enough
		{
			CvSeq* c_new;
			if (poly1_hull0)											// Polygonal approximation of the segmentation
				c_new = cvApproxPoly(c,								// ��poly1_hull0Ϊ1������ж���αƽ�
				sizeof(CvContour),
				mem_storage,
				CV_POLY_APPROX_DP,
				CVCONTOUR_APPROX_LEVEL,			// �������αƽ��ľ���
				0);
			else														// Convex Hull of the segmentation
				c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);	// ��Ϊ0�������hull�ز���
			cvSubstituteContour(scanner, c_new);						// �´���������ȡ��ԭ����
			numCont++;
			//delete c_new;
		}
	}
	contours = cvEndFindContours(&scanner);
	/*PAINT THE FOUND REGIONS BACK INTO THE IMAGE*/
	cvZero(mask);
	IplImage* maskTemp;
	/*CALC CENTER OF MASS AND OR BOUNDING RECTANGLES�����num�ǿվͼ���ĳЩ����*/
	if (num != NULL)
	{
		int N = *num, numFilled = 0, i = 0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);
		for (i = 0, c = contours; c != NULL; c = c->h_next, i++)
		{
			if (i<N)													// Only process up to *num of them
			{
				cvDrawContours(maskTemp, c, CV_CVX_WHITE, CV_CVX_WHITE, -1, CV_FILLED, 8);
				/*Find the center of each contour�����center�ǿվͼ���ͼ������*/
				if (centers != NULL)
				{
					cvMoments(maskTemp, &moments, 1);
					M00 = cvGetSpatialMoment(&moments, 0, 0);
					M10 = cvGetSpatialMoment(&moments, 1, 0);
					M01 = cvGetSpatialMoment(&moments, 0, 1);
					centers[i].x = (int)(M10 / M00);						// ͨ�����ľؼ���ͼ�������
					centers[i].y = (int)(M01 / M00);
				}
				/*Bounding rectangles around blobs�����bbs�ǿվͼ��������ı߽��*/
				if (bbs != NULL)
				{
					bbs[i] = cvBoundingRect(c);							// ����߽��
				}
				cvZero(maskTemp);
				numFilled++;
			}
			/*Draw filled contours into mask*/
			cvDrawContours(mask, c, CV_CVX_WHITE, CV_CVX_WHITE, -1, CV_FILLED, 8);
			//draw to central mask
		}	//end looping over contours
		*num = numFilled;
		cvReleaseImage(&maskTemp);
	}
	/*ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK�����numΪ����ֻ�������Ϳ�����*/
	else
	{
		for (c = contours; c != NULL; c = c->h_next)
		{
			cvDrawContours(mask, c, CV_CVX_WHITE, CV_CVX_BLACK, -1, CV_FILLED, 8);
		}
	}
	cvReleaseMemStorage(&mem_storage);
	delete c;

}


int CodeBook()
{
	VideoCapture capture(0);

	IplImage*	rawImage;
	IplImage*	yuvImage;
	IplImage*	ImaskCodeBook;
	IplImage*	ImaskSkin;
	Mat frame;

	capture >> frame;

	codeBook*	cB;
	unsigned	cbBounds[CHANNELS];
	uchar*		pColor;					//YUV pointer
	int			imageLen;
	int			nChannels = CHANNELS;
	int			minMod[CHANNELS];
	int			maxMod[CHANNELS];

	/*��ʼ��������������ͷ����Ӱ��*/
	//cvNamedWindow("Raw");
	//cvNamedWindow("CodeBook");
	//capture = cvCreateCameraCapture(0);


	Mat codeBookMat(frame.rows, frame.cols, CV_8UC1);
	Mat skinMat(frame.rows, frame.cols, CV_8UC1);
	Mat frameHSV(frame.rows, frame.cols, CV_8UC1);
	/*if (!capture){
		printf("Couldn't open the capture!");
		return -1;
	}*/
	rawImage = cvCreateImage(cvSize(frame.cols, frame.rows), 8, 3);
	rawImage->imageData = (char*)frame.data;								// ��Ӱ���л�ȡÿһ֡��ͼ��
	cvFlip(rawImage, rawImage, 1);
	yuvImage = cvCreateImage(cvGetSize(rawImage), 8, 3);					// ��yuvImage ����һ����rawImage �ߴ���ͬ,8λ3ͨ��ͼ��
	

	
	ImaskCodeBook = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);	// ΪImaskCodeBook ����һ����rawImage �ߴ���ͬ,8λ��ͨ��ͼ��
	ImaskSkin = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
	cvSet(ImaskCodeBook, cvScalar(255));									// ���õ�ͨ����������Ԫ��Ϊ255,����ʼ��Ϊ��ɫͼ��
	imageLen = rawImage->width * rawImage->height;							// Դͼ���������༴���ظ���
	cB = new codeBook[imageLen];											// �õ���ͼ��������Ŀ����һ����һ���뱾,�Ա��ÿ�����ؽ��д���
	for (int i = 0; i<imageLen; i++)
		cB[i].numEntries = 0;			// ��ʼ��ÿ���뱾����Ԫ��ĿΪ0����imageLen���뱾��ÿһ�����ض�Ӧһ���뱾
	for (int i = 0; i<nChannels; i++){
		cbBounds[i] = CBBOUNDS;				// ����ȷ����Ԫ��ͨ������ֵ
		minMod[i] = MINMOD;				// ���ڱ�����ֺ�����
		maxMod[i] = MAXMOD;				// ������ֵ�Դﵽ��õķָ�
	}

	/*��ʼ������Ƶÿһ֡ͼ��*/
	for (int i = 0;; i++){								// û������ѭ����������ѭ��
		cvCvtColor(rawImage, yuvImage, CV_BGR2YCrCb);	// ɫ�ʿռ�ת��,��rawImage ת����YUVɫ�ʿռ�,�����yuvImage
		// ��ʹ��ת��Ч����Ȼ�ܺ�
		//yuvImage = cvCloneImage(rawImage);
		if (i <= 50){								// 30֡�ڽ��б���ѧϰ			
			pColor = (uchar*)(yuvImage->imageData);		// pColorָ��ָ��yuvImageͼ���׵�ַ	
			for (int c = 0; c<imageLen; c++){
				cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);	// ��ͼ���ÿ������,���ô˺���,��׽��������ر仯ͼ��		
				// ��ÿһ����pColor�����ö�Ӧ���뱾cB[c]
				pColor += 3;											// 3ͨ��ͼ��, ָ����һ�����صĵ�һͨ�����ݣ��ں����ж�nͨ�����д���	
			}
			if (i == 50){											// ��30֡ʱ�������溯��,ɾ���뱾�г¾ɵ���Ԫ	
				for (int c = 0; c<imageLen; c++)
					cvclearStaleEntries(cB[c]);						// ���������뱾��ɾ��ÿһ���뱾�г¾ɵ���Ԫ
			}
		}
		else
		{
			uchar maskPixelCodeBook;								// 30֡����
			pColor = (uchar*)((yuvImage)->imageData);				// 3 channel yuv image
			uchar* pMask = (uchar*)((ImaskCodeBook)->imageData);	// 1 channel image
			// pMaskָ��ImaskCodeBookͼ����׵�ַ
			for (int c = 0; c<imageLen; c++)
			{
				maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, minMod, maxMod);
				// ����������ÿһ�������ж��Ƿ�Ϊǰ������ɫ������������ɫ��
				*pMask++ = maskPixelCodeBook;						// pMaskָ��ָ���Ԫ�أ����Լӣ��ٸ�ֵ
				// ����maskPixelCodeBook��ֵ����ImaskCodeBookͼ�񣨵�ͨ����
				pColor += 3;										// pColor ָ�����3ͨ��ͼ��																
			}
		}

	

		capture >> frame;
		rawImage->imageData = (char*)frame.data;
		//if (!(rawImage = cvQueryFrame(capture)))					// Ӱ�񲥷���ϣ�����forѭ��
		//	break;
		cvFlip(rawImage, rawImage, 1);
		cvconnectedComponents(ImaskCodeBook, 1, 4, NULL, NULL, NULL);
		codeBookMat.data = (uchar*)ImaskCodeBook->imageData;
		// ��ͨ����������

		
		cvtColor(frame, frameHSV, CV_BGR2HSV);
		Mat dstTemp1(frame.rows, frame.cols, CV_8UC1);
		Mat dstTemp2(frame.rows, frame.cols, CV_8UC1);
		inRange(frameHSV, Scalar(0, 30, 30), Scalar(40, 170, 256), dstTemp1);
		inRange(frameHSV, Scalar(156, 30, 30), Scalar(180, 170, 256), dstTemp2);
		bitwise_or(dstTemp1, dstTemp2, skinMat);
		ImaskSkin->imageData = (char*)skinMat.data;
		cvconnectedComponents(ImaskSkin, 1, 4, NULL, NULL, NULL);
		skinMat.data = (uchar*)ImaskSkin->imageData;
		//��ɫ��ֵ+ȥ��
		

		Mat codeBookSkin(frame.rows, frame.cols, CV_8UC1);
		bitwise_and(skinMat, codeBookMat, codeBookSkin);

		cvSetMouseCallback("Raw", cvmovecallback, (void*)rawImage);
		imshow("Raw", frame);								// ѭ����ʾͼƬ��������Ӱ��
		//imshow("SkinThreshold", skinMat);
		//imshow("CodeBook", codeBookMat);
		imshow("Result", codeBookSkin);

		if (cvWaitKey(1) == 27)									// ��ESC���˳�
			break;
	}

	/*�ͷ��ڴ棬���ٴ���*/
	//cvReleaseCapture(&capture);
	if (yuvImage)
		cvReleaseImage(&yuvImage);
	if (ImaskCodeBook)
		cvReleaseImage(&ImaskCodeBook);
	if (rawImage)
		cvReleaseImage(&yuvImage);
	if (ImaskSkin)
		cvReleaseImage(&ImaskSkin);
	cvDestroyAllWindows();
	delete[] cB;
	
	return 0;
}

void CodeBoookLearn(IplImage *yuvImage, uchar* pColor, codeBook* cB, unsigned* cbBounds, int imageLen, int nChannels){
	pColor = (uchar*)(yuvImage->imageData);		// pColorָ��ָ��yuvImageͼ���׵�ַ	
	for (int c = 0; c < imageLen; c++){
		cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);	// ��ͼ���ÿ������,���ô˺���,��׽��������ر仯ͼ��		
		// ��ÿһ����pColor�����ö�Ӧ���뱾cB[c]
		pColor += 3;											// 3ͨ��ͼ��, ָ����һ�����صĵ�һͨ�����ݣ��ں����ж�nͨ�����д���	
	}
}

void BgSub(IplImage* yuvImage, IplImage* ImaskCodeBook,uchar* pColor, codeBook* cB, int imageLen, int nChannels, int* minMod, int* maxMod){
	uchar maskPixelCodeBook;								// 50֡����
	pColor = (uchar*)((yuvImage)->imageData);				// 3 channel yuv image
	uchar* pMask = (uchar*)((ImaskCodeBook)->imageData);	// 1 channel image
	// pMaskָ��ImaskCodeBookͼ����׵�ַ
	for (int c = 0; c<imageLen; c++){
		maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, minMod, maxMod);
		// ����������ÿһ�������ж��Ƿ�Ϊǰ������ɫ������������ɫ��
		*pMask++ = maskPixelCodeBook;						// pMaskָ��ָ���Ԫ�أ����Լӣ��ٸ�ֵ
		// ����maskPixelCodeBook��ֵ����ImaskCodeBookͼ�񣨵�ͨ����
		pColor += 3;										// pColor ָ�����3ͨ��ͼ��																
	}
}

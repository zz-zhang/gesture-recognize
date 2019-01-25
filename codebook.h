#ifndef CODEBOOK_H
#define CODEBOOK_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//设置处理的图像通道数,要求小于等于图像本身的通道数
#define CHANNELS 3		
//某些颜色的宏定义
#define CV_CVX_WHITE	CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK	CV_RGB(0x00,0x00,0x00)


#define CBBOUNDS 10			// 用于确定码元各通道的阈值 初始10
#define MINMOD  5			// 用于背景差分函数中		初始20
#define MAXMOD  5			// 调整其值以达到最好的分割	初始20

/**********************************************************************************/
//For connected components:
#define CVCONTOUR_APPROX_LEVEL  2   // Approx.threshold - the bigger it is, the simpler is the boundary
#define CVCLOSE_ITR  1				// How many iterations of erosion and/or dialation there should be
//下面为码本码元的数据结构
//处理图像时每个像素对应一个码本code_book,每个码本中可有若干个码元code_element
typedef struct ce
{
	uchar learnHigh[CHANNELS];	// High side threshold for learning
	// 此码元各通道的阀值上限(学习界限)
	uchar learnLow[CHANNELS];	// Low side threshold for learning
	// 此码元各通道的阀值下限
	// 学习过程中如果一个新像素各通道值x[i],均有 learnLow[i]<=x[i]<=learnHigh[i],则该像素可合并于此码元
	uchar max[CHANNELS];		// High side of box boundary
	// 属于此码元的像素中各通道的最大值
	uchar min[CHANNELS];		// Low side of box boundary
	// 属于此码元的像素中各通道的最小值
	int	t_last_update;			// This is book keeping to allow us to kill stale entries
	// 此码元最后一次更新的时间,每一帧为一个单位时间,用于计算stale
	int	stale;					// max negative run (biggest period of inactivity)
	// 此码元最长不更新时间,用于删除规定时间不更新的码元,精简码本
} code_element;					// 码元的数据结构

typedef struct code_book
{
	code_element **cb;			// 码元的二维指针,理解为指向码元指针数组的指针,使得添加码元时不需要来回复制码元,只需要简单的指针赋值即可
	int	numEntries;				// 此码本中码元的数目	
	int	t;						// 此码本现在的时间,一帧为一个时间单位；记录从开始或最后一次清除操作之间累积的像素点的数目
} codeBook;						// 码本的数据结构
/**********************************************************************************/


int cvupdateCodeBook(uchar* p, codeBook &c, unsigned* cbBounds, int numChannels);
uchar cvbackgroundDiff(uchar* p, codeBook &c, int numChannels, int* minMod, int* maxMod);
int cvclearStaleEntries(codeBook &c);
void cvconnectedComponents(IplImage* mask, int poly1_hull0, float perimScale, int* num, CvRect* bbs, CvPoint* centers);
int CodeBook();
void CodeBoookLearn(IplImage *yuvImage, uchar* pColor, codeBook* cB, unsigned* cbBounds, int imageLen, int nChannels);
void BgSub(IplImage* yuvImage, IplImage* ImaskCodeBook, uchar* pColor, codeBook* cB, int imageLen, int nChannels, int* minMod, int* maxMod);
#endif
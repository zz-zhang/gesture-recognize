#ifndef CODEBOOK_H
#define CODEBOOK_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//���ô����ͼ��ͨ����,Ҫ��С�ڵ���ͼ�����ͨ����
#define CHANNELS 3		
//ĳЩ��ɫ�ĺ궨��
#define CV_CVX_WHITE	CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK	CV_RGB(0x00,0x00,0x00)


#define CBBOUNDS 10			// ����ȷ����Ԫ��ͨ������ֵ ��ʼ10
#define MINMOD  5			// ���ڱ�����ֺ�����		��ʼ20
#define MAXMOD  5			// ������ֵ�Դﵽ��õķָ�	��ʼ20

/**********************************************************************************/
//For connected components:
#define CVCONTOUR_APPROX_LEVEL  2   // Approx.threshold - the bigger it is, the simpler is the boundary
#define CVCLOSE_ITR  1				// How many iterations of erosion and/or dialation there should be
//����Ϊ�뱾��Ԫ�����ݽṹ
//����ͼ��ʱÿ�����ض�Ӧһ���뱾code_book,ÿ���뱾�п������ɸ���Ԫcode_element
typedef struct ce
{
	uchar learnHigh[CHANNELS];	// High side threshold for learning
	// ����Ԫ��ͨ���ķ�ֵ����(ѧϰ����)
	uchar learnLow[CHANNELS];	// Low side threshold for learning
	// ����Ԫ��ͨ���ķ�ֵ����
	// ѧϰ���������һ�������ظ�ͨ��ֵx[i],���� learnLow[i]<=x[i]<=learnHigh[i],������ؿɺϲ��ڴ���Ԫ
	uchar max[CHANNELS];		// High side of box boundary
	// ���ڴ���Ԫ�������и�ͨ�������ֵ
	uchar min[CHANNELS];		// Low side of box boundary
	// ���ڴ���Ԫ�������и�ͨ������Сֵ
	int	t_last_update;			// This is book keeping to allow us to kill stale entries
	// ����Ԫ���һ�θ��µ�ʱ��,ÿһ֡Ϊһ����λʱ��,���ڼ���stale
	int	stale;					// max negative run (biggest period of inactivity)
	// ����Ԫ�������ʱ��,����ɾ���涨ʱ�䲻���µ���Ԫ,�����뱾
} code_element;					// ��Ԫ�����ݽṹ

typedef struct code_book
{
	code_element **cb;			// ��Ԫ�Ķ�άָ��,���Ϊָ����Ԫָ�������ָ��,ʹ�������Ԫʱ����Ҫ���ظ�����Ԫ,ֻ��Ҫ�򵥵�ָ�븳ֵ����
	int	numEntries;				// ���뱾����Ԫ����Ŀ	
	int	t;						// ���뱾���ڵ�ʱ��,һ֡Ϊһ��ʱ�䵥λ����¼�ӿ�ʼ�����һ���������֮���ۻ������ص����Ŀ
} codeBook;						// �뱾�����ݽṹ
/**********************************************************************************/


int cvupdateCodeBook(uchar* p, codeBook &c, unsigned* cbBounds, int numChannels);
uchar cvbackgroundDiff(uchar* p, codeBook &c, int numChannels, int* minMod, int* maxMod);
int cvclearStaleEntries(codeBook &c);
void cvconnectedComponents(IplImage* mask, int poly1_hull0, float perimScale, int* num, CvRect* bbs, CvPoint* centers);
int CodeBook();
void CodeBoookLearn(IplImage *yuvImage, uchar* pColor, codeBook* cB, unsigned* cbBounds, int imageLen, int nChannels);
void BgSub(IplImage* yuvImage, IplImage* ImaskCodeBook, uchar* pColor, codeBook* cB, int imageLen, int nChannels, int* minMod, int* maxMod);
#endif
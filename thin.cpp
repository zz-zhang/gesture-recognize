#include "thin.h"

void thin(const InputArray _src, OutputArray _dst, const int iterations = 1)
{
	Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	Mat dst = _dst.getMat();


	const int height = src.rows - 1;
	const int width = src.cols - 1;

	//拷贝一个数组给另一个数组
	if (src.data != dst.data)
	{
		src.copyTo(dst);
	}


	int n = 0, i = 0, j = 0;
	Mat tmpImg;
	uchar *pU, *pC, *pD;
	bool isFinished = false;

	for (n = 0; n<iterations; n++)
	{
		dst.copyTo(tmpImg);
		isFinished = false;   //一次 先行后列扫描 开始
		//扫描过程一 开始
		for (i = 1; i<height; i++)
		{
			pU = tmpImg.ptr<uchar>(i - 1);
			pC = tmpImg.ptr<uchar>(i);
			pD = tmpImg.ptr<uchar>(i + 1);
			for (int j = 1; j<width; j++)
			{
				if (pC[j] > 0)
				{
					int ap = 0;
					int p2 = (pU[j] >0);
					int p3 = (pU[j + 1] >0);
					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}
					int p4 = (pC[j + 1] >0);
					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}
					int p5 = (pD[j + 1] >0);
					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}
					int p6 = (pD[j] >0);
					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}
					int p7 = (pD[j - 1] >0);
					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}
					int p8 = (pC[j - 1] >0);
					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}
					int p9 = (pU[j - 1] >0);
					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9)>1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9)<7)
					{
						if (ap == 1)
						{
							if ((p2*p4*p6 == 0) && (p4*p6*p8 == 0))
							{
								dst.ptr<uchar>(i)[j] = 0;
								isFinished = true;
							}

							//   if((p2*p4*p8==0)&&(p2*p6*p8==0))
							//    {                           
							//         dst.ptr<uchar>(i)[j]=0;
							//         isFinished =TRUE;                            
							//    }

						}
					}
				}

			} //扫描过程一 结束


			dst.copyTo(tmpImg);
			//扫描过程二 开始
			for (i = 1; i<height; i++)  //一次 先行后列扫描 开始
			{
				pU = tmpImg.ptr<uchar>(i - 1);
				pC = tmpImg.ptr<uchar>(i);
				pD = tmpImg.ptr<uchar>(i + 1);
				for (int j = 1; j<width; j++)
				{
					if (pC[j] > 0)
					{
						int ap = 0;
						int p2 = (pU[j] >0);
						int p3 = (pU[j + 1] >0);
						if (p2 == 0 && p3 == 1)
						{
							ap++;
						}
						int p4 = (pC[j + 1] >0);
						if (p3 == 0 && p4 == 1)
						{
							ap++;
						}
						int p5 = (pD[j + 1] >0);
						if (p4 == 0 && p5 == 1)
						{
							ap++;
						}
						int p6 = (pD[j] >0);
						if (p5 == 0 && p6 == 1)
						{
							ap++;
						}
						int p7 = (pD[j - 1] >0);
						if (p6 == 0 && p7 == 1)
						{
							ap++;
						}
						int p8 = (pC[j - 1] >0);
						if (p7 == 0 && p8 == 1)
						{
							ap++;
						}
						int p9 = (pU[j - 1] >0);
						if (p8 == 0 && p9 == 1)
						{
							ap++;
						}
						if (p9 == 0 && p2 == 1)
						{
							ap++;
						}
						if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9)>1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9)<7)
						{
							if (ap == 1)
							{
								//   if((p2*p4*p6==0)&&(p4*p6*p8==0))
								//   {                           
								//         dst.ptr<uchar>(i)[j]=0;
								//         isFinished =TRUE;                            
								//    }

								if ((p2*p4*p8 == 0) && (p2*p6*p8 == 0))
								{
									dst.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}

							}
						}
					}

				}

			} //一次 先行后列扫描完成          
			//如果在扫描过程中没有删除点，则提前退出
			if (isFinished == false)
			{
				break;
			}
		}

	}
}
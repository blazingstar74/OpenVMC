#include "VehicleProc.h"
#include "windows.h"
#include "..\include\ItsVehicleTypeRec.h"
typedef void* (*VehicleTypeRec)(void* ,V_Image* ,ITS_Vehicle_Result* );
CVehicleProc::CVehicleProc(void)
{
	pFrame = 0;
	memset(vehinfo,0,sizeof(VehicleInfo));
	vehCnt = 0;
	nfrnum = 0;
}
CVehicleProc::~CVehicleProc(void)
{
	//ITS_VehicleRecRelease(pInstance); //�ͷ�
}
/*˵�������ݳ���λ�û�ó���λ��
/ plateRect��������ͼƬ�е�λ��
/ pVehicleRect:��õĳ���λ��
*/
void GetVehicleRect(S_Rect plateRect, S_Rect* pVehicleRect, int iImageWidth,int iImageHeight)
{
	float fExpTopR = 16;
	float fExpBottomR = 2.0;
	float fExpLefRigR = 2.0; //�������䱶��
	int iPlateHeight = plateRect.iBottom - plateRect.iTop + 1;
	int iPlateWidth = plateRect.iRight - plateRect.iLeft + 1;

	pVehicleRect->iTop = plateRect.iTop - fExpTopR*iPlateHeight;
	pVehicleRect->iBottom = plateRect.iBottom + fExpBottomR*iPlateHeight;
	pVehicleRect->iLeft = plateRect.iLeft - fExpLefRigR*iPlateWidth;
	pVehicleRect->iRight = plateRect.iRight + fExpLefRigR*iPlateWidth;

	if (pVehicleRect->iTop < 0)
	{
		pVehicleRect->iTop = 0;
	}
	if (pVehicleRect->iBottom >= iImageHeight)
	{
		pVehicleRect->iBottom = iImageHeight - 1;
	}
	if (pVehicleRect->iLeft < 0)
	{
		pVehicleRect->iLeft = 0;
	}
	if (pVehicleRect->iRight >= iImageWidth)
	{
		pVehicleRect->iRight = iImageWidth-1;
	}
	return;
}

void CVehicleProc::VehiclePreProc(IplImage* pSrcImage)
{
	ITS_Vehicle_Result sResult;
	/*IplImage* pImage = cvCreateImage(cvSize(pSrcImage->width,pSrcImage->height),IPL_DEPTH_8U,3);
	cvCopyImage(pSrcImage,pImage);*/
	
	V_Image* pVImage = (V_Image*)malloc(sizeof(V_Image)*1);
	pVImage->eType =E_BGR ;
	pVImage->iImageHeight = pSrcImage->height;
	pVImage->iImageWidth = pSrcImage->width;
	pVImage->pImageData = (unsigned char*)malloc(sizeof(unsigned char)*pVImage->iImageHeight*pVImage->iImageWidth*3);
	int iIndex = 0;
	for (int i = 0; i < pVImage->iImageHeight; i ++)
	{
		unsigned char* p = (unsigned char*)(pSrcImage->imageData + pSrcImage->widthStep*i);
		for (int j = 0; j < pVImage->iImageWidth; j ++)
		{
			pVImage->pImageData[iIndex ++] = p[j*3 + 0];
			pVImage->pImageData[iIndex ++] = p[j*3 + 1];
			pVImage->pImageData[iIndex ++] = p[j*3 + 2];
		}
	}

		//ITS_VehicleTypeRec(pInstance,pVImage,&sResult);
	//���ô���
	HMODULE dlh = NULL;
	if (!(dlh=LoadLibrary("VehicleTypeRecognition_D.dll")))      
	{
		printf("LoadLibrary() failed: %d\n", GetLastError()); 
	}
	VehicleTypeRec vehicleTypeRec;
	vehicleTypeRec = (VehicleTypeRec)GetProcAddress(dlh, "ITS_VehicleTypeRec");
	(*vehicleTypeRec)(pInstance,pVImage,&sResult);
		CvPoint point1,point2;
		CvRect rect;
		vehCnt = sResult.iNum;
		
		//printf("num:%d",vehCnt);
		for (int j = 0; j < sResult.iNum; j++)
		{
			point1.x = sResult.tempResult[j].plateRect.iLeft;
			point1.y = sResult.tempResult[j].plateRect.iTop;
			point2.x = sResult.tempResult[j].plateRect.iRight;
			point2.y = sResult.tempResult[j].plateRect.iBottom;
            rect.x = point1.x;
			rect.y = point1.y;
			rect.width = point2.x-point1.x;
			rect.height = point2.y-point1.y;
			cvRectangle(pSrcImage,point1,point2,cvScalar(0,0,255,0),2,8,0);
			vehinfo[j].plateImg = cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,3);  //����ͼ��
			vehinfo[j].pointLT = point1;//����λ��
			vehinfo[j].width = rect.width; 
			vehinfo[j].height = rect.height;
			GetVehicleRect(sResult.tempResult[j].plateRect,&sResult.tempResult[j].pVehicleRect,pSrcImage->width,pSrcImage->height);
			point1.x = sResult.tempResult[j].pVehicleRect.iLeft;
			point1.y = sResult.tempResult[j].pVehicleRect.iTop;
			point2.x = sResult.tempResult[j].pVehicleRect.iRight;
			point2.y = sResult.tempResult[j].pVehicleRect.iBottom;
			rect.x = point1.x;
			rect.y = point1.y;
			rect.width = point2.x-point1.x;
			rect.height = point2.y-point1.y;
			vehinfo[j].vehicleImg = cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,3);  //����ͼ��
			cvSetImageROI(pSrcImage,rect);
			cvCopyImage(pSrcImage,vehinfo[j].vehicleImg); 
			cvResetImageROI(pSrcImage);
			/*cvSetImageROI(pImage,rect);
			cvCopyImage(pImage,vehinfo[j].vehicleImg); 
			cvResetImageROI(pImage);*/


			strcpy_s(vehinfo[j].platenumber ,sResult.tempResult[j].plateInfor.platenumber); //���ƺ�
			strcpy_s(vehinfo[j].tempVehicleType,sResult.tempResult[j].tempVehicleType);
			//printf("���ͣ�%s\n",sResult.tempResult[j].tempVehicleType);
			//printf("���ƣ�%s\n",sResult.tempResult[j].plateInfor.platenumber);

			switch (sResult.tempResult[j].eVehicleColor1)                            //������ɫ��Ϣ
			{
			case E_VEHICLE_RED:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_GREEN:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_BLUE:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_PINK:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_BROWN:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_YELLOW:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_WHITE:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_BLACK:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_GRAY:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			case E_VEHICLE_CYAN:
				//printf("��������ɫ����ɫ");
				vehinfo[j].eVehicleColor ="��ɫ";
				break;
			default:
				//printf("��������ɫ��δ֪");
				vehinfo[j].eVehicleColor ="δ֪";
				break;
			}

		}
		//cvReleaseImage(&pImage);
  return;
}
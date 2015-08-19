#pragma once
#include <cv.h>
#include <highgui.h>
#include "minmax.h"  //ע������ļ�ֻ�ܷ������λ�ð������������� ��ʾ�Ҳ��� min max
using namespace std;
#define  MAX_VEHICLE_NUM  (10)
typedef struct
{
  CvPoint pointLT;            //����λ������
  int width;          //���ƿ��
  int height;         //���Ƹ߶�
  IplImage* plateImg;        //����ͼ��
  IplImage* vehicleImg;        //����ͼ��
  char tempVehicleType[1024]; //��������
  char platenumber[20];         //���ƺ���
  char* eVehicleColor;      //������ɫ
} VehicleInfo;
class CVehicleProc
{
public:
	CVehicleProc(void);
	~CVehicleProc(void);
	void VehiclePreProc(IplImage* src);

	VehicleInfo vehinfo[MAX_VEHICLE_NUM];
	int vehCnt;
	void* pInstance;
	int nfrnum;
	IplImage * pFrame ;
	

};
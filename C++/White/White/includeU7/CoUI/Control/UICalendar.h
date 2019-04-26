#ifndef _UICALENDAR_H_
#define _UICALENDAR_H_

#include <vector>
#include "UIButton.h"
#include "../Core/UIContainer.h"
#include "../Control/UIOption.h"
#pragma warning(disable : 4251)//disable std::vector<_Ty> interface warning
/*
 * CCalendarUI�������ؼ���
 * CCalButtonUI���ؼ�����ʹ�õİ�ť��
 * CCalAdjustUI�����ڵ����ؼ���
 * CCalDateChangeUI:�ı䵱ǰ���ڿؼ���
 *
 */

#define MAX_COUNT  42
#define CAL_COL  6
#define CAL_ROW  7

namespace CoUI {

/********************************************************************************/
//����gLunarDay��������1901�굽2100��ÿ���е���������Ϣ��
//����ÿ��ֻ����29��30�죬һ����12����13����������λ��ʾ����ӦλΪ1��30�죬����Ϊ29��
	
static int gLunarMonthDay[]=
{
	//��������ֻ��1901.1.1 --2050.12.31
  0X4ae0, 0Xa570, 0X5268, 0Xd260, 0Xd950, 0X6aa8, 0X56a0, 0X9ad0, 0X4ae8, 0X4ae0,   //1910
  0Xa4d8, 0Xa4d0, 0Xd250, 0Xd548, 0Xb550, 0X56a0, 0X96d0, 0X95b0, 0X49b8, 0X49b0,   //1920
  0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada8, 0X2b60, 0X9570, 0X4978, 0X4970, 0X64b0,   //1930
  0Xd4a0, 0Xea50, 0X6d48, 0X5ad0, 0X2b60, 0X9370, 0X92e0, 0Xc968, 0Xc950, 0Xd4a0,   //1940
  0Xda50, 0Xb550, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950, 0Xb4a8, 0X6ca0,   //1950
  0Xb550, 0X55a8, 0X4da0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa950, 0Xe950, 0X6aa0, 0Xad50,   //1960
  0Xab50, 0X4b60, 0Xa570, 0Xa570, 0X5260, 0Xe930, 0Xd950, 0X5aa8, 0X56a0, 0X96d0,   //1970
  0X4ae8, 0X4ad0, 0Xa4d0, 0Xd268, 0Xd250, 0Xd528, 0Xb540, 0Xb6a0, 0X96d0, 0X95b0,   //1980
  0X49b0, 0Xa4b8, 0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada0, 0Xab60, 0X9370, 0X4978,   //1990
  0X4970, 0X64b0, 0X6a50, 0Xea50, 0X6b28, 0X5ac0, 0Xab60, 0X9368, 0X92e0, 0Xc960,   //2000
  0Xd4a8, 0Xd4a0, 0Xda50, 0X5aa8, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950,   //2010
  0Xb4a0, 0Xb550, 0Xb550, 0X55a8, 0X4ba0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa930, 0X74a8,   //2020
  0X6aa0, 0Xad50, 0X4da8, 0X4b60, 0X9570, 0Xa4e0, 0Xd260, 0Xe930, 0Xd530, 0X5aa0,   //2030
  0X6b50, 0X96d0, 0X4ae8, 0X4ad0, 0Xa4d0, 0Xd258, 0Xd250, 0Xd520, 0Xdaa0, 0Xb5a0,   //2040
  0X56d0, 0X4ad8, 0X49b0, 0Xa4b8, 0Xa4b0, 0Xaa50, 0Xb528, 0X6d20, 0Xada0, 0X55b0,   //2050
  
};

//����gLanarMonth�������1901�굽2050�����µ��·ݣ���û����Ϊ0��ÿ�ֽڴ�����
 static BYTE  gLunarMonth[]=
{
	0X00, 0X50, 0X04, 0X00, 0X20,   //1910
	0X60, 0X05, 0X00, 0X20, 0X70,   //1920
	0X05, 0X00, 0X40, 0X02, 0X06,   //1930
	0X00, 0X50, 0X03, 0X07, 0X00,   //1940
	0X60, 0X04, 0X00, 0X20, 0X70,   //1950
	0X05, 0X00, 0X30, 0X80, 0X06,   //1960
	0X00, 0X40, 0X03, 0X07, 0X00,   //1970
	0X50, 0X04, 0X08, 0X00, 0X60,   //1980
	0X04, 0X0a, 0X00, 0X60, 0X05,   //1990
	0X00, 0X30, 0X80, 0X05, 0X00,   //2000
	0X40, 0X02, 0X07, 0X00, 0X50,   //2010
	0X04, 0X09, 0X00, 0X60, 0X04,   //2020
	0X00, 0X20, 0X60, 0X05, 0X00,   //2030
	0X30, 0Xb0, 0X06, 0X00, 0X50,   //2040
	0X02, 0X07, 0X00, 0X50, 0X03    //2050
};

//����gLanarHoliDay���ÿ��Ķ�ʮ�Ľ�����Ӧ����������
//ÿ��Ķ�ʮ�Ľ�����Ӧ���������ڼ����̶���ƽ���ֲ���ʮ��������
//   1��          2��         3��         4��         5��         6��   
//С�� ��   ����  ��ˮ   ���� ����   ���� ����   ���� С��   â�� ����
//   7��          8��         9��         10��       11��        12��  
//С�� ����   ����  ����   ��¶ ���   ��¶ ˪��   ���� Сѩ   ��ѩ ����

//���ݸ�ʽ˵��:
//��1901��Ľ���Ϊ
//  1��     2��     3��   4��    5��   6��   7��    8��   9��    10��  11��     12��
// 6, 21, 4, 19,  6, 21, 5, 21, 6,22, 6,22, 8, 23, 8, 24, 8, 24, 8, 24, 8, 23, 8, 22
// 9, 6,  11,4,   9, 6,  10,6,  9,7,  9,7,  7, 8,  7, 9,  7,  9, 7,  9, 7,  8, 7, 15
//�����һ������Ϊÿ�½�����Ӧ����,15��ȥÿ�µ�һ������,ÿ�µڶ���������ȥ15�õڶ���
// ����ÿ������������Ӧ���ݶ�С��16,ÿ����һ���ֽڴ��,��λ��ŵ�һ����������,��λ���
//�ڶ�������������,�ɵ��±�

static BYTE gLunarHolDay[]=
{
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1901
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1902
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1903
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //1904
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1905
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1906
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1907
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1908
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1909
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1910
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1911
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1912
	0X95, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1913
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1914
	0X96, 0XA5, 0X97, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1915
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1916
	0X95, 0XB4, 0X96, 0XA6, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1917
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X77,   //1918
	0X96, 0XA5, 0X97, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1919
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1920
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1921
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X77,   //1922
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1923
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1924
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1925
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1926
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1927
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1928
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1929
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1930
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1931
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1932
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1933
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1934
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1935
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1936
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1937
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1938
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1939
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1940
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1941
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1942
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1943
	0X96, 0XA5, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1944
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1945
	0X95, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1946
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1947
	0X96, 0XA5, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1948
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X79, 0X78, 0X79, 0X77, 0X87,   //1949
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1950
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1951
	0X96, 0XA5, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1952
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1953
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X68, 0X78, 0X87,   //1954
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1955
	0X96, 0XA5, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1956
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1957
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1958
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1959
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1960
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1961
	0X96, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1962
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1963
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1964
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1965
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1966
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1967
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1968
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1969
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1970
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1971
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1972
	0XA5, 0XB5, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1973
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1974
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1975
	0X96, 0XA4, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X89, 0X88, 0X78, 0X87, 0X87,   //1976
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1977
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //1978
	0X96, 0XB4, 0X96, 0XA6, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1979
	0X96, 0XA4, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1980
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X77, 0X87,   //1981
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1982
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1983
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //1984
	0XA5, 0XB4, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1985
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1986
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X79, 0X78, 0X69, 0X78, 0X87,   //1987
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1988
	0XA5, 0XB4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1989
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1990
	0X95, 0XB4, 0X96, 0XA5, 0X86, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1991
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1992
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1993
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1994
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X76, 0X78, 0X69, 0X78, 0X87,   //1995
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1996
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1997
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1998
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1999
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2000
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2001
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2002
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //2003
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2004
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2005
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2006
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //2007
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2008
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2009
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2010
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //2011
	0X96, 0XB4, 0XA5, 0XB5, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2012
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2013
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2014
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2015
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2016
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2017
	0XA5, 0XB4, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2018
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2019
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X86,   //2020
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2021
	0XA5, 0XB4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2022
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2023
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2024
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2025
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2026
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2027
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2028
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2029
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2030
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2031
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2032
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X86,   //2033
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X78, 0X88, 0X78, 0X87, 0X87,   //2034
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2035
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2036
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2037
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2038
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2039
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2040
	0XA5, 0XC3, 0XA5, 0XB5, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2041
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2042
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2043
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X88, 0X87, 0X96,   //2044
	0XA5, 0XC3, 0XA5, 0XB4, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2045
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2046
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2047
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA5, 0X97, 0X87, 0X87, 0X88, 0X86, 0X96,   //2048
	0XA4, 0XC3, 0XA5, 0XA5, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X86,   //2049
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X78, 0X78, 0X87, 0X87    //2050
};

/**************CCalButtonUI������Ϣ************
 calbuttonui_buttondown��   ��ť����
 calbuttonui_mousemove��   ��껬��
***********************************************/

class UILIB_API CCalButtonUI : public CButtonUI
{
	public:
		CCalButtonUI();
		virtual~ CCalButtonUI();

		void DoInit();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual LPVOID GetInterface(LPCTSTR pstrName);

		virtual LPCTSTR GetClass() const;

		void DoEvent(TEventUI& event);

		void SetCalButtonStatus(UINT uStatus);
        
		BOOL ChangeCalButtonImage(LPCTSTR NormalImage, LPCTSTR HotImage, LPCTSTR PushedImage,
			                                        LPCTSTR FocusedImage = NULL, LPCTSTR ForeImage = NULL, LPCTSTR DisableImage = NULL);

	protected:
	private:
};

class CCalendarUI;
class UILIB_API CCalDateBtnUI: public COptionUI
{
public:
	CCalDateBtnUI();
	virtual~ CCalDateBtnUI();
	void DoInit();
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual LPCTSTR GetClass() const;
	void DoEvent(TEventUI& event);
    void PaintStatusImage(HDC hDC);
	void SetCalButtonStatus(UINT uStatus);

	void SetCalendar(CCalendarUI* pCanlendar);

	void Selected(bool bSelected);
	bool IsSelected(){return m_bSelected;};

	bool SetDate(int year, int month, int day);
	void GetDate(int &year, int &month, int &day);

	bool IsMaked();
	bool IsTodayDay();


private:
	CCalendarUI* m_pCanlendar;
	int m_year;
	int m_month;
	int m_day;

};

/**************CCalendarUI������Ϣ************
 calbtndown��   ��ť����
 pageup��      ���󻬶�
 pagedown��    ���һ���
***********************************************/
/**************CCalendarUI��������************
       ����                      ����˵��               ֵ˵�� 
 calendar_btnnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btnhotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btnpushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btnfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btnforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btndisableimage��   ��ťdisableimage                ͼƬ���·��
 calendar_isshowtooltip��      �Ƿ���ʾToolTip                   true �� false 
 calendar_isonlyshowcurdate      �Ƿ����ʾ��������        true �� false
 calendar_btnpos��            7*6����ť��������λ��    x, y, cx, cy
 calendar_titleheight                   ��������߶�             int
***********************************************/

//---------------------------------
class UILIB_API CCalendarUI : public CContainerUI
{
public:
	CCalendarUI();
	~CCalendarUI();
	void DoInit();
	void OnInitBtn(); //��ʼ�������ؼ��еİ�ť
	void OnInitTitle(); //��ʼ������

	virtual void SetPos(RECT rc);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual LPCTSTR GetClass() const;

	void DoEvent(TEventUI& event);

public:

	
	bool SetSelectedDate(int year, int month);			//���ܣ�����������ʾָ�������£�������������ָ�����£���ѡ���գ���
	bool SetSelectedDate(int year, int month, int day);//���ܣ�����������ʾָ���������գ�������������ָ�����£���ѡ���գ���
	//ͨ����������������ǰѡ�е����ڣ�ע��day��������Ϊ0����day����0ʱ��ʾû��ѡ���գ���û�����ڰ�ť��ѡ��״̬��
	void GetSelectedDate(int &year, int &month, int &day);

	//�õ�ѡ�еİ�ť,���ؿձ�ʾ��ǰû��ѡ���κΰ�ť
	CCalDateBtnUI* GetSelectedBtn();
	//�õ���������ڰ�ť��
	CCalDateBtnUI* GetTodayBtn();

	//�õ���׼���ڣ�����ʱ�䣩
	void GetTodayDate(int &year, int &month, int &day);
	void GetTodayTime(int &hour, int &minute, int &second);

	//������ҳ
	bool OnPageUp();
	bool OnPageDown();

	
	bool AddMark(int year, int month, int day);		//���һ�����
	void DeleteMark(int year, int month, int day);	//ɾ��ָ�����
	void DeleteAllMark();							//ɾ�����б��
	bool IsMarked(int year, int month, int day);	//�ж�ָ�������Ƿ񱻱��

	void SetTextShow(UINT uStyle, DWORD dwTextColor);	//�������ڰ�ť����
	void SetTextFont(int nFont);
	void SetTitleShow(UINT uStyle, DWORD dwTextColor);	//��������������
	void SetTitleFont(int nFont);
	void SetWeekName(CStdString *pstr = NULL);

	void SetIsShowToolTip(bool bOpen = true);				//�����Ƿ� ��ʾ��ʾ
	void SetIsOnlyShowCurDate(bool bOpen = true);			//�����Ƿ� ����ʾ��ǰ����
	void SetIsGesturePageTurning(bool bOpen = true);			//�����Ƿ� ����������ҳ
	CCalDateBtnUI * GetDateBtn(int year, int month, int day);				//�õ�ָ�����ڰ�ť
	bool GetBtnDate(CCalDateBtnUI *pBtn, int &year, int &month, int &day);	//�õ���ť������
	bool IsDateBtn(CControlUI * pControl);	//�ж�pControl �Ƿ��Ǹ����������ڰ�ť
	
public:
	bool IsRightDate(int year, int month, int day);
	bool IsRightDate(int year, int month);
	bool IsRightDate(int year);
	bool IsToday(int year, int month, int day);
	bool IsSelected(int year, int month, int day);

	static int GetTotalDays(int nYear, int nMonth){return __monthday(nYear, nMonth);};
	void OnSelected(CCalDateBtnUI *pBtn);
private:
	void UpdateUIDate();

	void SetTitlePos(RECT &rcPos);
	void SetCalButtonPos(RECT &rcPos);
	void SetToolTip(CCalDateBtnUI* pBtn, int nYear, int nMonth, int nDay);//��Tip��ʽ��ʾ��ǰѡ�����ڵ�ũ��
	void MakeSoleGroupName(CStdString & groupName);
	
	bool GetCalendarTheDate(int SelectedYear, int SelectedMonth,int index, int& year, int &month, int &day);
	bool GetCalendarTheIndex(int SelectedYear, int SelectedMonth,int year, int month, int day, int& index);

	void OnSelectedDateChange();



protected:
	/************�������ڵ��ڲ��ӿ�*****************/

	int __month(int m);  //����ƽ����m��֮ǰ������
	int __leap(int y);  //����y��֮ǰ�ܹ�������
	int __now(int y,int m);  //����y��m��֮ǰ��������
	int __week(int s); // ������Ǵ����ڼ���ʼ�� 
	static int __monthday(int y,int m); //�������������
	int __lunaryeardays(WORD iLunarYear);
	int __getleapmonth(int iLunarYear);
	int  __getlunarholday(int iYear, int iMonth, int iDay);
	LONG __calcdatediff(int iEndYear, int iEndMonth, int iEndDay,
		int  iStartYear = 1901, int iStartMonth = 1, int iStartDay = 1);
	//�ж�iYear�ǲ�������
	bool __isleapyear(int iYear){return !(iYear%4)&&(iYear%100) || !(iYear%400);}
	LONG __lunarmonthdays(WORD iLunarYear, WORD iLunarMonth);
	void  __calcLunardate(int& iYear, int& iMonth ,int& iDay, LONG iSpanDays);
	void __formatlunaryear(int  iYear, CStdString& strYear);
	void __formatLunarmonth(int iMonth, CStdString& strMonth);
	void __formatLunarday(int  iDay, CStdString& strDay);
	//
private:
	CCalDateBtnUI m_btn[MAX_COUNT];//��ű�ʾ���ڵİ�ť
	CLabelUI m_label[7]; //��ű�ʾ���ڵ�lable
	UINT m_nTitleHeight; //����ĸ�
	POINT m_point;

	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nSelectedYear; //
	int m_nSelectedMonth;//
	int m_nSelectedDay;  //

	bool m_bIsShowToolTip;		//�Ƿ���ʾ��ʾ
	bool m_bIsOnlyShowCurDate;	//�Ƿ�ֻ����ʾ��ǰ����
	bool m_bIsSlidePageTurning;

	std::vector<int> m_vecMarked;			//��ű���ǵ�����
	static UINT g_GroupNameVariate;

	CCalDateBtnUI *m_pSelectedBtn;


	CStdString m_strBtnNormalImage;
	CStdString m_strBtnHotImage;
	CStdString m_strBtnPushedImage;
	CStdString m_strBtnFocusedImage;
	CStdString m_strBtnDisabledImage;
	CStdString m_strBtnForeImage;
	CStdString m_strSelectedBtnNormalImage;
	CStdString m_strSelectedBtnHotImage;
	CStdString m_strSelectedBtnPushedImage;
	CStdString m_strSelectedBtnFocusedImage;
	CStdString m_strSelectedBtnDisabledImage;
	CStdString m_strSelectedBtnForeImage;

	CStdString m_strMarkedBtnNormalImage;
	CStdString m_strMarkedBtnHotImage;
	CStdString m_strMarkedBtnPushedImage;
	CStdString m_strMarkedBtnFocusedImage;
	CStdString m_strMarkedBtnDisabledImage;
	CStdString m_strMarkedBtnForeImage;
	CStdString m_strMarkedSelectedBtnNormalImage;
	CStdString m_strMarkedSelectedBtnHotImage;
	CStdString m_strMarkedSelectedBtnPushedImage;
	CStdString m_strMarkedSelectedBtnFocusedImage;
	CStdString m_strMarkedSelectedBtnDisabledImage;
	CStdString m_strMarkedSelectedBtnForeImage;

	CStdString m_strTodayBtnNormalImage;
	CStdString m_strTodayBtnHotImage;
	CStdString m_strTodayBtnPushedImage;
	CStdString m_strTodayBtnFocusedImage;
	CStdString m_strTodayBtnDisabledImage;
	CStdString m_strTodayBtnForeImage;
	CStdString m_strTodaySelectedBtnNormalImage;
	CStdString m_strTodaySelectedBtnHotImage;
	CStdString m_strTodaySelectedBtnPushedImage;
	CStdString m_strTodaySelectedBtnFocusedImage;
	CStdString m_strTodaySelectedBtnDisabledImage;
	CStdString m_strTodaySelectedBtnForeImage;
	//

	DWORD m_dwDateTextColor;
	int m_iDateFont;
	UINT m_uDateTextStyle;
};
//---------------
/**************CCalAdjustUI������Ϣ************
 caladjustui_btnleftdown��   ��ҳ��ť����
 caladjustui_btnrightdown��  �ҷ�ҳ��ť����
 caladjustui_btnmiddown��   �м䰴ť����
***********************************************/

/**************CCalAdjustUI��������************
       ����                      ����˵��               ֵ˵�� 
 calendar_btnleftnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btnlefthotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btnleftpushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btnleftfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btnleftforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btnleftdisableimage��   ��ťdisableimage                ͼƬ���·��

 calendar_btnmidnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btnmidhotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btnmidpushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btnmidfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btnmidforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btnmiddisableimage��   ��ťdisableimage                ͼƬ���·��

 calendar_btnrightnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btnrighthotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btnrightpushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btnrightfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btnrightforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btnrightdisableimage��   ��ťdisableimage                ͼƬ���·��
                                      
calendar_btnlrwidth                         ���ҷ�ҳ��ť���
***********************************************/
class UILIB_API CCalAdjustUI : public CContainerUI
{
public:
	CCalAdjustUI();
    ~CCalAdjustUI();
	void DoInit();

	void OnInitBtn();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual LPVOID GetInterface(LPCTSTR pstrName);

	virtual LPCTSTR GetClass() const;

	void DoEvent(TEventUI& event);

	virtual void SetPos(RECT rc);
/*
 * ���ܣ���ȡ���ڵ����ؼ��е�������ʾ��ť�ؼ����Ըı䵱ǰ������ʾ
 * 
 * ����ֵ��CCalButtonUI*  ������ʾ��ť�ؼ�ָ��
 * 
 *  ��������
 */
	CCalButtonUI* GetMidBtn(){return &m_btnMid;}

/*
 * ���ܣ�����������ʾ
 * 
 * ����ֵ����
 * 
 *  ������UINT          ��������
               DWORD      ������ɫ
 */
   void SetTextShow(UINT uStyle, DWORD dwTextColor);
   void SetTextFont(int nFont);

protected:

	void SetSubCtrlPos(RECT rcPos);
private:
	CCalButtonUI m_btnLeft;
	CCalButtonUI m_btnMid;
	CCalButtonUI m_btnRight;

	CCalendarUI* m_pCal;

	CStdString m_strYear;
	CStdString m_strMonth;
	CStdString m_strDay;

	UINT m_nBtnLRWidth;
};
/**************CCalDateChangeUI������Ϣ************
 caldatechangeui_btnup_y��   �����갴ť��
 caldatechangeui_btndown_y�� �����갴ť��
 caldatechangeui_btnup_m��   �����°�ť��
 caldatechangeui_btndown_m�� �����°�ť��
 caldatechangeui_btnup_d��   �����հ�ť��
 caldatechangeui_btndown_d�� �����հ�ť��

***********************************************/

/**************CCalDateChangeUI��������************
       ����                      ����˵��               ֵ˵�� 
 calendar_btnupnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btnuphotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btnuppushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btnupfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btnupforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btnupdisableimage��   ��ťdisableimage                ͼƬ���·��

 calendar_btndownnormalimage��   ��ťnormalimage                ͼƬ���·��
 calendar_btndownhotimage��      ��ťhotimage                      ͼƬ���·��
 calendar_btndownpushedimage��   ��ťpushedimage                ͼƬ���·��
 calendar_btndownfocusedimage��   ��ťfocusedimage               ͼƬ���·��
 calendar_btndownforeimage��      ��ťforeimage                     ͼƬ���·��
 calendar_btndowndisableimage��   ��ťdisableimage                ͼƬ���·��

 calendar_labelimage��            ��ǩͼƬ                  ͼƬ���·�� 
 calendar_btnsize��               ��ťλ��                   x, y, cx, cy
 calendar_labelsize��              ��ǩλ��                   x, y, cx, cy

***********************************************/
class UILIB_API CCalDateChangeUI : public CContainerUI
{
public:
	CCalDateChangeUI();
    ~CCalDateChangeUI();
	void DoInit();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual LPVOID GetInterface(LPCTSTR pstrName);

	virtual LPCTSTR GetClass() const;

	void DoEvent(TEventUI& event);

	void InitData(int nYear, int nMonth, int nDay);

	void SetYear(int& nYear);
	void SetMonth(int& nMonth);
	void SetDay(int& nDay);

	int GetYear(){return m_nYear;};
	int GetMonth(){return m_nMonth;};
	int GetDay(){return m_nDay;};

	void SetYearText(CStdString strYear);
	void SetMonthText(CStdString strMonth);
	void SetDayText(CStdString strDay);

/*
 * ���ܣ�����������ʾ
 * 
 * ����ֵ����
 * 
 *  ������UINT          ��������
               DWORD      ������ɫ
 */
   void SetTextShow(UINT uStyle, DWORD dwTextColor);
   void SetTextFont(int nFont);

protected:
/*
 * ���ܣ��Զ������ӿؼ�λ��
 * 
 * ����ֵ����
 * 
 *  ��������
 */
	void OnCalSize();

private:
	CCalButtonUI m_btnUp_Y;
	CCalButtonUI m_btnDown_Y;
	CCalButtonUI m_btnUp_M;
	CCalButtonUI m_btnDown_M;
	CCalButtonUI m_btnUp_D;
	CCalButtonUI m_btnDown_D;

	CLabelUI m_labYear;
    CLabelUI m_labMonth;
    CLabelUI m_labDay;
	CLabelUI m_labYear_N;
	CLabelUI m_labMonth_N;
	CLabelUI m_labDay_N;

	int m_btnWidth;
	int m_btnHeight;
	int m_labelWidth;
	int m_labelHeight;

	int m_nYear;
	int m_nMonth;
	int m_nDay;

};

}
#pragma warning(default: 4251)//default std::vector<_Ty> interface warning
#endif
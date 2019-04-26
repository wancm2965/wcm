#ifndef _UICALENDAR_H_
#define _UICALENDAR_H_

#include <vector>
#include "UIButton.h"
#include "../Core/UIContainer.h"
#include "../Control/UIOption.h"
#pragma warning(disable : 4251)//disable std::vector<_Ty> interface warning
/*
 * CCalendarUI：日历控件类
 * CCalButtonUI：控件中所使用的按钮类
 * CCalAdjustUI：日期调整控件类
 * CCalDateChangeUI:改变当前日期控件类
 *
 */

#define MAX_COUNT  42
#define CAL_COL  6
#define CAL_ROW  7

namespace CoUI {

/********************************************************************************/
//数组gLunarDay存入阴历1901年到2100年每年中的月天数信息，
//阴历每月只能是29或30天，一年用12（或13）个二进制位表示，对应位为1表30天，否则为29天
	
static int gLunarMonthDay[]=
{
	//测试数据只有1901.1.1 --2050.12.31
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

//数组gLanarMonth存放阴历1901年到2050年闰月的月份，如没有则为0，每字节存两年
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

//数组gLanarHoliDay存放每年的二十四节气对应的阳历日期
//每年的二十四节气对应的阳历日期几乎固定，平均分布于十二个月中
//   1月          2月         3月         4月         5月         6月   
//小寒 大寒   立春  雨水   惊蛰 春分   清明 谷雨   立夏 小满   芒种 夏至
//   7月          8月         9月         10月       11月        12月  
//小暑 大暑   立秋  处暑   白露 秋分   寒露 霜降   立冬 小雪   大雪 冬至

//数据格式说明:
//如1901年的节气为
//  1月     2月     3月   4月    5月   6月   7月    8月   9月    10月  11月     12月
// 6, 21, 4, 19,  6, 21, 5, 21, 6,22, 6,22, 8, 23, 8, 24, 8, 24, 8, 24, 8, 23, 8, 22
// 9, 6,  11,4,   9, 6,  10,6,  9,7,  9,7,  7, 8,  7, 9,  7,  9, 7,  9, 7,  8, 7, 15
//上面第一行数据为每月节气对应日期,15减去每月第一个节气,每月第二个节气减去15得第二行
// 这样每月两个节气对应数据都小于16,每月用一个字节存放,高位存放第一个节气数据,低位存放
//第二个节气的数据,可得下表

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

/**************CCalButtonUI新增消息************
 calbuttonui_buttondown：   按钮按下
 calbuttonui_mousemove：   鼠标滑动
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

/**************CCalendarUI新增消息************
 calbtndown：   按钮按下
 pageup：      向左滑动
 pagedown：    向右滑动
***********************************************/
/**************CCalendarUI新增属性************
       属性                      属性说明               值说明 
 calendar_btnnormalimage：   按钮normalimage                图片存放路径
 calendar_btnhotimage：      按钮hotimage                      图片存放路径
 calendar_btnpushedimage：   按钮pushedimage                图片存放路径
 calendar_btnfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btnforeimage：      按钮foreimage                     图片存放路径
 calendar_btndisableimage：   按钮disableimage                图片存放路径
 calendar_isshowtooltip：      是否显示ToolTip                   true 或 false 
 calendar_isonlyshowcurdate      是否仅显示当月日期        true 或 false
 calendar_btnpos：            7*6个按钮布局区域位置    x, y, cx, cy
 calendar_titleheight                   主题区域高度             int
***********************************************/

//---------------------------------
class UILIB_API CCalendarUI : public CContainerUI
{
public:
	CCalendarUI();
	~CCalendarUI();
	void DoInit();
	void OnInitBtn(); //初始化日历控件中的按钮
	void OnInitTitle(); //初始化星期

	virtual void SetPos(RECT rc);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual LPCTSTR GetClass() const;

	void DoEvent(TEventUI& event);

public:

	
	bool SetSelectedDate(int year, int month);			//功能：设置日历显示指定的年月（即将日历翻到指定年月，不选中日）；
	bool SetSelectedDate(int year, int month, int day);//功能：设置日历显示指定的年月日（即将日历翻到指定年月，并选中日）；
	//通过参数返回日历当前选中的日期，注意day参数可能为0（当day返回0时表示没有选中日，即没有日期按钮是选中状态）
	void GetSelectedDate(int &year, int &month, int &day);

	//得到选中的按钮,返回空表示当前没有选中任何按钮
	CCalDateBtnUI* GetSelectedBtn();
	//得到今天的日期按钮，
	CCalDateBtnUI* GetTodayBtn();

	//得到标准日期（电脑时间）
	void GetTodayDate(int &year, int &month, int &day);
	void GetTodayTime(int &hour, int &minute, int &second);

	//日历翻页
	bool OnPageUp();
	bool OnPageDown();

	
	bool AddMark(int year, int month, int day);		//添加一个标记
	void DeleteMark(int year, int month, int day);	//删除指定标记
	void DeleteAllMark();							//删除所有标记
	bool IsMarked(int year, int month, int day);	//判断指定日期是否被标记

	void SetTextShow(UINT uStyle, DWORD dwTextColor);	//设置日期按钮字体
	void SetTextFont(int nFont);
	void SetTitleShow(UINT uStyle, DWORD dwTextColor);	//设置星期栏字体
	void SetTitleFont(int nFont);
	void SetWeekName(CStdString *pstr = NULL);

	void SetIsShowToolTip(bool bOpen = true);				//设置是否 显示提示
	void SetIsOnlyShowCurDate(bool bOpen = true);			//设置是否 仅显示当前日期
	void SetIsGesturePageTurning(bool bOpen = true);			//设置是否 开启滑动翻页
	CCalDateBtnUI * GetDateBtn(int year, int month, int day);				//得到指定日期按钮
	bool GetBtnDate(CCalDateBtnUI *pBtn, int &year, int &month, int &day);	//得到按钮的日期
	bool IsDateBtn(CControlUI * pControl);	//判断pControl 是否是该日历的日期按钮
	
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
	void SetToolTip(CCalDateBtnUI* pBtn, int nYear, int nMonth, int nDay);//以Tip方式显示当前选中日期的农历
	void MakeSoleGroupName(CStdString & groupName);
	
	bool GetCalendarTheDate(int SelectedYear, int SelectedMonth,int index, int& year, int &month, int &day);
	bool GetCalendarTheIndex(int SelectedYear, int SelectedMonth,int year, int month, int day, int& index);

	void OnSelectedDateChange();



protected:
	/************计算日期的内部接口*****************/

	int __month(int m);  //返回平年中m月之前的天数
	int __leap(int y);  //计算y年之前总共的天数
	int __now(int y,int m);  //计算y年m月之前的总天数
	int __week(int s); // 求该月是从星期几开始的 
	static int __monthday(int y,int m); //计算该月总天数
	int __lunaryeardays(WORD iLunarYear);
	int __getleapmonth(int iLunarYear);
	int  __getlunarholday(int iYear, int iMonth, int iDay);
	LONG __calcdatediff(int iEndYear, int iEndMonth, int iEndDay,
		int  iStartYear = 1901, int iStartMonth = 1, int iStartDay = 1);
	//判断iYear是不是闰年
	bool __isleapyear(int iYear){return !(iYear%4)&&(iYear%100) || !(iYear%400);}
	LONG __lunarmonthdays(WORD iLunarYear, WORD iLunarMonth);
	void  __calcLunardate(int& iYear, int& iMonth ,int& iDay, LONG iSpanDays);
	void __formatlunaryear(int  iYear, CStdString& strYear);
	void __formatLunarmonth(int iMonth, CStdString& strMonth);
	void __formatLunarday(int  iDay, CStdString& strDay);
	//
private:
	CCalDateBtnUI m_btn[MAX_COUNT];//存放表示日期的按钮
	CLabelUI m_label[7]; //存放表示星期的lable
	UINT m_nTitleHeight; //标题的高
	POINT m_point;

	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nSelectedYear; //
	int m_nSelectedMonth;//
	int m_nSelectedDay;  //

	bool m_bIsShowToolTip;		//是否显示提示
	bool m_bIsOnlyShowCurDate;	//是否只是显示当前日期
	bool m_bIsSlidePageTurning;

	std::vector<int> m_vecMarked;			//存放被标记的日期
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
/**************CCalAdjustUI新增消息************
 caladjustui_btnleftdown：   左翻页按钮按下
 caladjustui_btnrightdown：  右翻页按钮按下
 caladjustui_btnmiddown：   中间按钮按下
***********************************************/

/**************CCalAdjustUI新增属性************
       属性                      属性说明               值说明 
 calendar_btnleftnormalimage：   按钮normalimage                图片存放路径
 calendar_btnlefthotimage：      按钮hotimage                      图片存放路径
 calendar_btnleftpushedimage：   按钮pushedimage                图片存放路径
 calendar_btnleftfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btnleftforeimage：      按钮foreimage                     图片存放路径
 calendar_btnleftdisableimage：   按钮disableimage                图片存放路径

 calendar_btnmidnormalimage：   按钮normalimage                图片存放路径
 calendar_btnmidhotimage：      按钮hotimage                      图片存放路径
 calendar_btnmidpushedimage：   按钮pushedimage                图片存放路径
 calendar_btnmidfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btnmidforeimage：      按钮foreimage                     图片存放路径
 calendar_btnmiddisableimage：   按钮disableimage                图片存放路径

 calendar_btnrightnormalimage：   按钮normalimage                图片存放路径
 calendar_btnrighthotimage：      按钮hotimage                      图片存放路径
 calendar_btnrightpushedimage：   按钮pushedimage                图片存放路径
 calendar_btnrightfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btnrightforeimage：      按钮foreimage                     图片存放路径
 calendar_btnrightdisableimage：   按钮disableimage                图片存放路径
                                      
calendar_btnlrwidth                         左右翻页按钮宽度
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
 * 功能：获取日期调整控件中的日期显示按钮控件，以改变当前日期显示
 * 
 * 返回值：CCalButtonUI*  日期显示按钮控件指针
 * 
 *  参数：无
 */
	CCalButtonUI* GetMidBtn(){return &m_btnMid;}

/*
 * 功能：设置字体显示
 * 
 * 返回值：无
 * 
 *  参数：UINT          字体类型
               DWORD      字体颜色
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
/**************CCalDateChangeUI新增消息************
 caldatechangeui_btnup_y：   设置年按钮加
 caldatechangeui_btndown_y： 设置年按钮减
 caldatechangeui_btnup_m：   设置月按钮加
 caldatechangeui_btndown_m： 设置月按钮减
 caldatechangeui_btnup_d：   设置日按钮加
 caldatechangeui_btndown_d： 设置日按钮减

***********************************************/

/**************CCalDateChangeUI新增属性************
       属性                      属性说明               值说明 
 calendar_btnupnormalimage：   按钮normalimage                图片存放路径
 calendar_btnuphotimage：      按钮hotimage                      图片存放路径
 calendar_btnuppushedimage：   按钮pushedimage                图片存放路径
 calendar_btnupfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btnupforeimage：      按钮foreimage                     图片存放路径
 calendar_btnupdisableimage：   按钮disableimage                图片存放路径

 calendar_btndownnormalimage：   按钮normalimage                图片存放路径
 calendar_btndownhotimage：      按钮hotimage                      图片存放路径
 calendar_btndownpushedimage：   按钮pushedimage                图片存放路径
 calendar_btndownfocusedimage：   按钮focusedimage               图片存放路径
 calendar_btndownforeimage：      按钮foreimage                     图片存放路径
 calendar_btndowndisableimage：   按钮disableimage                图片存放路径

 calendar_labelimage：            标签图片                  图片存放路径 
 calendar_btnsize：               按钮位置                   x, y, cx, cy
 calendar_labelsize：              标签位置                   x, y, cx, cy

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
 * 功能：设置字体显示
 * 
 * 返回值：无
 * 
 *  参数：UINT          字体类型
               DWORD      字体颜色
 */
   void SetTextShow(UINT uStyle, DWORD dwTextColor);
   void SetTextFont(int nFont);

protected:
/*
 * 功能：自动布局子控件位置
 * 
 * 返回值：无
 * 
 *  参数：无
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
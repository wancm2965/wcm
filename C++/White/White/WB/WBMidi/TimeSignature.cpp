#include "StdAfx.h"
#include "TimeSignature.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CTimeSignature::CTimeSignature( int num,int den,int qua,int tem )
{
	if (num == 5)
	{
		num = 4;
	}
	numerator = num;
	denominator = den;
	quarternote = qua;
	tempo = tem;
	int number;
	if (denominator == 2)
	{
		number = quarternote*2; 
	}
	else
	{
		number = quarternote/(denominator/4);
	}
	measure = numerator*number;
}

NoteDuration CTimeSignature::GetNoteDuration( int duration )
{
	int num = quarternote*4;
	NoteDuration result;
	if (duration >= 28*num/32)
	{
		result = Whole;
	}
	else
	{
		if (duration >= 20*num/32)
		{
			result = DottedHalf;
		}
		else
		{
			if (duration >= 14*num/32)
			{
				result = Half;
			}
			else
			{
				if (duration >= 10*num/32)
				{
					result = DottedQuarter;
				}
				else
				{
					if (duration >= 7*num/32)
					{
						result = Quarter;
					}
					else
					{
						if (duration >= 5*num/32)
						{
							result = DottedEighth;
						}
						else
						{
							if (duration >= 6*num/64)
							{
								result = Eighth;
							}
							else
							{
								if (duration >= 5*num/64)
								{
									result = Triplet;
								}
								else
								{
									if (duration >= 3*num/64)
									{
										result = Sixteenth;
									}
									else
									{
										result = ThirtySecond;
									}
								}

							}
						}
					}
				}
			}
		}
	}
	return result;
}

NoteDuration CTimeSignature::GetStemDuration( NoteDuration dur )
{
	NoteDuration reslut;
	if (dur == DottedHalf)
	{
		reslut = Half;
	}
	else
	{
		if (dur == DottedQuarter)
		{
			reslut = Quarter;
		}
		else
		{
			if (dur == DottedEighth)
			{
				reslut = Eighth;
			}
			else
			{
				reslut = dur;
			}
		}
	}
	return reslut;
}

int CTimeSignature::DurationToTime( NoteDuration dur )
{
	int num = quarternote/2;
	int num2 = num/2;
	int result;
	switch (dur)
	{
	case ThirtySecond:
		{
			result = num2/2;
		}
		break;
	case Sixteenth:
		result = num2;
		break;
	case Triplet:
		result = quarternote/3;
		break;
	case Eighth:
		result = num;
		break;
	case DottedEighth:
		result = 3*num2;
		break;
	case Quarter:
		result = quarternote;
		break;
	case DottedQuarter:
		result = 3*num;
		break;
	case Half:
		result = quarternote*2;
		break;
	case DottedHalf:
		result = quarternote*3;
		break;
	case Whole:
		result = quarternote*4;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

CTimeSignature::~CTimeSignature( void )
{

}

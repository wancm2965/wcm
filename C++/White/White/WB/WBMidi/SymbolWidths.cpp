#include "StdAfx.h"
#include "SymbolWidths.h"
#include <algorithm>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CSymbolWidths::CSymbolWidths( vecvecMusicSymbol tracks )
{

	for (int i=0;i<(int)tracks.size();i++)
	{		
		MapIntWidth tempmapint= GetTrackWidths(tracks[i]);
		widths.push_back(tempmapint);
	}
	VecMapIntWidth arrays;
	arrays = widths;
	
	for (int j = 0;j<(int)arrays.size();j++)
	{
		MapIntWidth dictionary = arrays[j];
		
		if (!dictionary.empty())
		{
			MapIntWidth::iterator itd = dictionary.begin();
			
			while(itd != dictionary.end())
			{
				int k = itd->first;
				itd++;
				if (maxwidths.empty())
				{					
					maxwidths.insert(MapIntWidth::value_type(k,dictionary[k]));					
				}
				else
				{
					MapIntWidth::iterator it = maxwidths.find(k);
					if (!(it != maxwidths.end()) ||maxwidths[k]<dictionary[k])
					{
						maxwidths.insert(MapIntWidth::value_type(k,dictionary[k]));	
					}
				}				
			}
		}
	}
	
	if (!widths.empty())
	{
		MapIntWidth::iterator it = maxwidths.begin();
		while(it != maxwidths.end())
		{
			int time = it->first;
			starttimes.push_back(time);
			it++;
		}
	}
	std::sort(starttimes.begin(),starttimes.end(),std::less<int>());

}

CSymbolWidths::~CSymbolWidths( void )
{
	starttimes.clear();
	maxwidths.clear();
	if (!widths.empty())
	{
		for (int i = 0;i<widths.size();i++)
		{
			((MapIntWidth)widths[i]).clear();
		}
		widths.clear();
	}
	

}

VecIntwidth CSymbolWidths::GetSTarttimes()
{
	VecIntwidth  getstimes ;
	if (!starttimes.empty())
	{
		getstimes = starttimes;
	}

	return getstimes;
}

//将musicsymbol元素中的开始时间和最小宽度分离存储在map中

MapIntWidth CSymbolWidths::GetTrackWidths( vecMusicSymbol symbols )
{
	MapIntWidth dictionary ;
	for (int i = 0;i<(int)symbols.size();i++)
	{
		CMusicSymbol* current;
		current = symbols[i];
		int sttime = current->GetStartTime(); //获取开始时间
		int mnwidth = current->GetMinWidth();//获取最小宽度
		if (dictionary.empty())
		{
			dictionary.insert(MapIntWidth::value_type(sttime,mnwidth));
		}
		else
		{
			MapIntWidth::iterator it = dictionary.find(sttime);
			if (it != dictionary.end())
			{
				int nfind = it->second;
				nfind += mnwidth;
				it->second = nfind;
				/*dictionary.insert(MapIntWidth::value_type(sttime, nfind));*/
			}
			else
			{
				dictionary.insert(MapIntWidth::value_type(sttime,mnwidth));
			}
		}
	}
	return dictionary;
}

int CSymbolWidths::GetEctraWidth( int track,int start )
{
	int result;
	if (!widths.empty())
	{
		//MapIntWidth finmap = widths[track];
		
		if (widths[track].empty())
		{
			if (!maxwidths.empty())
			{
				result = maxwidths[start];

			}
		}
		else
		{
			MapIntWidth::iterator it = widths[track].find(start);
			if (it != widths[track].end())
			{
				if (!maxwidths.empty())
				{
					result = maxwidths[start] - widths[track][start];

				}
			}
			else
			{
				if (!maxwidths.empty())
				{
					result = maxwidths[start];
				}
			}
		}
	}
	return result;
}

int CSymbolWidths::GetWidthBetween( int start,int end )
{
	int num = 0;
	int num2 = 0;
	for (int i = 0;i<(int)starttimes.size();i++)
	{
		if (starttimes[i] == start)
		{
			num2 = i;
			break;
		}
	}	
	
	num2++;
	while(num2 < (int)starttimes.size() && starttimes[num2] <= end)
	{
		num += maxwidths[starttimes[num2]];
		num2++;
	}
	return num;
}

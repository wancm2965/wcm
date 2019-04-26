class stringex : public string
{
public:
	void makelower()
	{
		string::iterator it;
		for(it = begin();it!=end();++it)
		{
			if((*it)>='A' && (*it)<='Z')
				(*it) = (*it)-'A'+'a';
		}
	}
	void makeupper()
	{
		string::iterator it;
		for(it = begin();it!=end();++it)
		{
			if((*it)>='a' && (*it)<='z')
				(*it) = (*it)-'a'+'A';
		}
	}
	string trimright()
	{		
		int iEPos = find_last_not_of(("\r\n\t "));
		if(iEPos<0)
			return ("");
		return substr(0,iEPos+1);
	}
	string trimleft()
	{
		int iBPos = find_first_not_of(("\r\n\t "));		
		if(iBPos<0)
			return ("");
		return substr(iBPos,size()-iBPos);		
	}
	int replace(string strOld,string strNew)	//返回替换的个数
	{
		int iPos=0;
		int iCurPos = 0;
		int iSize = strOld.size();
		int iCount = 0;
		while(iPos>=0)
		{
			iCurPos = find(strOld,iPos);
			if(iCurPos<0)
				break;
			string::replace(iCurPos,iSize,strNew);
			iCount ++;
			iPos = iCurPos+strNew.size();
		}
		return iCount;		
	}
	int replacenocase(string strOld,string strNew)	//返回替换的个数
	{
		if(size()==0)
			return 0;
		int iPos=0;
		int iCurPos = 0;
		int iSize = strOld.size();
		int iCount = 0;
		int i,j;
		char c;
		((stringex&)strOld).makeupper();		
		while(iPos>=0&&iPos<(int)size())
		{
			j = iPos;			
			iCurPos = -1;
			for(i = 0;i<(int)strOld.size();)
			{
				c = at(j);
				if(c>='a'&&c<='z')
					c = c-'a'+'A';
				if(c != strOld.at(i))
				{
					i = 0;
					iPos ++;
					j = iPos;
					if(j== size())
						break;
				}
				else
				{
					i++;
					j++;
					if(j== size())
						break;
				}
			}
			if(i == strOld.size())
				iCurPos = iPos;
			if(iCurPos<0)
				break;
			string::replace(iCurPos,iSize,strNew);
			iCount ++;
			iPos = iCurPos+strNew.size();
		}
		return iCount;
	}
};

class wstringex : public wstring
{
public:
	void makelower()
	{
		wstring::iterator it;
		for(it = begin();it!=end();++it)
		{
			if((*it)>='A' && (*it)<='Z')
				(*it) = (*it)-'A'+'a';
		}
	}
	void makeupper()
	{
		wstring::iterator it;
		for(it = begin();it!=end();++it)
		{
			if((*it)>='a' && (*it)<='z')
				(*it) = (*it)-'a'+'A';
		}
	}
	wstring trimright()
	{		
		int iEPos = find_last_not_of(L"\r\n\t ");
		if(iEPos<0)
			return L"";
		return substr(0,iEPos+1);
	}
	wstring trimleft()
	{
		int iBPos = find_first_not_of(L"\r\n\t ");		
		if(iBPos<0)
			return L"";
		return substr(iBPos,size()-iBPos);		
	}
	int replace(wstring strOld,wstring strNew)	//返回替换的个数
	{
		int iPos=0;
		int iCurPos = 0;
		int iSize = strOld.size();
		int iCount = 0;
		while(iPos>=0)
		{
			iCurPos = find(strOld,iPos);
			if(iCurPos<0)
				break;
			wstring::replace(iCurPos,iSize,strNew);
			iCount ++;
			iPos = iCurPos+strNew.size();
		}
		return iCount;		
	}
	int replacenocase(wstring strOld,wstring strNew)	//返回替换的个数
	{
		if(size()==0)
			return 0;
		int iPos=0;
		int iCurPos = 0;
		int iSize = strOld.size();
		int iCount = 0;
		int i,j;
		char c;
		((stringex&)strOld).makeupper();		
		while(iPos>=0&&iPos<(int)size())
		{
			j = iPos;			
			iCurPos = -1;
			for(i = 0;i<(int)strOld.size();)
			{
				c = at(j);
				if(c>='a'&&c<='z')
					c = c-'a'+'A';
				if(c != strOld.at(i))
				{
					i = 0;
					iPos ++;
					j = iPos;
					if(j== size())
						break;
				}
				else
				{
					i++;
					j++;
					if(j== size())
						break;
				}
			}
			if(i == strOld.size())
				iCurPos = iPos;
			if(iCurPos<0)
				break;
			wstring::replace(iCurPos,iSize,strNew);
			iCount ++;
			iPos = iCurPos+strNew.size();
		}
		return iCount;
	}
};

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#ifdef _UNICODE
#define tstringex wstringex
#else
#define tstringex stringex
#endif
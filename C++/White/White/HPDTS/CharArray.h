#pragma once

class CharArray
{
public:
	CharArray(long len) {
		m_curLen = len;
		buf = new char[len];
	}
	CharArray() {
		m_curLen = 0;
	}

	~CharArray() {
		if(m_curLen>0){
			delete [] buf;
			m_curLen = 0;
		}
	}
	void DelArray(){
		if(m_curLen>0){
			delete [] buf;
			m_curLen = 0;
		}
	}
	void CreateArray(long len){
		m_curLen = len;
		buf = new char[len];
	}
public:
	long m_curLen;
	char * buf;
};

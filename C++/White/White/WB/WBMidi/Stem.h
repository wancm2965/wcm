#pragma once
#include "stdafx.h"
#include "MidiDef.h"
#include "WhiteNote.h"




class CStem
{
public:
	CStem(CWhiteNote* bottomnote,CWhiteNote* topnote,NoteDuration duration,int direction,bool overlap);
	~CStem(void);

public:
	int Up;
	int Down;
	int LeftSide;
	int RightSide;
private:
	NoteDuration m_duration;
	int m_direction;
	CWhiteNote *m_top;
	CWhiteNote* m_bottom;
	CWhiteNote* m_endnote;
	bool m_notesoverlap;
	int m_side;
	int m_width_to_pair;
	bool m_receiver_in_pair;
	CStem* m_pair;
public:
	int GetDirection(){return m_direction;}
	void SetDirection(int nvalue);
	NoteDuration GetDuration(){return m_duration;}
	CWhiteNote* GetTop(){return m_top;}
	CWhiteNote* GetBottom(){return m_bottom;}
	CWhiteNote* GetEnd(){return m_endnote;}
	void SetEnd(CWhiteNote* note){m_endnote = note;}
	bool GetReceiver(){return m_receiver_in_pair;}
	void SetReceiver(bool bvalue){m_receiver_in_pair = bvalue;}
	bool isBeam(){return m_receiver_in_pair||m_pair!= NULL;}
	CWhiteNote* CalculateEnd();
	void ChangeDirection(int newdirection);
	void SetPair(CStem* paird,int width_to_pai);
	void Draw(Graphics* gdc,int ytop,CWhiteNote* staff);
	void DrawVerticalLine(Graphics* gdc,int ytop,CWhiteNote* staff);
	void DrawCurvyStem(Graphics* gdc,int ytop,CWhiteNote* staff);
	void DrawHorizBarStem(Graphics* gdc,int ytop,CWhiteNote* staff);

};




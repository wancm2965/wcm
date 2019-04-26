#pragma once
#include <limits>
#include <iostream>
#include "PointRetain.h"
#include "PointRectangle.h"
#include "MatchTemplate.h"
#include "TemplateLibrary.h"
#include "RecognitionResult.h"
using namespace std;

typedef vector<MatchTemplate> MatchTemplates;
typedef vector<PointRetain>  PointPath;
typedef PointPath::iterator PointPathIterator;
class GeometricRecognizer
{
protected:
	double halfDiagonal;
	double angleRange;
	double anglePrecision;
	double goldenRatio;

	int numPointsInGesture;
	int squareSize;

	bool shouldIgnoreRotation;
	 
	MatchTemplates templates;
	PointRectangle m_box;

public:
	GeometricRecognizer();
	~GeometricRecognizer();
	int addTemplate(string name, PointPath points);
	PointRectangle boundingBox(PointPath points);
	PointRetain centroid(PointPath points);
	double getDistance(PointRetain p1, PointRetain p2);
	bool   getRotationInvariance() { return shouldIgnoreRotation; }
	double distanceAtAngle(	PointPath points, MatchTemplate aTemplate, double rotation);
	double distanceAtBestAngle(PointPath points, MatchTemplate T);
	PointPath normalizePath(PointPath points);
	double pathDistance(PointPath pts1, PointPath pts2);
	double pathLength(PointPath points);
	RecognitionResult recognize(PointPath points);
	PointPath resample(PointPath points);
	PointPath rotateBy(PointPath points, double rotation);
	PointPath rotateToZero(PointPath points);
	PointPath scaleToSquare(PointPath points);
	void   setRotationInvariance(bool ignoreRotation);
	PointPath translateToOrigin(PointPath points);
	PointRectangle getboundingbox(){return m_box;}
	void loadTemplates();
	void ClearSomething();
};



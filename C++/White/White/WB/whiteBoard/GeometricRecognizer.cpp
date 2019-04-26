

#include "GeometricRecognizer.h"
#define MAX_DOUBLE std::numeric_limits<double>::max();



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



GeometricRecognizer::GeometricRecognizer()
{
	numPointsInGesture = 128;
	squareSize = 250;
	halfDiagonal = 0.5 * sqrt((250.0 * 250.0) + (250.0 * 250.0));
	setRotationInvariance(false);
	anglePrecision = 2.0;
	goldenRatio    = 0.5 * (-1.0 + sqrt(5.0));
		
			
}

void GeometricRecognizer::loadTemplates()
{
	TemplateLibrary samples;

	
	addTemplate("Circle", samples.GetTemplateCircle());
	addTemplate("Rectangle", samples.GetTemplateRectangle());
	addTemplate("Star", samples.GetTemplateStart());
	addTemplate("Triangle", samples.GetTemplateTriangle());
		
}

int GeometricRecognizer::addTemplate(string name, PointPath points)
{
	points = normalizePath(points);

	templates.push_back(MatchTemplate(name, points));
	int numInstancesOfGesture = 0;		
	return numInstancesOfGesture;
}

PointRectangle GeometricRecognizer::boundingBox(PointPath points)
{
	double minX =  MAX_DOUBLE;
	double maxX = -MAX_DOUBLE;
	double minY =  MAX_DOUBLE; 
	double maxY = -MAX_DOUBLE;

	for (PointPathIterator i = points.begin(); i != points.end(); i++)
	{
		PointRetain point = *i;
		if (point.PointRetainX < minX)
			minX = point.PointRetainX;
		if (point.PointRetainX > maxX)
			maxX = point.PointRetainX;
		if (point.PointRetainY < minY)
			minY = point.PointRetainY;
		if (point.PointRetainY > maxY)
			maxY = point.PointRetainY;
	}
	PointRectangle bounds(minX, minY, (maxX - minX), (maxY - minY));
	m_box = bounds;
	return bounds;
}

PointRetain GeometricRecognizer::centroid(PointPath points)
{
	double x = 0.0, y = 0.0;
	for (PointPathIterator i = points.begin(); i != points.end(); i++)
	{
		PointRetain point = *i;
		x += point.PointRetainX;
		y += point.PointRetainY;
	}
	x /= points.size();
	y /= points.size();
	return PointRetain(x, y);
}	

double GeometricRecognizer::getDistance(PointRetain p1, PointRetain p2)
{
	double dx = p2.PointRetainX- p1.PointRetainX;
	double dy = p2.PointRetainY - p1.PointRetainY;
	double distance = sqrt((dx * dx) + (dy * dy));
	return distance;
}

double GeometricRecognizer::distanceAtAngle(PointPath points, MatchTemplate aTemplate, double rotation)
{
	PointPath newPoints = rotateBy(points, rotation);
	return pathDistance(newPoints, aTemplate.ptpath);
}	

double GeometricRecognizer::distanceAtBestAngle(PointPath points, MatchTemplate aTemplate)
{
	double startRange = -angleRange;
	double endRange   =  angleRange;
	double x1 = goldenRatio * startRange + (1.0 - goldenRatio) * endRange;
	double f1 = distanceAtAngle(points, aTemplate, x1);
	double x2 = (1.0 - goldenRatio) * startRange + goldenRatio * endRange;
	double f2 = distanceAtAngle(points, aTemplate, x2);
	while (abs(endRange - startRange) > anglePrecision)
	{
		if (f1 < f2)
		{
			endRange = x2;
			x2 = x1;
			f2 = f1;
			x1 = goldenRatio * startRange + (1.0 - goldenRatio) * endRange;
			f1 = distanceAtAngle(points, aTemplate, x1);
		}
		else
		{
			startRange = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1.0 - goldenRatio) * startRange + goldenRatio * endRange;
			f2 = distanceAtAngle(points, aTemplate, x2);
		}
	}
	return min(f1, f2);
}

PointPath GeometricRecognizer::normalizePath(PointPath points)
{
		
	points = resample(points);
	if (getRotationInvariance())
		points = rotateToZero(points);
	points = scaleToSquare(points);
	points = translateToOrigin(points);

	return points;
}

double GeometricRecognizer::pathDistance(PointPath pts1, PointPath pts2)
{
		// assumes pts1.size == pts2.size

	double distance = 0.0;
	for (int i = 0; i < (int)pts1.size(); i++) 
		distance += getDistance(pts1[i], pts2[i]);
	double pathdis = distance / pts1.size();
	if (!pts1.empty())
	{
		pts1.clear();
	}
	return pathdis;
}

double GeometricRecognizer::pathLength(PointPath points)
{
	double distance = 0;
	for (int i = 1; i < (int)points.size(); i++)
		distance += getDistance(points[i - 1], points[i]);
	return distance;
}

RecognitionResult GeometricRecognizer::recognize(PointPath points)
{		
	if (templates.empty())
	{			
		return RecognitionResult("Unknown", NULL);
	}

	points = normalizePath(points);	
	double bestDistance = MAX_DOUBLE;
	double useDistance = 0.00;
	int indexOfBestMatch = -1;
	double maxscore = 0.00;
	for (int i = 0; i < (int)templates.size(); i++)
	{			
		double distance = distanceAtBestAngle(points, templates[i]);
		if (distance < bestDistance)
		{
			bestDistance     = distance;
			double score = 1.0 - (bestDistance / halfDiagonal);
			if (maxscore<score)
			{
				maxscore = score;
				indexOfBestMatch = i;
				useDistance = bestDistance;
			}

		}
	}

	double score = 1.0 - (useDistance / halfDiagonal);

	if (-1 == indexOfBestMatch)
	{
		return RecognitionResult("Unknown", 1);
	}

	RecognitionResult bestMatch(templates[indexOfBestMatch].Matchname, maxscore);
	return bestMatch;
};

PointPath GeometricRecognizer::resample(PointPath points)
{
	double interval = pathLength(points) / (numPointsInGesture - 1); // interval length
	double D = 0.0;
	PointPath newPoints;

	newPoints.push_back(points.front());
	for(int i = 1; i < (int)points.size(); i++)
	{
		PointRetain currentPoint  = points[i];
		PointRetain previousPoint = points[i-1];
		double d = getDistance(previousPoint, currentPoint);
		if ((D + d) >= interval)
		{
			double qx = previousPoint.PointRetainX + ((interval - D) / d) * (currentPoint.PointRetainX - previousPoint.PointRetainX);
			double qy = previousPoint.PointRetainY + ((interval - D) / d) * (currentPoint.PointRetainY - previousPoint.PointRetainY);
			PointRetain point(qx, qy);
			newPoints.push_back(point);
			points.insert(points.begin() + i, point);
			D = 0.0;
		}
		else D += d;
	}

	if (newPoints.size() == (numPointsInGesture - 1))
	{
		newPoints.push_back(points.back());
	}

	return newPoints;
}

PointPath GeometricRecognizer::rotateBy(PointPath points, double rotation) 
{
	PointRetain c     = centroid(points);
	double cosine = cos(rotation);	
	double sine   = sin(rotation);
		
	PointPath newPoints;
	for (PointPathIterator i = points.begin(); i != points.end(); i++)
	{
		PointRetain point = *i;
		double qx = (point.PointRetainX - c.PointRetainX) * cosine - (point.PointRetainY - c.PointRetainY) * sine   + c.PointRetainX;
		double qy = (point.PointRetainX - c.PointRetainX) * sine   + (point.PointRetainY - c.PointRetainY) * cosine + c.PointRetainY;
		newPoints.push_back(PointRetain(qx, qy));
	}
	return newPoints;
}

PointPath GeometricRecognizer::rotateToZero(PointPath points)
{
	PointRetain c = centroid(points);
	double rotation = atan2(c.PointRetainY - points[0].PointRetainY, c.PointRetainX - points[0].PointRetainX);
	return rotateBy(points, -rotation);
}

PointPath GeometricRecognizer::scaleToSquare(PointPath points)
{
	PointRectangle box = boundingBox(points);
	PointPath newPoints;
	for (PointPathIterator i = points.begin(); i != points.end(); i++)
	{
		PointRetain point = *i;			
		double scaledX = point.PointRetainX * (this->squareSize / box.RectangleWidth);
		double scaledY = point.PointRetainY * (this->squareSize / box.RectangleHeight);			
		newPoints.push_back(PointRetain(scaledX, scaledY));
	}
	return newPoints;
}

void   GeometricRecognizer::setRotationInvariance(bool ignoreRotation)
{
	shouldIgnoreRotation = ignoreRotation;

	if (shouldIgnoreRotation)
	{
		angleRange = 45.0;
	}
	else
	{
		angleRange = 15.0;
	}
}

PointPath GeometricRecognizer::translateToOrigin(PointPath points)
{
	PointRetain c = centroid(points);
	PointPath newPoints;
	for (PointPathIterator i = points.begin(); i != points.end(); i++)
	{
		PointRetain point = *i;
		double qx = point.PointRetainX- c.PointRetainX;
		double qy = point.PointRetainY - c.PointRetainY;
		newPoints.push_back(PointRetain(qx, qy));
	}
	return newPoints;
}

void GeometricRecognizer::ClearSomething()
{
	for (int i = 0;i<templates.size();++i)
	{
		MatchTemplate temp = templates[i];
		temp.ptpath.clear();
	}
	templates.clear();
	
}

GeometricRecognizer::~GeometricRecognizer()
{
	  ClearSomething();
}

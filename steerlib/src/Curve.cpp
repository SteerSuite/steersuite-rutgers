//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
// Copyright (c) 2015 Mahyar Khayatkhoei
// CG-F16-23

#include <algorithm>
#include <vector>
#include <util/Geometry.h>
#include <util/Curve.h>
#include <util/Color.h>
#include <util/DrawLib.h>
#include "Globals.h"
using namespace std;
using namespace Util;


Curve::Curve(const CurvePoint& startPoint, int curveType) : type(curveType)
{
	controlPoints.push_back(startPoint);
}

Curve::Curve(const std::vector<CurvePoint>& inputPoints, int curveType) : type(curveType)
{
	controlPoints = inputPoints;
	sortControlPoints();
}

// Add one control point to the vector controlPoints
void Curve::addControlPoint(const CurvePoint& inputPoint)
{
	controlPoints.push_back(inputPoint);
	sortControlPoints();
}

// Add a vector of control points to the vector controlPoints
void Curve::addControlPoints(const std::vector<CurvePoint>& inputPoints)
{
	for (int i = 0; i < inputPoints.size(); i++)
		controlPoints.push_back(inputPoints[i]);
	sortControlPoints();
}

// Draw the curve shape on screen, usign window as step size (bigger window: less accurate shape)
void Curve::drawCurve(Color curveColor, float curveThickness, int window)
{
#ifdef ENABLE_GUI

	//================DELETE THIS PART AND THEN START CODING===================
    if(!checkRobust())
        return;
    
    Point currentPoint = controlPoints[0].position;
    Point nextPoint;
    
    float time = 0.0;
    
    while(calculatePoint(nextPoint,time)){
        DrawLib::drawLine(currentPoint, nextPoint, curveColor, curveThickness);
        currentPoint = nextPoint;
        time = time + 0.01 * (float)window;
    }

	//=========================================================================

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
	return;
#endif
}

bool comparePoints(CurvePoint a, CurvePoint b)
{
	return a.time < b.time;
}

// Sort controlPoints vector in ascending order: min-first

void Curve::sortControlPoints()
{
	std::sort(controlPoints.begin(), controlPoints.end(), &comparePoints);
	return;
}

// Calculate the position on curve corresponding to the given time, outputPoint is the resulting position
// Note that this function should return false if the end of the curve is reached, or no next point can be found
bool Curve::calculatePoint(Point& outputPoint, float time)
{
	// Robustness: make sure there is at least two control point: start and end points
	if (!checkRobust())
		return false;

	// Define temporary parameters for calculation
	unsigned int nextPoint;

	// Find the current interval in time, supposing that controlPoints is sorted (sorting is done whenever control points are added)
	// Note that nextPoint is an integer containing the index of the next control point
	if (!findTimeInterval(nextPoint, time))
		return false;

	// Calculate position at t = time on curve given the next control point (nextPoint)
	if (type == hermiteCurve)
	{
		outputPoint = useHermiteCurve(nextPoint, time);
	}
	else if (type == catmullCurve)
	{
		outputPoint = useCatmullCurve(nextPoint, time);
	}

	// Return
	return true;
}

// Check Roboustness
// size at least for 2
bool Curve::checkRobust()
{

	//================DELETE THIS PART AND THEN START CODING===================
	if(controlPoints.size() < 2)
        return false;
    //=========================================================================


	return true;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	//control point time must > current time
	for (int i = 0; i < controlPoints.size(); i++){
		if (controlPoints[i].time > time){
			nextPoint = i;
			return true;
		}
	}
	return false;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;


	const unsigned int prePoint = nextPoint - 1;
	intervalTime = controlPoints[nextPoint].time - controlPoints[prePoint].time;
	normalTime = (time - controlPoints[prePoint].time) / intervalTime;

	float t2 = std::pow(normalTime, 2);
	float t3 = std::pow(normalTime, 3);

	newPosition = (2 * t3 - 3 * t2 + 1) * controlPoints[prePoint].position //f1t
		+ (-2 * t3 + 3 * t2) * controlPoints[nextPoint].position  //f2t
		+ (t3 - 2 * t2 + normalTime) * controlPoints[prePoint].tangent * intervalTime //f3t
		+ (t3 - t2) * controlPoints[nextPoint].tangent * intervalTime; //f4t
	return newPosition;

}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	float normalTime, intervalTime;

	const unsigned int i0 = nextPoint - 2;
	const unsigned int i1 = nextPoint - 1;
	const unsigned int i2 = nextPoint;
	const unsigned int i3 = nextPoint + 1;

	Point p1 = controlPoints[i1].position;
	Point p2 = controlPoints[i2].position;

	Vector s1, s2;

	// Calculate time interval, and normal time required for later curve calculations
	intervalTime = controlPoints[i2].time - controlPoints[i1].time;
	normalTime = (time - controlPoints[i1].time) / intervalTime;

	float t2 = std::pow(normalTime, 2);
	float t3 = std::pow(normalTime, 3);

	
	//compute tangent information
	if (i1 < 1) 
	{
		s1 = 2 * (p2 - p1) - (controlPoints[i3].position - p1) / 2;
		s2 = (controlPoints[i3].position - p1) / 2;
	}
	else if (i3 >= controlPoints.size()) 
	{
		s1 = (p2 - controlPoints[i0].position) / 2;
		s2 = 2 * (p2 - p1) - (p2 - controlPoints[i0].position) / 2;
	}
	else 
	{
		s1 = (p2 - controlPoints[i0].position) / 2;
		s2 = (controlPoints[i3].position - p1) / 2;
	}

	newPosition = (2 * t3 - 3 * t2 + 1) * p1 // f1t
		+ (-2 * t3 + 3 * t2) * p2 //f2t
		+ (t3 - 2 * t2 + normalTime) * s1 //f3t
		+ (t3 - t2) * s2;//f34
	
	// Return result
	return newPosition;
}

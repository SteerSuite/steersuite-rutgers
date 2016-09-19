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
<<<<<<< HEAD
        return false;
=======
<<<<<<< HEAD
        return;
=======
        return false;
>>>>>>> master
>>>>>>> joking
    
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
//I change here
bool compare(CurvePoint temp1,CurvePoint temp2)
{
	return temp1.time < temp2.time;
}
// Sort controlPoints vector in ascending order: min-first
bool compare(CurvePoint P1, CurvePoint P2)
{
	return P1.time < P2.time;
}
void Curve::sortControlPoints()
{
	sort(controlPoints.begin(), controlPoints.end(), compare);
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
	//================DELETE THIS PART AND THEN START CODING===================
    for(int i = 0; i < controlPoints.size(); i++){
        if(time < controlPoints[i].time){
            nextPoint = i;
            break;
        }
        else
            return false;
    }
    //=========================================================================


	return true;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;
	intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	normalTime = (time - controlPoints[nextPoint - 1].time) / intervalTime;
	float h1, h2, h3, h4;
	Point P1, P2,R1,R2;
	//The coordinates of P1,P2
	P1 = controlPoints[nextPoint - 1].position;
	P2 = controlPoints[nextPoint].position;


	//The coefficients
	h1 = 2 * pow(normalTime, 3) - 3 * pow(normalTime, 2) + 1;
	h2 = -2 * pow(normalTime, 3) + 3 * pow(normalTime, 2);
	h3 = (pow(normalTime, 3) - 2 * pow(normalTime, 2) + 1)*intervalTime;
	h4 = (pow(normalTime, 3) - pow(normalTime, 2))*intervalTime;

	newPosition = P1*h1 + P2*h2 + controlPoints[nextPoint - 1].tangent*h3 + controlPoints[nextPoint].tangent*h4;
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	//================DELETE THIS PART AND THEN START CODING===================
	static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function useCatmullCurve is not implemented!" << std::endl;
		flag = true;
	}
	//=========================================================================

	// Calculate position at t = time on Catmull-Rom curve
	
	// Return result
	return newPosition;
}

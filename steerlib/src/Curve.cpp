//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
// Copyright (c) 2015 Mahyar Khayatkhoei
//

#include <algorithm>
#include <vector>
#include <util/Geometry.h>
#include <util/Curve.h>
#include <util/Color.h>
#include <util/DrawLib.h>
#include "Globals.h"

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

	if(!checkRobust()) {
        return;
    }
    Point currentPoint = controlPoints[0].position;
    Point nextPoint;
    float time = 0.0;

   	while(calculatePoint(nextPoint, time)) {
	    DrawLib::drawLine(currentPoint, nextPoint, curveColor, curveThickness);
	    currentPoint = nextPoint;
	    time = time + (float) window;
    }

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
#endif
}

// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	if(controlPoints.size() == 1)
		return;
	else {
		for (int i = 1; i < controlPoints.size(); i++) {
			for (int j = i; j > 0 && controlPoints.at(j-1).time > controlPoints.at(j).time; j--) {
				CurvePoint tmp = controlPoints.at(j);
				controlPoints.at(j) = controlPoints.at(j - 1);
		 		controlPoints.at(j - 1) = tmp;
			}
		}
		return;
	}
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
bool Curve::checkRobust()
{
	//Accesses member fuction size in the vector class to determine length of controlPoints
	if (controlPoints.size() >= 2) {
		for(std::vector<CurvePoint>::iterator index = controlPoints.begin(); index != controlPoints.end(); index++) {
			std::vector<CurvePoint>::iterator index2 = index+1;
			if(index2 != controlPoints.end() && (index->time == index2->time)) {
				controlPoints.erase(index2);
			}
			else if(index2 == controlPoints.end()) {
				index2->time = index2->time + 0.05;
			}
		}
		return true;
	}
	else {
		return false;
	}
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
    for (int i = 0; i < controlPoints.size(); i++) {
        if (controlPoints.at(i).time <= time) {
            nextPoint = i+1;
        }
    }
    if(nextPoint == controlPoints.size() && time > controlPoints.at(controlPoints.size()-1).time) 
    	return false;
    else
    	return true;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float intervalTime, normalTime;
	const unsigned int currPoint = nextPoint - 1;
    intervalTime = controlPoints[nextPoint].time - controlPoints[currPoint].time;
    normalTime = (time - controlPoints[currPoint].time) / intervalTime;

    float t3 = normalTime * normalTime * normalTime;
    float t2 = normalTime * normalTime;

    float f1 = 2 * t3 - 3 * t2 + 1;
    float f2 = -2 * t3 + 3 * t2;
    float f3 = (t3 - 2 * t2 + normalTime) * intervalTime;
    float f4 = (t3 - t2) * intervalTime;

    Point p1 = controlPoints[currPoint].position;
    Point p2 = controlPoints[nextPoint].position;
    Vector v1 = controlPoints[currPoint].tangent;
    Vector v2 = controlPoints[nextPoint].tangent;

    float xsum = f1 * p1.x + f2 * p2.x + f3 * v1.x + f4 * v2.x;
    float ysum = f1 * p1.y + f2 * p2.y + f3 * v1.y + f4 * v2.y;
    float zsum = f1 * p1.z + f2 * p2.z + f3 * v1.z + f4 * v2.z;

    newPosition.x = xsum;
    newPosition.y = ysum;
    newPosition.z = zsum;

    // Return result
    return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
    float intervalTime, normalTime;
    const unsigned int currPoint = nextPoint - 1;
    const unsigned int prevPoint = nextPoint - 2;
    const unsigned int nnextPoint = nextPoint + 1;
    intervalTime = controlPoints[nextPoint].time - controlPoints[currPoint].time;
    normalTime = (time - controlPoints[currPoint].time) / intervalTime;

    float t3 = normalTime * normalTime * normalTime;
   	float t2 = normalTime * normalTime;

    float f1 = (-t3 + 2 * t2 - normalTime) * 0.5;
   	float f2 = (t3 - 5 * t2 + 2) * 0.5;
   	float f3 = (-3 * t3 + 4 * t2 + normalTime) * 0.5;
   	float f4 = (t3 - t2) * 0.5;

   	Point p1, p2, p3, p4;
   	if (nextPoint == 1) {
        p1 = controlPoints[currPoint].position;
        p2 = controlPoints[currPoint].position;
        p3 = controlPoints[nextPoint].position;
        p4 = controlPoints[nnextPoint].position;
   	} else if (nextPoint == controlPoints.size() - 1) {
        p1 = controlPoints[prevPoint].position;
        p2 = controlPoints[currPoint].position;
        p3 = controlPoints[nextPoint].position;
        p4 = controlPoints[nextPoint].position;
   	} else if (nextPoint >= 2) {
        p1 = controlPoints[prevPoint].position;
        p2 = controlPoints[currPoint].position;
        p3 = controlPoints[nextPoint].position;
        p4 = controlPoints[nnextPoint].position;
   	}
   	newPosition = p1 * f1 + p2 * f2 + p3 * f3 + p4 * f4;
    return newPosition;
}
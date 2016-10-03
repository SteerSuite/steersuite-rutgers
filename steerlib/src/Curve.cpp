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
	for (int t = 0; t <= controlPoints.size(); t += window) {
		DrawLib::drawLine(controlPoints[t].position, controlPoints[t + window].position, curveColor, curveThickness);
	}

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
	return;
#endif
}

//parameter function for sorting
bool timeSort(const CurvePoint &lhs, const CurvePoint &rhs) { return lhs.time < rhs.time; }
// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	std::sort(controlPoints.begin(), controlPoints.end(), timeSort);
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

// Check Roboustness (at least 1 start and end point)
bool Curve::checkRobust()
{
	if (controlPoints.size()>=2)
		return true;
	return false;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	for (nextPoint = 0; nextPoint < controlPoints.size(); nextPoint++) {
		if (controlPoints[nextPoint].time > time)
			return true;
	}
	return false;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;

	//getting t scaled to [0, 1]
	normalTime = controlPoints[nextPoint-1].time;
	intervalTime = controlPoints[nextPoint].time;
	float t = (time - normalTime) / (intervalTime - normalTime);

	//hermite functions
	float h1 = 2.0f*pow(t, 3.0f) - 3.0f*pow(t, 2.0f) + 1.0f;
	float h2 = -2.0f*pow(t, 3.0f) + 3.0f * pow(t, 2.0f);
	float h3 = pow(t, 3.0f) - 2.0f * pow(t, 2.0f) + t;
	float h4 = pow(t, 3.0f) - pow(t, 2.0f);

	//hermite parameters
	Point p1 = controlPoints[nextPoint - 1].position;
	Point p2 = controlPoints[nextPoint].position;
	Vector v1 = controlPoints[nextPoint - 1].tangent;
	Vector v2 = controlPoints[nextPoint].tangent;

	// Calculate position at t = time on Hermite curve
	newPosition = h1*p1 + h2*p2 + h3*v1 + h4*v2;

	// Return result
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	//catmull parameters
	Point p0 = controlPoints[nextPoint - 2].position;
	Point p1 = controlPoints[nextPoint-1].position;
	Point p2 = controlPoints[nextPoint].position;
	Point p3 = controlPoints[nextPoint + 1].position;

	//getting t
	float t = (time - controlPoints[nextPoint-1].time)/ (controlPoints[nextPoint].time-controlPoints[nextPoint-1].time);
	
	// Calculate position at t = time on Catmull-Rom curve
	newPosition = 0.5f*(
		(2 * p1) + 
		(-1*p0 + p2)*t + 
		((2*p0 - 5*p1) + (4*p2 - 1*p3))*pow(t, 2.0f) +
		((-1*p0 + 3*p1) - (3*p2 + 1*p3))*pow(t, 3.0f));
	
	// Return result
	return newPosition;
}
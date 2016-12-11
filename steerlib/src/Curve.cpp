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

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve

	if (!checkRobust()) return;

	Point lineEnd ;
	Point lineStart;
	calculatePoint(lineStart, 0);

	for (float t = 0; t <= (controlPoints[controlPoints.size()-1].time + window); t += window)
	{
		calculatePoint(lineEnd, t);
		Util::DrawLib::drawLine( lineStart, lineEnd, curveColor, curveThickness);

		lineStart  = lineEnd;
	}
	
	return;
#endif
}

// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	CurvePoint temp = controlPoints[0];

	int i, j;

	for (i=1; i< (controlPoints.size()); i++)
	{
		temp = controlPoints[i];
		j = i - 1;
		while (j>=0 && controlPoints[j].time > temp.time)
		{
			controlPoints[j+1] = controlPoints[j];
			j--;
		}
		controlPoints[j + 1] = temp;
	}

	return;
}

// Calculate the position on curve corresponding to the given time, outputPoint is the resulting position
// Note that this function should return false if the end of the curve is reached, or no next point can be found
bool Curve::calculatePoint(Point& outputPoint, float time)
{
	int yCounter = 0;

	// Robustness: make sure there is at least two control point: start and end points
	if (!checkRobust())
		return false;

	// Define temporary parameters for calculation
	unsigned int nextPoint;

	// Find the current interval in time, supposing that controlPoints is sorted (sorting is done whenever control points are added)
	// Note that nextPoint is an integer containing the index of the next control point
	if (!findTimeInterval(nextPoint, time))
		return false;
	//else std::cout << nextPoint<< "<-point   " << time << "<-time " <<controlPoints.size() << std::endl;//DELETE THIS LINE WHEN DONE

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
	for (int k = 0; k < controlPoints.size() - 1; k++)
	{
		if (controlPoints[k].time == controlPoints[k + 1].time)
		{
			controlPoints.erase(controlPoints.begin() + k + 1);
		}
	}

	return (controlPoints.size()>=2);
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	unsigned int index = 0;
	int counter = 0;

	if (controlPoints[controlPoints.size()-1].time <= time) return false;

	while (controlPoints[index].time <= time)
	{
		index++;
	}
	nextPoint = index;

	return true;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;
	float ratio;

	normalTime = time - controlPoints[nextPoint - 1].time;
	intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;

	ratio = normalTime / intervalTime;

	Point t0ToPoint = Point((controlPoints[nextPoint - 1].tangent.x),
		(controlPoints[nextPoint - 1].tangent.y),
		(controlPoints[nextPoint - 1].tangent.z));
	Point t1ToPoint = Point((controlPoints[nextPoint].tangent.x),
		(controlPoints[nextPoint].tangent.y),
		(controlPoints[nextPoint].tangent.z));

	Point p0h0, p1h1, t0h2, t1h3;

	p0h0 = controlPoints[nextPoint - 1].position * (2.0 * ratio * ratio * ratio - 3.0 * ratio * ratio + 1.0);
	p1h1 = controlPoints[nextPoint].position * (-2.0 * ratio * ratio * ratio + 3.0* ratio * ratio);
	t0h2 = t0ToPoint * (ratio * ratio * ratio - 2.0 * ratio * ratio + ratio) * intervalTime;
	t1h3 = t1ToPoint * (ratio * ratio * ratio - ratio * ratio) * intervalTime;

	// Return result
	newPosition = p0h0 + p1h1 + t0h2 + t1h3;

	return newPosition;
}



// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;
	float ratio;
	normalTime = time - controlPoints[nextPoint - 1].time;
	intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	ratio = normalTime / intervalTime;
	float tp0 = 0.5*(-ratio + 2 * ratio*ratio - ratio*ratio*ratio);
	float tp1 = 0.5*(2 - 5 * ratio*ratio + 3 * ratio*ratio*ratio);
	float tp2 = 0.5*(ratio + 4 * ratio*ratio - 3 * ratio*ratio*ratio);
	float tp3 = 0.5*(-ratio*ratio + ratio*ratio*ratio);

	if (nextPoint==1)
	{ 
		Point P0 = controlPoints[nextPoint-1].position;
		Point P1 = controlPoints[nextPoint-1].position;
		Point P2 = controlPoints[nextPoint].position;
		Point P3 = controlPoints[nextPoint + 1].position;
		newPosition = P0*tp0 + P1*tp1 + P2*tp2 + P3*tp3;
	}
	else if (nextPoint == controlPoints.size() - 1)
	{
		Point P0 = controlPoints[nextPoint - 2].position;
		Point P1 = controlPoints[nextPoint - 1].position;
		Point P2 = controlPoints[nextPoint].position;
		Point P3 = controlPoints[nextPoint].position;
		newPosition = P0*tp0 + P1*tp1 + P2*tp2 + P3*tp3;
	}

	else
	{
		Point P0 = controlPoints[nextPoint - 2].position;
		Point P1 = controlPoints[nextPoint - 1].position;
		Point P2 = controlPoints[nextPoint].position;
		Point P3 = controlPoints[nextPoint + 1].position;
		newPosition = P0*tp0 + P1*tp1 + P2*tp2 + P3*tp3;
	}

	return newPosition;
}
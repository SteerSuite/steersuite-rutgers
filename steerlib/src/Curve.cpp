

//
// Copyright (c) 2015 Mahyar Khayatkhoei
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
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

	// if not robust, don't attempt to draw
	if (!checkRobust()) {
		return;
	}

	float begin, end, interval;

	begin = controlPoints.front().time;
	end = controlPoints.back().time;
	interval = end - begin;

	int numberPoints = (int)(interval / window);

	float time = begin + window;
	Point prev = controlPoints.front().position;

	for (int i = 0; i < numberPoints; i++) {
		Point curr;

		//if observing the last point, set time to end
		if (i == numberPoints - 1) {
			curr = controlPoints.back().position;
			time = end;
		}
		else if (calculatePoint(curr, time) == false) {
			std::cerr << "Error: Could not draw curve at point: " << i << " at time: " << time;
			return;
		}

		DrawLib::drawLine(prev, curr, curveColor, curveThickness);
		prev = curr;
		time = time + window;
	}

	return;
#endif
}

bool compare(CurvePoint a, CurvePoint b)
{
	return (a.time < b.time);
}

// Sort controlPoints vector in ascending order
void Curve::sortControlPoints()
{

	std::sort(Curve::controlPoints.begin(), Curve::controlPoints.end(), compare);

	//accounts for multiple points at any time t
	for (int i = 0; i < controlPoints.size() - 1; i++) {
		if (controlPoints[i].time == controlPoints[i + 1].time) {
			controlPoints.erase(controlPoints.begin() + i + 1);
		}
	}

	return;
}

// Calculate the position on curve corresponding to the given time, outputPoint is the resulting position
bool Curve::calculatePoint(Point& outputPoint, float time)
{
	// Robustness: make sure there is at least two control point: start and end point
	if (!checkRobust())
		return false;

	// Define temporary parameters for calculation
	unsigned int nextPoint;
	float normalTime, intervalTime;

	// Find the current interval in time, supposing that controlPoints is sorted (sorting is done whenever control points are added)
	if (!findTimeInterval(nextPoint, time))
		return false;

	// Calculate position at t = time on curve
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
	if (getControPoints().size() < 2) {
		return false;
	}
	else {
		return true;
	}
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	for (int i = 1; i < controlPoints.size(); ++i)
	{
		CurvePoint point = controlPoints[i];
		if (time < point.time)
		{
			nextPoint = i;
			return true;
		}
	}
	//can not find the point
	return false;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;

	intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	normalTime = (time - controlPoints[nextPoint - 1].time) / intervalTime;
	float h1, h2, h3, h4, t2, t3;
	t2 = pow(normalTime, 2);
	t3 = pow(normalTime, 3);

	h1 = (2 * t3) - (3 * t2) + 1;
	h2 = (-2 * t3) + (3 * t2);
	h3 = (t3 - 2 * t2 + normalTime) * intervalTime;
	h4 = (t3 - t2) * intervalTime;
	newPosition = h1*controlPoints[nextPoint - 1].position + h2*controlPoints[nextPoint].position
		+ h3*controlPoints[nextPoint - 1].tangent + h4*controlPoints[nextPoint].tangent;

	// Return result
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	// Calculate time interval, and normal time required for later curve calculations
	float normalTime, intervalTime;
	intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	normalTime = (time - controlPoints[nextPoint - 1].time) / intervalTime;

	float h1, h2, h3, h4, t2, t3;
	t2 = pow(normalTime, 2);
	t3 = pow(normalTime, 3);

	h1 = (2 * t3) - (3 * t2) + 1;
	h2 = (-2 * t3) + 3 * t2;
	h3 = t3 - 2 * t2 + normalTime;
	h4 = t3 - t2;

	Vector s0, s1;

	//For the first point
	if (nextPoint == 1)
	{
		s0 = 2 * (controlPoints[nextPoint].position - controlPoints[nextPoint - 1].position)
		- (controlPoints[nextPoint + 1].position - controlPoints[nextPoint - 1].position) / 2;
		s1 = (controlPoints[nextPoint + 1].position - controlPoints[nextPoint - 1].position) / 2;
	}
	else if (nextPoint == getControPoints().size() - 1)		//For the ending of point
	{
		s0 = (controlPoints[nextPoint].position - controlPoints[nextPoint - 2].position) / 2;
		s1 = (controlPoints[nextPoint].position - controlPoints[nextPoint - 2].position) / 2
		- 2 * (controlPoints[nextPoint - 1].position - controlPoints[nextPoint].position);
	}
	else
	{
		//Any other non edge case set of points
		s0 = (controlPoints[nextPoint].position - controlPoints[nextPoint - 2].position) / 2;
		s1 = (controlPoints[nextPoint + 1].position - controlPoints[nextPoint - 1].position) / 2;
	}

	newPosition = h1*controlPoints[nextPoint - 1].position + h2*controlPoints[nextPoint].position + h3*s0 + h4*s1;
	// Return result position
	return newPosition;
}


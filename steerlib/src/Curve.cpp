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
	init(); 
	for (int i = 0; i <= controlPoints.size()-2; i++)
	{
		drawLine(controlPoints[i], controlPoints[i+1], curvecolor, curveThickness);
	}
	

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
	return;
#endif
}

// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
	// Using Selection Sort in ascending order as the sorting algorithm
{
	for (int i = 0; i < controlPoints.size() - 1; i++)
	{
		int j = i;

		for (int y = i + 1; y < controlPoints.size(); y++)
		{
			if (controlPoints[y].time < controlPoints[j].time)
			{
				j = y;
			}
		}

		std::swap(controlPoints[i], controlPoints[j]);
	}
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
bool Curve::checkRobust()
{
	if (controlPoints.size() < 2) {

		return false;
	}


	return true;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		if (controlPoints[i].time > time)
		{
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

	// Used the rescaling formula found here to normalize : https://en.wikipedia.org/wiki/Feature_scaling

	float normT = time - controlPoints[nextPoint - 1].time;
	float intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	float normalTime = normT / intervalTime;
	
	// basis functions for hermite curves based on slides
	/*
		f0(t) = 2t^3 - 3t^2 +1
		f1(t) = -2t^3 + 3t^2
		f2(t) = t^3 -2t^2 +t
		f3(t) = t^3 - t^2

	*/

	float f0 = 2 * pow(normalTime, 3) - 3 * pow(normalTime, 2) + 1;
	float f1 = -2 * pow(normalTime, 3) + 3 * pow(normalTime, 2);
	float f2 = pow(normalTime, 3) - 2pow(normalTime, 2) + normalTime;
	float f3 = pow(normalTime, 3) - pow(normalTime, 2);

	// Hermite Curve Blending Function

	newPosition = ((f0*controlPoints[nextPoint - 1].position) + (f1*controlPoints[nextPoint].position)
		+ ((f2*controlPoints[nextPoint - 1].tangent)*(intervalTime)) + ((f3*controlPoints[nextPoint].tangent)*(intervalTime)));

	// Return result
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	// Calculate position at t = time on Catmull-Rom curve
	Point degree0 = (2 * controlPoints[nextPoint - 1].position);
	Point degree1 = (-1 * controlPoints[nextPoint - 2].position + controlPoint[nextPoint].position);
	Point degree2 = (2*controlPoints[nextPoint - 2].position - 5*controlPoints[nextPoint - 1].position + 4*controlPoints[nextPoint].position - controlPoints[nextPoint + 1].position);
	Point degree3 = (-1*controlPoints[nextPoint - 2].position + 3*controlPoints[nextPoint - 1].position - 3*controlPoints[nextPoint].position + controlPoints[nextPoint + 1].position);

	newPosition = 0.5 * ( degree0 + degree1*time + degree2 * pow(time,2) + degree3 * pow(time,3))
	
	// Return result
	return newPosition;
}
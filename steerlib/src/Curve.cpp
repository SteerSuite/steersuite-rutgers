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
	Point firstpoint = controlPoints[0];

	if (checkRobust()) 
	{
		init();
		for (int i = 0; i <= controlPoints.size() - 1; i = i + window)
		{
			Point startPosition = controlPoints[i].position;
			Point lastPostion = controlPoints[i + 1].position; 
			float startTime = controlPoints[i].time;
			float endTime = controlPoints[i + 1].time;

			if (type == hermiteCurve) {
				1astpoint = useHermiteCurve(i*window, controlpoints[i].time); 
				drawLine(controlPoints[i], controlPoints[i + 1], curvecolor, curveThickness);
			}
			if (type == catMullCurve) {
				1astpoint = useHermiteCurve(i*window, controlpoints[i].time);
				drawLine(controlPoints[i], controlPoints[i + 1], curvecolor, curveThickness);
			}
		}
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
	
	// basis/blending functions for hermite curves based on slides
	/*
		f0(t) = 2t^3 - 3t^2 +1
		f1(t) = -2t^3 + 3t^2
		f2(t) = t^3 -2t^2 +t
		f3(t) = t^3 - t^2

	*/

	float f0 = 2 * pow(normalTime, 3) - 3 * pow(normalTime, 2) + 1;
	float f1 = -2 * pow(normalTime, 3) + 3 * pow(normalTime, 2);
	float f2 = pow(normalTime, 3) - 2*pow(normalTime, 2) + normalTime;
	float f3 = pow(normalTime, 3) - pow(normalTime, 2);

	

	newPosition = ((f0*controlPoints[nextPoint - 1].position) + (f1*controlPoints[nextPoint].position)
		+ ((f2*controlPoints[nextPoint - 1].tangent)*(intervalTime)) + ((f3*controlPoints[nextPoint].tangent)*(intervalTime)));

	// Return result
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;

	// Used the rescaling formula found here to normalize : https://en.wikipedia.org/wiki/Feature_scaling
	float normT = time - controlPoints[nextPoint - 1].time;
	float intervalTime = controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time;
	float normalTime = normT / intervalTime;
	/*
		Used this website to gain more insight on CatMull-Rom splines :http://www.mvps.org/directx/articles/catmull/

		For CatMull-Rom Spline we need 4 control points: [P@(n-1), P@(n), P@(n+1), P@(n+2)]
		A point before time = 0 : p0
		A point at time = 0     : p1
		A point at time = 1     : p2
		A point after time = 1  : p3

	*/

	//Blending Functions for Catmull-Rom Splines were obtained from :http://research.cs.wisc.edu/graphics/Courses/559-f2005/LectureNotes/15-handouts.pdf

	float f0 = (-0.5*normalTime + 1 * pow(normalTime, 2) - 0.5*pow(normalTime, 3));
	float f1 = (1 - 2.5*pow(normalTime, 2) + 1.5*pow(normalTime, 3));
	float f2 = (0.5*normalTime - 2 * pow(normalTime, 2) - 1.5*pow(normalTime, 3));
	float f3 = (-0.5*pow(normalTime, 2) + 0.5*pow(normalTime, 3));
	Point p0, p1, p2, p3;
	if (nextPoint == 1)
	{
		 p0 = controlPoints[nextPoint - 1].position;
		 p1 = p0;
	}
	else 
	{
		 p0 = controlPoints[nextPoint - 2].position;
		 p1 = controlPoints[nextPoint - 1].position;
	}

	if (nextPoint == controlPoints.size()-1)
	{

         p2 = controlPoints[nextPoint].position;
		 p3 = p2;
	}
	else
	{
		 p2 = controlPoints[nextPoint].position;
		 p3 = controlPoints[nextPoint + 1].position;
	}

	newPosition = (p0*((-0.5*normalTime + 1 * pow(normalTime, 2) - 0.5*pow(normalTime, 3))) + p1*((1 - 2.5*pow(normalTime, 2) + 1.5*pow(normalTime, 3)))
		+ p2*((0.5*normalTime - 2 * pow(normalTime, 2) - 1.5*pow(normalTime, 3))) + p3*((-0.5*pow(normalTime, 2) + 0.5*pow(normalTime, 3))));
	// Return result
	return newPosition;
}
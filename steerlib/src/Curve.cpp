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

	//================DELETE THIS PART AND THEN START CODING===================
	static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function drawCurve is not implemented!" << std::endl;
		flag = true;
	}
	//=========================================================================

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
	return;
#endif
}

//Sorting function
bool comparingCurvePoints (Util::CurvePoint a, Util::CurvePoint b) {
	return (a.time < b.time);
}


// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	//Print the vector
	//comment out later..............................................................
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "We are printing the control Points:" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	for (std::vector<Util::CurvePoint>::iterator it = controlPoints.begin(); it != controlPoints.end(); ++it) {
		Util::CurvePoint currentIteration = *it;
		std::cout<<currentIteration.time<<std::endl;
	} 
	//...............................................................................
	
	//CurvePoint parts:
	//Point position;Vector tangent;float time;

	//SORT
	std::sort(controlPoints.begin(), controlPoints.end(), comparingCurvePoints);

	//Print the vector
	//comment out later..............................................................
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "We are printing the sorted! control Points:" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	for (std::vector<Util::CurvePoint>::iterator it = controlPoints.begin(); it != controlPoints.end(); ++it) {
		Util::CurvePoint currentIteration = *it;
		std::cout << currentIteration.time << std::endl;
	}
	//...............................................................................

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
	//================DELETE THIS PART AND THEN START CODING===================
	static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function checkRobust is not implemented!" << std::endl;
		flag = true;
	}
	//=========================================================================


	return true;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	//Print the time
	//comment out later..............................................................
	std::cout << "the time we are looking for is " << time << std::endl;
	//...............................................................................

	//if time is negative, return false
	if (time < 0) {
		return 0;
	}
	//if the time is past the last element, return false
	if (controlPoints.back().time < time) {
		return false;
	}
	//iterate over all the control points and see when time is greater, at which point return 
	for (int i = 1; i < controlPoints.size(); i++) {
		if (time <= controlPoints[i].time) {
			//std::cout << "the time we are looking for is definitely " << time << std::endl;
			//std::cout << "the controlPoint time at " << i << " is " << controlPoints[i].time <<  std::endl;
			nextPoint = i;
			//std::cout << "the next point is " << nextPoint <<  std::endl;
			return true;
		}
	}

	/*if (time > controlPoints.back().time) {
		return false;
	}
	else {
		for (int i = 1; i < controlPoints.size(); i++) {
			if()
		}
	}*/

	return false;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;

	float t1 = controlPoints[nextPoint - 1].time;
	float t2 = controlPoints[nextPoint].time;

	//comment out later..............................................................
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "We are printing the control Points:" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	for (std::vector<Util::CurvePoint>::iterator it = controlPoints.begin(); it != controlPoints.end(); ++it) {
		Util::CurvePoint currentIteration = *it;
		std::cout << currentIteration.time << std::endl;
	}
	std::cout << "Current nextPOint: " << nextPoint << std::endl;
	//...............................................................................

	intervalTime = t2 - t1;
	//normalize time
	normalTime = (time - t1) / intervalTime;
	std::cout << "time: " << time << std::endl;
	std::cout << "normalTime: " << normalTime << std::endl;

	// Calculate position at t = time on Hermite curve

	//calculate the functions
	//f1(t)=2*t^3-3*t^2+1
	float f1 = 2 * normalTime*normalTime*normalTime - 3 * normalTime*normalTime + 1;
	std::cout << "f1: " << f1 << std::endl;
	//f2(t)=-2*t^3+3*t^2
	float f2 = -2 * normalTime*normalTime*normalTime + 3 * normalTime*normalTime;
	std::cout << "f2: " << f2 << std::endl;
	//f3(t)=t^3-2*t^2+t
	float f3 = normalTime*normalTime*normalTime - 2 * normalTime*normalTime + normalTime;
	std::cout << "f3: " << f3 << std::endl;
	//f4(t)=t^3-t^2
	float f4 = normalTime*normalTime*normalTime - normalTime*normalTime;
	std::cout << "f4: " << f4 << std::endl;

	//find Gh=[P1 P4 R1 R4]
	Point P1 = controlPoints[nextPoint - 1].position;
	std::cout << "P1: " << P1 << std::endl;
	Point P4 = controlPoints[nextPoint].position;
	std::cout << "P4: " << P4 << std::endl;
	Vector R1 = controlPoints[nextPoint - 1].tangent;
	std::cout << "R1: " << R1 << std::endl;
	Vector R4 = controlPoints[nextPoint].tangent;
	std::cout << "R4: " << R4 << std::endl;

	//calculate the x(t), y(t) and z(t) functions
	//x(t)=[f1 f2 f3 f4]Gx...
	float xt = f1*P1.x + f2*P4.x + f3*R1.x + f4*R4.x;
	float yt = f1*P1.y + f2*P4.y + f3*R1.y + f4*R4.y;
	float zt = f1*P1.z + f2*P4.z + f3*R1.z + f4*R4.z;

	newPosition.x = xt;
	newPosition.y = yt;
	newPosition.z = zt;

	//std::cout << "the new position for poitn at time " << time << " is: (" << xt << "," << yt << "," << zt << ")." << std::endl;
	std::cout << "newPosition: " << newPosition << std::endl;
	// Return result
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
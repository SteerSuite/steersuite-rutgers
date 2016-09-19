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
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function drawCurve is not implemented!" << std::endl;
		flag = true;
	}*/
	if (!checkRobust())
		return;
	
	for (int i = 0; i < controlPoints.size() ; i++) {
		CurvePoint zeroPoint = controlPoints[i];
		Point startPoint = zeroPoint.position;
		float time_tmp = zeroPoint.time;
		Point endPoint = startPoint;
		while (time_tmp <= controlPoints[i + 1].time) {
			if (!calculatePoint(endPoint, time_tmp))	return;
			Util::DrawLib::drawLine(startPoint, endPoint, curveColor, curveThickness);
			startPoint = endPoint;
			time_tmp += window;
		}
	}
	//=========================================================================

	// Robustness: make sure there is at least two control point: start and end points
	// Move on the curve from t=0 to t=finalPoint, using window as step size, and linearly interpolate the curve points
	// Note that you must draw the whole curve at each frame, that means connecting line segments between each two points on the curve
	
	return;
#endif
}
//sort according to the time
static bool comp_time(CurvePoint& p1, CurvePoint& p2) {
	return p1.time < p2.time;
}

// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	//================DELETE THIS PART AND THEN START CODING===================
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function sortControlPoints is not implemented!" << std::endl;
		flag = true;
	}*/
	//=========================================================================
	std::sort(controlPoints.begin(), controlPoints.end(), comp_time);
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
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function checkRobust is not implemented!" << std::endl;
		flag = true;
	}*/
	//=========================================================================
	return controlPoints.size()>=2;
	//return true;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	//================DELETE THIS PART AND THEN START CODING===================
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function findTimeInterval is not implemented!" << std::endl;
		flag = true;
	}*/
	for (int i = 0; i < controlPoints.size(); i++) {
		if (controlPoints[i].time >= time && i != 0) {
			nextPoint = i;
			return true;
		}
	}
	//=========================================================================


	return false;
}

// Implement Hermite curve
Point Curve::useHermiteCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	float normalTime, intervalTime;

	//================DELETE THIS PART AND THEN START CODING===================
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function useHermiteCurve is not implemented!" << std::endl;
		flag = true;
	}*/
	//=========================================================================

	// Calculate position at t = time on Hermite curve
	CurvePoint nextCurvePoint = controlPoints[nextPoint];
	CurvePoint previousCurvePoint = controlPoints[nextPoint - 1];
	/*normalTime = time;
	intervalTime = (normalTime - previousCurvePoint.time) / (nextCurvePoint.time - previousCurvePoint.time);
	float h00 = 2*pow(intervalTime, 3.0f) - 3*pow(intervalTime, 2.0f) + 1;
	float h10 = pow(intervalTime, 3.0f) - 2*(intervalTime, 2.0f) + intervalTime;
	float h01 = 3*pow(intervalTime, 2.0f) - 2*pow(intervalTime, 3.0f);
	float h11 = pow(intervalTime, 3.0f) - pow(intervalTime, 2.0f);
	newPosition = h00 * previousCurvePoint.position
		+ (h10 * (nextCurvePoint.time - previousCurvePoint.time)) * previousCurvePoint.tangent
		+ h01 * nextCurvePoint.position
		+ (h11 * (nextCurvePoint.time - previousCurvePoint.time)) * nextCurvePoint.tangent;

*/
	normalTime = time - previousCurvePoint.time;
	intervalTime = nextCurvePoint.time - previousCurvePoint.time;
	newPosition = previousCurvePoint.position*(2 * pow(normalTime, 3) / pow(intervalTime, 3) - 3 * pow(normalTime, 2) / pow(intervalTime, 2) + 1)
		+ nextCurvePoint.position*(3 * pow(normalTime, 2) / pow(intervalTime, 2) - 2 * pow(normalTime, 3) / pow(intervalTime, 3))
		+ previousCurvePoint.tangent *(pow(normalTime, 3) / pow(intervalTime, 2) - 2 * pow(normalTime, 2) / intervalTime + normalTime)
		+ nextCurvePoint.tangent *(pow(normalTime, 3) / pow(intervalTime, 2) - pow(normalTime, 2) / intervalTime);
// Return result
	return newPosition;
}

//float calc_t(Point& P1, Point& P2, float pre_t) {
//	float tmp1 = pow((P2.x - P1.x), 2) + pow((P2.y - P1.y), 2) + pow((P2.z - P1.z), 2);
//	tmp1 = sqrt(sqrt(tmp1));
//	return tmp1 + pre_t;
//}


Point minus(Point &P1) {
	return Point(-P1.x, -P1.y, -P1.z);
}
// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;
	//================DELETE THIS PART AND THEN START CODING===================
	/*static bool flag = false;
	if (!flag)
	{
		std::cerr << "ERROR>>>>Member function useCatmullCurve is not implemented!" << std::endl;
		flag = true;
	}*/
	//=========================================================================
	Point P0;
	Point P1;
	Point P2;
	Point P3;
	if (nextPoint > 1 && nextPoint < controlPoints.size() - 1) {
		P0 = controlPoints[nextPoint - 2].position;
		P1 = controlPoints[nextPoint - 1].position;
		P2 = controlPoints[nextPoint].position;
		P3 = controlPoints[nextPoint + 1].position;
	}
	else if (nextPoint == controlPoints.size() - 1) {
		P0 = controlPoints[nextPoint - 2].position;
		P1 = controlPoints[nextPoint - 1].position;
		P2 = controlPoints[nextPoint].position;
		P3 = 2.0f * P2 + minus(P1);
	}
	else {
		P0 = 2.0f * P1 + minus(P2);
		P1 = controlPoints[nextPoint - 1].position;
		P2 = controlPoints[nextPoint].position;
		P3 = controlPoints[nextPoint + 1].position;
	}
		//float t3 = controlPoints[nextPoint + 1].time;
		/*float t0 = 0;
		float t1 = calc_t(P0, P1, t0);
		float t2 = calc_t(P1, P2, t1);
		float t3 = calc_t(P2, P3, t2);*/
		float t = (time - controlPoints[nextPoint - 1].time) / (controlPoints[nextPoint].time - controlPoints[nextPoint - 1].time);
		//time *= (t2 - t1);
		/*Point A1 = P0 * (t1 - time) / (t1 - t0)  + P1 * (time - t0) / (t1 - t0);
		Point A2 = P1 * (t2 - time) / (t2 - t1) + P2 * (time - t1) / (t2 - t1);
		Point A3 = P2 * (t3 - time) / (t3 - t2) + P3 * (time - t2) / (t3 - t2);
		Point B1 = A1 * (t2 - time) / (t2 - t0) + A2 * (time - t0) / (t2 - t0);
		Point B2 = A2 * (t3 - time) / (t3 - t1)  + A3 * (time - t1) / (t3 - t1);
		newPosition = B1 * (t2 - time) / (t2 - t1) + B2 *(time - t1) / (t2 - t1);*/
		//std::cout << time- t2 << std::endl;
		//newPosition = 0.5 *((2 * P1) + (P0 + P2) * t + (2 * P0 - (5 * P1) + (4 * P2) - P3));
		Point tmp1 = 0.5 *(P1 * 2.0f);
		Point tmp2 = (P2 + minus(P0)) * 0.5;
		Point tmp3 = 0.5f * (P0 * 2.0f + minus(P1) * 5.0f + 4.0f * P2 + minus(P3));
		Point tmp4 = 0.5f *(3.0f * P1 + minus(P0) + 3.0f * minus(P2) + P3);
		newPosition = tmp1 + tmp2 * t + tmp3 * t * t + tmp4 * t * t * t;
	// Calculate position at t = time on Catmull-Rom curve
	
	// Return result
	return newPosition;
}
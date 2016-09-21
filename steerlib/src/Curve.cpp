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
	
	Util::Point current = controlPoints[0].position;
	Util::Point startPoint;
	for (int i = 0; i <= (int)controlPoints[controlPoints.size()-1].time; i=i+window) {
		startPoint = current;
		if (!calculatePoint(current, (float)i)) {
			break;
		}
		Util::DrawLib::drawLine(startPoint, current, curveColor, curveThickness);
	}

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

bool sameFunction(Util::CurvePoint a, Util::CurvePoint b) {
	
	//erase duplicates
	return (a.time == b.time);

}


// Sort controlPoints vector in ascending order: min-first
void Curve::sortControlPoints()
{
	//CurvePoint parts:
	//Point position;Vector tangent;float time;

	//SORT
	std::sort(controlPoints.begin(), controlPoints.end(), comparingCurvePoints);
	//erase duplicates
	controlPoints.erase(std::unique(controlPoints.begin(), controlPoints.end(), sameFunction), controlPoints.end());
   // std::vector<Util::CurvePoint>::iterator it;
	 //std::unique(controlPoints.begin(), controlPoints.end(), sameFunction);
	//controlPoints.resize(std::distance(controlPoints.begin(), it));

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
	// Robustness: make sure there is at least two control point: start and end points
	if (controlPoints.size() < 2) {
		return false;
	}
	if (controlPoints[0].time < 0) {
		return false;
	}

	return true;
}

// Find the current time interval (i.e. index of the next control point to follow according to current time)
bool Curve::findTimeInterval(unsigned int& nextPoint, float time)
{
	//Print the time
	//comment out later..............................................................
	//std::cout << "the time we are looking for is " << time << std::endl;
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

	//if (time<t1 || time>t2) {
	//	return;
	//}

	

	intervalTime = t2 - t1;
	//normalize time
	normalTime = (time - t1) / intervalTime;
	

	// Calculate position at t = time on Hermite curve

	//calculate the functions
	//f1(t)=2*t^3-3*t^2+1
	float f1 = 2 * normalTime*normalTime*normalTime - 3 * normalTime*normalTime + 1;
	
	//f2(t)=-2*t^3+3*t^2
	float f2 = -2 * normalTime*normalTime*normalTime + 3 * normalTime*normalTime;
	
	//f3(t)=t^3-2*t^2+t
	float f3 = (normalTime*normalTime*normalTime - 2 * normalTime*normalTime + normalTime)*intervalTime;
	
	//f4(t)=t^3-t^2
	float f4 = (normalTime*normalTime*normalTime - normalTime*normalTime)*intervalTime;
	

	//find Gh=[P1 P4 R1 R4]
	Point P1 = controlPoints[nextPoint - 1].position;
	
	Point P4 = controlPoints[nextPoint].position;

	Vector R1 = controlPoints[nextPoint - 1].tangent;

	Vector R4 = controlPoints[nextPoint].tangent;

	//calculate the x(t), y(t) and z(t) functions
	//x(t)=[f1 f2 f3 f4]Gx...
	float xt = f1*P1.x + f2*P4.x + f3*R1.x + f4*R4.x;
	float yt = f1*P1.y + f2*P4.y + f3*R1.y + f4*R4.y;
	float zt = f1*P1.z + f2*P4.z + f3*R1.z + f4*R4.z;

	newPosition.x = xt;
	newPosition.y = yt;
	newPosition.z = zt;

	// Return result
	return newPosition;
}

// Implement Catmull-Rom curve
Point Curve::useCatmullCurve(const unsigned int nextPoint, const float time)
{
	Point newPosition;

	float normalTime, intervalTime;
	float t1 = controlPoints[nextPoint - 1].time;
	float t2 = controlPoints[nextPoint].time;
	intervalTime = t2 - t1;
	//normalize time
	normalTime = (time - t1) / intervalTime;

	//calculate the functions
	//f1(t)=2*t^3-3*t^2+1
	float f1 = 2 * normalTime*normalTime*normalTime - 3 * normalTime*normalTime + 1;
	//f2(t)=-2*t^3+3*t^2
	float f2 = -2 * normalTime*normalTime*normalTime + 3 * normalTime*normalTime;
	//f3(t)=t^3-2*t^2+t
	float f3 = (normalTime*normalTime*normalTime - 2 * normalTime*normalTime + normalTime)*intervalTime;
	//f4(t)=t^3-t^2
	float f4 = (normalTime*normalTime*normalTime - normalTime*normalTime)*intervalTime;

	// Calculate position at t = time on Catmull-Rom curve
	//4 options
	//1. only 2 points
	//2. point is last one
	//3. point is first
	//4. else

	if (controlPoints.size() == 2) {
		//find Gh=[P1 P4 R1 R4]
		Point P1 = controlPoints[0].position;
		Point P4 = controlPoints[1].position;

		//figure out the two tangents:
		//figure out the first tangent:
		Point y0 = P1;
		Point y1 = P4;

		float t0 = controlPoints[0].time;
		t1 = controlPoints[1].time;

		//first tangent
		Vector R1 = (P4-P1)/(t1-t0);
		//last tangent
		Vector R4 = R1;

		//calculate the x(t), y(t) and z(t) functions
		//x(t)=[f1 f2 f3 f4]Gx...
		newPosition = f1*P1 + f2*P4 + f3*R1 + f4*R4;
	}
	if (nextPoint == controlPoints.size()-1) {
		//find Gh=[P1 P4 R1 R4]
		Point P1 = controlPoints[nextPoint - 1].position;
		Point P4 = controlPoints[nextPoint].position;

		//figure out the two tangents:
		//figure out the first tangent:
		Point y1 = controlPoints[nextPoint - 2].position;
		Point y2 = controlPoints[nextPoint - 1].position;
		Point y3 = controlPoints[nextPoint].position;
		//Point y4 = controlPoints[nextPoint + 1].position;

		t1 = controlPoints[nextPoint - 2].time;
		t2 = controlPoints[nextPoint - 1].time;
		float t3 = controlPoints[nextPoint].time;
		//float t4 = controlPoints[nextPoint + 1].time;

		//pre last point tangent
		Vector R1 = ((t2 - t1) / (t3 - t1))*((y3 - y2) / (t3 - t2)) + ((t3 - t2) / (t3 - t1))*((y2 - y1) / (t2 - t1));
		//last point tangent
		Vector R4 = ((t3 - t1) / (t2 - t1))*((y3 - y2) / (t3 - t2)) - ((t3 - t2) / (t2 - t1))*((y3 - y1) / (t3 - t1));

		//calculate the x(t), y(t) and z(t) functions
		//x(t)=[f1 f2 f3 f4]Gx...
		newPosition = f1*P1 + f2*P4 + f3*R1 + f4*R4;
	} 
	else if (nextPoint == 1) {
		//find Gh=[P1 P4 R1 R4]
		Point P1 = controlPoints[0].position;
		Point P4 = controlPoints[1].position;

		//figure out the two tangents:
		//figure out the first tangent:
		Point y0 = controlPoints[0].position;
		Point y1 = controlPoints[1].position;
		Point y2 = controlPoints[2].position;

		float t0 = controlPoints[0].time;
		 t1 = controlPoints[1].time;
		 t2 = controlPoints[2].time;

		 //1st point tangent
		Vector R1 = ((t2 - t0) / (t2 - t1))*((y1 - y0) / (t1 - t0)) - ((t1 - t0) / (t2 - t1))*((y2 - y0) / (t2 - t0));
		//2nd point tangent
		Vector R4 = ((t1 - t0) / (t2 - t0))*((y2 - y1) / (t2 - t1)) + ((t2 - t1) / (t2 - t0))*((y1 - y0) / (t1 - t0));

		//calculate the x(t), y(t) and z(t) functions
		//x(t)=[f1 f2 f3 f4]Gx...
		newPosition = f1*P1 + f2*P4 + f3*R1 + f4*R4;
	}
	//non boundry condition
	else {
		//find Gh=[P1 P4 R1 R4]
		Point P1 = controlPoints[nextPoint - 1].position;
		Point P4 = controlPoints[nextPoint].position;
		
		
		//figure out the two tangents:
		//figure out the first tangent:
		Point y1 = controlPoints[nextPoint - 2].position;
		Point y2 = controlPoints[nextPoint - 1].position;
		Point y3 = controlPoints[nextPoint].position;
		Point y4 = controlPoints[nextPoint + 1].position;

		 t1 = controlPoints[nextPoint - 2].time;
		 t2 = controlPoints[nextPoint - 1].time;
		float t3 = controlPoints[nextPoint].time;
		float t4 = controlPoints[nextPoint + 1].time;

		Vector R1 = ((t2 - t1) / (t3 - t1))*((y3 - y2) / (t3 - t2)) + ((t3 - t2) / (t3 - t1))*((y2 - y1) / (t2 - t1));
		Vector R4 = ((t3 - t2) / (t4 - t2))*((y4 - y3) / (t4 - t3)) + ((t4 - t3) / (t4 - t2))*((y3 - y2) / (t3 - t2));

		//calculate the x(t), y(t) and z(t) functions
		//x(t)=[f1 f2 f3 f4]Gx...
		newPosition = f1*P1 + f2*P4 + f3*R1 + f4*R4;
	}



	// Return result
	return newPosition;
}
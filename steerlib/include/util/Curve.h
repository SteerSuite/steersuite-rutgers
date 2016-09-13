//
// Copyright (c) 2015 Mahyar Khayatkhoei
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_CURVE_H__
#define __UTIL_CURVE_H__

/// @file AnimationCurve.h
/// @brief Declares a helpful Util::Curve for implementing animation curves.

#include <iostream>
#include <vector>
#include <util/Geometry.h>
#include <util/Color.h>
#include "Globals.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace Util {

	static const int hermiteCurve = 0;
	static const int catmullCurve = 1;

	/* Class containing the data structure that a Curve uses. */

	class UTIL_API CurvePoint
	{
	public:
		CurvePoint(Point inputPosition, Vector& inputTangent, float inputTime) : position(inputPosition),
																tangent(inputTangent), time(inputTime) {}
		Point position;
		Vector tangent;
		float time;
	};

	/* Class for implementing an animation curve. From within any module, class or function, a Curve object can be created.
	** Control points must be passed to this class as CurvePoint objects (one by one, or together in a vector).
	** Any animatable parameter setpoints, can be used to create a vector of CurvePoints and then used to instantiate a Curve object.
	** Curve object, after instantiation, is completely independant and can be used separatly.
	** Any family of curves can be implemented within calculatePoint member function, and no other part of the class needs to be altered.
	*/

	class UTIL_API Curve
	{
	public:
		// Constructors and Destructor
		Curve(int curveType = catmullCurve) : type(curveType) {}
		Curve(const CurvePoint& startPoint, int curveType = catmullCurve);
		Curve(const std::vector<CurvePoint>& inputPoints, int curveType = catmullCurve);
		~Curve() {}

		// Get and set type
		void setType(int curveType) { type = curveType; }
		int getType() { return type; }

		// Add one control point to the vector controlPoints
		void addControlPoint(const CurvePoint& inputPoint);

		// Add a vector of control points to the vector controlPoints
		void addControlPoints(const std::vector<CurvePoint>& inputPoints);

		// Return the controlPoints vector
		std::vector<CurvePoint> getControPoints() { return controlPoints; }

		// Draw the curve shape on screen, usign window as step size (bigger window: less accurate shape)
		void drawCurve(Color curveColor = gOrange, float curveThickness = 2.f, int window = 5);

		// Calculate the position on curve corresponding to the given time, outputPoint is the resulting position
		bool calculatePoint(Point& outputPoint, float time);

		// Sort controlPoints vector in ascending order: min-first
		void sortControlPoints();

	private:
		// Contains the controlPoints that are used to create the curve
		std::vector<CurvePoint> controlPoints;

		// Type of curve
		int type;

		// Check Roboustness
		bool checkRobust();

		// Find the current time interval (i.e. index of the next control point to follow according to current time)
		bool findTimeInterval(unsigned int& nextPoint, float time);

		// Implement Hermite curve
		Point useHermiteCurve(const unsigned int nextPoint, const float time);

		// Implement Catmull-Rom curve
		Point useCatmullCurve(const unsigned int nextPoint, const float time);
	};
}

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif

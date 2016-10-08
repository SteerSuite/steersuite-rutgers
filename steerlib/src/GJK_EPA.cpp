#include "obstacles/GJK_EPA.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <unordered_map>

using Object = ::std::vector<Util::Vector>;


namespace {
	// This works for polygons
	Util::Vector supportVector(const Object &o, const Util::Vector &v)
	{
		assert(!o.empty());
		using ValVec = ::std::pair<float, Util::Vector>;
		ValVec max(o[0] * v, o[0]);

		for (const Util::Vector &vert : o) {
			float dist = vert * v;
			if (dist > max.first)
				max = ::std::make_pair(dist, vert);
		}
		return max.second;
	}

	Util::Vector minkDiffSupport(const Object &a, const Object &b,
	                             const Util::Vector &d)
	{
		return supportVector(a, d) - supportVector(b, -d);
	}

	Util::Vector normalInY(const Util::Vector &v)
	{
		assert(v.y == 0);
		Util::Vector y(0, 1, 0);
		Util::Vector res = cross(v, y);
		assert(res.y == 0);
		return res;
	}

	/* Return true if the simplex contains origin */
	bool testSimplex(::std::deque<Util::Vector> &simplex, Util::Vector &dir)
	{
		switch (simplex.size())
		{
		case 1:
			dir = -dir;
			return simplex.back() == Util::Vector();
		case 2: {
			/* Our last search direction is in the direction of
			 * simplex[0] -> simplex[1]. the direction from
			 * simplex[1] -> O is acute, there's a point on the
			 * line from simplex[0] -> simplex[1], that is closer
			 * to origin than either endpoint. Otherwise, simplex[1]
			 * is closest than anything else */
			Util::Vector connect = (simplex[0] - simplex[1]);
			if ((connect * -simplex[1]) > 0) {
				/* Go perpendicular to the line. */
				dir = cross(cross(connect, -simplex.back()), -dir);
			} else {
				/* Remove the earlier point and form
				 * a new point simplex. This will become
				 * a line again after next call to support */
				simplex.pop_front();
				dir = -simplex.back();
			}
			/* if the length of the direction is 0,
			 * the origin is on the line */
			return dir == Util::Vector();
		}
		case 3: {
			/* We already know that the origin was closer to the
		         * line simplex[0]->simplex[1] than any of the two
		         * vertices. Thus we need to check, the 2 newly formed
		         * lines, the last vertex, or the inside of the
		         * triangle (we end there) */
			Util::Vector connect1 = (simplex[0] - simplex[2]);
			Util::Vector connect2 = (simplex[1] - simplex[2]);
			if (((connect1 * -simplex.back()) <= 0) &&
			    ((connect2 * -simplex.back()) <= 0)) {
				/* Reduce simplex to one point */
				simplex.pop_front();
				simplex.pop_front();
				dir = -simplex.back();
				return false;
			}

			Util::Vector norm = cross(connect1, connect2);
			Util::Vector connect1_norm = cross(connect1, norm);
			Util::Vector connect2_norm = cross(norm, connect2);

			float dot_1 = connect1_norm * -simplex.back();
			float dot_2 = connect2_norm * -simplex.back();

			if (dot_1 >= 0) {
				dir = connect1_norm;
				simplex[1] = simplex[0];
				simplex.pop_front();
				return dot_1 == 0;
			}

			if (dot_2 >= 0) {
				dir = connect2_norm;
				simplex.pop_front();
				return dot_2 == 0;
			}
			/* Now we are inside the triangle */
			float dot_triangle = norm * -simplex.back();
			dir = dot_triangle > 0 ? norm : -norm;

			/* All our points are on a plane */
			assert(dot_triangle == 0);
			return dot_triangle == 0;
		}
		case 4: /* we work in 3d space so tetrahedron is theoretically
			 * possible. However, our points are expected to be on
			 * one plane so this should not happen */
		default:
			assert(false);
		}
	}
}



SteerLib::GJK_EPA::GJK_EPA()
{
}

//Look at the GJK_EPA.h header file for documentation and instructions
bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	/* GJK first */
	return_penetration_depth =.0f;
	/* Direction from a point to zero */
	Util::Vector dir(_shapeB.front() - _shapeA.front());
	/* I don't add any points to simplex before computing support to
	 * guarantee that the points are on boundaries of the Mink shape */
	::std::deque<Util::Vector> simplex;
	do {
		Util::Vector point = minkDiffSupport(_shapeA, _shapeB, dir);
		/* For any direction, projection needs to include the origin.
		 * Otherwise there's a separating line/plane between the shape
		 * and the origin */
		if (((-dir) * (-point)) < 0)
			return false;
		simplex.push_back(point);
	} while (!testSimplex(simplex, dir));

	/* We only reach this place if there is a collision.
	 * Run EPA to get the vector and depth */

	return true;
}

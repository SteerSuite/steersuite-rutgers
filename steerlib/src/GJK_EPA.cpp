/**/
#include "obstacles/GJK_EPA.h"


SteerLib::GJK_EPA::GJK_EPA()
{
}

//find the polygon centers
Util::Vector findCenter(const std::vector<Util::Vector>& _shape)
{
	Util::Vector res(0,0,0);
	for (int i = 0; i < _shape.size(); i++)
	{
		res.x += _shape[i].x;
		res.y += _shape[i].y;
		res.z += _shape[i].z;
	}
	if (_shape.size() == 0)
		return res;
	res.x /= _shape.size();
	res.y /= _shape.size();
	res.z /= _shape.size();

	return res;
}

Util::Vector getFarthestPointInDirection(const std::vector<Util::Vector>& _shape, Util::Vector d)
{
	Util::Vector res(0,0,0);
	float current = 0;
	float highst = FLT_MIN;

	for (int i = 0; i < _shape.size(); i++)
	{
		current = _shape[i] * d;
		if (current > highst)
		{
			highst = current;
			res = _shape[i];
		}
	}
	return res;
}
Util::Vector support(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, Util::Vector d)
{
	Util::Vector p1 = getFarthestPointInDirection(_shapeA,d);
	Util::Vector p2 = getFarthestPointInDirection(_shapeB, -1 * d);

	return (p1 - p2);
}

Util::Vector findNewDirection(std::vector<Util::Vector>& simplex)
{
	Util::Vector last = simplex.back();
	Util::Vector nextToLast = simplex[simplex.size() - 2];

	Util::Vector Origin(0,0,0);
	Util::Vector AB = last - nextToLast;
	Util::Vector AO = Origin - last;

	//a x b x c = b(a.c) - c(a.b)
	Util::Vector newDirection = AO * (AB*AB) - AB * (AB*AO);
	return newDirection;
}
struct Edge
{
	float closestDis;
	Util::Vector normal;
	int index;
};

Edge findCloestEdge(std::vector<Util::Vector>& simplex)
{
	Edge res;
	float dist;
	Util::Vector normal;
	float shortestDist = FLT_MAX;

	for (int i = 0; i < simplex.size(); i++)
	{
		int j = i + 1 == simplex.size() ? 0 : i + 1;
		Util::Vector A = simplex[i];
		Util::Vector B = simplex[j];

		Util::Vector AB = simplex[j] - simplex[i];
		Util::Vector AO = -1 * A;

		if (fabs(fabs(AB*AO) - AB.norm() * AO.norm()) >= 0.0001)
			normal = AO*(AB*AB) - AB*(AB*AO);
		else {
			normal.x = -1 * AB.z;
			normal.y = 0;
			normal.z = AB.x;
		}

		normal = Util::normalize(normal);
		dist = fabs(AO*normal);

		normal = Util::normalize(normal);

		if (shortestDist > dist)
		{
			res.normal = -1 * normal;
			res.index = j;
			res.closestDis = dist;
			shortestDist = dist;
		}
	}

	return res;
}

bool onEdge(Util::Vector A, Util::Vector B)
{
	float x_ratio = 0, y_ratio = 0, z_ratio = 0;
	bool x_para = false, y_para = false, z_para = false;
	Util::Vector Origin(0, 0, 0);
	Util::Vector AB = B - A;
	Util::Vector AO = Origin - A;
	int rsize;

	if (AB.x == 0 || AO.x == 0) {
		if (AB.x == 0 && AO.x == 0)
		{
			x_para = true;
		}
		else
		{
			return false;
		}
	}
	else
		x_ratio = AO.x / AB.x;

	if (AB.y == 0 || AO.y == 0) {
		if (AB.y == 0 && AO.y == 0)
		{
			y_para = true;

		}
		else
		{
			return false;
		}
	}
	else
		y_ratio = AO.y / AB.y;

	if (AB.z == 0 || AO.z == 0) {
		if (AB.z == 0 && AO.z == 0)
		{
			z_para = true;
		}
		else
		{
			return false;
		}
	}
	else
		z_ratio = AO.z / AB.z;

	std::vector<float> r;
	if (!x_para)
		r.push_back(x_ratio);
	if (!y_para)
		r.push_back(y_ratio);
	if (!z_para)
		r.push_back(z_ratio);

	rsize = r.size();
	if (rsize == 1)
		return(r[0] >= 0 && r[0] <= 1);
	else if (rsize == 2) {
		if (r[0] == r[1])
			return(r[0] >= 0 && r[0] <= 1);
		else
			return false;
	}
	else if (rsize == 3) {
		if (r[0] == r[1] && r[1] == r[2])
			return(r[0] >= 0 && r[0] <= 1);
		else
			return false;
	}
	else
		return true;
}

bool containsOrigin(std::vector<Util::Vector> &simplex)
{
	Util::Vector A = simplex[2];
	Util::Vector B = simplex[1];
	Util::Vector C = simplex[0];

	Util::Vector AO = -1 * A;

	Util::Vector AB = B - A;
	Util::Vector AC = C - A;

	Util::Vector ABperp = AC * (AB * AB) - AB* (AB * AC);
	Util::Vector ACperp = AB * (AC * AC) - AC * (AC * AB);

	//check if origin is on edge
	if (onEdge(A, B) || onEdge(A, C)) {
		return true;
	}

	//if the origin in area
	if (ABperp * AO <= 0) {
		simplex.erase(simplex.begin());
		return false;
	}
	else if (ACperp * AO <= 0) {
		simplex.erase(simplex.begin() + 1);
		return false;
	}
	else {
		return true;
	}
}
void EPA(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector> &simplex)
{
	Edge closestEdge;

	while (true)
	{
		closestEdge = findCloestEdge(simplex);
		Util::Vector newSupport = support(_shapeA, _shapeB, closestEdge.normal);

		float distance = fabs(newSupport*closestEdge.normal - closestEdge.closestDis);

		if (distance <= 0.001)
		{
			return_penetration_depth = closestEdge.closestDis;
			return_penetration_vector = closestEdge.normal;
			return;
		}
		else
		{
			simplex.insert(simplex.begin() + closestEdge.index, newSupport);
		}
	}
}

//Look at the GJK_EPA.h header file for documentation and instructions
bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	//Simplex to be build using points found via Minkowski differences
	std::vector<Util::Vector> simplex;

	//calculates for collision
	//gets centers of Minkowski difference viadifference of center of both shapes.
	Util::Vector direction = findCenter(_shapeA) - findCenter(_shapeB);
	if (direction == Util::Vector(0, 0, 0)) {
		EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
		return true;
	}

	//first 2 points on the simplex
	simplex.push_back(support(_shapeA, _shapeB, direction));
	direction = direction*-1;
	simplex.push_back(support(_shapeA, _shapeB, direction));

	//build the simplex
	while (true) {
		//get new direction
		direction = findNewDirection(simplex);

		//check if direction is origin
		if (direction == Util::Vector(0, 0, 0)) {
			Util::Vector pt1 = simplex[0];
			Util::Vector pt2 = simplex[1];
			//origin on edge
			if (onEdge(pt1, pt2)) {
				EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
				return true;
			}
			else {
				return false;
			}
		}

		//add new minkowski difference vertex
		simplex.push_back(support(_shapeA, _shapeB, direction));

		//check simplex contains origin
		if (simplex[2] * direction <= 0) {
			//simplex past origin
			return false;
		}
		else {
			//Check the simplex area
			if (containsOrigin(simplex))
			{
				EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
				return true;
			}
		}

	}
}


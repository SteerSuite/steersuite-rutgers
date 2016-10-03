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
	float rx = 0, ry = 0, rz = 0;
	bool ex = false, ey = false, ez = false;
	Util::Vector Origin(0, 0, 0);
	Util::Vector AB = B - A;
	Util::Vector AO = Origin - A;
	int rsize;

	if (AB.x == 0 || AO.x == 0) {
		if (!(AB.x == 0 && AO.x == 0))
			return false;
		ex = true;
	}
	else
		rx = AO.x / AB.x;

	if (AB.y == 0 || AO.y == 0) {
		if (!(AB.y == 0 && AO.y == 0))
			return false;
		ey = true;
	}
	else
		ry = AO.y / AB.y;

	if (AB.z == 0 || AO.z == 0) {
		if (!(AB.z == 0 && AO.z == 0))
			return false;
		ez = true;
	}
	else
		rz = AO.z / AB.z;

	std::vector<float> r;
	if (!ex)
		r.push_back(rx);
	if (!ey)
		r.push_back(ry);
	if (!ez)
		r.push_back(rz);

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



//#include "obstacles/GJK_EPA.h"
//
//	   SteerLib::GJK_EPA::GJK_EPA()
//{
//
//
//}
//
////calculates the center of input shape
//Util::Vector calculateCenter(const std::vector<Util::Vector>& _shape) {
//	Util::Vector retVect(0, 0, 0);
//
//	//add the x/y/z vals of every point in the shape
//	for (std::vector<Util::Vector>::const_iterator iter = _shape.begin(); iter != _shape.end(); ++iter)
//	{
//		retVect.x = retVect.x + iter->x;
//		retVect.y = retVect.y + iter->y;
//		retVect.z = retVect.z + iter->z;
//	}
//
//	//divide by number of points to get average
//	if (_shape.size() > 0)
//	{
//		retVect.x = retVect.x / _shape.size();
//		retVect.y = retVect.y / _shape.size();
//		retVect.z = retVect.z / _shape.size();
//	}
//
//	return retVect;
//}
//
////given a shape and direction vector, determines farthest point on the shape in that direction
//Util::Vector farthestPoint(const std::vector<Util::Vector>& _shape, Util::Vector direction)
//{
//	//iterator to keep track of current farthest point
//	std::vector<Util::Vector>::const_iterator currentFarthest = _shape.begin();
//	float currentDot = 0;
//	float highestDot = -999999999;
//
//	//loop through all points in shape, find one with greatest dot product with the desired direction. will be farthest point
//	for (std::vector<Util::Vector>::const_iterator iter = _shape.begin(); iter != _shape.end(); iter++)
//	{
//		currentDot = (iter->x * direction.x) + (iter->y * direction.y) + (iter->z * direction.z);
//		if (currentDot > highestDot)
//		{
//			highestDot = currentDot;
//			currentFarthest = iter;
//		}
//	}
//	Util::Vector returnVector(currentFarthest->x, currentFarthest->y, currentFarthest->z);
//	return returnVector;
//}
//
////outputs a point for the minkowski difference
//Util::Vector support(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, Util::Vector direction)
//{
//	//Calculates a Minkowski difference
//	Util::Vector pointA = farthestPoint(_shapeA, direction);
//	Util::Vector pointB = farthestPoint(_shapeB, (direction*-1));
//
//	return (pointA - pointB);
//
//}
//
////calculates new direction to grow the simplex, tries to enclose origin
//Util::Vector calculateDirection(const std::vector<Util::Vector>& simplex)
//{
//	//Uses last 2 points in the simplex to calculate a new direction
//	std::vector<Util::Vector>::const_reverse_iterator reverse = simplex.rbegin();
//	Util::Vector last(reverse->x, reverse->y, reverse->z);
//	reverse = reverse + 1;
//	Util::Vector nextLast(reverse->x, reverse->y, reverse->z);
//
//
//	//calculates new direction in way that most likely encloses origin if collision exists
//	Util::Vector Origin(0, 0, 0);
//	Util::Vector AB = last - nextLast;// nextLast - last;
//	Util::Vector AO = Origin - last;
//
//	Util::Vector retDir = AO*(AB*AB) - AB*(AB*AO);//cross(cross(AB, AO), AB);
//	return retDir;
//}
//
//
//// (AC x AB) x AB = AB(AB.dot(AC)) - AC(AB.dot(AB))
///*
//Code taken out because of infinite looping error
//Util::Vector tripCross(Util::Vector AC, Util::Vector AB) {
//return cross(cross(AC, AB), AB);
//}
//*/
//
//
//
//bool onEdge(Util::Vector A, Util::Vector B) {
//	float rx = 0, ry = 0, rz = 0;
//	bool ex = false, ey = false, ez = false;
//	Util::Vector Origin(0, 0, 0);
//	Util::Vector AB = B - A;
//	Util::Vector AO = Origin - A;
//	int rsize;
//
//	if (AB.x == 0 || AO.x == 0) {
//		if (!(AB.x == 0 && AO.x == 0))
//			return false;
//		ex = true;
//	}
//	else
//		rx = AO.x / AB.x;
//
//	if (AB.y == 0 || AO.y == 0) {
//		if (!(AB.y == 0 && AO.y == 0))
//			return false;
//		ey = true;
//	}
//	else
//		ry = AO.y / AB.y;
//
//	if (AB.z == 0 || AO.z == 0) {
//		if (!(AB.z == 0 && AO.z == 0))
//			return false;
//		ez = true;
//	}
//	else
//		rz = AO.z / AB.z;
//
//	std::vector<float> r;
//	if (!ex) r.push_back(rx);
//	if (!ey) r.push_back(ry);
//	if (!ez) r.push_back(rz);
//
//	rsize = r.size();
//	if (rsize == 1)
//		return(r[0] >= 0 && r[0] <= 1);
//	else if (rsize == 2) {
//		if (r[0] == r[1])
//			return(r[0] >= 0 && r[0] <= 1);
//		else
//			return false;
//	}
//	else if (rsize == 3) {
//		if (r[0] == r[1] && r[1] == r[2])
//			return(r[0] >= 0 && r[0] <= 1);
//		else
//			return false;
//	}
//	else
//		return true;
//}
///*
//bool onEdge(Util::Vector A, Util::Vector B) {
//return false;
//}
//*/
//
//bool checkSimplex(std::vector<Util::Vector>& simplex)
//{  // get the last point added to the simplex
//	/*
//	std::vector<Util::Vector>::const_reverse_iterator reverse = simplex.rbegin();
//	Util::Vector A(reverse->x, reverse->y, reverse->z);
//	// compute AO
//	Util::Vector AO = Util::Vector(0, 0, 0) - A;
//	// then its the triangle case
//	// get b and c
//	reverse++;
//	Util::Vector B(reverse->x, reverse->y, reverse->z);
//	reverse++;
//	Util::Vector C(reverse->x, reverse->y, reverse->z);
//	*/
//
//
//	Util::Vector A = simplex[2];
//	Util::Vector B = simplex[1];
//	Util::Vector C = simplex[0];
//
//	Util::Vector AO = Util::Vector(0, 0, 0) - A;
//
//	// compute the edges
//	Util::Vector AB = B - A;
//	Util::Vector AC = C - A;
//	// compute the normals
//	Util::Vector ABperp = AC*(dot(AB, AB)) - AB*(dot(AB, AC));//tripCross(AC, AB);
//	Util::Vector ACperp = AB*(dot(AC, AC)) - AC*(dot(AC, AB));//tripCross(AB, AC);
//
//	//checks simplex edges for the origin of remaining 2 edges
//	if (onEdge(A, B) || onEdge(A, C)) {
//		return true;
//	}
//
//	//following checks area around simplex
//	// is the origin in R4
//	if (dot(ABperp, AO) <= 0) {
//		// remove point c
//		simplex.erase(simplex.begin());
//		return false;
//	}
//	else if (dot(ACperp, AO) <= 0) {
//		// is the origin in R3
//		// remove point b
//		simplex.erase(simplex.begin() + 1);
//		return false;
//	}
//	else {
//		// otherwise we know its in R5 so we can return true
//		return true;
//	}
//
//}
//
//typedef struct Edge {
//	float dist;
//	Util::Vector perp;
//	unsigned int index;
//
//};
//
////given simplex, finds the vector 
//Edge findClosest(std::vector<Util::Vector> simplex) {
//
//	Edge closest;
//	int i, next;
//	float dist;
//	float shortest;
//	Util::Vector AO, AB, normal;
//
//	for (i = 0; i < simplex.size(); i++) {
//		next = i + 1;
//		if (next == simplex.size())
//			next = 0;
//
//		AO = -1 * simplex[i];
//		AB = simplex[next] - simplex[i];
//
//		if (fabs(fabs(AB*AO) - AB.norm() * AO.norm()) >= 0.0001)
//			normal = AO*(AB*AB) - AB*(AB*AO);
//		else {
//			normal.x = -1 * AB.z;
//			normal.y = 0;
//			normal.z = AB.x;
//		}
//
//		normal = Util::normalize(normal);
//		dist = fabs(AO*normal);
//
//		shortest = FLT_MAX;
//
//		if (shortest > dist) {
//			closest.perp = -1 * normal;
//			closest.index = next;
//			closest.dist = dist;
//			shortest = dist;
//		}
//	}
//	return closest;
//}
//
//void EPA(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>Simplex)
//{
//
//	Edge closestEdge;
//	float dist;
//
//	while (true) {
//		//given simplex
//		closestEdge = findClosest(Simplex);
//		Util::Vector supp = support(_shapeA, _shapeB, closestEdge.perp);
//
//		dist = fabs(supp*closestEdge.perp - closestEdge.dist);
//
//		if (dist >= 0.0002)
//			Simplex.insert(Simplex.begin() + closestEdge.index, supp);
//		else {
//			return_penetration_vector = closestEdge.perp;
//			return_penetration_depth = closestEdge.dist;
//			return;
//		}
//	}
//}
//
////Look at the GJK_EPA.h header file for documentation and instructions
//bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
//{
//	//Simplex to be build using points found via Minkowski differences
//	std::vector<Util::Vector> simplex;
//
//	//calculates for collision
//	//gets centers of Minkowski difference viadifference of center of both shapes.
//	Util::Vector direction = calculateCenter(_shapeA) - calculateCenter(_shapeB);
//	if (direction == Util::Vector(0, 0, 0)) {
//		EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
//		return true;
//	}
//
//	//first 2 points on the simplex
//	simplex.push_back(support(_shapeA, _shapeB, direction));
//	direction = direction*-1;
//	simplex.push_back(support(_shapeA, _shapeB, direction));
//
//	//build the simplex
//	while (true) {
//		//get new direction
//		direction = calculateDirection(simplex);
//
//		//check if direction is origin
//		if (direction == Util::Vector(0, 0, 0)) {
//			Util::Vector pt1 = simplex[0];
//			Util::Vector pt2 = simplex[1];
//			//origin on edge
//			if (onEdge(pt1, pt2)) {
//				EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
//				return true;
//			}
//			else {
//				return false;
//			}
//		}
//
//		//add new minkowski difference vertex
//		simplex.push_back(support(_shapeA, _shapeB, direction));
//
//		//check simplex contains origin
//		if (simplex[2] * direction <= 0) {
//			//simplex past origin
//			return false;
//		}
//		else {
//			//Check the simplex area
//			if (checkSimplex(simplex))
//			{
//				EPA(return_penetration_depth, return_penetration_vector, _shapeA, _shapeB, simplex);
//				return true;
//			}
//		}
//
//	}
//	return false;
//}

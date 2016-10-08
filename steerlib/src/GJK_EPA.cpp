#include "obstacles/GJK_EPA.h"
#include <math.h>
#include "include/util/Geometry.h"

SteerLib::GJK_EPA::GJK_EPA()
{
}

//Look at the GJK_EPA.h header file for documentation and instructions

//Using this video as referance to write the algorithms: https://www.youtube.com/watch?v=Qupqu1xe7Io


bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	std::vector<Util::Vector> simplex;

	if (GJK(_shapeA, _shapeB, simplex))
	{

		EPA(_shapeA, _shapeB, simplex, return_penetration_vector, return_penetration_depth);
		return true; // There is a collision.
	}
	else
	{
		return false; // There is no collision.
	}

}
//Used to find the farthest point in a shape along a given direction. This is needed to find the Minkowski Difference.
Util::Vector getFarthestPoint(const std::vector<Util::Vector>& shape, const Util::Vector& direction)
{


	float farthestDistance = 0;
	int index = 0;
	int i = 0;

	do
	{
		
		float check = dot(shape[i], direction);
		if (check > farthestDistance)
		{
			farthestDistance = check;
			index = i;

		}
		i++;
	
	} while (index<shape.size());
	
	return shape[index];

}

//Solves for the Minkowski Difference.
Util::Vector support(const std::vector<Util::Vector>& shapeA, const std::vector<Util::Vector>& shapeB, const Util::Vector& direction)
{
	Vector opposite = (-1) * direction;
	Util::Vector MinDiff = getFarthestPoint(shapeA, direction) -  getFarthestPoint(shapeB, opposite);
	return MinDiff;
}

bool SteerLib::GJK_EPA::GJK(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>& simplex)
{
	Util::Vector dir(0, 0, 1);
	Util::Vector newdir = (-1) * dir;
	Util::Vector i = support(_shapeA, _shapeB, dir);
	
	simplex.push_back(i);

	do
	{
		i = support(_shapeA, _shapeB, dir);
		simplex.push_back(i);

		if (originInside(newdir, simplex)) 
		{
			return true;
		}

		else
		{
			if (dot(simplex.back(), newdir)<= 0) 
			{
				return false;
			}
		}

	} while (true);
}

//either = 3 or < 3, checks if area is greater than 3 
bool SteerLib::GJK_EPA::originInside(Util::Vector& d, std::vector<Util::Vector>& simplex)
{
	//Watch video, need to find if the shape has the origin contained inside of it, based on the size of the simplex.
	Vector a = simplex.back();
	Vector b;
	Vector c;
	Vector ab;
	Vector ac;
	Vector abNorm; // normal vector to ab
	Vector acNorm; // normal vector to ac
	Vector a_neg = -1 * a;

	if (simplex.size() == 3) {
		//triangle case
		//get vectors b(middle) and c(first)
		b = simplex[1];
		c = simplex[0];
		//create edges from vectors
		ab = b - a;
		ac = c - a;

		//calculate the norms of the edges
		abNorm = tripleProduct(ac, ab, ab);
		acNorm = tripleProduct(ab, ac, ac);
		//
		if (abNorm.dot(a_neg) > 0) {
			//erase the c vector
			simplex.erase(c);

			d = abNorm;
		}
		else {
			//
			if (acNorm.dot(a_neg) > 0) {
				//erase the b vector
				simplex.erase(b);
				d = acNorm;
			}
			else {
				return true;
			}

		}
	}

	else {

		b = simplex.getB();
		//calculate AB
		ab = b - a;
		// normal vector  to ab in the direction pointing toward the origin
		abNorm = tripleProduct(ab, a_neg, ab);
		//set the direction to be equal to abNorm
		d = abNorm;
	}

	return false;

} 

//called by epa 
float SteerLib::GJK_EPA:closestEdge(std::vector<Util::Vector> s, Util::Vector& closestEdge, int& closestEdgeIndex)
{
	float closest = 100000;

	for (int i = 0; i < s.size(); i++ )
	{
		int j; 

		Util::Vector a = s.at(i);
		Util::Vector b = s.at(i);

		if (i + 1 == s.size()) 
		{
			Util::Vector a = s.at(i);
			Util::Vector b = s.at(0);
		}else{
			Util::Vector a = s.at(i);
			Util::Vector b = s.at(i + 1);
		}

		Util::Vector edge;
		std::set_difference(a.front, a.back, b.front, b.back, std::back_inserter(edge));

		Util::Vector edgeNormalTowardOrigin = tripleProduct(edge, a, edge); 
		normalize(edgeNormalTowardOrigin);

		int init = 100;
		double distance = std::inner_product(edgeNormalTowardOrigin.front, edgeNormalTowardOrigin.back, a.front, init);
		
		if (distance < closestEdgeDistance)
		{
			closest = distance;
			closestEdge = edgeNormalTowardOrigin;
			closestEdgeIndex = i;
		}

	}

	return closest; 
}



//is there a collision, return penitration vector, depth 
float SteerLib::GJK_EPA::EPA(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, const std::vector<Util::Vector>& simplex, Util::Vector& penetration_vector, float& depth)
{
	Until::Vector closestEdge; 
	int closestEdgeIndex; 

	while (true)
	{
		float edgeDistance = edgeDistance(simplex, closestEdge, closestEdgeIndex);
		Util::Vector support = support(_shapeA, _shapeB, noralize(edge));
		double distance = std::inner_product(support, support., normalize(closestEdge));

		if (distance - edgeDistance < 0.00001)
		{
			penetration_vector = normalize(closestEdge);
			depth = distance;
		}
		else() {
			simplex.insert(support, closestEdgeIndex);
		}
	}


	}
}

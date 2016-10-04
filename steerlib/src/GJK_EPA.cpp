#include "obstacles/GJK_EPA.h"


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
//Used to find the farthest point in a shape along a given direction. This is needed to find the Minowski Difference.
Util::Vector getFarthestPoint(const std::vector<Util::Vector>& shape, const Util::Vector& direction)
{
	Util::Vector farthestPoint(0, 0, 0);

	float farthestDistance = 0;
	int index = 0;

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

//Solves for the Minowski Difference.
Util::Vector support(const std::vector<Util::Vector>& shapeA, const std::vector<Util::Vector>& shapeB, const Util::Vector& direction)
{
	Vector opposite = (-1) * direction;
	Util::Vector MinDiff = getFarthestPoint(shapeA, direction) -  getFarthestPoint(shapeB, opposite);
	return MinDiff;
}

bool SteerLib::GJK_EPA::GJK(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>& simplex)
{

}
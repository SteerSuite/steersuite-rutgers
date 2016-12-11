#include "obstacles/GJK_EPA.h"
#include <math.h> 

SteerLib::GJK_EPA::GJK_EPA()
{
}
//Look at the GJK_EPA.h header file for documentation and instructions
bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	std::vector<Util::Vector> Simplex;
	if (GJK(_shapeA, _shapeB, Simplex))
	{
		return_penetration_depth = EPA(_shapeA, _shapeB, Simplex, return_penetration_vector );
		return true;
	}
	else
	{
		return false;
	}
}

float SteerLib::GJK_EPA::EPA(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, const std::vector<Util::Vector>& Simplex, Util::Vector& penetration_vector )
{
	std::vector<Util::Vector> s = Simplex;
	float tolerance = 0.000001;
	Util::Vector closestEdge;
	int closestEdgeIndex;
	float d;
	while(true)
	{
		d = findClosestEdgeDistance(s, closestEdge, closestEdgeIndex);
		Util::Vector p = Support(_shapeA, _shapeB, closestEdge/closestEdge.norm());

		float distance = p*(closestEdge/closestEdge.norm());

		if(distance - d < tolerance)
		{
			penetration_vector = closestEdge/closestEdge.norm();
			return distance;
		}
		else
		{
			s.insert(s.begin()+closestEdgeIndex, p);
		}
	}

}

float SteerLib::GJK_EPA::findClosestEdgeDistance(std::vector<Util::Vector> s, Util::Vector& closestEdge, int& closestEdgeIndex)
{
	float closest = INFINITY;
	for (int i=0; i<s.size(); i++)
	{
		//std::cout<<"Simplex["<<i<<"] = "<<s[i]<<std::endl;
		//get the next point. If reached the end of the vector get the first point
		int j = i+1 == s.size() ? 0 : i+1;
		Util::Vector a = s[i];
		Util::Vector b = s[j];
		Util::Vector e = b - a;

		Util::Vector originA = a;
		//get the triple product of e origin and e. This is a vector from the edge to the origin
		Util::Vector n = originA*(e*e) - e*(originA*e);

		Util::Vector nNorm = n/n.norm();

		float distance = a*(nNorm);
		if(distance<closest)
		{
			closest = distance;
			closestEdge = n;
			closestEdgeIndex = j;
		}
	}
	return closest;
}

bool SteerLib::GJK_EPA::GJK(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>& simplex)
{
	Util::Vector DirectionVector=_shapeA[0]-_shapeB[0];
	simplex.push_back(Support(_shapeA, _shapeB, DirectionVector));
	Util::Vector newDirection = -1.0f * DirectionVector;

	while (true)
	{
		simplex.push_back(Support(_shapeA, _shapeB, newDirection));

		if (simplex.back() * newDirection <= 0)
		{

			return false;
		}
		else
		{
			if (CheckContainsOrigin(newDirection, simplex))
			{
				return true;
			}
		}
	}
}


int SteerLib::GJK_EPA::GetFarthestIndexInDirection(Util::Vector direction, const std::vector<Util::Vector>& ShapeA)
{
	float MaxDot = direction*ShapeA[0];
	int FarthestIndex = 0;


	for (unsigned int i = 1; i < ShapeA.size(); i++)
	{
		float CurrentDot = direction*ShapeA[i];

		if (CurrentDot>MaxDot)
		{
			MaxDot = CurrentDot;
			FarthestIndex = i;
		}
	}

	return FarthestIndex;

}

Util::Vector SteerLib::GJK_EPA::Support(const std::vector<Util::Vector>& ShapeA, const std::vector<Util::Vector>& ShapeB, Util::Vector direction)
{
	Util::Vector FirstPoint = ShapeA[GetFarthestIndexInDirection(direction, ShapeA)];
	Util::Vector newDirection = -1.0f * direction;
	Util::Vector SecondPoint = ShapeB[GetFarthestIndexInDirection(newDirection, ShapeB)];
	Util::Vector MinkowskiDifference = FirstPoint - SecondPoint;

	return MinkowskiDifference;

}

bool SteerLib::GJK_EPA::CheckContainsOrigin(Util::Vector& Direction, std::vector<Util::Vector>& simplex)
{
	Util::Vector a = simplex.back();
	Util::Vector ao = -1 * a;
	Util::Vector b;
	Util::Vector c;
	Util::Vector ab;
	Util::Vector ac;

	if (simplex.size() == 3)
	{
		b = simplex[1];
		c = simplex[0];
		ab = b - a;
		ac = c - a;

		Direction = Util::Vector(-1*ab.z, ab.y, ab.x);

		if (Direction * c > 0)
		{
			Direction = Direction * -1;
		}

		if (Direction * ao > 0)
		{
			simplex.erase(simplex.begin() + 0);
			return false;
		}

		Direction = Util::Vector(ac.z, ac.y, -1 * ac.x);

		if (Direction * ao > 0)
		{
			simplex.erase(simplex.begin() + 1);
			return false;
		}

		return true;
	}
	else //line segment
	{
		b = simplex[0];
		ab = b - a;

		Direction = Util::Vector(-1*ab.z, ab.y, ab.x);

		if (Direction * ao < 0)
		{
			Direction = -1 * Direction;
		}

	}

	return false;


}

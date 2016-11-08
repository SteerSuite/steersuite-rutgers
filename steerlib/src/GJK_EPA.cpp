#include "obstacles/GJK_EPA.h"
#include <math.h>


SteerLib::GJK_EPA::GJK_EPA()
{
}

//Look at the GJK_EPA.h header file for documentation and instructions

//Using this video as referance to write the algorithms: https://www.youtube.com/watch?v=Qupqu1xe7Io
// We also found this website, that implemented GJK-EPA in java, and we used it as a referance to understand how EPA is implemented: http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/



bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	std::vector<Util::Vector> simplex;

	if (GJK(_shapeA, _shapeB, simplex))
	{

		return_penetration_depth = EPA(_shapeA, _shapeB, simplex, return_penetration_vector);
		return true; // There is a collision.
	}
	else
	{
		return false; // There is no collision.
	}

}
//Used to find the farthest point in a shape along a given direction. This is needed to find the Minkowski Difference.
<<<<<<< HEAD
Util::Vector SteerLib::GJK_EPA::getFarthestPoint(const std::vector<Util::Vector>& shape, const Util::Vector& direction)
=======
Util::Vector getFarthestPoint(const std::vector<Util::Vector>& shape, const Util::Vector& direction)
>>>>>>> origin/master
{


	float farthestDistance = 0;
	int index = 0;
<<<<<<< HEAD
	int i = 1;
=======
	int i = 0;
>>>>>>> origin/master

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
<<<<<<< HEAD
Util::Vector SteerLib::GJK_EPA::support(const std::vector<Util::Vector>& shapeA, const std::vector<Util::Vector>& shapeB, const Util::Vector& direction)
=======
Util::Vector support(const std::vector<Util::Vector>& shapeA, const std::vector<Util::Vector>& shapeB, const Util::Vector& direction)
>>>>>>> origin/master
{
	Util::Vector opposite = (-1) * direction;
	Util::Vector MinDiff = getFarthestPoint(shapeA, direction) -  getFarthestPoint(shapeB, opposite);
	return MinDiff;
}

bool SteerLib::GJK_EPA::GJK(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>& simplex)
{
<<<<<<< HEAD
	Util::Vector dir(1, 0, 1);
=======
	Util::Vector dir(0, 0, 1);
>>>>>>> origin/master
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
<<<<<<< HEAD
	Util::Vector a = simplex.back();
	Util::Vector b;
	Util::Vector c;
	Util::Vector ab;
	Util::Vector ac;
	Util::Vector abNorm; // normal vector to ab
	Util::Vector acNorm; // normal vector to ac
	Util::Vector a_neg = -1 * a;
=======
	Vector a = simplex.back();
	Vector b;
	Vector c;
	Vector ab;
	Vector ac;
	Vector abNorm; // normal vector to ab
	Vector acNorm; // normal vector to ac
	Vector a_neg = -1 * a;
>>>>>>> origin/master

	if (simplex.size() == 3) {
		//triangle case
		//get vectors b(middle) and c(first)
		b = simplex[1];
		c = simplex[0];
		//create edges from vectors
		ab = b - a;
		ac = c - a;

		//calculate the norms of the edges
		abNorm = tripleProd(ac, ab, ab);
		acNorm = tripleProd(ab, ac, ac);
		//
<<<<<<< HEAD
		if (dot(abNorm,a_neg) > 0) {
			//erase the c vector
			simplex.erase(simplex.begin()+0);
=======
		if (abNorm.dot(a_neg) > 0) {
			//erase the c vector
			simplex.erase(c);
>>>>>>> origin/master

			d = abNorm;
		}
		else {
			//
<<<<<<< HEAD
			if (dot(acNorm,a_neg) > 0) {
				//erase the b vector
				simplex.erase(simplex.begin()+1);
=======
			if (acNorm.dot(a_neg) > 0) {
				//erase the b vector
				simplex.erase(b);
>>>>>>> origin/master
				d = acNorm;
			}
			else {
				return true;
			}

		}
	}

	else {

<<<<<<< HEAD
		b = simplex[0];;
=======
		b = simplex.getB();
>>>>>>> origin/master
		//calculate AB
		ab = b - a;
		// normal vector  to ab in the direction pointing toward the origin
		abNorm = tripleProd(ab, a_neg, ab);
		//set the direction to be equal to abNorm
		d = abNorm;
	}

	return false;

} 

//called by epa 
<<<<<<< HEAD
float SteerLib::GJK_EPA::closestEdge(std::vector<Util::Vector> s, Util::Vector& closestEdge, int& closestEdgeIndex)
{
	float closest = 100000;

	for (int i = 0; i < s.size(); i++ )
	{
		int j= i+1; 

		Util::Vector a = s[i];
		Util::Vector b = s[j];
		Util::Vector diff = b - a;
		

		Util::Vector edgeTowardOrigin = tripleProd(diff, a, diff); 
		Util::Vector edgeNormalTowardOrigin = edgeTowardOrigin / edgeTowardOrigin.norm();
		float distance = edgeNormalTowardOrigin * a;
		
		if (distance < closest)
=======
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
>>>>>>> origin/master
		{
			closest = distance;
			closestEdge = edgeNormalTowardOrigin;
			closestEdgeIndex = i;
		}

	}

	return closest; 
}



//is there a collision, return penitration vector, depth 
<<<<<<< HEAD
float SteerLib::GJK_EPA::EPA(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, const std::vector<Util::Vector>& simplex, Util::Vector& penetration_vector)
{
	std::vector<Util::Vector> s = simplex;
	Util::Vector closestE;
	int closestEdgeIndex;
	float edgeDistance;


	while (true)
	{
		edgeDistance = closestEdge(s, closestE, closestEdgeIndex);
		Util::Vector t = support(_shapeA, _shapeB, closestE / closestE.norm());
		float distance = t*(closestE / closestE.norm());

		if (distance - edgeDistance < 0.00001)
		{
			penetration_vector = closestE / closestE.norm();
			return distance;
		}
		else
		{
			s.insert(s.begin() + closestEdgeIndex, t);
		}
	}

}




Util::Vector SteerLib::GJK_EPA::tripleProd(Util::Vector& a, Util::Vector& b, Util::Vector& c)
=======
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

Util::Vector tripleProd(Util::Vector& a, Util::Vector& b, Util::Vector& c)
>>>>>>> origin/master
{
	return b*dot(a, c) - c*dot(a, b);
}

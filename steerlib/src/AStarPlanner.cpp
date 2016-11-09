//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman, Rahul Shome
// See license.txt for complete license.
//


#include <vector>
#include <stack>
#include <set>
#include <map>
#include <iostream>
#include <algorithm> 
#include <functional>
#include <queue>
#include <math.h>
#include "planning/AStarPlanner.h"


#define COLLISION_COST  1000
#define GRID_STEP  1
#define OBSTACLE_CLEARANCE 1
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define MANHATTAN 1
#define WEIGHT 1.5

namespace SteerLib
{
	AStarPlanner::AStarPlanner(){}

	AStarPlanner::~AStarPlanner(){}

	bool AStarPlanner::canBeTraversed ( int id ) 
	{
		double traversal_cost = 0;
		int current_id = id;
		unsigned int x,z;
		gSpatialDatabase->getGridCoordinatesFromIndex(current_id, x, z);
		int x_range_min, x_range_max, z_range_min, z_range_max;

		x_range_min = MAX(x-OBSTACLE_CLEARANCE, 0);
		x_range_max = MIN(x+OBSTACLE_CLEARANCE, gSpatialDatabase->getNumCellsX());

		z_range_min = MAX(z-OBSTACLE_CLEARANCE, 0);
		z_range_max = MIN(z+OBSTACLE_CLEARANCE, gSpatialDatabase->getNumCellsZ());


		for (int i = x_range_min; i<=x_range_max; i+=GRID_STEP)
		{
			for (int j = z_range_min; j<=z_range_max; j+=GRID_STEP)
			{
				int index = gSpatialDatabase->getCellIndexFromGridCoords( i, j );
				traversal_cost += gSpatialDatabase->getTraversalCost ( index );
				
			}
		}

		if ( traversal_cost > COLLISION_COST ) 
			return false;
		return true;
	}



	Util::Point AStarPlanner::getPointFromGridIndex(int id)
	{
		Util::Point p;
		gSpatialDatabase->getLocationFromIndex(id, p);
		return p;
	}

	float AStarPlanner::calculate_h(Util::Point p1, Util::Point p2)
	{
		if (MANHATTAN)
		{
			return fabs(p1.x - p2.x) + fabs(p1.z - p2.z);
		}
		else
		{
			return sqrt(pow((p1.x - p2.x),2) + pow((p1.z - p2.z),2));
		}
	}

	void AStarPlanner::findSuccessor(std::vector<SteerLib::AStarPlannerNode>& successors, SteerLib::AStarPlannerNode p, Util::Point goal){
		/* return all AStarPlannerNodes */
		std::vector<Util::Point> points;

		int x = p.point.x;
		int y = p.point.y;
		int z = p.point.z;

		points.push_back(Util::Point(x - 1, y, z + 1));
		points.push_back(Util::Point(x, y, z + 1));
		points.push_back(Util::Point(x + 1, y, z + 1));
		points.push_back(Util::Point(x - 1, y, z));
		points.push_back(Util::Point(x + 1, y, z));
		points.push_back(Util::Point(x - 1, y, z - 1));
		points.push_back(Util::Point(x, y, z - 1));
		points.push_back(Util::Point(x + 1, y, z - 1));

		for (int i = 0; i < points.size(); i++){
			if (canBeTraversed(gSpatialDatabase->getCellIndexFromLocation(points[i]))){
				if ( i == 1 || i == 3 || i ==4 || i == 6)
					successors.push_back(SteerLib::AStarPlannerNode(points[i], p.g + 1, p.g + WEIGHT*calculate_h(goal, points[i]), &p));
				else
					successors.push_back(SteerLib::AStarPlannerNode(points[i], p.g + 1.414, p.g + WEIGHT*calculate_h(goal, points[i]), &p));
			}
		}
	}

	bool AStarPlanner::computeNormalAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		gSpatialDatabase = _gSpatialDatabase;
		// Setup
		MyHeap<SteerLib::AStarPlannerNode> openList;
		std:set<SteerLib::AStarPlannerNode> closedSet;

		SteerLib::AStarPlannerNode startNode(start, 0, 0, NULL);
		SteerLib::AStarPlannerNode goalNode(goal, 99999, 99999, NULL);

		openList.insert(startNode);

		while (!openList.empty())
		{
			SteerLib::AStarPlannerNode current = openList.top();

			//Find path
			if (current == goalNode)
			{
				cout << "Path Find." << endl;
				cout << "Path length is " << current.g << endl;
				cout << "Number of expand nodes is " << closedSet.size();

				while (current.point != start){
					agent_path.push_back(current.point);
					current = *current.parent;
				}
				
				reverse(agent_path.begin(), agent_path.end());
				return true;
			}

			openList.pop();
			closedSet.insert(current);

			std::vector<SteerLib::AStarPlannerNode> successor;
			findSuccessor(successor, current, goal);

			for (int i = 0; i < successor.size(); i++){
				/* if neighbour in closed list continue*/
				if (closedSet.find(successor[i]) != closedSet.end()){
					continue;
				}
				else{
					float tentGScore = current.g + calculate_h(current.point, successor[i].point);
					if (tentGScore < successor[i].g){
						successor[i].parent = &current;
						successor[i].g = tentGScore;
						successor[i].f = tentGScore + WEIGHT*calculate_h(successor[i].point, goal);
					}

					if (!openList.find(successor[i])){
						openList.insert(successor[i]);
					}
					else
					{
						openList.remove(successor[i]);
						openList.insert(successor[i]);
					}

				}
			}

		}

		std::cout << "no path with A*";
		return false;
	}

	bool AStarPlanner::computePath(std::vector<Util::Point>& agent_path,  Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		gSpatialDatabase = _gSpatialDatabase;

		//TODO
		std::cout<<"\nIn A*";
		bool normalAstar = computeNormalAstar(agent_path, start, goal, _gSpatialDatabase, append_to_path);

		return false;
	}
}
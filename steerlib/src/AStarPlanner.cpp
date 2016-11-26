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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "planning/AStarPlanner.h"


#define COLLISION_COST  1000
#define GRID_STEP  1
#define OBSTACLE_CLEARANCE 1
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define MANHATTAN 0
#define WEIGHT 1.5

namespace SteerLib
{
	AStarPlanner::AStarPlanner(){}

	AStarPlanner::~AStarPlanner(){}

	bool AStarPlanner::canBeTraversed(int id)
	{
		double traversal_cost = 0;
		int current_id = id;
		unsigned int x, z;
		gSpatialDatabase->getGridCoordinatesFromIndex(current_id, x, z);
		int x_range_min, x_range_max, z_range_min, z_range_max;

		x_range_min = MAX(x - OBSTACLE_CLEARANCE, 0);
		x_range_max = MIN(x + OBSTACLE_CLEARANCE, gSpatialDatabase->getNumCellsX());

		z_range_min = MAX(z - OBSTACLE_CLEARANCE, 0);
		z_range_max = MIN(z + OBSTACLE_CLEARANCE, gSpatialDatabase->getNumCellsZ());


		for (int i = x_range_min; i <= x_range_max; i += GRID_STEP)
		{
			for (int j = z_range_min; j <= z_range_max; j += GRID_STEP)
			{
				int index = gSpatialDatabase->getCellIndexFromGridCoords(i, j);
				traversal_cost += gSpatialDatabase->getTraversalCost(index);

			}
		}

		if (traversal_cost > COLLISION_COST)
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
			return sqrt(pow((p1.x - p2.x), 2) + pow((p1.z - p2.z), 2));
		}
	}

	float AStarPlanner::calculate_dist(Util::Point p1, Util::Point p2)
	{

		return fabs(p1.x - p2.x) + fabs(p1.z - p2.z) == 2 ? 1.414 : 1;
	}

	void AStarPlanner::findSuccessor(std::vector<SteerLib::AStarPlannerNode*>& successors, SteerLib::AStarPlannerNode* p, Util::Point goal){
		/* return all AStarPlannerNodes */
		std::vector<Util::Point> points;

		int x = p->point.x;
		int y = p->point.y;
		int z = p->point.z;

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

				if (i == 1 || i == 3 || i == 4 || i == 6)
				{
					AStarPlannerNode *temp = new AStarPlannerNode(points[i], p->g + 1, p->g + WEIGHT*calculate_h(goal, points[i]), p);
					successors.push_back(temp);
				}

				else
				{
					AStarPlannerNode *temp = new AStarPlannerNode(points[i], p->g + 1.414, p->g + WEIGHT*calculate_h(goal, points[i]), p);
					successors.push_back(temp);
				}

			}
		}
	}

	bool AStarPlanner::computeNormalAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		gSpatialDatabase = _gSpatialDatabase;
		// Setup
		MyHeap<SteerLib::AStarPlannerNode*> *openList = new MyHeap<AStarPlannerNode*>;
		MyHeap<SteerLib::AStarPlannerNode*> *closedSet = new MyHeap<AStarPlannerNode*>;

		SteerLib::AStarPlannerNode *startNode = new AStarPlannerNode(start, 0, 0, NULL);
		SteerLib::AStarPlannerNode *goalNode = new AStarPlannerNode(goal, 999999999, 999999999, NULL);

		openList->insert(&startNode);

		SteerLib::AStarPlannerNode* current = (openList->top());

		while (!openList->empty())
		{
			current = (openList->top());
			openList->pop();
			//Find path
			if (*current == *goalNode)
			{
				cout << "Path Find." << endl;
				cout << "Path length is " << current->g << endl;
				cout << "Number of expand nodes is " << closedSet->size();
				cout << "Number of generated nodes is " << openList->size() + closedSet->size();

				while (current->point != start){
					agent_path.push_back(current->point);
					current = (current->parent);
				}
				agent_path.push_back(start);
				reverse(agent_path.begin(), agent_path.end());
				return true;
			}


			closedSet->insert(&current);

			std::vector<SteerLib::AStarPlannerNode*> successor;
			findSuccessor(successor, current, goal);

			for (int i = 0; i < successor.size(); i++){
				/* if neighbour in closed list continue*/
				if (closedSet->find(successor[i])){
					continue;
				}
				else{
					float tentGScore = current->g + calculate_dist(current->point, successor[i]->point);
					if (tentGScore < successor[i]->g){
						(successor[i]->parent) = current;
						successor[i]->g = tentGScore;
						successor[i]->f = tentGScore + WEIGHT*calculate_h(successor[i]->point, goal);
					}

					if (!openList->find(successor[i])){
						openList->insert(&successor[i]);
					}
					else
					{
						openList->updateState(successor[i]);
					}

				}
			}

		}

		std::cout << "no path with A*";
		return false;
	}

	void AStarPlanner::findSuccessor(std::vector<Util::Point>& points, SteerLib::AStarPlannerNode* p){
		/* return all AStarPlannerNodes */

		int x = p->point.x;
		int y = p->point.y;
		int z = p->point.z;

		points.push_back(Util::Point(x - 1, y, z + 1));
		points.push_back(Util::Point(x, y, z + 1));
		points.push_back(Util::Point(x + 1, y, z + 1));
		points.push_back(Util::Point(x - 1, y, z));
		points.push_back(Util::Point(x + 1, y, z));
		points.push_back(Util::Point(x - 1, y, z - 1));
		points.push_back(Util::Point(x, y, z - 1));
		points.push_back(Util::Point(x + 1, y, z - 1));
	}

	void AStarPlanner::improvePath(MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet,
		MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight)
	{
		while (goalNode->g > openList.top()->f)
		{
			AStarPlannerNode* s = openList.top();
			openList.pop();
			closedSet.insert(&s);

			std::vector<Util::Point> points;
			findSuccessor(points, s);

			for (int i = 0; i < points.size(); i++)
			{
				if (canBeTraversed(gSpatialDatabase->getCellIndexFromLocation(points[i])))
				{
					AStarPlannerNode* ss = new AStarPlannerNode(points[i], -1, -1, s);
					if (!openList.find(ss) && !closedSet.find(ss) && !inconsList.find(ss))
					{
						ss->g = 999999999;
						ss->f = 999999999;
					}
					else if (inconsList.find(ss))
					{
						ss = inconsList.getValue(ss);
					}
					else if (openList.find(ss))
					{
						ss = openList.getValue(ss);
					}
					else if (closedSet.find(ss))
					{
						ss = closedSet.getValue(ss);
					}

					float tentGScore = s->g + calculate_dist(s->point, ss->point);
					if (tentGScore < ss->g)
					{
						(ss->parent) = s;
						ss->g = tentGScore;
						ss->f = tentGScore + calculate_h(ss->point, goalNode->point);

						if (!closedSet.find(ss))
						{
							ss->f = tentGScore + weight*calculate_h(ss->point, goalNode->point);
							if (openList.find(ss))
							{
								openList.updateState(ss);
							}
							else
							{
								openList.insert(&ss);
							}
						}
						else
						{
							if (inconsList.find(ss))
							{
								inconsList.updateState(ss);
							}
							else
							{
								inconsList.insert(&ss);
							}
						}
					}
					if (*ss == *goalNode)
					{
						*goalNode = *ss;
						if (openList.find(goalNode))
							openList.remove(goalNode);
						if (inconsList.find(goalNode))
							inconsList.remove(goalNode);
					}
				}
			}
		}
	}

	bool AStarPlanner::computeARAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		clock_t startTimer, endTimer;
		double timeDuration = 50;
		startTimer = clock();

		gSpatialDatabase = _gSpatialDatabase;
		// Setup
		MyHeap<SteerLib::AStarPlannerNode*> *openList = new MyHeap<AStarPlannerNode*>;
		MyHeap<SteerLib::AStarPlannerNode*> *closedSet = new MyHeap<AStarPlannerNode*>;
		MyHeap<SteerLib::AStarPlannerNode*> *inconsList = new MyHeap<AStarPlannerNode*>;


		float weight = 10000;

		SteerLib::AStarPlannerNode *startNode = new AStarPlannerNode(start, 0, calculate_h(start, goal), NULL);
		SteerLib::AStarPlannerNode *goalNode = new AStarPlannerNode(goal, 999999999, 999999999, NULL);

		openList->insert(&startNode);

		improvePath(*openList, *closedSet, *inconsList, startNode, goalNode, weight);


		AStarPlannerNode *current = goalNode;
		while (current->point != start){
			agent_path.push_back(current->point);
			current = (current->parent);
		}
		agent_path.push_back(start);
		reverse(agent_path.begin(), agent_path.end());


		float minList = 9999999999;
		if (!openList->empty())
			minList = openList->top()->g + calculate_h(openList->top()->point, goal);
		if (!inconsList->empty())
			minList = min(minList, inconsList->top()->g + calculate_h(openList->top()->point, goal));
		weight = min(weight, goalNode->g / minList);

		while (weight > 1)
		{
			weight -= 0.002;

			while (!inconsList->empty())
			{
				AStarPlannerNode* temp = inconsList->top();
				inconsList->pop();
				openList->insert(&temp);
			}


			for (int i = 1; i < openList->heapDataVec.size(); i++)
			{
				openList->heapDataVec[i]->f = openList->heapDataVec[i]->g + weight*calculate_h(openList->heapDataVec[i]->point, goal);
			}
			openList->makeHeap();


			while (!closedSet->empty())
			{
				closedSet->pop();
			}
			improvePath(*openList, *closedSet, *inconsList, startNode, goalNode, weight);

			float minList = 9999999999;
			if (!openList->empty())
				minList = openList->top()->g + calculate_h(openList->top()->point, goal);
			if (!inconsList->empty())
				minList = min(minList, inconsList->top()->g + calculate_h(openList->top()->point, goal));
			weight = min(weight, goalNode->g / minList);

			agent_path.clear();
			AStarPlannerNode *current = goalNode;
			while (current->point != start){
				agent_path.push_back(current->point);
				current = (current->parent);
			}
			agent_path.push_back(start);
			reverse(agent_path.begin(), agent_path.end());

			endTimer = clock();
			if (endTimer - startTimer > timeDuration)
			{
				cout << "run time is " << timeDuration << endl;
				break;
			}
		}

		cout << "Path Find." << endl;
		cout << "Path length is " << goalNode->g << endl;
		cout << "Number of generated nodes is " << openList->size() + closedSet->size() + inconsList;

		std::cout << "find path" << endl;
		return false;
	}

	
	float AStarPlanner::key(AStarPlannerNode* s, Point start, float weight)
	{
		if (s->g > s->rhs)
		{
			return 1000 * (s->rhs + weight*calculate_h(s->point, start)) + s->rhs;
		}
		else
		{
			return 1000 * (s->g + weight*calculate_h(s->point, start)) + s->rhs;
		}
	}

	void AStarPlanner::UpdateState(AStarPlannerNode* s, MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet, MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight)
	{
		if (!openList.find(s) && !closedSet.find(s) && !inconsList.find(s))
		{
			s->g = 999999999;
			s->f = 999999999;
		}
		else if (inconsList.find(s))
		{
			*s = *inconsList.getValue(s);
		}
		else if (openList.find(s))
		{
			*s = *openList.getValue(s);
		}
		else if (closedSet.find(s))
		{
			*s = *closedSet.getValue(s);
		}

		if (*s != *goalNode)
		{
			std::vector<Util::Point> points;
			findSuccessor(points, s);
			float minSucc = FLT_MAX;
			for (int i = 0; i < points.size(); i++)
			{
				if (canBeTraversed(gSpatialDatabase->getCellIndexFromLocation(points[i])))
				{
					AStarPlannerNode* ss = new AStarPlannerNode(points[i], -1, -1, s);
					if (!openList.find(ss) && !closedSet.find(ss) && !inconsList.find(ss))
					{
						ss->g = 999999999;
						ss->f = 999999999;
					}
					else if (inconsList.find(ss))
					{
						ss = inconsList.getValue(ss);
					}
					else if (openList.find(ss))
					{
						ss = openList.getValue(ss);
					}
					else if (closedSet.find(ss))
					{
						ss = closedSet.getValue(ss);
					}
					minSucc = min(minSucc, ss->g + calculate_dist(ss->point, s->point));
				}
			}
			s->rhs = minSucc;
		}

		if (openList.find(s))
		{
			openList.remove(s);
		}

		if (s->g != s->rhs)
		{
			if (!closedSet.find(s))
			{
				s->f = key(s, startNode->point, weight);
				if (openList.find(s))
				{
					openList.updateState(s);
				}
				else
				{
					openList.insert(&s);
				}
			}
			else
			{
				if (inconsList.find(s))
				{
					inconsList.updateState(s);
				}
				else
				{
					inconsList.insert(&s);
				}
			}
		}
	}

	void AStarPlanner::ComputeorImprovePath(MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet,
		MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight)
	{
		int count = 0;
		while (key(openList.top(), startNode->point, weight) < key(startNode, startNode->point, weight) || startNode->rhs != startNode->g)
		{
			count++;
			if (count % 1000 == 0)
				count++;
			AStarPlannerNode* s = openList.top();
			openList.pop();

			

			if (s->g > s->rhs)
			{
				s->g = s->rhs;
				closedSet.insert(&s);
				//for all s0 ¡Ê Pred(s) UpdateState(s0);
				std::vector<Util::Point> points;
				findSuccessor(points, s);
				for (int i = 0; i < points.size(); i++)
				{
					AStarPlannerNode* ss = new AStarPlannerNode(points[i], -1, -1, s);
					UpdateState(ss, openList, closedSet, inconsList, startNode, goalNode, weight);
				}
			}
			else
			{
				s->g = 999999999;
				//for all s0 ¡Ê Pred(s) ¡È{ s } UpdateState(s0);
				std::vector<Util::Point> points;
				findSuccessor(points, s);
				UpdateState(s, openList, closedSet, inconsList, startNode, goalNode, weight);
				for (int i = 0; i < points.size(); i++)
				{
					AStarPlannerNode* ss = new AStarPlannerNode(points[i], -1, -1, s);
					UpdateState(ss, openList, closedSet, inconsList, startNode, goalNode, weight);
				}
			}
			if (*s == *startNode)
				*startNode = *s;
		}
	}

	bool AStarPlanner::computeDAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		clock_t startTimer, endTimer;
		double timeDuration = 50;
		startTimer = clock();

		gSpatialDatabase = _gSpatialDatabase;
		// Setup
		MyHeap<SteerLib::AStarPlannerNode*> *openList = new MyHeap<AStarPlannerNode*>;
		MyHeap<SteerLib::AStarPlannerNode*> *closedSet = new MyHeap<AStarPlannerNode*>;
		MyHeap<SteerLib::AStarPlannerNode*> *inconsList = new MyHeap<AStarPlannerNode*>;

		float weight = 3;

		SteerLib::AStarPlannerNode *startNode = new AStarPlannerNode(start, 999999999, calculate_h(start, goal), NULL, 999999999);
		SteerLib::AStarPlannerNode *goalNode = new AStarPlannerNode(goal, 999999999, 999999999, NULL, 0);

		startNode->f = key(startNode, start, weight);
		openList->insert(&goalNode);

		ComputeorImprovePath(*openList, *closedSet, *inconsList, startNode, goalNode, weight);

		AStarPlannerNode *current = startNode;
		while (current->point != goal)
		{
			agent_path.push_back(current->point);
			current = (current->parent);
		}
		agent_path.push_back(goal);
		//reverse(agent_path.begin(), agent_path.end());

		while (true)
		{
			if (weight > 1)
				weight -= 0.002;

			while (!inconsList->empty())
			{
				AStarPlannerNode* temp = inconsList->top();
				inconsList->pop();
				openList->insert(&temp);
			}


			for (int i = 1; i < openList->heapDataVec.size(); i++)
			{
				openList->heapDataVec[i]->f = openList->heapDataVec[i]->g + weight*calculate_h(openList->heapDataVec[i]->point, goal);
			}
			openList->makeHeap();


			while (!closedSet->empty())
			{
				closedSet->pop();
			}
			ComputeorImprovePath(*openList, *closedSet, *inconsList, startNode, goalNode, weight);

			agent_path.clear();
			AStarPlannerNode *current = startNode;
			while (current->point != goal){
				agent_path.push_back(current->point);
				current = (current->parent);
			}
			agent_path.push_back(goal);
			//reverse(agent_path.begin(), agent_path.end());

			endTimer = clock();
			if (endTimer - startTimer > timeDuration)
			{
				//cout << "run time is " << timeDuration << endl;
				break;
			}
		}

		return false;
	}

	bool AStarPlanner::computePath(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path)
	{
		gSpatialDatabase = _gSpatialDatabase;

		//TODO
		std::cout << "\nIn A* \n";
		clock_t startTimer, endTimer;
		startTimer = clock();
		//bool normalAstar = computeNormalAstar(agent_path, start, goal, _gSpatialDatabase, append_to_path);
		//bool ARAstar = computeARAstar(agent_path, start, goal, _gSpatialDatabase, append_to_path);
		bool DAstar = computeDAstar(agent_path, start, goal, _gSpatialDatabase, append_to_path);
		endTimer = clock();
		cout << "run time: " << (endTimer - startTimer)<< endl;

		cout << "path" << endl;
		return true;
	}
}
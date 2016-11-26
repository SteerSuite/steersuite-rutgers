//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman, Rahul Shome
// See license.txt for complete license.
//


#ifndef __STEERLIB_A_STAR_PLANNER_H__
#define __STEERLIB_A_STAR_PLANNER_H__


#include <iostream>  
#include <functional>  

#include <vector>
#include <stack>
#include <set>
#include <map>
#include "SteerLib.h"

const int StartIndex = 1;//容器中堆元素起始索引  
using namespace std;
using namespace SteerLib;
#define MIN_VALUE new AStarPlannerNode(Point(0.0f,0.0f,0.0f),-999999999,-999999999,NULL) //某个很大的值，存放在vector的第一个位置（最大堆）



namespace SteerLib
{
	//堆类定义  
	//默认比较规则less  
	template <class ElemType>
	class MyHeap{
	private:
		int numCounts;//堆中元素个数  
		bool comp(ElemType n1, ElemType n2)
		{
			if (n1->f == n2->f)
				return n1->g > n2->g;
			return n1->f > n2->f;
		};

	public:
		MyHeap();

		vector<ElemType> heapDataVec;//存放元素的容器  

		void initHeap(ElemType *data, const int n);//初始化操作  
		void printfHeap();//输出堆元素  
		void makeHeap();//建堆  
		ElemType top();
		void insert(ElemType *elem);//向堆中插入元素  
		void pop();//从堆中取出堆顶的元素  
		void adjustHeap(int childTree, ElemType adjustValue);//调整子树  
		void percolateUp(int holeIndex, ElemType adjustValue);//上溯操作  
		bool empty();
		bool find(ElemType adjustValue);
		void remove(ElemType adjustValue);
		int size();
		void updateState(ElemType adjustValue);
		ElemType getValue(ElemType adjustValue);
	};

	template <class ElemType>
	MyHeap<ElemType>::MyHeap()
		:numCounts(0)
	{
		heapDataVec.push_back(MIN_VALUE);
	}


	template <class ElemType>
	void MyHeap<ElemType>::initHeap(ElemType *data, const int n)
	{
		//拷贝元素数据到vector中  
		for (int i = 0; i < n; ++i)
		{
			heapDataVec.push_back(*(data + i));
			++numCounts;
		}
	}

	template <class ElemType>
	void MyHeap<ElemType>::printfHeap()
	{
		cout << "Heap : ";
		for (int i = 1; i <= numCounts; ++i)
		{
			cout << heapDataVec[i]->f << " ";
		}
		cout << endl;
	}

	template <class ElemType>
	void MyHeap<ElemType>::makeHeap()
	{
		//建堆的过程就是一个不断调整堆的过程，循环调用函数adjustHeap依次调整子树  
		if (numCounts < 2)
			return;
		//第一个需要调整的子树的根节点多音  
		int parent = numCounts / 2;
		while (1)
		{
			adjustHeap(parent, heapDataVec[parent]);
			if (StartIndex == parent)//到达根节点  
				return;

			--parent;
		}
	}

	template <class ElemType>
	void MyHeap<ElemType>::insert(ElemType* elem)
	{
		//将新元素添加到vector中  
		heapDataVec.push_back(*elem);
		++numCounts;

		//执行一次上溯操作，调整堆，以使其满足最大堆的性质  
		percolateUp(numCounts, heapDataVec[numCounts]);
	}

	template <class ElemType>
	ElemType MyHeap<ElemType>::top()
	{
		return heapDataVec[StartIndex];
	}

	template <class ElemType>
	void MyHeap<ElemType>::pop()
	{
		if (heapDataVec.size() <= 1)
		{
			std::cout << "The heap is empty." << std::endl;
			return;
		}
		//将堆顶的元素放在容器的最尾部，然后将尾部的原元素作为调整值，重新生成堆  
		ElemType adjustValue = heapDataVec[numCounts];
		//堆顶元素为容器的首元素  
		heapDataVec[numCounts] = heapDataVec[StartIndex];
		//堆中元素数目减一  
		--numCounts;

		heapDataVec.pop_back();

		if (heapDataVec.size() == 1)
			return;

		adjustHeap(StartIndex, adjustValue);
	}

	//调整以childTree为根的子树为堆  
	template <class ElemType>
	void MyHeap<ElemType>::adjustHeap(int childTree, ElemType adjustValue)
	{
		//洞节点索引  
		int holeIndex = childTree;
		int secondChid = 2 * holeIndex + 1;//洞节点的右子节点（注意：起始索引从1开始）  
		while (secondChid <= numCounts)
		{
			if (comp(heapDataVec[secondChid], heapDataVec[secondChid - 1]))
			{
				--secondChid;//表示两个子节点中值较大的那个  
			}

			//上溯  
			heapDataVec[holeIndex] = heapDataVec[secondChid];//令较大值为洞值  
			holeIndex = secondChid;//洞节点索引下移  
			secondChid = 2 * secondChid + 1;//重新计算洞节点右子节点  
		}
		//如果洞节点只有左子节点  
		if (secondChid == numCounts + 1)
		{
			//令左子节点值为洞值  
			heapDataVec[holeIndex] = heapDataVec[secondChid - 1];
			holeIndex = secondChid - 1;
		}
		//将调整值赋予洞节点  
		heapDataVec[holeIndex] = adjustValue;

		//此时可能尚未满足堆的特性，需要再执行一次上溯操作  
		percolateUp(holeIndex, adjustValue);
	}

	//上溯操作  
	template <class ElemType>
	void MyHeap<ElemType>::percolateUp(int holeIndex, ElemType adjustValue)
	{
		//将新节点与其父节点进行比较，如果键值比其父节点大，就父子交换位置。  
		//如此，知道不需要对换或直到根节点为止  
		int parentIndex = holeIndex / 2;
		while (holeIndex > StartIndex && comp(heapDataVec[parentIndex], adjustValue))
		{
			heapDataVec[holeIndex] = heapDataVec[parentIndex];
			holeIndex = parentIndex;
			parentIndex /= 2;
		}
		heapDataVec[holeIndex] = adjustValue;//将新值放置在正确的位置  
	}

	//上溯操作  
	template <class ElemType>
	bool MyHeap<ElemType>::empty()
	{
		if (heapDataVec.size() <= 1)
			return true;
		else
			return false;
	}

	template <class ElemType>
	bool MyHeap<ElemType>::find(ElemType element)
	{
		for (int i = 1; i < heapDataVec.size(); i++)
		{
			if (heapDataVec[i]->point == element->point)
				return true;
		}
		return false;
	}

	template <class ElemType>
	void MyHeap<ElemType>::remove(ElemType element)
	{
		for (int i = 1; i < heapDataVec.size(); i++)
		{
			if (heapDataVec[i]->point == element->point)
			{
				heapDataVec.erase(heapDataVec.begin() + i);
				numCounts--;
				break;
			}
		}
		this->makeHeap();
	}

	template <class ElemType>
	int MyHeap<ElemType>::size()
	{
		return heapDataVec.size() - 1;
	}

	template <class ElemType>
	void MyHeap<ElemType>::updateState(ElemType element)
	{
		for (int i = 1; i < heapDataVec.size(); i++)
		{
			if (heapDataVec[i]->point == element->point)
			{
				if (element->f < heapDataVec[i]->f)
				{
					heapDataVec[i]->f = element->f;
				}
				if (element->g < heapDataVec[i]->g)
				{
					this->remove(element);
					this->insert(&element);
				}
				break;
			}
		}
		this->makeHeap();
	}

	template <class ElemType>
	ElemType MyHeap<ElemType>::getValue(ElemType element)
	{
		for (int i = 1; i < heapDataVec.size(); i++)
		{
			if (heapDataVec[i]->point == element->point)
			{
				return heapDataVec[i];
			}
		}
		return NULL;
	}

	/*
	@function The AStarPlannerNode class gives a suggested container to build your search tree nodes.
	@attributes
	f : the f value of the node
	g : the cost from the start, for the node
	point : the point in (x,0,z) space that corresponds to the current node
	parent : the pointer to the parent AStarPlannerNode, so that retracing the path is possible.
	@operators
	The greater than, less than and equals operator have been overloaded. This means that objects of this class can be used with these operators. Change the functionality of the operators depending upon your implementation

	*/

	class STEERLIB_API AStarPlannerNode{
	public:
		double f;
		double g;
		double rhs;
		Util::Point point;
		AStarPlannerNode* parent;
		AStarPlannerNode(Util::Point _point, double _g, double _f, AStarPlannerNode* _parent)
		{
			f = _f;
			point = _point;
			g = _g;
			parent = _parent;
		}

		AStarPlannerNode(Util::Point _point, double _g, double _f, AStarPlannerNode* _parent, double _rhs)
		{
			f = _f;
			point = _point;
			g = _g;
			parent = _parent;
			rhs = _rhs;
		}

		bool operator<(AStarPlannerNode other) const
		{
			if (this->f == other.f)
				return this->g < other.g;
			return this->f < other.f;
		}
		bool operator>(AStarPlannerNode other) const
		{
			if (this->f == other.f)
				return this->g > other.g;
			return this->f > other.f;
		}
		bool operator==(AStarPlannerNode other) const
		{
			return ((this->point.x == other.point.x) && (this->point.z == other.point.z));
		}

		bool operator!=(AStarPlannerNode other) const
		{
			return ((this->point.x != other.point.x) || (this->point.z != other.point.z));
		}
	};

	class STEERLIB_API AStarPlanner{
	public:
		AStarPlanner();
		~AStarPlanner();
		// NOTE: There are four indices that need to be disambiguated
		// -- Util::Points in 3D space(with Y=0)
		// -- (double X, double Z) Points with the X and Z coordinates of the actual points
		// -- (int X_GRID, int Z_GRID) Points with the row and column coordinates of the GridDatabase2D. The Grid database can start from any physical point(say -100,-100). So X_GRID and X need not match
		// -- int GridIndex  is the index of the GRID data structure. This is an unique id mapping to every cell.
		// When navigating the space or the Grid, do not mix the above up

		/*
		@function canBeTraversed checkes for a OBSTACLE_CLEARANCE area around the node index id for the presence of obstacles.
		The function finds the grid coordinates for the cell index  as (X_GRID, Z_GRID)
		and checks cells in bounding box area
		[[X_GRID-OBSTACLE_CLEARANCE, X_GRID+OBSTACLE_CLEARANCE],
		[Z_GRID-OBSTACLE_CLEARANCE, Z_GRID+OBSTACLE_CLEARANCE]]
		This function also contains the griddatabase call that gets traversal costs.
		*/
		bool canBeTraversed(int id);
		/*
		@function getPointFromGridIndex accepts the grid index as input and returns an Util::Point corresponding to the center of that cell.
		*/
		Util::Point getPointFromGridIndex(int id);

		/*
		@function computePath
		DO NOT CHANGE THE DEFINITION OF THIS FUNCTION
		This function executes an A* query
		@parameters
		agent_path : The solution path that is populated by the A* search
		start : The start point
		goal : The goal point
		_gSpatialDatabase : The pointer to the GridDatabase2D from the agent
		append_to_path : An optional argument to append to agent_path instead of overwriting it.
		*/
		float AStarPlanner::calculate_dist(Util::Point p1, Util::Point p2);

		float calculate_h(Util::Point p1, Util::Point p2);

		void findSuccessor(std::vector<SteerLib::AStarPlannerNode*>& successors, SteerLib::AStarPlannerNode* p, Util::Point goal);

		bool computeNormalAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path);

		void findSuccessor(std::vector<Util::Point>& points, SteerLib::AStarPlannerNode* p);

		void improvePath(MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet,
			MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight);

		bool computeARAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path);

		float AStarPlanner::key(AStarPlannerNode* s, Point start, float weight);

		void AStarPlanner::UpdateState(AStarPlannerNode* s, MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet, MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight);

		void AStarPlanner::ComputeorImprovePath(MyHeap<SteerLib::AStarPlannerNode*>& openList, MyHeap<SteerLib::AStarPlannerNode*>& closedSet,
			MyHeap<SteerLib::AStarPlannerNode*>& inconsList, AStarPlannerNode *startNode, AStarPlannerNode *goalNode, float weight);

		bool AStarPlanner::computeDAstar(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path);

		bool computePath(std::vector<Util::Point>& agent_path, Util::Point start, Util::Point goal, SteerLib::SpatialDataBaseInterface * _gSpatialDatabase, bool append_to_path = false);

	private:
		SteerLib::SpatialDataBaseInterface * gSpatialDatabase;
	};

}


#endif
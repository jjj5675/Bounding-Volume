#include "Precompiled.h"

bool CK::QuadTree::Insert(const std::string& InKey, const CK::Rectangle& InBound)
{
	//현재 Quadtree에 속하지 않는 객체는 무시
	if (!Bound.Intersect(InBound))
	{
		return false;
	}

	//최대 깊이에 도달하면 현재 Quadtree에 객체를 추가하고 종료
	if (MaxLevel <= Level)
	{
		Nodes.emplace_back(InKey, InBound);
		return true;
	}

	//현재 QuadTree에 공간이 존재하고 세분화하지 않았다면 여기에 객체를 추가
	if (Nodes.size() < NodeCapacity && SubTrees[SubNames::NorthWest] == nullptr)
	{
		Nodes.emplace_back(InKey, InBound);
		return true;
	}

	//현재 Quadtree를 세분화 한다.
	if (SubTrees[SubNames::NorthWest] == nullptr)
	{
		IsLeaf = false;
		SubDivide();
	}

	if (FindSubTree(InBound) == nullptr)
	{
		Nodes.emplace_back(InKey, InBound);
		return true;
	}
	else
	{
		for (QuadTree* subTree : SubTrees)
		{
			if (subTree->Insert(InKey, InBound))
			{
				return true;
			}
		}
	}

	return false;
}

void CK::QuadTree::QueryRange(const CK::Rectangle& InRectangleToQuery, std::vector<std::string>& InOutKeys) const
{
	if (!Bound.Intersect(InRectangleToQuery))
	{
		return;
	}

	//마지막 노드면 현재 노드 목록을 모두 추가
	for (const TreeNode& node : Nodes)
	{
		InOutKeys.emplace_back(node.NodeKey);
	}

	if (!IsLeaf)
	{
		for (QuadTree* subTree : SubTrees)
		{
			assert(subTree != nullptr);
			subTree->QueryRange(InRectangleToQuery, InOutKeys);
		}
	}
}

void CK::QuadTree::SubDivide()
{
	Vector2 centre, extent;
	Bound.GetCenterAndExtent(centre, extent);

	Vector2 xExtent = Vector2(extent.X, 0.f);
	Vector2 yExtent = Vector2(0.f, extent.Y);

	int nextLevel = Level + 1;

	SubTrees[SubNames::NorthWest] = new QuadTree(CK::Rectangle(centre - xExtent, centre + yExtent), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::NorthEast] = new QuadTree(CK::Rectangle(centre, centre + extent), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::SouthEast] = new QuadTree(CK::Rectangle(centre - extent, centre), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::SouthWest] = new QuadTree(CK::Rectangle(centre - yExtent, centre + xExtent), nextLevel, MaxLevel, NodeCapacity);
}

bool CK::QuadTree::Contains(const CK::Rectangle& InBox) const
{
	return Bound.IsInside(InBox);
}

QuadTree* CK::QuadTree::FindSubTree(const CK::Rectangle& InBound)
{
	// 네 개의 자식 노드를 돌면서 주어진 영역을 완전 포함하는 자식 트리가 있는지 조사.
	for (QuadTree* subTree : SubTrees)
	{
		assert(subTree != nullptr);
		if (subTree->Contains(InBound))
		{
			return subTree;
		}
	}

	// 없으면 겹친다고 판단하고 null 반환
	return nullptr;
}

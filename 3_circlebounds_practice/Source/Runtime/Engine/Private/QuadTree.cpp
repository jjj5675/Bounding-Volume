#include "Precompiled.h"

bool CK::QuadTree::Insert(const std::string& InKey, const CK::Rectangle& InBound)
{
	if (!Bound.Intersect(InBound))
	{
		return false;
	}

	if (MaxLevel <= Level)
	{
		Nodes.emplace_back(InKey, InBound);
		return true;
	}

	Split();
	IsLeaf = false;

	if (FindSubTree(InBound) == nullptr)
	{
		Nodes.emplace_back(InKey, InBound);
	}
	else
	{
		for (QuadTree* subTree : SubTrees)
		{
			subTree->Insert(InKey, InBound);
		}
	}

	return true;
}

void CK::QuadTree::Query(const CK::Rectangle& InRectangleToQuery, std::vector<std::string>& InOutKeys) const
{
	if (Bound.Intersect(InRectangleToQuery))
	{
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
				subTree->Query(InRectangleToQuery, InOutKeys);
			}
		}
	}
}

void CK::QuadTree::Split()
{
	if (SubTrees[SubNames::TopLeft] != nullptr)
	{
		return;
	}

	Vector2 centre, extent;
	Bound.GetCenterAndExtent(centre, extent);

	Vector2 xExtent = Vector2(extent.X, 0.f);
	Vector2 yExtent = Vector2(0.f, extent.Y);

	int nextLevel = Level + 1;

	SubTrees[SubNames::TopLeft] = new QuadTree(CK::Rectangle(centre - xExtent, centre + yExtent), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::TopRight] = new QuadTree(CK::Rectangle(centre, centre + extent), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::BottomLeft] = new QuadTree(CK::Rectangle(centre - extent, centre), nextLevel, MaxLevel, NodeCapacity);
	SubTrees[SubNames::BottomRight] = new QuadTree(CK::Rectangle(centre - yExtent, centre + xExtent), nextLevel, MaxLevel, NodeCapacity);
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

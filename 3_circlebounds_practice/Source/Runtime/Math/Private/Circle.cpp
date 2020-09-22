
#include "Precompiled.h"

Circle::Circle(const std::vector<Vector2> InVertices)
{
	// ��ġ ������ �޾� �߽ɰ� �������� ���� ���ϴ� ���� ( ���� ������ �� )

	for (auto vertex : InVertices)
	{
		Center += vertex;
	}

	//���� ���
	Center /= InVertices.size();

	Radius = (*std::max_element(InVertices.begin(), InVertices.end(), [&](Vector2 const& InLeft, Vector2 const& InRight)
		{
			return (Center - InLeft).SizeSquared() < (Center - InRight).SizeSquared();
		})).Size();
}

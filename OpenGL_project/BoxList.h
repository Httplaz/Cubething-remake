#pragma once
#include <iostream>
template <typename T>
struct BoxListNode
{
	T info;
	BoxListNode* right = nullptr , * up = nullptr, * back = nullptr, * left = nullptr, * down = nullptr, * front = nullptr;
};

template <typename T>
class BoxList
{
public:
	class Iterator;
	BoxList(T* arr, int sizeX, int sizeY, int sizeZ);
	void shift(int x, int y, int z);

	Iterator begin();
	Iterator end();

	BoxListNode<T>* getRinged(int x, int z, int y);
	BoxListNode<T>* get(int x, int z, int y);

private:
	BoxListNode<T>* anchors[2][2][2];
	const int sizeX, sizeY, sizeZ;

};

template <typename T>
class BoxList<T>::Iterator
{
public:

	using valueType = T;
	using reference = valueType;
	using pointer = valueType*;



	Iterator(BoxListNode<T>* node, int x, int y, int z, int dirX, int dirY, int dirZ, int sizeX, int sizeY, int sizeZ);
	Iterator& operator ++ ();
	Iterator operator ++ (int);
	BoxListNode<T>& operator * ();
	BoxListNode<T>* operator ->();
	bool operator == (const Iterator& other);
	bool operator != (const Iterator& other);
private:
	BoxListNode<T>* node;
	int sizeX, sizeY, sizeZ;
	int dirX, dirY, dirZ;
	int x, y, z;
};

//definitions


//BoxList
template<typename T>
BoxList<T>::BoxList(T* arr, int sizeX, int sizeY, int sizeZ): sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ)
{

	BoxListNode<T>** arrN = new BoxListNode<T>*[sizeX * sizeY * sizeZ];
	for (int i = 0; i < sizeX * sizeY * sizeZ; i++)
		arrN[i] = nullptr;

	int te = 0;

	for (int x = 0; x < sizeX; x++)
		for (int y = 0; y < sizeY; y++)
			for (int z = 0; z < sizeZ; z++)
			{
				BoxListNode<T>* node = new BoxListNode<T>{ arr[x + y * sizeX + z * sizeX * sizeY] };
				arrN[x + y * sizeX + z * sizeX * sizeY] = node;
			}


	for (int x = 0; x < sizeX; x++)
		for (int y = 0; y < sizeY; y++)
			for (int z = 0; z < sizeZ; z++)
			{
				BoxListNode<T>* node = arrN[x + y * sizeX + z * sizeX * sizeY];
				BoxListNode<T>* neighbor;

				neighbor = arrN[(x - 1 + sizeX) % sizeX + y * sizeX + z * sizeX * sizeY];
				node->left = neighbor, neighbor->right = node;

				neighbor = arrN[x + (y - 1 + sizeY) % sizeY * sizeX + z * sizeX * sizeY];
				node->down = neighbor, neighbor->up = node;

				neighbor = arrN[x + y * sizeX + (z - 1 + sizeZ) % sizeZ * sizeX * sizeY];
				node->front = neighbor, neighbor->back = node;

				if (x % (sizeX - 1) == 0 && y % (sizeY - 1) == 0 && z % (sizeZ - 1) == 0)
					anchors[(x + 1) / sizeX][(y + 1) / sizeY][(z + 1) / sizeZ] = node;
			}
	delete[] arrN;
	/*int x = 0, y = 0, z = 0;
	int dirX = 1, dirY = 1, dirZ = 1;
	BoxListNode<T>* node = new BoxListNode<T>({ arr[0] });
	int te = 0;
	while (true)
	{
		te++;
		node->info = arr[x + y * sizeX + z * sizeX * sizeY];
		if (x % (sizeX - 1) == 0 && y % (sizeY - 1) == 0 && z % (sizeZ - 1) == 0)
			anchors[(x + 1) / sizeX][(y + 1) / sizeY][(z + 1) / sizeZ] = node;


		if (x < sizeX - 1 && dirX>0)
		{
			node->right = new BoxListNode<T>({ arr[0], nullptr, nullptr, nullptr, node, nullptr, nullptr });
			node = node->right, x++;
		}
		else if (x > 1 && dirX < 0)
		{
			node->left = new BoxListNode<T>({ arr[0], node, nullptr, nullptr, nullptr, nullptr, nullptr });
			node = node->left, x--;
		}
		else if (y < sizeY - 1 && dirY > 0)
		{
			dirX *= -1;
			node->up = new BoxListNode<T>({ arr[0], nullptr, nullptr, nullptr, nullptr, node, nullptr });
			node = node->up, y++;
		}
		else if (y > 1 && dirY < 0)
		{
			dirX *= -1;
			node->down = new BoxListNode<T>({ arr[0], nullptr, node, nullptr, nullptr, nullptr, nullptr });
			node = node->down, y--;
		}

		else if (z < sizeZ - 1 && dirZ > 0)
		{
			dirY *= -1;
			node->back = new BoxListNode<T>({ arr[0], nullptr, nullptr, nullptr, nullptr, nullptr, node });
			node = node->back, z++;
		}
		else if (z > 1 && dirZ < 0)
		{
			dirY *= -1;
			node->front = new BoxListNode<T>({ arr[0], nullptr, nullptr, node, nullptr, nullptr, nullptr });
			node = node->front, z--;
		}
		else
		{
			z += dirZ;
			node->info = arr[x + y * sizeX + z * sizeX * sizeY];
			break;
		}
	}
	std::cout << anchors[0][0][0]->info->getMeshID();*/

}

template<typename T>
void BoxList<T>::shift(int dx, int dy, int dz)
{
	std::cout << dx << "\n";
	for (int x = 0; x < 2; x++)
		for (int y = 0; y < 2; y++)
			for (int z = 0; z < 2; z++)
			{
				while (dx > 0)
					anchors[x][y][z] = anchors[x][y][z]->right, dx--;// std::cout << dx << " ";
				while (dy > 0)
					anchors[x][y][z] = anchors[x][y][z]->up, dy--;
				while (dz > 0)
					anchors[x][y][z] = anchors[x][y][z]->back, dz--;
				while (dx < 0)
					anchors[x][y][z] = anchors[x][y][z]->left, dx++;
				while (dy < 0)
					anchors[x][y][z] = anchors[x][y][z]->down, dy++;
				while (dz < 0)
					anchors[x][y][z] = anchors[x][y][z]->front, dz++;
			}
}


template<typename T>
BoxListNode<T>* BoxList<T>::getRinged(int x, int y, int z)
{
	return nullptr;
}


template<typename T>
typename BoxList<T>::Iterator BoxList<T>::begin()
{
	//Iterator a(anchors[0][0][0], 0, 0, 0, sizeX, sizeY, sizeZ);
	return Iterator(anchors[0][0][0], 0, 0, 0, 1, 1, 1, sizeX, sizeY, sizeZ);
}

template<typename T>
typename BoxList<T>::Iterator BoxList<T>::end()
{
	return ++Iterator(anchors[1][1][1], sizeX-1, sizeX-1, sizeX-1, 1, 1, 1, sizeX, sizeY, sizeZ);
}


//Iterator
template<typename T>
BoxList<T>::Iterator::Iterator(BoxListNode<T>* node, int x, int y, int z, int dirX, int dirY, int dirZ, int sizeX, int sizeY, int sizeZ) : sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ), node(node), x(x), y(y), z(z), dirX(dirX), dirY(dirY), dirZ(dirZ)
{
}

template<typename T>
typename BoxList<T>::Iterator& BoxList<T>::Iterator::operator++ ()
{
	//std::cout << x << " " << y << " " << z << " " << dirX << " " << dirY << "\n";
	if (x < sizeX - 1 && dirX>0)
	{
		node = node->right, x++;
	}
	else if (x > 0 && dirX < 0)
	{
		//std::cout << "ADV " << x << " " << y << " " << z << " " << "\n";
		node = node->left, x--;
	}
	else if (y < sizeY - 1 && dirY > 0)
	{
		dirX *= -1;
		node = node->up, y++;
	}
	else if (y > 0 && dirY < 0)
	{
		dirX *= -1;
		node = node->down, y--;
	}

	else if (z < sizeZ - 1 && dirZ > 0)
	{
		dirX *= -1;
		dirY *= -1;
		node = node->back, z++;
	}
	else if (z > 0 && dirZ < 0)
	{
		dirX *= -1;
		dirY *= -1;
		node = node->front, z--;
	}
	else
	{
		z += dirZ;
	}
	//std::cout << x << " " << y << " " << z << " " << "\n";
	return (*this);
}

template<typename T>
typename BoxList<T>::Iterator BoxList<T>::Iterator::operator++ (int)
{
	Iterator temp = *this;
	(*this)++;
	return temp;
}

template<typename T>
BoxListNode<T>& BoxList<T>::Iterator::operator*()
{
	return *node;
}

template<typename T>
BoxListNode<T>* BoxList<T>::Iterator::operator->()
{
	return node;
}

template<typename T>
bool BoxList<T>::Iterator::operator != (const Iterator& other)
{
	return (node != other.node || x != other.x || y != other.y || z != other.z);
}

template<typename T>
bool BoxList<T>::Iterator::operator == (const Iterator& other)
{
	return !(this != other);
}

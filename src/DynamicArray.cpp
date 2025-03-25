#include "../inc/DynamicArray.hpp"

template <typename T>
DynamicArray<T>::DynamicArray() : _data(nullptr), _count(0), _capacity(0) {}

template <typename T>
DynamicArray<T>::DynamicArray(int size) : _data(new T[size]), _count(0), _capacity(size) {}

template <typename T>
DynamicArray<T>::~DynamicArray()
{
	delete[] _data;	
}

template <typename T>
void	DynamicArray<T>::append(T elem)
{
	// if no space then resize
	if (_capacity < _count + 1)
	{
		_capacity *= 2;
		T *newArray = new T[_capacity];
		for (size_t i = 0; i < _count; i++)
			newArray[i] = _data[i];
		delete[] _data;
		_data = newArray;
	}
	//append elem to list
	_data[_count] = elem;
	_count++;
}

template <typename T>
int	DynamicArray<T>::getCapacity()
{
	return (_capacity);
}

template <typename T>
T	*DynamicArray<T>::getData()
{
	return (_data);
}
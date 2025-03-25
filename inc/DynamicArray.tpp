#include "DynamicArray.hpp"

template <typename T>
DynamicArray<T>::DynamicArray() : _data(NULL), _count(0), _capacity(0) {}

template <typename T>
DynamicArray<T>::DynamicArray(int size) : _data(new T[size]), _count(0), _capacity(size) {}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other)
{
	_capacity = other._capacity;
	_count = other._count;
	_data = new T[_capacity];
	for (unsigned int i = 0; i < other._count; i++)
		_data[i] = other._data[i];	
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& other)
{
	if (this != &other)
	{
		_capacity = other._capacity;
		_count = other._count;
		delete[] _data;

		_data = new T[_capacity];
		for (unsigned int i = 0; i < other._count; i++)
			_data[i] = other._data[i];	
	}
	return *this;
}

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
		for (unsigned int i = 0; i < _count; i++)
			newArray[i] = _data[i];
		delete[] _data;
		_data = newArray;
	}
	//append elem to array
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

template<typename T>
T& DynamicArray<T>::operator[](unsigned int i)
{
	if (i >= _count)
		throw OutOfBoundsException();
	return (_data[i]);
}

template<typename T>
const char	*DynamicArray<T>::OutOfBoundsException::what() const throw()
{
	return ("DynamicArray index out of bounds!");
}
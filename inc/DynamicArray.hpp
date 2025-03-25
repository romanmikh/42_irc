#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <iostream>

template <typename T>
class DynamicArray
{
	private:
		T	*data;
		int	count;
		int	capacity;
		
	public:
		DynamicArray();
		DynamicArray(int capacity);
		~DynamicArray();

		void	append(T data);
		int		getCapacity();
		T 		*getData();
		
};

#endif
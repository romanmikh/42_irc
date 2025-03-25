#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <iostream>

template <typename T>
class DynamicArray
{
	private:
		T	*_data;
		unsigned int	_count;
		unsigned int	_capacity;
		
	public:
		DynamicArray();
		DynamicArray(int capacity);
		DynamicArray&	operator=(const DynamicArray& other);
		DynamicArray(const DynamicArray& other);
		~DynamicArray();

		T&	operator[](unsigned int i);
		void	append(T data);
		int		getCapacity();
		T 		*getData();
	
		class OutOfBoundsException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		
};

#include "DynamicArray.tpp"
#endif
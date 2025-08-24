#pragma once

template<class T>
class VertexDictionary
{
public:
	VertexDictionary(int size);
	~VertexDictionary();
	
	void clear();
	int addElement(T element, int index);
	int getNumberOfElements();
private:
	T** array = nullptr;
	int _size = 0;
	int nmber_of_elements = 0;
	int* used_size = 0;
	int* size_array = nullptr;
};

template<class T>
int VertexDictionary<T>::getNumberOfElements()
{
	return nmber_of_elements;
}

template<class T>
void VertexDictionary<T>::clear()
{
	nmber_of_elements = 0;
	memset(used_size,  0, _size * sizeof(int));
}

template<class T>
int VertexDictionary<T>::addElement(T element, int index)
{
	if(index >= _size) {return 0;}
	
	//try to find if object already exist
	for(int i = 0; i < used_size[index]; i++) {
		if(array[index][i] == element) {
			return array[index][i].index;
		}
	}
	
	//if not enougth memory
	if(used_size[index] >= size_array[index])
	{
		T* new_one = new T[size_array[index] * 2];
		for(int i = 0; i < size_array[index]; i++)
		{
			new_one[i] = array[index][i];
		}
		delete[] array[index];
		array[index] = new_one;
		size_array[index] *= 2 ;
	}
	
	nmber_of_elements++;
	array[index][used_size[index]] = element;
	used_size[index]++;
	return element.index;
}

#include "VertexDictionary.imp"
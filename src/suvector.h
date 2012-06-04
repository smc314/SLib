#ifndef SUVECTOR_H
#define SUVECTOR_H
/*
 * Copyright (c) 2001,2002 Steven M. Cherry. All rights reserved.
 *
 * This file is a part of slib - a c++ utility library
 *
 * The slib project, including all files needed to compile 
 * it, is free software; you can redistribute it and/or use it and/or modify 
 * it under the terms of the GNU Lesser General Public License as published by 
 * the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this program.  See file COPYING for details.
 */

#include <vector>
#include <algorithm>
//using namespace std;

//namespace std {

/**
* This template extends std::vector to provide the functionality
* of a sorted, unique vector.  All of the interfaces of the vector
* class are maintained, with the additional functionality being
* added to the push_back method.
*
* @author Steven M. Cherry
* @version $Revision: 1.1.1.1 $
* @coypright 2002 Steven M. Cherry
*/
template < class T > class suvector : 
	public std::vector < T >
{
	public:

		virtual ~suvector() { };

		virtual void push_back(T item) {
			if(std::vector<T>::size() == 0){
				std::vector<T>::push_back(item);
			} else {
				typename std::vector<T>::iterator lb = std::lower_bound(
					this->begin(), this->end(), item);
				if(lb == this->end()){
					std::vector<T>::insert(lb, item);
				} else {
					if(*lb == item){
						return;
					} else {
						std::vector<T>::insert(lb, item);
					}
				}
			}
		}
};

//} // End namespace

#endif // SUVECTOR_H Defined.


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


#include "Pool.h"
#include "Lock.h"
#include "Log.h"
#include "AnException.h"
using namespace SLib;

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
Pool()
{
	TRACE(FL, "Enter Pool::Pool()");

	m_min_size = 0;
	m_grow_by = 5;
	m_max_size = 100;

	// Now actually create the lists:
	m_pool = new vector < pair <DataType, int> * > ;

	// Initialize the mutex:
	m_mut = new Mutex();

	TRACE(FL, "Exit Pool::Pool()");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
~Pool()
{
	TRACE(FL, "Enter Pool::~Pool()");

	for(int i = 0; i < (int)m_pool->size(); i++){
		m_free((*m_pool)[i]->first);
		delete (*m_pool)[i];
	}
	delete m_pool;
	delete m_mut;

	TRACE(FL, "Exit Pool::~Pool()");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
CreateNewObj(int locked)
{
	pair < DataType, int > *apair;
	int i;
	Lock the_lock;

	TRACE(FL, "Enter Pool::CreateNewObj()");

	// Acquire the mutex
	if(locked == 0)
		the_lock.SetMutex(m_mut);

	// Check to see if we have any destroyed obj's that we can re-build
	for(i = 0; i < (int)m_pool->size(); i ++) {
		if ( (*m_pool)[i]->second == 2) {
			try {
				(*m_pool)[i]->first = m_new(m_init_info);
				(*m_pool)[i]->second = 0;
			} catch (AnException e) {
				e.AddMsg("Pool Caught exception creating object");
				TRACE(FL, "Exit Pool::CreateNewObj()");
				throw;
			}
			return ;
		}
	}
				
	// Last, if we are still under the max cap, then
	// add a brand new DB to the list:
	if((int)m_pool->size() >= m_max_size){
		throw AnException(0, FL,
				  "No more pool objects available.  Reached "
				  "Pool max of %d", m_max_size);
	}
		
	for(i = 0; i < m_grow_by; i++){
		if((int)m_pool->size() >= m_max_size)
			break;	
		try {
			apair = new pair < DataType , int>;
			DEBUG(FL, "calling m_new");
			apair->first = m_new(m_init_info);
			apair->second = 0;
			DEBUG(FL, "adding new obj to m_pool");
			m_pool->push_back(apair);
		} catch (AnException e) {
			e.AddMsg("Pool Caught exception creating object");
			TRACE(FL, "Exit Pool::CreateNewObj()");
			throw;
		}
	}

	TRACE(FL, "Exit Pool::CreateNewObj()");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
DataType Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
Acquire(void)
{
	int i;

	TRACE(FL, "Enter Pool::Acquire()");

	// Acquire the mutex:
	Lock the_lock(m_mut);

	// Walk the list and find an available connection
	for(i=0; i < (int)m_pool->size(); i++){
		if( (*m_pool)[i]->second == 0 ){
			(*m_pool)[i]->second = 1;
			TRACE(FL, "Exit Pool::Acquire()");
			return (*m_pool)[i]->first;
		}
	}

	// If that didn't work, then try and create a new connection:
	CreateNewObj(1);	

	// CreateNewDB will throw an exception if we've reached our max
	// db connection limit.  So if we get here, then there should be
	// an available db in the list now.

	// Walk the list and find an available connection
	for(i=0; i < (int)m_pool->size(); i++){
		if( (*m_pool)[i]->second == 0 ){
			(*m_pool)[i]->second = 1;
			TRACE(FL, "Exit Pool::Acquire()");
			return (*m_pool)[i]->first;
		}
	}

	// If we get to here, then our logic has failed and the programmer
	// needs to be slapped, or get some sleep.  either one. ;-)
	throw AnException(0, FL, "Could not find available DB "
		"connection. Logic problem.");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
Release(DataType dt)
{
	// Acquire the mutex
	DEBUG(FL, "Acquiring mutex.");
	Lock the_lock(m_mut);

	int i;

	TRACE(FL, "Enter Pool::Release(DataType dt)");

	// Walk the list and find a matching connection
	for(i=0; i < (int)m_pool->size(); i++){
		if( ( (*m_pool)[i]->second == 1 ) && 
		    ( (*(*m_pool)[i]->first) == (*dt) ) )
		{
			(*m_pool)[i]->second = 0;
			TRACE(FL, "Exit Pool::Release()");
			return;
		}
	}

	// If we get here then the object given to us was not in our pool.
	// That's a caller error:
	throw AnException(0, FL, "The Object given was not in our pool.");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
Remove(DataType dt)
{
	// Acquire the mutex
	DEBUG(FL, "Acquiring mutex.");
	Lock the_lock(m_mut);

	int i;

	TRACE(FL, "Enter Pool::Remove(DataType dt)");

	// Walk the list and find a matching connection
	for(i=0; i < (int)m_pool->size(); i++){
		if( ( (*m_pool)[i]->second == 1 ) && 
		    ( (*(*m_pool)[i]->first) == (*dt) ) )
		{
			TRACE(FL, "Calling m_free in Pool::Remove()");
			m_free((*m_pool)[i]->first);
			TRACE(FL, "Back from m_free in Pool::Remove()");
			(*m_pool)[i]->second = 2;
			(*m_pool)[i]->first = NULL;
			TRACE(FL, "Exit Pool::Remove()");
			return;
		}
	}

	// If we get here then the object given to us was not in our pool.
	// That's a caller error:
	throw AnException(0, FL, "The Object given was not in our pool.");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
int Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
GetPoolSize(void)
{
	// Acquire the mutex
	DEBUG(FL, "Acquiring mutex.");
	Lock the_lock(m_mut);
	return (int)m_pool->size();
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
int Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
GetInUseSize(void)
{
	int i;
	int count;

	// Acquire the mutex
	DEBUG(FL, "Acquiring mutex.");
	Lock the_lock(m_mut);

	count = 0;
	// Walk the list and find an available connection
	for(i=0; i < (int)m_pool->size(); i++){
		if( (*m_pool)[i]->second == 1 ){
			count ++;
		}
	}

	return count;
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
SetMinSize(int newmin)
{
	TRACE(FL, "Enter Pool::SetMinSize(int newmin)");
	if(newmin > m_max_size){
		throw AnException(0, FL, "New Minimum is greater than current "
			"maximum.  Re-set max first.");
	}

	Lock the_lock(m_mut);
	if(newmin > (int)m_pool->size()){
		do {
			CreateNewObj(1);
		} while (newmin > (int)m_pool->size());
	}

	m_min_size = newmin;
	TRACE(FL, "Exit Pool::SetMinSize(int newmin)");
}

template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
SetGrowthFactor(int newgrowth)
{
	TRACE(FL, "Enter Pool::SetGrowthFactor(int newgrowth)");
	Lock the_lock(m_mut);
	m_grow_by = newgrowth;
	TRACE(FL, "Exit Pool::SetGrowthFactor(int newgrowth)");
}
	
template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
void Pool<DataType, PoolInfo, NewDataType, FreeDataType>::
SetMaxSize(int newmax)
{
	TRACE(FL, "Enter Pool::SetMaxSize(int newmax)");

	if(newmax < (int)m_pool->size()){
		throw AnException(0, FL, "New max is smaller than current pool "
			"size.  Refusing to change max value of %d.", 
			m_max_size);
	}

	Lock the_lock(m_mut);
	m_max_size = newmax;
	TRACE(FL, "Exit Pool::SetMaxSize(int newmax)");
}

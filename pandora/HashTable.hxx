
/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <vector>
#include <list>
#include <algorithm>
#include <Jenkins12Bit.hxx> 

namespace Engine 
{

class HashKey
{
public:

	HashKey()
		: m_code(0)
	{
	}

	HashKey( const HashKey& other )
	{
		m_code = other.m_code;
	}

	const HashKey& operator=( const HashKey& other )
	{
		m_code = other.m_code;
		return *this;
	}

	~HashKey()
	{
	}
	
	void add( unsigned k );
	void add( std::vector<unsigned>& k );

	unsigned code() const
	{
		 return m_code;
	}

	unsigned operator()() const
	{
		return m_code;
	}

protected:

	unsigned	m_code;

};

inline void HashKey::add( unsigned k )
{
	m_code = jenkins_hash( (ub1*)&k, sizeof(unsigned), m_code );
}

inline void HashKey::add( std::vector<unsigned>& k )
{
	if ( k.empty() )
	{
		unsigned k2 = 0;
		m_code = jenkins_hash( (ub1*)&k2, sizeof(unsigned), m_code );
		return;
	}

	std::sort( k.begin(), k.end() );
	m_code = jenkins_hash( (ub1*)(&k[0]), sizeof(unsigned), m_code );
	for ( unsigned i = 1; i < k.size(); i++ )
	{
		m_code = jenkins_hash( (ub1*)(&k[i]), sizeof(unsigned), m_code );
	}	
	
}

template <typename T>
class HashTable
{
public:
	typedef T								Object;
	typedef std::list< std::pair< unsigned, Object* > > 			Node_List;
	typedef std::vector< Node_List > 					Table;

	HashTable( unsigned hash_sz );
	~HashTable();

        void setOwner(bool o) { m_owner = o; } 	
	void clear();

	/*	
	size_t compute_hash( std::vector<unsigned>& atoms) const
	{
		if ( atoms.empty() )
		{
			unsigned k = 0;
			unsigned h = jenkins_hash( (ub1*)&k, sizeof(unsigned), 0 );
			return h;
		}

		std::sort( atoms.begin(), atoms.end() );
		unsigned h = jenkins_hash( (ub1*)(&atoms[0]), sizeof(unsigned), 0 );
		for ( unsigned i = 1; i < atoms.size(); i++ )
		{
			h = jenkins_hash( (ub1*)(&atoms[i]), sizeof(unsigned), h );
		}	
		return h;
	}
	*/
	Object* get_element( HashKey& h, Object* other ){ return get_element( h(), other ); }
	Object* get_element( unsigned h, Object* other )
	{
		assert( m_buckets.size() > 0 );
		unsigned address = h & ( m_buckets.size()-1 );
	
		for ( typename Node_List::iterator i = m_buckets[address].begin();
			i != m_buckets[address].end(); i++ )
			if ( i->first == h )
			{
			       	if( *other == *(i->second) )  
				{
				       	return i->second;
				}
			}
		return NULL;
	}	

        void add_element( HashKey& h, Object* obj ){ add_element( h(), obj );  }
	void add_element( unsigned h, Object* obj )
	{
		unsigned address = h & ( m_buckets.size() - 1);
		m_buckets[address].push_back( std::make_pair( h, obj ) );
	}

        void remove_element( HashKey& h, Object* obj ){ remove_element( h(), obj );  }
	void remove_element( unsigned h, Object* obj )
	{
		unsigned address = h & ( m_buckets.size() - 1);
		
		typename  Node_List::iterator p = m_buckets[address].end();
		for ( typename Node_List::iterator i = m_buckets[address].begin();
			i != m_buckets[address].end(); i++ )
		{
			if ( i->first == h && ( *obj == *(i->second)) ) // element found
			{
                                if (m_owner)
                                        delete i->second;
				p = i;
				break;
			}
		}	
		if ( p != m_buckets[address].end() )
			m_buckets[address].erase( p );
	}
	
protected:
	Table 		m_buckets;

        // delete on clear/remove/etc ?
        bool           m_owner;

};

// Inlined methods
template <typename T>
HashTable<T>::HashTable( unsigned hash_sz )
{
        m_owner = true;
	m_buckets.resize(hash_sz);
}

template <typename T>
HashTable<T>::~HashTable()
{
	for ( unsigned i = 0; i < m_buckets.size(); i++ )
	{
                if(m_owner)
                        for ( typename Node_List::iterator it = m_buckets[i].begin();
                              it != m_buckets[i].end(); it++ )
                                delete it->second;
		m_buckets[i].clear();	
	}
	m_buckets.clear();
}

template <typename T>
void HashTable<T>::clear()
{
	for ( unsigned i = 0; i < m_buckets.size(); i++ )
	{
                if(m_owner)
                        for ( typename Node_List::iterator it = m_buckets[i].begin();
                              it != m_buckets[i].end(); it++ )
                                delete it->second;
		m_buckets[i].clear();	
	}
}

}

#endif

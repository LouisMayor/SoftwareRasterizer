#pragma once

#include <type_traits>

/*
	RAII (Resource Acquisition Is Initialisation) http://en.cppreference.com/w/cpp/language/raii
	
	RAII in this example is basically managed memory using an class, 
	synonmous with std::unique_ptr http://en.cppreference.com/w/cpp/memory/unique_ptr 
	minus a few functions and functions overloads.

	Provides basic functionality:
	+ dereferences to get access to stored data
	+ manages own memory
	+ various functions (get, ->, etc...)

	- no swap()
	- lots of (potential) function overloads missing
	- no custom deleter
	- DOESNT SUPPORT ARRAYS (SINGLE TYPES ONLY), see std::unique_ptr in memory.h

	TODOs:
	+ write version that supports arrays

	Dont ask me why you'd use this over std::unique_ptr, you probably wouldn't.
	Still worth the time to implement.
*/

namespace raii {

	template<typename T>
	class RAIIManager {
	public:
		RAIIManager( RAIIManager&& _class_handle ) {
			reset( _class_handle.release( ) );
		};
		RAIIManager( T* _memory_handle ) {
			this->data = _memory_handle;
		};
		~RAIIManager( ) {
			delete_self( );
		};

		// returns reference to contained data
		T& operator* ( ) {
			return *get( );
		}
		// returns pointer to contained data
		T* operator-> ( ) {
			return get( );
		}
		// take ownership of the passed data
		RAIIManager& operator= ( const RAIIManager&& _assignee ) {
			reset( _assignee.release( ) );
			return (*this);
		}
		// take ownership of the passed data
		RAIIManager& operator= ( RAIIManager& _assignee ) {
			reset( _assignee.release( ) );
			return (*this);
		}

		// returns pointer to contained data
		T* get( ) {
			return data;
		}
		// deletes owned object, can pass new data to replace
		void reset( T* _new_memory_handle = nullptr ) {
			if( _new_memory_handle != data ) {
				delete_self( );
			}
			data = _new_memory_handle;
			return;
		}
		// releases ownership of memory, allowing any object to take ownership
		T* release( ) {
			T* tmp = get( );
			this->data = nullptr;
			return tmp;
		}

	private:
		RAIIManager( const RAIIManager& ) = delete;

		void delete_self( ) const {
			delete data;
		}
		T* data;
	};

	template<typename T, class... types>
	RAIIManager<T> make_unique_ptr( types&&... arguments ) {
		return (RAIIManager<T>( new T( std::forward<types>( arguments )... ) ));
	}
}
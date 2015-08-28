
#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "SerializableException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	Serializable
	\author 	Castronovo Michael
	
	\brief 	Interface of a Serializable Object.
			
	\date 	2015-08-28
*/
// ===========================================================================
class Serializable
{
	public:
		// =================================================================
		//	Public Destructor
		// =================================================================
		/**
			\brief	Destructor.
		*/
		virtual ~Serializable() {}
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "Serializable"; }

		
		/**
			\brief	Create an instance of class T and return a pointer
					of type Serializable pointing to it.
					Class T must fullfil the following condition:
						- class T must admit a 'T(std::istream)'
						  public constructor.
			
			\param[is	The 'istream' to use in order to initialize the
					instance to create.
					
			\return	A pointer of type Serializable of the created
			          instance.
		*/
		template<typename T>
		static Serializable* createInstance(std::istream& is)
		{
			return (new T(is));
		}
		
		
		/**
			\brief			Create an instance of type 'className'.
							This method is throws a
							SerializableException if 'className' has not
							been registered yet.
							(see 'checkIn()' method)
			
			\param[className	The name of the class to instantiate.
			\param[is			The 'istream' to use in order to initialize
							the instance to return.
		*/
		static Serializable* createInstance(
				std::string className, std::istream& is)
										throw (SerializableException)
		{
			if (newMap.find(className) == newMap.end())
			{
				std::string msg;
				msg += "Unable to retrieve the constructor of class '";
				msg += className;
				msg += "'! (have you called 'Serializable::checkIn()' ";
				msg += "first?)";
				
				throw SerializableException(msg);
			}
			
			return newMap[className](is);
		}
		
		
		/**
			\brief	Register class T in order to allow the
					usage of 'createInstance(string, istream)'
					method.

					class T must fullfil those conditions:
						- class T must admit a 'T(std::istream)'
						  public constructor.
						- function 'static string T::toString()'
						  must exist.
							
					Example usage:
						'checkIn<T>(&Serializable::createInstance<T>);'
			
			\param[f	See comments.
		*/
		template<typename T>
		static void checkIn(Serializable*(*f)(std::istream&))
		{
			newMap[T::toString()] = f;
		}
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
		   return Serializable::toString();
		}

		
		/**
			\brief	Serialize this Object.
					If overloaded, the new 'serialize()' method should
					call the 'serialize()' method of the base class
					before doing anything else.
		*/
		virtual void serialize(std::ostream& os) const;
		
		
		/**
			\brief	Serialize this Object, and compress the data (zlib).
					
						'compressionLevel' possible values:
							Z_NO_COMPRESSION		(0)
							Z_BEST_SPEED			(1)
							Z_BEST_COMPRESSION		(9)
							Z_DEFAULT_COMPRESSION	(-1)
					
					In case of error, return to the beginning of the
					stream before throwing a SerializableException.
		*/
		void zSerialize(std::ostream& os, int compressionlevel) const
									throw (SerializableException);
		
		
		/**
			\brief	Deserialize this Object.
					If overloaded, the new 'deserialize()' method should
					call the 'deserialize()' method of the base class
					before doing anything else.
		*/
		virtual void deserialize(std::istream& is)
									throw (SerializableException);
		
		/**
			\brief	Deserialize this Object, assuming the stream contains
					compressed data (zlib).
					
					In case of error, return to the beginning of the
					stream before throwing a SerializableException.
		*/
		void dDeserialize(std::istream& is)
									throw (SerializableException);


	protected:
		// =================================================================
		//	Protected Constructor
		// =================================================================
		/**
			\brief	Prevent instantiation.
		*/
		Serializable() {}
		
		
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
			\brief	Clone an instance 'x' of class T and return a pointer
					of type Serializable pointing to it.
					Class T must fullfil those conditions:
						- class T must be derived from Serializable.
						- class T must admit a 'T(std::istream)'
						  public constructor.
			
					Used by 'Serializable' to define 'clone()' methods
					easily.
			
			\param[x	The instance to clone.
					
			\return	A pointer of type Serializable of the created
			          instance.
		*/
		template<typename T>
		T* cloneInstance(const T* x) const throw (SerializableException)
		{
			if (x == 0)
			{
				std::string msg;
				msg += "No instance provided!";
				
				throw SerializableException(msg);
			}

			
			std::stringstream sstr;
			x->serialize(sstr);
			
			return (new T(sstr));
		}
		
		
		/**
			\brief		Return a message notifying that EOF has been 
						eached too early.
					
			\param[varName	The first variable which has not been retrieved.
		*/
		static void throwEOFMsg(std::string varName)
									throw (SerializableException)
		{
			std::string msg;
			msg += "EOF reached too early! ('";
			msg += varName;
			msg += "' missing)";
			
			throw SerializableException(msg);
		}
	
	
	private:
		// =================================================================
		//	Private static attributes
		// =================================================================
		/**
			\brief	A map between an agent's class name and its
					constructor.
		*/
		static std::map<std::string, Serializable*(*)(std::istream&)> newMap;
};

#endif

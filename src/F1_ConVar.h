#pragma once

#include <string>
#include <vector>

class F1_IConVar
{
public:

	virtual ~F1_IConVar()
	{}

	virtual void increment() = 0;

	virtual void decrement() = 0;

	virtual const char *print() = 0;

	virtual const char *name() = 0;
};

// base convar impl's some basic functionality
class F1_BaseConVar : public F1_IConVar
{
	char *Name;

public:

	F1_BaseConVar( const char *n )
	{
		Name = new char[ 32 ];
		strcpy_s( Name, 32, n );
	};

	~F1_BaseConVar()
	{
		delete[ ] Name;
	}

	virtual const char *name() override
	{
		return Name;
	}
};

// impl of IConVar for template type T
// add specialisations as needed
template<typename T>
class F1_ConVar : public F1_BaseConVar
{
	T value, Min, Max, inc;

public:

	// default initialiser
	F1_ConVar( const char *n, T val, T min, T max, T inc ) : F1_BaseConVar( n ), value( val ), Min( min ), Max( max ), inc( inc )
	{
		// clamp
		if( val > max )
			val = max;
		if( val < min )
			val = min;
	};

	void increment() override
	{
		T newVal = value + inc;
		if( newVal <= Max )
			value = newVal;
	}

	void decrement() override
	{
		T newVal = value - inc;
		if( newVal >= Min )
			value = newVal;
	}

	virtual void set( T newVal )
	{
		if( newVal > Max )
			newVal = Max;
		if( newVal < Min )
			newVal = Min;
		value = newVal;
	}

	const char *print() override
	{
		// TODO PLEASE DEALLOCATE THIS MEMORY
		char *pString = new char[ 25 ];

		memset( ( void * ) pString, 0, 25 );

		strcpy_s( pString, 25, std::to_string( value ).c_str() );

		return pString;
	}

	T getValue()
	{
		return value;
	}
};

template<>
class F1_ConVar<bool> : public F1_BaseConVar
{
	bool value;
public:

	F1_ConVar( const char *name, bool val ) : F1_BaseConVar( name ), value( val ) {};

	const char *print() override
	{
		return value ? "True" : "False";
	}

	void increment() override
	{
		value = true;
	}

	void decrement() override
	{
		value = false;
	}

	bool getValue()
	{
		return value;
	}
};

struct Switch
{
	bool val;

	operator bool()
	{
		return val;
	}
};

template<>
class F1_ConVar<Switch> : public F1_BaseConVar
{
	bool value;
public:

	F1_ConVar( const char *name, bool val ) : F1_BaseConVar( name ), value( val ) {};

	const char *print() override
	{
		return value ? "True" : "False";
	}

	void increment() override
	{
		value = true;
	}

	void decrement() override
	{
		value = false;
	}

	bool getValue()
	{
		return value;
	}
};

template<typename T>
struct translation_t
{
	T v;
	const char *t;
};

template<typename T>
struct Enum
{
	T value;

	// we use vector here to make things much easier
	std::vector<translation_t<T>> translations;

	Enum( T val, std::vector<translation_t<T>> trans ) : value( val ), translations( trans )
	{}

	// TODO improve efficiency
	const char *findTranslation()
	{
		for( auto &tr : translations )
		{
			if( tr.v == value )
				return tr.t;
		}

		return "<null>";
	}
};

template<typename T>
class F1_ConVar<Enum<T>> : public F1_BaseConVar
{
	Enum<T> value;

	T Min;
	T Max;

public:

	F1_ConVar( const char *name, Enum<T> val, T min, T max ) : F1_BaseConVar( name ), value( val ), Min( min ), Max( max )
	{
		// clamp value
		if( value.value > max )
			value.value = max;
		if( value.value < min )
			value.value = min;
	};

	const char *print() override
	{
		return value.findTranslation();
	}

	void increment() override
	{
		T newVal = ( T ) ( ( int ) value.value + 1 );
		if( newVal <= Max )
			value.value = newVal;
	}

	void decrement() override
	{
		T newVal = ( T ) ( ( int ) value.value - 1 );
		if( newVal >= Min )
			value.value = newVal;
	}

	T getValue()
	{
		return value.value;
	}
};
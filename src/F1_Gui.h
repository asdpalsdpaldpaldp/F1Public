#pragma once

#include <cstring>
#include <vector>

// define for debugging gui
//#define F1_GUI_DEBUG

struct F1_Point
{
	int x;
	int y;

	F1_Point operator-( const F1_Point &other )
	{
		return F1_Point{ x - other.x, y - other.y };
	}
};

struct F1_Rect
{
	int x;
	int y;
	int w;
	int h;
};

enum class mouseButton
{
	LDOWN,
	RDOWN,
	LUP,
	RUP,
	SCROLLDOWN,
	SCROLLUP,
	NONE,
};

// base component class
class F1_IComponent
{
public:

	using F1_Handle = unsigned int;

	virtual ~F1_IComponent() {};

	virtual bool handleMouseInput( int x, int y, mouseButton m ) = 0;

	virtual void render() = 0;

	virtual bool hasParent() = 0;
	virtual F1_IComponent *getParent() = 0;
	virtual void setParent( F1_IComponent *p ) = 0;

	//virtual F1_IComponent **getChildren() = 0;
	virtual int getChildrenCount() = 0;

	virtual F1_Handle getHandle() = 0;

	// called each frame
	virtual void think() = 0;
};

class F1_CComponent : public F1_IComponent
{
	// internal representation
	// OFFSETS OF THEIR PARENTS
	int posX, posY;

	int posW, posH;

	F1_IComponent *parent;

	std::vector<F1_IComponent *> children;

	F1_CComponent::F1_Handle thisHandle;

	// static incremental handle
	static F1_CComponent::F1_Handle lastHandle;

	// TODO maybe create a static linked list system to allow people to iterator over all components

protected:

	using BaseClass = F1_IComponent;

public:

	F1_CComponent( int x, int y ) : posX( x ), posY( y )
	{
		parent = nullptr;
		posW = posH = 0;
		thisHandle = ++lastHandle;
	}

	F1_CComponent( int x, int y, int w, int h ) : posX( x ), posY( y ), posW( w ), posH( h )
	{
		parent = nullptr;
		thisHandle = ++lastHandle;
	}

	F1_CComponent( F1_IComponent *parent, int x, int y ) : posX( x ), posY( y ), parent( parent )
	{
		posW = posH = 0;
		thisHandle = ++lastHandle;
	}

	F1_CComponent( F1_IComponent *parent, int x, int y, int w, int h ) : posX( x ), posY( y ), posW( w ), posH( h ), parent( parent )
	{
		thisHandle = ++lastHandle;
	}

	virtual ~F1_CComponent() override
	{
		// we want to destruct and delete all of our children
		for( auto &child : children )
		{
			// return memory to heap
			// destructor is auto called by operator delete
			delete child;
		}
	}

	virtual F1_Point getPos()
	{
		return{ posX, posY };
	}

	virtual void setPos( F1_Point p )
	{
		posX = p.x;
		posY = p.y;
	}

	virtual F1_Point getWidthHeight()
	{
		return{ posW, posH };
	}

	virtual void setWidthHeight( F1_Point p )
	{
		posW = p.x;
		posH = p.y;
	}

	virtual void render() override;

	virtual bool handleMouseInput( int x, int y, mouseButton m ) override
	{
		bool ret = false;

		// make sure that we handle mouse input for our children
		for( auto &child : children )
		{
			ret = child->handleMouseInput( x, y, m ) ? true : ret;
		}

		return ret;
	}

	virtual bool hasParent() override
	{
		return parent != nullptr;
	}
	virtual F1_IComponent *getParent() override
	{
		return parent;
	}
	virtual void setParent( F1_IComponent *p ) override
	{
		parent = p;
	}
	virtual int getChildrenCount() override
	{
		return children.size();
	}
	virtual F1_Handle getHandle() override
	{
		return thisHandle;
	}

	virtual void think() override
	{
		// handle childrens think

		for( auto &child : children )
		{
			child->think();
		}
	}

	virtual F1_Handle addChild( F1_IComponent *c )
	{
		// never add yourself as a child
		// never steal a child
		if( c->getHandle() == thisHandle || c->hasParent() == true )
			throw;

		children.push_back( c );

		c->setParent( this );

		return c->getHandle();
	}

	// returns the child that it just removed
	// does not destruct the child that it removes
	virtual F1_IComponent *removeChild( F1_Handle childHandle )
	{
		// find the handle in our children

		int foundIndex = -1;

		auto it = children.begin();

		for( ; it != children.end(); it++ )
		{
			if( ( *it )->getHandle() == childHandle )
			{
				break;
			}
		}

		F1_IComponent *c = *it;

		// auto calls destructor
		children.erase( it );

		return c;
	}

	virtual void destroyChildren()
	{
		for( auto &child : children )
		{
			// auto calls destructor
			delete child;
		}

		// clear the vector
		children.clear();
	}
};

// base window class (always top level)
class F1_CWindow : public F1_CComponent
{
	bool isMoving;

	F1_Point oldM;
	F1_Point newM;
	F1_Point difM;
	F1_Point oldP;

	F1_Rect bounds;

	F1_Rect oldBounds;

	static bool inMove;

protected:

	using BaseClass = F1_CComponent;

public:

	F1_CWindow( int x, int y ) : F1_CComponent( x, y ), isMoving( false )
	{
		bounds = { x,y, 0, 0 };
		oldBounds = { 0,0 };
	}

	F1_CWindow( int x, int y, int w, int h ) : F1_CComponent( x, y, w, h ), isMoving( false )
	{
		bounds = { x, y, w, h };
		oldBounds = { 0,0,0,0 };
	}

	F1_CWindow( int x, int y, int w, int h, int bX, int bY, int bW, int bH ) : F1_CComponent( x, y, w, h ), isMoving( false )
	{
		bounds = { bX,bY, bW, bH };
		oldBounds = { 0,0,0,0 };
	}

	bool handleMouseInput( int x, int y, mouseButton m ) override;

	virtual void think() override;

	virtual void render() override;

	virtual void setBounds( F1_Rect b )
	{
		bounds = b;
	}

	//if your bounds change, overload this function
	virtual F1_Rect getBounds()
	{
		return bounds;
	}
};
#pragma once
#include <iostream>
#include <vector>
#include <algorithm>    // std::sort
#include <math.h>

#define CELL_SIZE 40
#define RES_WIDTH 1280
#define RES_HEIGHT 720
#define DIMENSION_X RES_WIDTH / CELL_SIZE
#define DIMENSION_Y RES_HEIGHT / CELL_SIZE

static int Sign (int val) 
{
	return val >= 0 ? 1 : -1;
}


struct Vector2i 
{
	int x;
	int y;
	Vector2i() { x = 0; y = 0; }
	Vector2i(int X, int Y) { x = X; y = Y; }
	//A v1 is LT v2 if v1 is more to the top right of the gridspace
	bool operator < (const Vector2i v) const { return (y < v.y || x < v.x && y == v.y); }
	bool operator == (const Vector2i v) const { return (x == v.x && y == v.y); }
	bool operator != (const Vector2i v) const { return !(x == v.x && y == v.y); }
};

struct Vector2f
{
	float x;
	float y;
	Vector2f() { x = 0; y = 0; }
	Vector2f(float X, float Y) { x = X; y = Y; }
	//A v1 is LT v2 if v1 is more to the top right of the gridspace
	bool operator < (const Vector2f v) const { return (y < v.y || x < v.x && y == v.y); }
	bool operator == (const Vector2f v) const { return (x == v.x && y == v.y); }
	bool operator != (const Vector2f v) const { return !(x == v.x && y == v.y); }
	float length()
	{
		return sqrt((x) * (x) + (y) * (y));
	}
	Vector2f getUnit() 
	{
		return Vector2f ((x / length()), (y / length()));
	}
};


struct AABB
{
	//In terms of absolute/world coordinates only
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	AABB() { xmin = 0; xmax = 0; ymin = 0; ymax = 0; }
	AABB(float xm, float xM, float ym, float yM)
	{
		xmin = xm;
		xmax = xM;
		ymin = ym;
		ymax = yM;
	}
	Vector2f centrePoint() 
	{
		return Vector2f((xmax - ((xmax - xmin) / 2)), (ymax - ((ymax - ymin) / 2)));
	}

	void set(float xm, float xM, float ym, float yM) 
	{
		xmin = xm;
		xmax = xM;
		ymin = ym;
		ymax = yM;
	}
	void translate(float x, float y) 
	{
		xmin += x;
		xmax += x;
		ymin += y;
		ymin += y;
	}

	//Check for a collision between two AABB's
	static bool checkCollision(AABB&, AABB&);

	//Square scaling
	void scaleBox(float factor) 
	{
		xmin *= factor;
		xmax *= factor;
		ymin *= factor;
		ymax *= factor;
	
	}

	//Independent scaling by factor
	void scaleBox(float factor_x, float factor_y) 
	{
		xmin *= factor_x;
		xmax *= factor_x;
		ymin *= factor_y;
		ymax *= factor_y;
	}

};

struct Entity
{
	int id;

	//FLAG HANDLING CODE NOT IMPLEMENTED
	enum Flag {
		Static,			//Object cannot move but collides
		Dynamic,		//Object can mvoe and collide
		NoCollision,	//Object can move and is not processed for collision
		Layered			//Object can move and collide but only to other 'Layered' entities.
	};

	Flag flag = Dynamic;
	AABB box;
	//In terms of grid cordinates only.
	std::vector<Vector2i> grid_positions;	
	float velocity[2];							//Translation per step, [0] = X component, [1] = Y component
	//float rotation = 0.0f;						//Rotation per step. - no implementation on AABB's

	Entity() 
	{
		id = 0;
		box.set(0, 0, 0, 0);
		velocity[0] = 0.0f;
		velocity[1] = 0.0f;
	}
	Entity(int val, AABB position)
	{
		id = val;
		box = position;
		velocity[0] = 0.0f;
		velocity[1] = 0.0f;
	}
	Entity(int val, AABB position, std::vector<Vector2i> grid_loc) 
	{
		id = val;
		box = position;
		grid_positions = grid_loc;
		velocity[0] = 0.0f;
		velocity[1] = 0.0f;
	}

	//Apply movement components to AABB
	void Update() 
	{
		this->box.xmin += velocity[0];
		this->box.xmax += velocity[0];
		this->box.ymin += velocity[1];
		this->box.ymax += velocity[1];
	}
};

//Temporary
struct collisionPair 
{
	Entity* first;
	Entity* second;
	collisionPair(Entity* a, Entity* b) 
	{
		first = a;
		second = b;
	}
};

//Only for use with SweepBoxBox()
struct sweepData 
{
	Entity a;
	Entity b;
	Vector2f v;
	Vector2f outVel;
	Vector2f hitNormal;
	Vector2f overlapTime;
	float hitTime = 0.0f;
	float outTime = 0.0f;
	bool collided = false;
};

//Grid is expected to be reconstructed on every world step - might be a bit expensive
struct SpatialGrid 
{
	//Choosing a relative value for Powered Pong - 1280/40 = 32, 720/40 = 18
	float cellSize = CELL_SIZE;

	//[row][col][entity.id]
	std::vector<int> grid[DIMENSION_X][DIMENSION_Y];

	//Keep a copy of the entity instantiation data
	std::vector<Entity*> entityList;

	//Get entity obj from entityList by id parameter
	Entity* getEntity(int id) 
	{
		//Entities should be added in ID order, so this approach should be fine. 
		//	- There is no 'Deleting' of entities on reconstruction
		return entityList[id];
	}

	//List of occupied cell coordinates (sorted)
	std::vector<Vector2i> occupiedCells;

	//Return the cell in which a point resides
	Vector2i findCell(Vector2f point);

	//Return the cell(s) which an AABB overlaps
	std::vector<Vector2i> findCell(AABB box);

	//Create the grid populated with the list of entities
	static SpatialGrid reconstruction(std::vector<Entity*>&);

	//Get all colliding pairs of entities in the grid
	std::vector<collisionPair> queryCellCollisions();
};


struct World 
{
	//Grid pointer - expect to get refreshed each simulation frame
	SpatialGrid* c_World = NULL;

	//World maintains a persistent copy of entities used in the grid for updating per step.
	std::vector<Entity*> worldEntityList;

	//Update all entities in the list, reconstruct grid - NO HANDLING FOR COLLISIONS OCCURING
	void Tick();

	World(std::vector<Entity*>& entities)
	{
		worldEntityList = entities;
		c_World = &SpatialGrid::reconstruction(entities);
	}
};

//Define Separating Axis Test (SAT) for intersection testing?
struct My_Math 
{
	//DOT PRODUCT - map first onto second (incl length)
	static float dProduct(Vector2f first, Vector2f second) 
	{
		return first.x * second.x + first.y * second.y;
	}
	//Projection of first on second, where second is converted to unit length
	static float projection(Vector2f first, Vector2f second)
	{
		return first.x * (second.x / second.length()) 
			+ first.y * (second.y / second.length());
	}

	//SEPARATING AXIS THEOREM - ONLY APPLYING X&Y checks for AABB's
	static bool sepAxis(AABB& first, AABB& second);

	//Seconds displacement as observed by first.
	static Vector2f relativeVel(Entity& first, Entity& second) 
	{
		std::cout << "f.vel[0] = " << first.velocity[0]
			<< " || s.vel[0] = " << second.velocity[0]
			<< " || f.vel[1] = " << first.velocity[1]
			<< " || s.vel[1] = " << second.velocity[1] << "\n" << std::endl;
		return Vector2f(first.velocity[0] - second.velocity[0],
			first.velocity[1] - second.velocity[1]);
	}

	//Get edge normal, assuming edge going from (x1, y1) to (x2, y2)
	// 0 - left normal, 1 - right normal
	static Vector2f* edgeNormal(Vector2f first, Vector2f second);

	//Testing method from stackOverflow - if data is persistent need to invert velocity on return.
	// Sweep a in the direction of v against b, returns true & info if there was a hit
	static void SweepBoxBox(sweepData&);
};
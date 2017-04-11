#include "Collision.h"

//Check for a collision between two AABB's
bool AABB::checkCollision(AABB& a, AABB& b) 
{
	float lowest_min_x = std::min(a.xmin, b.xmin);
	float lowest_min_y = std::min(a.ymin, b.ymin);
	//a is more to the left
	if (lowest_min_x == a.xmin) 
	{
		if (a.xmin + (a.xmax - a.xmin) >= b.xmin) 
		{
			if (a.ymin + (a.ymax - a.ymin) >= b.ymin) 
			{
				//Collision on both axis
				return true;
			}
			else 
			{
				//Collision on only one axis
				return false;
			}
		}
		else 
		{
			//No collision as both axis need to intersect
			return false;
		}
	}
	//b is more to the left
	else 
	{
		if (b.xmin + (b.xmax - b.xmin) >= a.xmin)
		{
			if (b.ymin + (b.ymax - b.ymin) >= a.ymin)
			{
				//Collision on both axis
				return true;
			}
			else
			{
				//Collision on only one axis
				return false;
			}
		}
		else
		{
			//No collision as both axis need to intersect
			return false;
		}
	}
}


//Return the cell in which a point resides
Vector2i SpatialGrid::findCell(Vector2f point) 
{
	// Math.floor( (position - gridMinimum) / gridCellSize )

	int col = floor(point.x / cellSize);
	int cell = floor(point.y / cellSize);

	return Vector2i(col, cell); // grid value in which point resides
}

//Return the cell(s) which an AABB overlaps
std::vector<Vector2i> SpatialGrid::findCell(AABB box)
{
	int cell_count_x = floor(box.xmax / cellSize) - floor(box.xmin / cellSize);
	int cell_count_y = floor(box.ymax / cellSize) - floor(box.ymin / cellSize);
	int cell_min_x = floor(box.xmin / cellSize);
	int cell_min_y = floor(box.ymin / cellSize);
	//In case count = 0, we set to 1 to take at least 1 input.
	cell_count_x = std::max(1, cell_count_x);
	cell_count_y = std::max(1, cell_count_y);

	std::vector<Vector2i> cellList;

	for (int y = cell_min_y; y < cell_count_y + cell_min_y; y++) {
		for (int x = cell_min_x; x < cell_count_x + cell_min_x; x++)
		{
			cellList.push_back(Vector2i(x, y));
		}
	}

	return cellList;
}

//Create the grid populated with the list of entities - NEEDS DETAILED COMMENTING
SpatialGrid SpatialGrid::reconstruction(std::vector<Entity*>& entityList) 
{
	//Pseudocode @ https://gyazo.com/5bbff8737f59218ee62e344c54760301
	SpatialGrid newGrid;
	std::vector<Vector2i> cells;
	for (auto &entity : entityList) 
	{
		//Find the cells that an entities AABB overlaps with
		cells = newGrid.findCell(entity->box);
		//Clear the entity data
		entity->grid_positions.clear();
		//For every cell an AABB overlaps with:
		for (auto point : cells)
		{
			//Store the entity ID into the container in [row][col]
			newGrid.grid[point.x][point.y].push_back(entity->id);
			//For every cell overlap we add the cell coordinate, multiple of the same  = collision.
			newGrid.occupiedCells.push_back(Vector2i(point.x, point.y));
			//Push_back cell coordinates into entity data
			entity->grid_positions.push_back(Vector2i(point.x, point.y));
		}
	}
	//Sort the occupiedCellslist by x and y coordinate in grid - for comparisons
	std::sort(newGrid.occupiedCells.begin(), newGrid.occupiedCells.end());
	//Keep a copy of (edited) parameter data
	newGrid.entityList = entityList;
	return newGrid;
}

//SEPARATING AXIS THEOREM - ONLY APPLYING X&Y checks for AABB's
// - Returns true of there is a separating axis between the AABB's
bool My_Math::sepAxis(AABB& first, AABB& second)
{

	//X-AXIS
	if (first.xmax < second.xmin || second.xmax < first.xmin)
	{
		//No collision on X
	}
	else
	{
		//Collision on X
		return true;
	}

	//Y-AXIS
	if (first.ymax < second.ymin || second.ymax < first.ymin)
	{
		//No collision on Y
	}
	else
	{
		//Collision on Y
		return true;
	}
}

//Get edge normal, assuming edge going from (x1, y1) to (x2, y2)
// 0 - left normal, 1 - right normal
Vector2f* My_Math::edgeNormal(Vector2f first, Vector2f second)
{
	float dy = second.y - first.y;
	float dx = second.x - first.x;
	Vector2f normals[2];
	normals[0] = Vector2f(-1 * dy, dx);
	normals[1] = Vector2f(dy, -1 * dx);
	return normals;
}

//Get all colliding pairs in the grid
/*
	Traverse occupiedCells to find duplicates (which indicate a collision),
	Mark the processed/processing collisions so you don't re-process them (even in other cells),
	process all collisions within multiple-occupancy cell.
*/
std::vector<collisionPair> SpatialGrid::queryCellCollisions() 
{
	//Used to prevent double checking cells
	Vector2i prev_entry;
	Vector2i new_entry;

	std::vector<std::vector<int>> mEntries;
	std::vector<collisionPair> container;

	//If there is 1 or less entries we know there is no collision
	if (occupiedCells.size() <= 1) 
	{
		return container;
	}
	//Check if the two occupied cells are equal (duplicates mean double occupancy) and save id list
	for (int i = 1; i < occupiedCells.size(); i++)
	{
		new_entry.x = occupiedCells[i].x;
		new_entry.y = occupiedCells[i].y;
		if ((occupiedCells[i-1] == occupiedCells[i]) && (new_entry != prev_entry)) 
		{
			mEntries.push_back(grid[occupiedCells[i].x][occupiedCells[i].y]);
			prev_entry = new_entry;
		}
	}

	//Check for collisions within the flagged id's
	for (auto id_list : mEntries) 
	{
		//Check each object against every other object - don't do twice
		for (int i = 0; i < id_list.size(); i++)
		{
			for (int j = i+1; j < id_list.size(); j++)
			{
				//If there is a collision then add it to container.
				if (AABB::checkCollision(this->getEntity(id_list[i])->box, this->getEntity(id_list[j])->box)) 
				{
					container.push_back(collisionPair(this->getEntity(id_list[i]), this->getEntity(id_list[j])));
				}
			}
		}
	}
	return container;
}

//Update all entity components & reconstruct grid
void World::Tick() 
{
	for (auto entity_ptr : worldEntityList) 
	{
		entity_ptr->Update();
	}
	c_World = &SpatialGrid::reconstruction(worldEntityList);
}
void My_Math::SweepBoxBox(sweepData& data)
{
	//Initialise out info
	data.outVel = data.v;
	data.hitNormal = Vector2f(0.0f, 0.0f);

	// Return early if a & b are already overlapping
	if (AABB::checkCollision(data.a.box, data.b.box))
	{
		data.collided = true;
		std::cout << "\n\nPre-calc collision!\n" << std::endl;
		return;
	}

	// Treat b as stationary, so invert v to get relative velocity
	data.v = Vector2f (-1 * data.v.x, -1* data.v.y);

	data.hitTime = 0.0f;
	data.outTime = 1.0f;
	data.overlapTime = Vector2f (0.0f, 0.0f);

	// X axis overlap
	if (data.v.x < 0)
	{
		if (data.b.box.xmax < data.a.box.xmin) return;
		if (data.b.box.xmax > data.a.box.xmin) data.outTime = std::min((data.a.box.xmin - data.b.box.xmax) / data.v.x, data.outTime);

		if (data.a.box.xmax < data.b.box.xmin)
		{
			data.overlapTime.x = (data.a.box.xmax - data.b.box.xmin) / data.v.x;
			data.hitTime = std::max(data.overlapTime.x, data.hitTime);
		}
	}
	else if (data.v.x > 0)
	{
		if (data.b.box.xmin > data.a.box.xmax) return;
		if (data.a.box.xmax > data.b.box.xmin) data.outTime = std::min((data.a.box.xmax - data.b.box.xmin) / data.v.x, data.outTime);

		if (data.b.box.xmax < data.a.box.xmin)
		{
			data.overlapTime.x = (data.a.box.xmin - data.b.box.xmax) / data.v.x;
			data.hitTime = std::max(data.overlapTime.x, data.hitTime);
		}
	}

	if (data.hitTime > data.outTime) return;

	//=================================

	// Y axis overlap
	if (data.v.y < 0)
	{
		if (data.b.box.ymax < data.a.box.ymin) return;
		if (data.b.box.ymax > data.a.box.ymin) data.outTime = std::min((data.a.box.ymin - data.b.box.ymax) / data.v.y, data.outTime);

		if (data.a.box.ymax < data.b.box.ymin)
		{
			data.overlapTime.y = (data.a.box.ymax - data.b.box.ymin) / data.v.y;
			data.hitTime = std::max(data.overlapTime.y, data.hitTime);
		}
	}
	else if (data.v.y > 0)
	{
		if (data.b.box.ymin > data.a.box.ymax) return;
		if (data.a.box.ymax > data.b.box.ymin) data.outTime = std::min((data.a.box.ymax - data.b.box.ymin) / data.v.y, data.outTime);

		if (data.b.box.ymax < data.a.box.ymin)
		{
			data.overlapTime.y = (data.a.box.ymin - data.b.box.ymax) / data.v.y;
			data.hitTime = std::max(data.overlapTime.y, data.hitTime);
		}
	}

	if (data.hitTime > data.outTime) return;

	// Scale resulting velocity by normalized hit time
	data.outVel = Vector2f (-1 * data.v.x * data.hitTime, -1 * data.v.y * data.hitTime);

	// Hit normal is along axis with the highest overlap time
	if (data.overlapTime.x > data.overlapTime.y)
	{
		data.hitNormal = Vector2f(Sign(data.v.x), 0);
	}
	else if (fmax(data.overlapTime.x, fmax(data.overlapTime.y, 0.0f)) == 0.0f) 
	{
		//No collision
		std::cout << "\nNo overlap!\n" << std::endl;
		return;
	}
	else
	{
		data.hitNormal = Vector2f(0, Sign(data.v.y));
	}

	std::cout << "\nStandard Collision!\n" << std::endl;
	data.collided = true;
}



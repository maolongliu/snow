#ifndef GRID_H
#define	GRID_H

#include "PointCloud.h"
#include "Vector2f.h"
#include "SimConstants.h"
#include <math.h>
#include <cstring>
#include <stdio.h>

const float BSPLINE_EPSILON = 1e-6;

//Grid node data
typedef struct GridNode{
	float mass;
	bool has_velocity;
	Vector2f force,
		velocity,
		velocity_new;
	
	//All the following variables are used by the implicit linear solver
	bool active;	//are we still solving for vf
	Vector2f err,	//error of estimate
		r,			//residual of estimate
		p,			//residual gradient? squared residual?
		Ep,	Er;		//yeah, I really don't know how this works...
	float rEr;		//r.dot(Er)
} GridNode;

class Grid {
public:
	Vector2f origin, size, cellsize;
	PointCloud* obj;	
	float node_volume;
	//Nodes: use (y*size[0] + x) to index, where zero is the bottom-left corner (e.g. like a cartesian grid)
	int nodes_length;
	GridNode* nodes;
	
	//Grid be at least one cell; there must be one layer of cells surrounding all particles
	Grid(Vector2f pos, Vector2f dims, Vector2f cells, PointCloud* obj);
	Grid(const Grid& orig);
	virtual ~Grid();

	//Map particles to grid
	void initializeMass();
	void initializeVelocities();
	//Map grid volumes back to particles (first timestep only)
	void calculateVolumes() const;
	//Compute grid velocities
	void explicitVelocities(const Vector2f& gravity);
	void implicitVelocities();
	void recomputeImplicitForces();
	//Map grid velocities back to particles
	void updateVelocities() const;
	
	//Cubic B-spline shape/basis/interpolation function
	//A smooth curve from (0,1) to (1,0)
	static float bspline(float x){
		float w;
		if (x < 1)
			w = x*x*(x/2 - 1) + 2/3.0;
		else if (x < 2)
			w = x*(x*(-x/6 + 1) - 2) + 4/3.0;
		else return 0;
		//Clamp between 0-1
		if (w < BSPLINE_EPSILON) return 0;
		if (w > 1) return 1;
		return w;
	}
	//Slope of interpolation function
	static float bsplineSlope(float x){
		float w;
		if (x < 1)
			w = x*(3/2.0*x - 2);
		else if (x < 2)
			w = x*(2 - x/2) - 2;
		else return 0;
		//Clamp between 0-1
		if (w < BSPLINE_EPSILON) return 0;
		if (w > 1) return 1;
		return w;
	}
};

#endif
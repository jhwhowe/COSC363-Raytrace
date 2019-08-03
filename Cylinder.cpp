/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
	//getting things needed for the quadratic
	float dx = dir.x;
	float dy = dir.y;
	float dz = dir.z;
	float xo = posn.x;
	float yo = posn.y;
	float zo = posn.z;
	float xc = center.x;
	float yc = center.y;
	float zc = center.z;
	float R = radius;
	
	//getting a, b and c
	float a = dx * dx + dz * dz;
	float b = 2 * (dx * (xo - xc) + dz * (zo - zc));
	float c = (xo - xc) * (xo - xc)  + (zo - zc) * (zo - zc) - R * R;
	
	float descriminant = b * b - 4 * a * c;
	
	if(fabs(descriminant) < 0.001 || descriminant < 0){
		return -1;
	}
	
	float t1 = (-b + sqrt(descriminant)) / (2 * a);
	float t2 = (-b - sqrt(descriminant)) / (2 * a);
	
	if(fabs(t1) < 0.001){
		if (t2 > 0){
			return t2;
		} else {
			t1 = -1.0;
		}
	}	

    if(fabs(t2) < 0.001){
		t2 = -1.0;
	}
    
    float max;
    float min;

	if(t1 > t2){
		max = t1;
		min = t2;
	} else {
		max = t2;
		min = t1;
	}

	float y1 = (yo + min * dy) - yc;
	float y2 = (yo + max * dy) - yc;

	if(! (y1 < 0 || y1 > height) && min != -1.0){
		return min;
	} else if(!(y2 < 0 || y2 > height) && max != -1.0){
		 return max;
	} else {
		return -1.0;
	}
}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    n[1] = 0;
    n = glm::normalize(n);
    return n;
}

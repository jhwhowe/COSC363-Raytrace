/*========================================================================
* COSC 363  Computer Graphics (2017)
* Assignment 2
* Doing some good ray tracing stuff.
*=========================================================================
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float PI = 3.14159265358979323846;  /* pi */

//for the procedural sphere
const float RADIUS4 = 4.0;
const glm::vec3 COORDS4(-7.0, -8.0, -50.0);

TextureBMP floorTexture;
TextureBMP wallTexture;
TextureBMP ballTexture;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

//function to return different colours depending on what the x,y,z 
//coordinates of the sphere are - wow!
glm::vec3 proceedText(Ray ray){
	//separate sphere into zones
	//pick random colour for zone
	//return colour of zone ray is in
	glm::vec3 col;
	float r = RADIUS4; //radius of the sphere
	glm::vec3 coords = COORDS4; //coordinates of the sphere
	float botPoint = coords[1] - r; //base of the sphere
	float leftPoint = coords[0] - r; //base of the sphere
	
	//horizontal strips
	float strip0 = botPoint + 2 * r * 0.25;
	float strip1 = botPoint + 2 * r * 0.50;
	float strip2 = botPoint + 2 * r * 0.75;
	float strip3 = botPoint + 2 * r;
	
	//vertical strips
	float strip4 = leftPoint + 2 * r * 0.25;
	float strip5 = leftPoint + 2 * r * 0.50;
	float strip6 = leftPoint + 2 * r * 0.75;
	float strip7 = leftPoint + 2 * r;
	
	if(ray.xpt.x < strip4){
		if(ray.xpt.y < strip0){
			col = glm::vec3(0,0,0);
		} else if (ray.xpt.y < strip1){
			col = glm::vec3(0,0,1);
		} else if (ray.xpt.y < strip2){
			col = glm::vec3(0,1,0);
		} else if (ray.xpt.y < strip3){
			col = glm::vec3(0,1,1);
		}
	} else if (ray.xpt.x < strip5){
		if(ray.xpt.y < strip0){
			col = glm::vec3(1,0,0);
		} else if (ray.xpt.y < strip1){
			col = glm::vec3(1,0,1);
		} else if (ray.xpt.y < strip2){
			col = glm::vec3(1,1,1);
		} else if (ray.xpt.y < strip3){
			col = glm::vec3(1,1,1);
		}
	} else if (ray.xpt.x < strip6){
		if(ray.xpt.y < strip0){
			col = glm::vec3(1,1,1);
		} else if (ray.xpt.y < strip1){
			col = glm::vec3(1,1,0);
		} else if (ray.xpt.y < strip2){
			col = glm::vec3(1,0,1);
		} else if (ray.xpt.y < strip3){
			col = glm::vec3(1,0,0);
		}
	} else if (ray.xpt.x < strip7){
		if(ray.xpt.y < strip0){
			col = glm::vec3(0,1,1);
		} else if (ray.xpt.y < strip1){
			col = glm::vec3(0,1,0);
		} else if (ray.xpt.y < strip2){
			col = glm::vec3(0,0,1);
		} else if (ray.xpt.y < strip3){
			col = glm::vec3(0,0,0);
		}
	}
	return col;
}

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 light2(-40,30, -10);
	float ambientTerm = 0.2;
	float shininess = 50;
	glm::vec3 colorSum;
	float lightDist = 0;
	float light2Dist = 0;
	float specTerm = 0;
	float specTerm2 = 0;
    glm::vec3 white = glm::vec3(1);
    glm::vec3 white2 = glm::vec3(1.0, 0.0, 1.0);
    float rDotV = 0;
    float lDotN = 0;
    float rDotV2 = 0;
    float lDotN2 = 0;
    float texcoords = 0;
	float texcoordt = 0;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1){
		 return backgroundCol;      //If there is no intersection return background colour
	}

    glm::vec3 col = sceneObjects[ray.xindex]->getColor(); //else return bject's colour    
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    normalVector = normalize(normalVector);
    glm::vec3 lightVector = light - ray.xpt;
    glm::vec3 light2Vector = light2 - ray.xpt;
    lightVector = normalize(lightVector);
    light2Vector = normalize(light2Vector);
    lightDist =  glm::distance(light, normalVector);
    light2Dist =  glm::distance(light2, normalVector);
    
	Ray shadow(ray.xpt, lightVector);
	shadow.closestPt(sceneObjects);
	
	Ray shadow2(ray.xpt, light2Vector);
	shadow2.closestPt(sceneObjects);
    
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    rDotV = glm::dot(reflVector, -ray.dir);
    lDotN = glm::dot(lightVector, normalVector);
    
    glm::vec3 refl2Vector = glm::reflect(-light2Vector, normalVector);
    rDotV2 = glm::dot(refl2Vector, -ray.dir);
    lDotN2 = glm::dot(light2Vector, normalVector);
    
    if (rDotV >= 0) {
		specTerm = pow(rDotV, shininess) ;
    }
    
    if (rDotV2 >= 0) {
		specTerm2 = pow(rDotV2, shininess) ;
    }
	
	if(ray.xindex == 3){ //textured floor
		texcoords = (ray.xpt.x + 50)/(50 - -50);
		texcoordt = (ray.xpt.z + -40)/(-200);
		col = floorTexture.getColorAt(texcoordt, texcoords);
	}	
	
	if(ray.xindex == 1 && step < MAX_STEPS){ //refractive sphere
		float eta = 1/1.03;
		glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
		Ray refrRay1(ray.xpt, g);
		refrRay1.closestPt(sceneObjects);
		glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
		glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
		
		Ray refrRay2(refrRay1.xpt, h);
		glm::vec3 refCol = trace(refrRay2, step + 1); 
		
		colorSum = refCol;
	}
	
	if(ray.xindex == 2){ //texture sphere
		glm::vec3 sphereCenter(0.0, 10.0, -85.0);
		glm::vec3 sphereNormal = normalize(ray.xpt - sphereCenter);
		
		texcoords = 0.5 + atan2(sphereNormal.z, sphereNormal.x) / (2 * PI);
		texcoordt = 0.5 - asin(sphereNormal.y) / PI;	
		
		if(texcoords < 0){
			texcoords = texcoords + 2 * PI;
		}
		
		col = ballTexture.getColorAt(texcoords, texcoordt);
	}
	
	
    if(ray.xindex == 11){ //random sphere
		col = proceedText(ray);
	}
        
    if((lDotN <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDist))
		&& (lDotN2 <= 0 || (shadow2.xindex > -1 && shadow2.xdist < light2Dist))){
		if(ray.xindex == !1){
			colorSum = ambientTerm * col;
		}
	} else if((!(lDotN <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDist))
		&& (lDotN2 <= 0 || (shadow2.xindex > -1 && shadow2.xdist < light2Dist))) || 
		(((lDotN <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDist)))
		&& !(lDotN2 <= 0 || (shadow2.xindex > -1 && shadow2.xdist < light2Dist)))){
		if(ray.xindex != 1){
			colorSum = 1.5f * ambientTerm * col;
		}
	} else {
		if(ray.xindex != 1){
			colorSum = (ambientTerm * col + lDotN * col + white * specTerm + white2 * specTerm2);
		}
	}
	
    if(ray.xindex == 0 && step < MAX_STEPS){ //reflective sphere
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
		colorSum = colorSum + (0.8f*reflectedCol);
	}
	
    if(ray.xindex == 4){ //textured wall
		texcoordt = (ray.xpt.x + 50)/(50 - -50);
		texcoords = (ray.xpt.y + 20)/(50 - -20);
		colorSum = wallTexture.getColorAt(texcoordt, texcoords);
	}
	
	return colorSum;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;
			
			
			//~ glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST); //original ray
			//~ Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			//~ ray.normalize();				//Normalize the direction of the ray to a unit vector
		    //~ glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			
			
			//split the pixel up into 4 
		    glm::vec3 dir1(xp+0.25*cellX, yp+0.25*cellY, -EDIST); //bottom left
		    glm::vec3 dir2(xp+0.25*cellX, yp+0.75*cellY, -EDIST); //bottom right
		    glm::vec3 dir3(xp+0.75*cellX, yp+0.25*cellY, -EDIST); //top left
		    glm::vec3 dir4(xp+0.75*cellX, yp+0.75*cellY, -EDIST); //top right

		    Ray ray1 = Ray(eye, dir1);		//Create a ray originating from the camera in the direction 'dir1'
			ray1.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
		    
		    Ray ray2 = Ray(eye, dir2);		//Create a ray originating from the camera in the direction 'dir2'
			ray2.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col2 = trace (ray2, 1); //Trace the primary ray and get the colour value
		    
		    Ray ray3 = Ray(eye, dir3);		//Create a ray originating from the camera in the direction 'dir3'
			ray3.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col3 = trace (ray3, 1); //Trace the primary ray and get the colour value
		    
		    Ray ray4 = Ray(eye, dir4);		//Create a ray originating from the camera in the direction 'dir4'
			ray4.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col4 = trace (ray4, 1); //Trace the primary ray and get the colour value
			
			//take the average of the colour values from these rays 
			glm::vec3 col = (col1 + col2 + col3 + col4) / 4.0f;
			
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    
    floorTexture = TextureBMP((char*)"./floor.bmp"); //the (char*) is to remove some annoying error
    wallTexture = TextureBMP((char*)"./PANDABOYE.bmp");
    ballTexture = TextureBMP((char*)"./check.bmp");

	//All of the spheres.
	Sphere *sphere1 = new Sphere(glm::vec3(-8.0, -5.0, -90.0), 10, glm::vec3(1, 1, 1)); //big reflective sphere
	Sphere *sphere2 = new Sphere(glm::vec3(10.0, 8.0, -70.0), 5.0, glm::vec3(0, 1, 0)); //clear refractive one
	Sphere *sphere3 = new Sphere(glm::vec3(-4.0, 10.0, -85.0), 7.0, glm::vec3(0.29, 0.255, 0.165)); //make a green boye
	Sphere *sphere4 = new Sphere(COORDS4, RADIUS4, glm::vec3(1.0, 0.0, 1.0)); //procedural texted 3D circle
	
	//The floor
	Plane *plane = new Plane (
	 glm::vec3(-50., -20, -40), //Point A
	 glm::vec3(50., -20, -40), //Point B
	 glm::vec3(50., -20, -200), //Point C
	 glm::vec3(-50., -20, -200), //Point D
	 glm::vec3(1, 1, 1)); //Colour
	 
	 //wall
	Plane *wall = new Plane (
	 glm::vec3(50., -20, -150), //Point C
	 glm::vec3(50., 50, -150), //Point D
	 glm::vec3(-50., 50, -150), //Point A
	 glm::vec3(-50., -20, -150), //Point B
	 glm::vec3(0, 1, 0)); //Colour
	 
	//make a bunch o planes to be a cube
	Plane *base = new Plane ( //base
	 glm::vec3(5, -20, -80), //front left
	 glm::vec3(15, -20, -80), //front right
	 glm::vec3(15, -20, -90), //back left
	 glm::vec3(5, -20, -90), //back right
	 glm::vec3(0.0, 0.35, 0)); //Colour
	  
	Plane *top = new Plane (
	 glm::vec3(5, -10, -80), //Point A
	 glm::vec3(15, -10, -80), //Point B
	 glm::vec3(15, -10, -90), //Point C
	 glm::vec3(5, -10, -90), //Point D
	 glm::vec3(0.0, 0.35, 0)); //Colour

	Plane *front = new Plane (
	 glm::vec3(5, -10, -80), //Point A
	 glm::vec3(5, -20, -80), //Point B
	 glm::vec3(15, -20, -80), //Point C
	 glm::vec3(15, -10, -80), //Point D
	 glm::vec3(0.0, 0.35, 0)); //Colour
	 
	Plane *back = new Plane (
	 glm::vec3(5, -10, -90), //Point A
	 glm::vec3(5, -20, -90), //Point B
	 glm::vec3(15, -20, -90), //Point C
	 glm::vec3(15, -10, -90), //Point D
	 glm::vec3(0.0, 0.35, 0)); //Colour
	
	Plane *left = new Plane (
	 glm::vec3(5, -10, -90), //Point A
	 glm::vec3(5, -20, -90), //Point B
	 glm::vec3(5, -20, -80), //Point C
	 glm::vec3(5, -10, -80), //Point D
	 glm::vec3(0.0, 0.35, 0)); //Colour
	
	Plane *right = new Plane (
	 glm::vec3(15, -10, -80), //Point A
	 glm::vec3(15, -20, -80), //Point B
	 glm::vec3(15, -20, -90), //Point C
	 glm::vec3(15, -10, -90), //Point D
	 glm::vec3(0.0, 0.35, 0)); //Colour
	 
	Cylinder *cyl = new Cylinder(glm::vec3(10.0, 0.0, -60.0), 1.0, 5.0, glm::vec3(0.29, 0.255, 0.165)); //make a tube
	
	
	//R O U N D B O Y E S
	sceneObjects.push_back(sphere1); 
	sceneObjects.push_back(sphere2); 
	sceneObjects.push_back(sphere3); 
	
	//floor and wall
	sceneObjects.push_back(plane);
	sceneObjects.push_back(wall);
	
	//cube boyes
	sceneObjects.push_back(base);
	sceneObjects.push_back(front);
	sceneObjects.push_back(back);
	sceneObjects.push_back(top);
	sceneObjects.push_back(left);
	sceneObjects.push_back(right);
	
	sceneObjects.push_back(sphere4); 
	
	//da tube
	sceneObjects.push_back(cyl);
	
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Wow what a great scene");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}

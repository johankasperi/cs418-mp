//
//  main.cpp
//  cs418_kasperi2_mp2
//
//  Created by Johan Kasperi on 27/02/15.
//  Copyright (c) 2015 cs418. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

float sealevel;
float polysize;

GLfloat snowamb[] = {0.7,0.7,0.7,1.0};
GLfloat snowdiff[] = {1.0,1.0,1.0,1.0};
GLfloat greyamb[] = {0.2,0.2,0.2,1.0};
GLfloat greydiff[] = {0.3,0.3,0.3,1.0};
GLfloat greenamb[] = {0.0,0.1,0.05,1.0};
GLfloat greendiff[] = {0.0,0.3,0.08,1.0};

int seed(float x, float y) {
    static int a = 1588635695, b = 1117695901;
    int xi = *(int *)&x;
    int yi = *(int *)&y;
    return ((xi * a) % b) - ((yi * b) % a);
}

void mountain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float s)
{
    float x01,y01,z01,x12,y12,z12,x20,y20,z20;
    
    if (s < polysize) {
        x01 = x1 - x0;
        y01 = y1 - y0;
        z01 = z1 - z0;
        
        x12 = x2 - x1;
        y12 = y2 - y1;
        z12 = z2 - z1;
        
        x20 = x0 - x2;
        y20 = y0 - y2;
        z20 = z0 - z2;
        
        float nx = y01*(-z20) - (-y20)*z01;
        float ny = z01*(-x20) - (-z20)*x01;
        float nz = x01*(-y20) - (-x20)*y01;
        
        float den = sqrt(nx*nx + ny*ny + nz*nz);
        
        if (den > 0.0) {
            nx /= den;
            ny /= den;
            nz /= den;
        }
        glNormal3f(nx,ny,nz);
        glBegin(GL_TRIANGLES);
        glVertex3f(x0,y0,z0);
        glVertex3f(x1,y1,z1);
        
        glVertex3f(x2,y2,z2);
        glEnd();
        
        return;
    }
    x01 = 0.5*(x0 + x1);
    y01 = 0.5*(y0 + y1);
    z01 = 0.5*(z0 + z1);
    
    x12 = 0.5*(x1 + x2);
    y12 = 0.5*(y1 + y2);
    z12 = 0.5*(z1 + z2);
    
    
    x20 = 0.5*(x2 + x0);
    y20 = 0.5*(y2 + y0);
    z20 = 0.5*(z2 + z0);
    
    // Making it look like the alps by checking the z01 value and apply different lighting based on that.
    if(z01>0.08) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, snowamb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, snowdiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, snowdiff);
    }
    else if(z01<0.08 && z01>0.04) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, greyamb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, greydiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, greydiff);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, greenamb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, greendiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, greendiff);
    }
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
    s *= 0.5;
    
    srand(seed(x01,y01));
    z01 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
    srand(seed(x12,y12));
    z12 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
    srand(seed(x20,y20));
    z20 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
    
    mountain(x0,y0,z0,x01,y01,z01,x20,y20,z20,s);
    mountain(x1,y1,z1,x12,y12,z12,x01,y01,z01,s);
    mountain(x2,y2,z2,x20,y20,z20,x12,y12,z12,s);
    mountain(x01,y01,z01,x12,y12,z12,x20,y20,z20,s);
}

void init(void)
{
    GLfloat white[] = {1.0,1.0,1.0,1.0};
    GLfloat lpos[] = {0.0,1.0,0.0,0.0};
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
    glClearColor (0.5, 0.5, 1.0, 0.0);
    /* glShadeModel (GL_FLAT); */
    glEnable(GL_DEPTH_TEST);
    
    sealevel = -0.05;
    polysize = 0.01;
}

// Helper class for vectors and common vector algebra
class Vector {
public:
    double x, y, z;
    Vector(double a, double b, double c) {
        x = a; y = b; z = c;
    }
    
    // Vector times a scalar
    Vector times(double v) {
        return Vector(x*v,y*v, z*v);
    }
    
    // Vector addition
    Vector add(Vector i) {
        return Vector(x+i.x, y+i.y, z+i.z);
    }
    
    // Vector subtraction
    Vector sub(Vector i) {
        return Vector(x-i.x, y-i.y, z-i.z);
    }
    
    // Dot product of two vectors
    double dot(Vector i) {
        return x*i.x+y*i.y+z*i.z;
    }
    
    // Get length of vector
    double length() {
        return sqrt(x*x+y*y+z*z);
    }
    
    // Normalize vector (make it unit length)
    void normalize() {
        double length = this->length();
        if(length > 1.000001 || length < 0.999999) { // Do not perform normalization if we don't need to
            x /= length;
            y /= length;
            z /= length;
        }
    }
    
    // Crossproduct of two vectors
    Vector cross(Vector i) {
        Vector o = Vector(0.0,0.0,0.0);
        o.x = y*i.z-z*i.y;
        o.y = z*i.x-x*i.z;
        o.z = x*i.y-y*i.x;
        return o;
    }
};

// Helper class for our airplane
class Plane {
public:
    
    // Initialize vectors
    Vector eye = Vector(0.0, 0.0, 0.4);
    Vector lookAt = Vector(0.4, 0.1, -1);
    Vector up = Vector(0.0,1.0,0.0);
    Vector right = Vector(1.0, 0.0, 0.0);
    
    // Move the airplane forward
    void moveForward(double speed) {
        eye = eye.add(lookAt.times(speed));
    }
    
    // Pitch plane on key up or down
    void pitch(double angle) {
        lookAt.normalize();
        up.normalize();
        
        // Rotate lookAt about right as done in http://math.kennesaw.edu/~plaval/math4490/rotgen.pdf
        // r = rotation axis, v = vector to be rotated
        // T(v) = (1 - cos phi)(v x r)r + vcos phi + (r x v)sin phi
        lookAt = right.times((1-cos(angle))*lookAt.dot(right))
        .add(lookAt.times(cos(angle)))
        .add(right.cross(lookAt).times(sin(angle)));
        
        // lookAt and right is unit length and orthogonal, get up vector by taking cross product of these instead of rotating it.
        up = right.cross(lookAt);
    }
    
    // Roll plane on key up or down
    void roll(double angle) {
        up.normalize();
        right.normalize();
        
        // Rotate up about lookAt as done in http://math.kennesaw.edu/~plaval/math4490/rotgen.pdf
        // r = rotation axis, v = vector to be rotated
        // T(v) = (1 - cos phi)(v x r)r + vcos phi + (r x v)sin phi
        up = lookAt.times((1-cos(angle))*up.dot(lookAt))
        .add(up.times(cos(angle)))
        .add(lookAt.cross(up).times(sin(angle)));
        
        // lookAt and up is unit length and orthogonal, get right vector by taking cross product of these instead of rotating it.
        right = lookAt.cross(up);
    }
};

// Initialize our airplane and angles for pitch and roll
Plane plane;
double pitchAngle = 0;
double rollAngle = 0;
float speed = 0.001;

void display(void)
{
    // Apply pitch to our airplane if key up or down is pressed
    if(pitchAngle != 0) {
        plane.pitch(pitchAngle);
    }
    
    // Apply pitch to our airplane if key up or down is pressed
    if(rollAngle != 0) {
        plane.roll(rollAngle);
    }
    
    // Move the plane forward
    plane.moveForward(speed);
    
    glLoadIdentity();
    gluLookAt(plane.eye.x, plane.eye.y, plane.eye.z, plane.eye.x+plane.lookAt.x, plane.eye.y+plane.lookAt.y, plane.eye.z+plane.lookAt.z, plane.up.x, plane.up.y, plane.up.z);
    
    // Not my code
    GLfloat seaamb[] = {0.0,0.0,0.2,1.0};
    GLfloat seadiff[] = {0.0,0.0,0.8,1.0};
    GLfloat seaspec[] = {0.5,0.5,1.0,1.0};
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    glTranslatef (-0.5, -0.5, 0.0);
    
    mountain(0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 1.0);
    mountain(1.0,1.0,0.0, 0.0,1.0,0.0, 1.0,0.0,0.0, 1.0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
    
    glNormal3f(0.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(0.0,0.0,sealevel);
    glVertex3f(1.0,0.0,sealevel);
    glVertex3f(1.0,1.0,sealevel);
    glVertex3f(0.0,1.0,sealevel);
    glEnd();
    
    glutSwapBuffers();
    glFlush ();
    
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(90.0,1.0,0.01,10.0);
    glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case '-':
            sealevel -= 0.01;
            break;
        case '+':
        case '=':
            sealevel += 0.01;
            break;
        case 'f':
            polysize *= 0.5;
            break;
        case 'c':
            polysize *= 2.0;
            break;
        case 'w':
            speed += 0.0005;
            break;
        case 's':
            speed -= 0.0005;
            break;
        case 27:
            exit(0);
            break;
    }
}

void specialKey(int key, int x, int y)
{
    // Catch key presses to apply pitch or roll
    switch (key) {
        case GLUT_KEY_UP:
            pitchAngle = -0.05;
            break;
        case GLUT_KEY_DOWN:
            pitchAngle = 0.05;
            break;
        case GLUT_KEY_LEFT:
            rollAngle = -0.05;
            break;
        case GLUT_KEY_RIGHT:
            rollAngle = 0.05;
            break;
    }
}

void releaseKey(int key, int x, int y)
{
    // Set the pitchAngle or rollAngle to zero when key is released
    switch (key) {
        case GLUT_KEY_UP:
            pitchAngle = 0;
            break;
        case GLUT_KEY_DOWN:
            pitchAngle = 0;
            break;
        case GLUT_KEY_LEFT:
            rollAngle = 0;
            break;
        case GLUT_KEY_RIGHT:
            rollAngle = 0;
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(releaseKey);
    glutMainLoop();
    return 0;
}
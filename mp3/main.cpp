//
//  main.cpp
//  cs418_mp3_kasperi2
//
//  Created by Johan Kasperi on 10/04/15.
//  Copyright (c) 2015 cs418. All rights reserved.
//

#include <stdlib.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "SOIL.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Class for 2D vectors
class Vector2f {
public:
    float s, t;
    
    Vector2f(float _s, float _t) {
        s = _s;
        t = _t;
    }
    
    Vector2f operator+(Vector2f inpVector) {
        return Vector2f(s+inpVector.s, t+inpVector.t);
    }
    
    Vector2f operator-(Vector2f inpVector) {
        return Vector2f(s-inpVector.s, t-inpVector.t);
    }
};

// Class for 3D vectors
class Vector3f {
public:
    float x, y, z;
    
    Vector3f(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
    
    Vector3f operator+(Vector3f inpVector) {
        return Vector3f(x+inpVector.x, y+inpVector.y, z+inpVector.z);
    }
    
    Vector3f operator-(Vector3f inpVector) {
        return Vector3f(x-inpVector.x, y-inpVector.y, z-inpVector.z);
    }
    
    Vector3f operator*(float scalar) {
        return Vector3f(x*scalar, y*scalar, z*scalar);
    }
    
    Vector3f cross(Vector3f inpVector) {
        return Vector3f(y*inpVector.z-z*inpVector.y, z*inpVector.x-x*inpVector.z, x*inpVector.y-y*inpVector.x);
    }
    
    void normalize() {
        float length = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
        x /= length;
        y /= length;
        z /= length;
    }
};

// Initiate ymax that is used for the texture coordinates
float ymax = 0;

// Class for vertices
class Vertex {
public:
    Vector3f pos = Vector3f(0,0,0); // The vertex position
    Vector3f normal = Vector3f(0,0,0); // The vertex normal
    Vector2f texcoord = Vector2f(0,0); // The vertex texture coordinates
    
    Vertex(float x, float y, float z) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    
    // Method for calculating the cylindrical texture coordinates, using the same calculation as given in the MP instructions.
    void calcTexcoord() {
        float theta = atan2(pos.z,pos.x);
        texcoord.s = (theta + M_PI)/(2*M_PI);
        texcoord.t = pos.y/ymax;
    }
};

// Vector array for storing the vertices
std::vector<Vertex> vertices;

// Class for faces
class Face {
public:
    int v1, v2, v3; // Storing the index of the three vertices that forms the face
    
    Face(int _v1, int _v2, int _v3) {
        v1 = _v1;
        v2 = _v2;
        v3 = _v3;
    }
    
    // Method for calculating the face normal
    void calcNormal() {
        
        // Find two vectors that forms the triangle
        Vector3f vector1 = vertices[v3].pos-vertices[v1].pos;
        Vector3f vector2 = vertices[v2].pos-vertices[v1].pos;
        
        // Find the face normal by taking the cross product of these two vectors.
        Vector3f normal = vector2.cross(vector1);
        
        // Add the face normal to the normal of the faces three vertices
        vertices[v1].normal = vertices[v1].normal+normal;
        vertices[v2].normal = vertices[v2].normal+normal;
        vertices[v3].normal = vertices[v3].normal+normal;
    }
};

// Vector array for storing the faces
std::vector<Face> faces;

// Function for loading .obj-files
bool loadObj() {
    FILE * file = fopen("cs418_mp3_kasperi2/teapot_0.obj", "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }
    
    // Read each line of the .obj-file
    while(1){
        char lineHeader[128];

        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // If it is a vertex, create a Vertex and push it to vertices array
        if (strcmp(lineHeader, "v") == 0) {
            float x, y, z;
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            if(y > ymax)
                ymax = y;
            Vertex vertex = Vertex(x,y,z);
            vertices.push_back(vertex);
        }
        
        // If it is a face, create a Face and push it to faces array
        else if (strcmp(lineHeader, "f") == 0) {
            int v1, v2, v3;
            fscanf(file, "%i %i %i\n", &v1, &v2, &v3);
            Face face = Face(v1-1,v2-1,v3-1);
            faces.push_back(face);
        }
    }
    return true;
}

// Function for looping over the vertices and calculate the texture coordinates for everyone.
void calcTexcoords() {
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].calcTexcoord();
    }
}

// Function for looping over the faces and calculating the face normal. Then looping over the vertices and normalize their normal
void calcNormals() {
    for(int i = 0; i < faces.size(); i++) {
        faces[i].calcNormal();
    }
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].normal.normalize();
    }
}

// Initate some vars
int textures[2]; // array for storing the textures
const char * textureBMP = "cs418_mp3_kasperi2/checker.bmp"; // the path to the img used for texture mapping
const char * environmentBMP = "cs418_mp3_kasperi2/kitchen_probe.hdr"; // the path to the img used for environment mapping

bool spinobj = true; // should the object rotate or not
float angle = 0.0; // angle for rotation of the object around the Y axis
float rotateX = 0.0; // angle for rotation of the object around the X axis
float rotateZ = 0.0; // angle for rotation of the object around the Z axis

// Function for loading the images for the two textures.
void loadTextures() {
    textures[0] = SOIL_load_OGL_texture(
        textureBMP,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
    );
    
    textures[1] = SOIL_load_OGL_texture(
        environmentBMP,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
    );
}

// Setup the texture and environment mapping
void setupTextures() {
    
    // If the texture for texture mapping exists, enable texture mapping
    if(textures[0] != 0) {
        glActiveTextureARB (GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }
    
    // If the texture for environment mapping exists, enable environment mapping
    if(textures[1] != 0) {
        glActiveTextureARB (GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
}

// Init function that calls various of the function above, for example loading the object
void init()
{
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    
    // Set background color to grey
    glClearColor (0.2, 0.2, 0.2, 0.2);

    // Load the object and calculate the normals and texture coordinates
    loadObj();
    calcTexcoords();
    calcNormals();
    
    // Load and setup the textures
    loadTextures();
    setupTextures();
    
    // Vars used for Phong lighting and material
    GLfloat white[] = {1.0,1.0,1.0,1.0};
    GLfloat whiteamb[] = {0.2,0.2,0.2,0.0};
    GLfloat whitediff[] = {0.8,0.8,0.8,0.0};
    GLfloat lpos[] = {5.0,7.0,1.0,1.0};
    
    // Enable Phong lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
    // Set the default material of the object to white
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whiteamb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whitediff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
}

void display(void)
{
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(rotateX,1.0,0.0,0.0);
    glRotatef(rotateZ,0.0,0.0,1.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    
    // Draw the object
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < faces.size(); i++) {
        // Vertex no 1 of the face
        Vertex v1 = vertices[faces[i].v1];
        glMultiTexCoord2f(GL_TEXTURE0_ARB, v1.texcoord.s, v1.texcoord.t);
        glNormal3f(v1.normal.x, v1.normal.y, v1.normal.z);
        glVertex3f(v1.pos.x, v1.pos.y, v1.pos.z);
        
        // Vertex no 2 of the face
        Vertex v2 = vertices[faces[i].v2];
        glMultiTexCoord2f(GL_TEXTURE0_ARB, v2.texcoord.s, v2.texcoord.t);
        glNormal3f(v2.normal.x, v2.normal.y, v2.normal.z);
        glVertex3f(v2.pos.x, v2.pos.y, v2.pos.z);
        
        // Vertex no 3 of the face
        Vertex v3 = vertices[faces[i].v3];
        glMultiTexCoord2f(GL_TEXTURE0_ARB, v3.texcoord.s, v3.texcoord.t);
        glNormal3f(v3.normal.x, v3.normal.y, v3.normal.z);
        glVertex3f(v3.pos.x, v3.pos.y, v3.pos.z);
    }
    glEnd();
    
    glFlush();
    glutSwapBuffers();
    
    if (spinobj)
        angle += 1.0;
    
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5.0, 5.0, -5.0, 5.0, 1, 10.0); // Enable perspective correction
    glMatrixMode (GL_MODELVIEW);
}

// Listens to some key presses for selection of texture images, enabling/disabling textures, enabling/disabling rotation of the obj and exit.
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case '1':
            textureBMP = "cs418_mp3_kasperi2/checker.bmp";
            loadTextures();
            setupTextures();
            break;
        case '2':
            textureBMP = "cs418_mp3_kasperi2/dots.bmp";
            loadTextures();
            setupTextures();
            break;
        case '3':
            textureBMP = "cs418_mp3_kasperi2/rodzynka.bmp";
            loadTextures();
            setupTextures();
            break;
        case '4':
            textureBMP = "cs418_mp3_kasperi2/earth.bmp";
            loadTextures();
            setupTextures();
            break;
        case '5':
            textureBMP = "";
            glActiveTextureARB (GL_TEXTURE0_ARB);
            glDisable(GL_TEXTURE_2D);
            break;
        case 'q':
            environmentBMP = "cs418_mp3_kasperi2/kitchen_probe.hdr";
            loadTextures();
            setupTextures();
            break;
        case 'w':
            environmentBMP = "cs418_mp3_kasperi2/rnl_probe.hdr";
            loadTextures();
            setupTextures();
            break;
        case 'e':
            environmentBMP = "cs418_mp3_kasperi2/stpeters_probe.hdr";
            loadTextures();
            setupTextures();
            break;
        case 'r':
            environmentBMP = "";
            glActiveTextureARB (GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
            break;
        case 's':
            spinobj == true ? spinobj = false : spinobj = true;
            break;
        case 27:
            exit(0);
            break;
    }
}

// Listen on the arrow keys for rotation of the object around the X and Z axis
void specialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            rotateX -= 10;
            break;
        case GLUT_KEY_DOWN:
            rotateX += 10;
            break;
        case GLUT_KEY_RIGHT:
            rotateZ -= 10;
            break;
        case GLUT_KEY_LEFT:
            rotateZ += 10;
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
    init();
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutMainLoop();
    return 0;
}

//
//  main.cpp
//  CS418 kasperi2 MP1
//
//  Created by Johan Kasperi on 06/02/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>

// vars for animation
float fAnimSpeed = 0.f;
int dipMode = 1;
int rotateMode = 0;
int translateMode = 0;
int danceMode = 0;

// vars for fps
char fpsArr[10];
int frame=0;
int tme;
int timebase=0;


void init(void)
{
    // init your data, setup OpenGL environment here
    glClearColor(1.0,1.0,1.0,1.0); // clear color is white
    glPointSize(4.0);
}

void renderText(float x, float y,const char *string){
    const char *c;
    glColor3d(1.0,0.0,0.0);
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

float dance(float c, float amp, float offset) {
    return danceMode == 1 ? c+amp*sin(3.14*(fAnimSpeed+offset)) : c;
}

void display(void)
{
    if(dipMode==1)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    
    // put your OpenGL display commands here
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // reset OpenGL transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // reset transformation matrix to identity
    
    // setup look at transformation so that
    // eye is at : (0,0,3)
    // look at center is at : (0,0,0)
    // up direction is +y axis
    gluLookAt(0.f,0.f,3.f,0.f,0.f,0.f,0.f,1.f,0.f);
    
    // colors for the letter
    float color1[] = {1,.9,0};
    float color2[] = {1,.8,0};
    float color3[] = {1,.4,0};
    float color4[] = {1,.3,0};
    
    // vertices
    float v1[] = {dance(-0.6, 0.06, 1.0), dance(1, 0.05, 0.05)};
    float v2[] = {dance(-0.6, 0.1, 0.6), dance(0.6, 0.1, 0.01)};
    float v3[] = {dance(-0.2, 0.05, 0.1), dance(0.6, 0.08, -0.1)};
    float v4[] = {dance(-0.2, 0.04, 0.6), dance(-0.6, -0.03, 0)};
    float v5[] = {dance(-0.6, 0.09, 0.2), dance(-0.6, 0.04, -0.3)};
    float v6[] = {dance(-0.6, 0.08,-1.0), dance(-1.0, 0.07, 0.5)};
    float v7[] = {dance(0.6, 0.02,-0.5), dance(-1.0, 0.03, 0.2)};
    float v8[] = {dance(0.6, -0.03, 0.09), dance(-0.6, -0.05, 0.05)};
    float v9[] = {dance(0.2, -0.01, 0.8), dance(-0.6, -0.03, 0)};
    float v10[] = {dance(0.2, -0.02, 0.7), dance(0.6, -0.05, 0.6)};
    float v11[] = {dance(0.6, -0.05, 0.8), dance(0.6, 0.03, 0.4)};
    float v12[] = {dance(0.6, -0.02, 0.5), dance(1.0, -0.02, 0.1)};
    
    
    glPushMatrix();
    // walk if 'w' key is pressed
    if(translateMode == 1) {
        glTranslatef(0.2*sin(3.14*(fAnimSpeed-0.3)), 0, 0);
    }
    
    // rotate if 'r' key is pressed
    if(rotateMode == 1) {
        glRotatef(10*sin(3.14*(fAnimSpeed+0.5)),0.f,0.f,1.f);
    }
    
    // triangles
    glBegin(GL_TRIANGLE_FAN);
        glColor3fv(color2);
        glVertex2fv(v3);
        glVertex2fv(v2);
        
        glColor3fv(color1);
        glVertex2fv(v1);
        glVertex2fv(v12);
        
        glColor3fv(color2);
        glVertex2fv(v10);
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        glColor3fv(color2);
        glVertex2fv(v10);
        glVertex2fv(v11);
        
        glColor3fv(color1);
        glVertex2fv(v12);
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        glColor3fv(color3);
        glVertex2fv(v9);
        
        glColor3fv(color2);
        glVertex2fv(v10);
        glVertex2fv(v3);
        
        glColor3fv(color3);
        glVertex2fv(v4);
        
        glColor3fv(color4);
        glVertex2fv(v6);
        glVertex2fv(v7);
        
        glColor3fv(color3);
        glVertex2fv(v8);
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        glColor3fv(color3);
        glVertex2fv(v5);
        
        glColor3fv(color4);
        glVertex2fv(v6);
        
        glColor3fv(color3);
        glVertex2fv(v4);
    glEnd();
    
    // outline
    glBegin(GL_LINE_LOOP);
        glColor3f(0,0,1.f);
        glVertex2fv(v1);
        glVertex2fv(v12);
        glVertex2fv(v11);
        glVertex2fv(v10);
        glVertex2fv(v9);
        glVertex2fv(v8);
        glVertex2fv(v7);
        glVertex2fv(v6);
        glVertex2fv(v5);
        glVertex2fv(v4);
        glVertex2fv(v3);
        glVertex2fv(v2);
    glEnd();
    glPopMatrix();
    
    renderText(-2,2,"Keyboard controls:");
    renderText(-2,1.9,"h = dipmode");
    renderText(-2,1.8,"r = rotate");
    renderText(-2,1.7,"w = walk");
    renderText(-2,1.6,"d = dance");
    renderText(-2,1.5, "esc = exit");
    
    frame++;
    tme=glutGet(GLUT_ELAPSED_TIME);
    if (tme - timebase > 1000) {
        float fps = frame*1000.0/(tme-timebase);
        timebase = tme;
        frame = 0;
        sprintf(fpsArr, "%f", fps);
    }
    renderText(-2,1.3,"FPS:");
    renderText(-2,1.2,fpsArr);
    
    glFlush();
    glutSwapBuffers();	// swap front/back framebuffer to avoid flickering
    
}

void reshape (int w, int h)
{
    // reset viewport ( drawing screen ) size
    glViewport(0, 0, w, h);
    float fAspect = ((float)w)/h;
    // reset OpenGL projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.f,fAspect,0.001f,30.f);
}

void keyboard(unsigned char key, int x, int y)
{
    // put your keyboard control here
    if(key == 27) {
        // ESC hit, so quit
        printf("demonstration finished.\n");
        exit(0);
    }
    if(key == 'h') {
        dipMode = 1-dipMode;
    }
    if(key == 'r') {
        rotateMode = 1-rotateMode;
    }
    if(key == 'd') {
        danceMode = 1-danceMode;
    }
    if(key == 'w') {
        translateMode = 1-translateMode;
    }
}

void timer(int v)
{
    // speed for animation
    fAnimSpeed += 0.08;
    
    glutPostRedisplay(); // trigger display function by sending redraw into message queue
    glutTimerFunc(30,timer,0); // restart timer again
}

int main(int argc, char* argv[])
{
    glutInit(&argc, (char**)argv);
    // set up for double-buffering & RGB color buffer & depth test
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (600, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ((const char*)"CS418 MP1 kasperi2");
    
    init(); // setting up user data & OpenGL environment
    
    // set up the call-back functions
    glutDisplayFunc(display);  // called when drawing
    glutReshapeFunc(reshape);  // called when change window size
    glutKeyboardFunc(keyboard); // called when received keyboard interaction
    glutTimerFunc(30,timer,0); // a periodic timer. Usually used for updating animation
    
    glutMainLoop(); // start the main message-callback loop
    
    return 0;
}

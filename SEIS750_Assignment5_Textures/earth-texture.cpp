/*
 *Skeleton lighting program
 *COS490
 *Fall 2011
 **/

#include <GL/Angel.h>
#include <math.h>
#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846

GLuint program1, program2, program3;

GLuint vao[1];
GLuint vbo[2];
int spherevertcount;
int mode = 0;
bool multisample = false;

int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;



//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal;
GLuint light_position;
GLuint light_color;
GLuint ambient_light;


vec4* sphere_verts;
vec3* sphere_normals;

void reshape(int width, int height){
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(45.0, (float)width/(float)height, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}


//In this particular case, our normal vectors and vertex vectors are identical since the sphere is centered at the origin
//For most objects this won't be the case, so I'm treating them as separate values for that reason
//This could also be done as separate triangle strips, but I've chosen to make them just triangles so I don't have to execute multiple glDrawArrays() commands
int generateSphere(float radius, int subdiv){
	float step = (360.0/subdiv)*(M_PI/180.0);

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(sphere_normals){
		delete[] sphere_normals;
	}
	sphere_normals = new vec3[totalverts];
	if(sphere_verts){
		delete[] sphere_verts;
	}
	sphere_verts = new vec4[totalverts];

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
	
			sphere_normals[k]= vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			k++;
			
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			//triangle 2
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
		}
	}
	return totalverts;
}

void display(void)
{
	if (multisample) glEnable(GL_MULTISAMPLE);
	else glDisable(GL_MULTISAMPLE);
  /*clear all pixels*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mv = LookAt(vec4(0, 0, 10+z_distance, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));

	mv = mv * RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	

	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.5, 0, 0, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1.0f,1.0f,1.0f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 10.0);
	glUniform4fv(light_position, 1, mv*vec4(50, 50, 50, 1));
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.5, .5, .5, 5));

	if(mode == 0){
		glBindVertexArray( vao[0] );
		glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere 
	}else{
		glBindVertexArray(0);
		glutSolidTeapot(2); //not very bandwidth efficient
	}
    
    glFlush();
  /*start processing buffered OpenGL routines*/
  glutSwapBuffers();
}

void setupShader(GLuint prog){
	
	glUseProgram( prog );
	//glLinkProgram( prog);
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");
	
	vAmbientDiffuseColor = glGetAttribLocation(prog, "vAmbientDiffuseColor");
	vSpecularColor = glGetAttribLocation(prog, "vSpecularColor");
	vSpecularExponent = glGetAttribLocation(prog, "vSpecularExponent");
	light_position = glGetUniformLocation(prog, "light_position");
	light_color = glGetUniformLocation(prog, "light_color");
	ambient_light = glGetUniformLocation(prog, "ambient_light");

	glBindVertexArray( vao[0] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
}


void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'g'){
		setupShader(program1);
	}
	if (key == 'p'){
		setupShader(program2);
	}
	if (key == 'c'){
		setupShader(program3);
	}
	if (key == 's'){
		mode = 0;
	}
	if (key == 't'){
		mode = 1;
	}
	if (key == 'm'){
		multisample = multisample ? multisample=false : multisample=true;
		printf("multisample: %d\n", multisample);
	}
	reshape(ww,wh);
	glutPostRedisplay();

}

void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {
		thetaY = 360.0 *(x-prevMouseX)/ww;    
		thetaX = 360.0 *(prevMouseY - y)/wh;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;
	}
	else if (right_button_down) {
		z_distance = 5.0*(prevMouseY-y)/wh;
	}
  glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
	  prevMouseX= x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP) {
      left_button_down = FALSE;
	  right_button_down = FALSE;
	}
}

void init() {

  /*select clearing (background) color*/
  glClearColor(1.0, 1.0, 1.0, 1.0);


  //populate our arrays
  spherevertcount = generateSphere(2, 30);


   // Load shaders and use the resulting shader program
    program1 = InitShader( "vshader-lighting.glsl", "fshader-lighting.glsl" );
	program2 = InitShader( "vshader-phongshading.glsl", "fshader-phongshading.glsl" );
	program3 = InitShader( "vshader-celshading.glsl", "fshader-celshading.glsl" );
    glUseProgram(0 );

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	setupShader(program3);

  //Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutCreateWindow("Lighting Exercise");  

  glewExperimental = GL_TRUE;

	glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutReshapeFunc(reshape);
  //glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(mouse_dragged);

  glutMainLoop();
  return 0;
}
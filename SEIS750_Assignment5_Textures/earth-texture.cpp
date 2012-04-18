/*
 * Mark Norgren
 * Assignment 5
 * Earth Textures
 *
 **/

#include <GL/Angel.h>
#include <math.h>
#include <GL/il.h>
#pragma comment(lib, "glew32.lib")
//We have additional libraries to link for texturing
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")

//store window width and height
int ww=800, wh=800;

#define M_PI 3.14159265358979323846

GLuint program, program1, program2, program3;

// Texture Files
static GLuint texName[3];

GLuint vao[1];
GLuint vbo[3];

//pointers to our shader variables
GLuint model_view;
GLuint projection;

GLuint vPosition;
GLuint vTexCoord;
GLuint texCoord;
GLuint texMap;
GLuint specMap;

GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal;
GLuint light_position;
GLuint light_color;
GLuint ambient_light;


int multiflag = 0;

//Modified slightly from the OpenIL tutorials
ILuint loadTexFile(const char* filename){
	
	ILboolean success;
	/* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  success = ilLoadImage(filename); /* Loading of image from file */
	if (success){ /* If no error occured: */
		//We need to figure out whether we have an alpha channel or not
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every color component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}

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
double globe_revolution = 0.0;
double z_distance;



//our modelview and perspective matrices
mat4 mv, p;




vec4* sphere_verts;
vec3* sphere_normals;
vec2* texcoords;

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
	texcoords = new vec2[totalverts];

	int k = 0;
	float tex=0;
	float texCoordX = 0;
	float texCoordY = 0;
	int texCoordCount = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		texCoordY = i/(M_PI/2);
		for(float j = -M_PI; j<=M_PI; j+=step){
			texCoordX = j/M_PI;
			//triangle 1
			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			//						s							t
			texcoords[k] = vec2( (j+M_PI)/(2*M_PI), ( (i+(M_PI/2)) / (M_PI) ) );
			k++;
	
			sphere_normals[k]= vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			texcoords[k] = vec2( (j+M_PI)/(2*M_PI), ( ((i+step)+(M_PI/2)) / (M_PI) ) );
			k++;
			
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			texcoords[k] = vec2( ((j+step) + M_PI)/(2*M_PI), ( ((i+step)+(M_PI/2)) / (M_PI) ) );
			k++;

			//triangle 2
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			texcoords[k] = vec2( ((j+step) + M_PI)/(2*M_PI), ( ((i+step)+(M_PI/2)) / (M_PI) ) );
			k++;

			sphere_normals[k]= vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			texcoords[k] = vec2( ((j+step) + M_PI)/(2*M_PI), ( ((i)+(M_PI/2)) / (M_PI) ) );
			k++;

			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			texcoords[k] = vec2( ((j) + M_PI)/(2*M_PI), ( ((i)+(M_PI/2)) / (M_PI) ) );
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
	
	
	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.5, 0, 0, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1.0f,1.0f,1.0f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 10.0);
	glUniform4fv(light_position, 1, mv*vec4(90, 90, 90, 1));
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.2, .2, .2, 5));

	mv = mv * RotateZ(globe_revolution);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	if(mode == 0){
		
		glBindVertexArray( vao[0] );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texName[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texName[1]);
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

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	vTexCoord = glGetAttribLocation(prog, "texCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
  glEnable(GL_DEPTH_TEST);


  //populate our arrays
  spherevertcount = generateSphere(3, 40);


  //vec2 texcoords[spherevertcount];

   // Load shaders and use the resulting shader program

	program = InitShader( "vshader-texture.glsl", "fshader-texture.glsl" );
    program1 = InitShader( "vshader-lighting.glsl", "fshader-lighting.glsl" );
	program2 = InitShader( "vshader-phongshading.glsl", "fshader-phongshading.glsl" );
	program3 = InitShader( "vshader-celshading.glsl", "fshader-celshading.glsl" );
    glUseProgram(program );

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 3, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	

	//and now our normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	/* TEXTURE SETUP */
	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec2), texcoords, GL_STATIC_DRAW);

	ILuint ilTexID[3]; /* ILuint is a 32bit unsigned integer.
    //Variable texid will be used to store image name. */

	ilInit(); /* Initialization of OpenIL */
	ilGenImages(3, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(3, texName); //and we eventually want the data in an OpenGL texture

	ilBindImage(ilTexID[0]); /* Binding of IL image name */
	loadTexFile("images/Earth.png");
	glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

   
   //Note how we depend on OpenIL to supply information about the file we just loaded in
   glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   glGenerateMipmap(GL_TEXTURE_2D);

   
   
	//Now repeat the process for the second image
	ilBindImage(ilTexID[1]);
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	loadTexFile("images/EarthSpec.png");
	
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   glGenerateMipmap(GL_TEXTURE_2D);


	//And the third image
	/*
	ilBindImage(ilTexID[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	loadTexFile("images/opengl.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	*/
    ilDeleteImages(3, ilTexID); //we're done with OpenIL, so free up the memory

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setupShader(program);

  //Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	
	texMap = glGetUniformLocation(program, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0

	specMap = glGetUniformLocation(program, "specMapTexture");
	glUniform1i(specMap, 1); //assign this one to texture unit 1


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	texCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void my_timer (int v)
	{
		globe_revolution+=0.3;
		glutPostRedisplay();
		glutTimerFunc(1000/v, my_timer, v);
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
  glutTimerFunc(500,my_timer,60);
  glutMainLoop();
  return 0;
}
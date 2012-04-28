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
GLuint cloudsShader;

bool showClouds = true;

// Texture Files
enum textureNames {
	EARTH_TEXTURE,
	EARTH_SPEC_TEXTURE,
	EARTH_NIGHT_TEXTURE,
	CLOUDS_TEXTURE,
	NUMBER_OF_TEXTURES
};

static GLuint texName[NUMBER_OF_TEXTURES];
void setupCloudShader(GLuint prog);


enum vaoEnum {
	EARTH_VAO,
	CLOUDS_VAO,
	NUMBER_OF_VAOS
};
GLuint vao[NUMBER_OF_VAOS];

enum vboEnum {
	GLOBE_VERTICIES,
	GLOBE_NORMALS,
	GLOBE_TEXTURE_COORDS,
	CLOUD_VERTICIES,
	CLOUD_NORMALS,
	CLOUD_TEXTURE_COORDS,
	NUMBER_OF_VBOS
};
GLuint vbo[NUMBER_OF_VBOS];

//pointers to our shader variables
GLuint model_view;
GLuint projection;

GLuint vPosition;
GLuint vTexCoord;
GLuint texCoord;
GLuint texMap;
GLuint specMap;
GLuint nightMap;
GLuint cloudsTexture;

GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal;
GLuint light_position;
GLuint light_color;
GLuint ambient_light;

int multiflag = 0;

void setupShader(GLuint prog);

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
int cloudSphereCount;
int mode = 0;
bool multisample = false;

int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double clouds_view_rotx = 0.0;
double clouds_view_roty = 0.0;
double clouds_view_rotz = 0.0;
double globe_revolution = 0.0;
double cloud_revolution = 0.0;
double z_distance;

//our modelview and perspective matrices
mat4 mv, p;
mat4 mvClouds;

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
	glUniform4fv(light_position, 1, mv*vec4(90, 290, 90, 1));
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.7, .7, .7, 5));
	mvClouds = mv;
	p = Perspective(45.0, (float)ww/(float)wh, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	//earth rotation
	vec4 lightPos_mv = mv*vec4(90, 290, 90, 1);
	mv = mv * RotateZ(globe_revolution);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);


	setupShader(program);
	p = Perspective(45.0, (float)ww/(float)wh, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.5, 0, 0, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1.0f,1.0f,1.0f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 10.0);

	glUniform4fv(light_position, 1, lightPos_mv);
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.7, .7, .7, 5));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texName[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texName[CLOUDS_TEXTURE]);

	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere 

	if(showClouds)
	{
		setupCloudShader(cloudsShader);
		mv = mvClouds;
		mv = mv * RotateZ(cloud_revolution);
		p = Perspective(45.0, (float)ww/(float)wh, 1.0, 100.0);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

		glUniform4fv(light_position, 1, lightPos_mv);
		glUniform4fv(light_color, 1, vec4(1,1,1,1));
		glUniform4fv(ambient_light, 1, vec4(.7, .7, .7, 5));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texName[CLOUDS_TEXTURE]);

		glBindVertexArray( vao[CLOUDS_VAO] );
		glDrawArrays( GL_TRIANGLES, 0, cloudSphereCount );    // draw the sphere 
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

void setupCloudShader(GLuint prog){

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

	glBindVertexArray( vao[CLOUDS_VAO] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_VERTICIES] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_NORMALS] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_TEXTURE_COORDS] );
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
		showClouds ? showClouds=false : showClouds=true;
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
	glEnable(GL_CULL_FACE);
	/*select clearing (background) color*/
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	//vec2 texcoords[spherevertcount];

	// Load shaders and use the resulting shader program

	program = InitShader( "vshader-texture.glsl", "fshader-texture.glsl" );
	cloudsShader = InitShader( "vshader-texture.glsl", "fshader-clouds.glsl");
	glUseProgram(program );

	//populate our arrays
	spherevertcount = generateSphere(3, 80);

	// Create a vertex array object
	glGenVertexArrays( NUMBER_OF_VAOS, vao );

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

	/* CLOUDS SPHERE */
	//populate our arrays
	cloudSphereCount = generateSphere(3.02, 80);

	// Create and initialize any buffer objects
	glBindVertexArray( vao[CLOUDS_VAO] );
	glGenBuffers( 3, &vbo[CLOUD_VERTICIES] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_VERTICIES] );
	glBufferData( GL_ARRAY_BUFFER, cloudSphereCount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);

	//and now our normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, cloudSphereCount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	/* TEXTURE SETUP */
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_TEXTURE_COORDS] );
	glBufferData( GL_ARRAY_BUFFER, cloudSphereCount*sizeof(vec2), texcoords, GL_STATIC_DRAW);

	ILuint ilTexID[NUMBER_OF_TEXTURES]; /* ILuint is a 32bit unsigned integer.
										//Variable texid will be used to store image name. */

	ilInit(); /* Initialization of OpenIL */
	ilGenImages(NUMBER_OF_TEXTURES, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(NUMBER_OF_TEXTURES, texName); //and we eventually want the data in an OpenGL texture

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
	ilBindImage(ilTexID[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	loadTexFile("images/EarthNight.png");



	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	ilDeleteImages(3, ilTexID); //we're done with OpenIL, so free up the memory

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//And the fourth image - CLOUDS
	ilBindImage(ilTexID[CLOUDS_TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, texName[CLOUDS_TEXTURE]);
	loadTexFile("images/earthcloudmap.png");



	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	ilDeleteImages(NUMBER_OF_TEXTURES, ilTexID); //we're done with OpenIL, so free up the memory

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	//Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	texMap = glGetUniformLocation(program, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0

	specMap = glGetUniformLocation(program, "specMapTexture");
	glUniform1i(specMap, 1); //assign this one to texture unit 1

	nightMap = glGetUniformLocation(program, "nightMapTexture");
	glUniform1i(nightMap, 2); // assign this one to texture unit 2

	cloudsTexture = glGetUniformLocation(program, "cloudsTexture");
	glUniform1i(cloudsTexture, 3); // assign this one to texture unit 2


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	texCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glUseProgram(cloudsShader);
	cloudsTexture = glGetUniformLocation(cloudsShader, "cloudsTexture");
	glUniform1i(cloudsTexture, 3); // assign this one to texture unit 2

	setupShader(program);

}

void my_timer (int v)
{
	globe_revolution-=0.15;
	cloud_revolution-=0.08;
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
	glutCreateWindow("Assignment 5 - Earth Texturing");  

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
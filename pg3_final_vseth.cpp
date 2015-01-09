 #include "stdafx.h"
#include <cstdio>
#include <iostream>
#include <gl\glut.h>
#include <glm\glm.hpp>
#include "glm.h"
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
//Method declarations
int parseObject();
int setWindow();
int setLightSource();
void initBarrier();
void initBarrierSecLevel();
void initAlien();
void initAlienSecLevel();
void initGame();
typedef struct windowSize {
	int width;
	int height;
} displayWindow;
typedef struct lightSource {
	glm::vec3 pos;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	std::string light_name;
} lightSRC;
typedef struct alienStruct {
	float x;
	float y = 0.0;
	float z;
	bool active = true;
	bool mov_rt;
} ALIEN;
typedef struct cannonStruct {
	float x = -.7;
	float y = 0.0;
	float z = 1.0;
	bool active = true;
} CANNON;
typedef struct bulletShotStruct {
	float x;
	float y = 0.0 ;
	float z;
	bool active = true;
} BULLETSHOT;
typedef struct barrierStruct {
	float x;
	float y =0.0;
	float z;
	bool active = true;
} BARRIER;
typedef struct UFOStruct {
	float x = -2.5;
	float y = 0.0;
	float z = -2.0;
	bool active = true;
} UFO;
using namespace std;
unsigned int width, height;
GLMmodel* alien_model = NULL;
GLMmodel* cannon_model = NULL;
GLMmodel* ufo_model = NULL;
glm::vec3 eye(0, 1, 2);
glm::vec3 lookat(0, 0, 0);
glm::vec3 viewup(0, .5, 0);
displayWindow window;
std::vector< lightSRC> light_source_list;
std::vector< BULLETSHOT> cannon_shot_list;
std::vector< BULLETSHOT> alien_shot_list;
std::vector< BARRIER> barrier_list;
std::vector< ALIEN> alien_list;
UFO ufo;
CANNON cannon;
float alien_horz = 0.0;
float alien_vert = 0.0;
int score = 0;
int curr_life = 3;
int curr_level = 1;
bool game_over = false;
bool game_won = false;
bool level_1_complete = false;
bool level_2_complete = false;
bool died = false;
unsigned char* data;
static GLuint texName;
time_t game_start_time;
bool ufo_show = false;
float ufo_move = 0.1;
int move_rt = 1;
float speed = 0.05;
time_t alien_time;
void initGame(){
	barrier_list.clear();
	alien_list.clear();
	cannon_shot_list.clear();
	alien_shot_list.clear();
	if (curr_level == 1){
		initBarrier();
		//initAlien();
		initAlien();
	}
	if (curr_level == 2){
		//initBarrierSecLevel();
		initAlienSecLevel();
	}
	game_start_time = time(NULL);
	alien_time = time(NULL);
	ufo.x = -2.5;
	ufo.active = true;
	cannon.x = 0.0;
	cannon.active = true;
	alien_horz = 0.0;
	alien_vert = 0.0;
	game_over = false;
	game_won = false;
	if (level_1_complete){
		curr_life = 3;
		level_1_complete = false;
	}
	score = 0;
	ufo_show = false;
	died = false;
	ufo_move = 0.1;
	move_rt = 1;
	speed = 0.05;
}

int loadBMP(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height)
{

	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Open the file in binary mode
	FILE* fp = fopen(path, "rb");
	if (!fp)
	{
		cout << "Image could not be opened\n";
		return 0;
	}

	if (fread(header, 1, 54, fp) != 54)
	{
		// If not 54 bytes read : problem
		cout << "Not a correct BMP file\n";
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		cout << "Not a correct BMP file\n";
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = (width)*(height)* 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done 

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	if (fread(data, 1, imageSize, fp) != imageSize)
	{
		fclose(fp);
		return 0;
	}

	//Everything is in memory now, the file can be closed
	fclose(fp);
	return 1;
}
void loadTexture(char* Filename) {

	glGenTextures(1, &texName);//generate
	glBindTexture(GL_TEXTURE_2D, texName);//bind to a 2-D texture

	//wrap the image along s coordinate (width)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//wrap the image along t coordinate (height)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//how to treat magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//how to treat de-magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//bmp stores BGR instead of RGB
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
}
void initBackgroundTexture() {
	if (!loadBMP("inputs/galaxy.bmp", data, width, height))
		cout << "\nError loading bmp\n";
	else
		cout << "\nLoaded bmp successfully!";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 0.0,-4);
	glTexCoord2d(1.0, 0.0); glVertex3d(1024.0, 0.0,-4);
	glTexCoord2d(1.0, 1.0); glVertex3d(1024.0, 512.0,-4);
	glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 512.0,-4);
	glEnd();

glDisable(GL_TEXTURE_2D);

}
void init()
{
	parseObject();
	initBarrier();
	initAlien();
	game_start_time = time(NULL);
	alien_time = time(NULL);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	

}
void setMaterial(GLfloat ambientR, GLfloat ambientG, GLfloat ambientB,
	GLfloat diffuseR, GLfloat diffuseG, GLfloat diffuseB,
	GLfloat specularR, GLfloat specularG, GLfloat specularB,
	GLfloat shininess) {

	GLfloat ambient[] = { ambientR, ambientG, ambientB, 1 };
	GLfloat diffuse[] = { diffuseR, diffuseG, diffuseB, 1 };
	GLfloat specular[] = { specularR, specularG, specularB, 1 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
void render(GLMmodel *model){
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0);
	GLMgroup *grp = model->groups;
	GLMmaterial *materials = model->materials;
	GLMtriangle *triangles = model->triangles;
	GLfloat* vertices = model->vertices;
	while (grp != NULL){

		GLuint num_tri = grp->numtriangles;
		GLuint *grp_tri = grp->triangles;
		GLuint mat_idx = grp->material;
		GLMmaterial mat = *(materials + mat_idx);
		GLfloat *am = mat.ambient;
		GLfloat *dif = mat.diffuse;
		GLfloat *spec = mat.specular;
		GLfloat sh = mat.shininess;
		GLfloat kar = *am;
		GLfloat kag = *(am + 1);
		GLfloat kab = *(am + 2);
		GLfloat kdr = *(dif);
		GLfloat kdg = *(dif + 1);
		GLfloat kdb = *(dif + 2);
		GLfloat ksr = *spec;
		GLfloat ksg = *(spec + 1);
		GLfloat ksb = *(spec + 2);
		GLfloat shine = sh;
		setMaterial(kar, kag, kab, kdr, kdg, kdb, ksr, ksg, ksb, shine);
		int i = 0;

		glBegin(GL_TRIANGLES);
		for (i = 0; i < num_tri; i++){
			int idx = *(grp_tri + i);
			GLMtriangle *tri = (triangles + idx);
			glNormal3fv(&model->normals[3 * tri->nindices[0]]);
			glVertex3fv(&model->vertices[3 * tri->vindices[0]]);
			glNormal3fv(&model->normals[3 * tri->nindices[1]]);
			glVertex3fv(&model->vertices[3 * tri->vindices[1]]);
			glNormal3fv(&model->normals[3 * tri->nindices[2]]);
			glVertex3fv(&model->vertices[3 * tri->vindices[2]]);
		}
		glEnd();
		grp = grp->next;
	}
}
void initAlien(){
	int row = 0;
	int col = 0;
	for (row = 0; row < 3; row++){
		for (col = 0; col < 5; col++){
			ALIEN al;
			al.x = -2 + col*0.3;
			al.y = 0;
			al.z = -3 + 0.5 *row;
			alien_list.push_back(al);
		}
	}
}
void drawAliensSecLevel(){
	float max_al_x_rt;
	float min_al_x_rt;
	float max_al_x_lt;
	float min_al_x_lt;
	float max_al_z;
	float min_al_z;
	if (alien_list.size() != 0){
		std::vector< ALIEN> new_alien_list;
		int i = 0;
		max_al_x_rt = -1000;
		min_al_x_rt = 1000;
		max_al_x_lt = -1000;
		min_al_x_lt = 1000;
		max_al_z = alien_list[0].z;
		min_al_z = alien_list[0].z;
		for (i = 0; i < alien_list.size(); i++){
			ALIEN al = alien_list[i];
			if (al.active){
				if (al.mov_rt)
					al.x = al.x + alien_horz;
				else 
					al.x = al.x - alien_horz;
				al.y = al.y;
				al.z = al.z + alien_vert;
				glPushMatrix();
				//glTranslatef(-2 + col*0.3 + alien_horz,0, -3 + 0.5 *row+alien_vert);
				glTranslated(al.x, al.y, al.z);
				glRotated(-100, 1.0, 0.0, 0.0);
				//glRotated(10 - col * 1, 0.0, 1.0, 0.0);
				//glRotated(-15 - col * 1, 0.0, 0.0, 1.0);
				glmUnitize(alien_model);
				glmScale(alien_model, 0.06);
				render(alien_model);
				glPopMatrix();
				if (al.mov_rt){
					if (al.x > max_al_x_rt){
						max_al_x_rt = al.x;
					}
					if (al.x < min_al_x_rt){
						min_al_x_rt = al.x;
					}
				}
				else{
					if (al.x > max_al_x_lt){
						max_al_x_lt = al.x;
					}
					if (al.x < min_al_x_lt){
						min_al_x_lt = al.x;
					}
				}
				if (al.z > max_al_z){
					max_al_z = al.z;
				}
				new_alien_list.push_back(al);
			}
		}
		alien_list = new_alien_list;
		if (move_rt == 1){
			alien_horz = speed;
			if ((max_al_x_rt + alien_horz > 1.5 && min_al_x_lt - alien_horz <-1.5) || (max_al_x_rt + alien_horz > 1.5 && min_al_x_lt  == 1000) || (max_al_x_rt == -1000 && min_al_x_lt - alien_horz <-1.5)) {
				move_rt = 0;
				speed += .008;
				alien_vert += .003;
			}

		}
		else{
			alien_horz = -speed;
			if ((min_al_x_rt + alien_horz < -1.5 && max_al_x_lt - alien_horz > 1.5) || (min_al_x_rt + alien_horz < -1.5 && max_al_x_lt == -1000) || (min_al_x_rt == 1000 && max_al_x_lt - -alien_horz> 1.5)){
				move_rt = 1;
				speed += .008;
				alien_vert += .003;
			}
		}
		if (max_al_z > .38){
			//exit(0);
			curr_life--;
			died = true;
			if (curr_life == 0){
				game_over = true;
			}
		}
	}
}
void initAlienSecLevel(){
	int row = 0;
	int col = 0;
	for (row = 0; row < 4; row++){
		for (col = 0; col < 5; col++){
			ALIEN al;
			al.y = 0;
			al.z = -3 + 0.5 *row;
			if (row % 2 == 0){
				al.mov_rt = true;
				al.x = -2 + col*0.3;
			}
			else{
				al.mov_rt = false;
				al.x = 2 - col*0.3;
			}
			alien_list.push_back(al);
		}
	}
}
void drawAliens(){
	float max_al_x;
	float min_al_x;
	float max_al_z;
	float min_al_z;
	if (alien_list.size() != 0){
		std::vector< ALIEN> new_alien_list;
		int i = 0;
		max_al_x = alien_list[0].x;
		min_al_x = alien_list[0].x;
		max_al_z = alien_list[0].z;
		min_al_z = alien_list[0].z;
		for (i = 0; i < alien_list.size(); i++){
			ALIEN al = alien_list[i];
			if (al.active){
				al.x = al.x + alien_horz;
				al.y = al.y;
				al.z = al.z + alien_vert;
				glPushMatrix();
				//glTranslatef(-2 + col*0.3 + alien_horz,0, -3 + 0.5 *row+alien_vert);
				glTranslated(al.x , al.y, al.z);
				glRotated(-100, 1.0, 0.0, 0.0);
				//glRotated(10 - col * 1, 0.0, 1.0, 0.0);
				//glRotated(-15 - col * 1, 0.0, 0.0, 1.0);
				glmUnitize(alien_model);
				glmScale(alien_model, 0.06);
				render(alien_model);
				glPopMatrix();
				if (al.x > max_al_x){
					max_al_x = al.x;
				}
				if (al.x < min_al_x){
					min_al_x = al.x;
				}
				if (al.z > max_al_z){
					max_al_z = al.z;
				}
				new_alien_list.push_back(al);
			}
		}
		alien_list = new_alien_list;
		if (move_rt == 1){
			alien_horz = speed;
			if (max_al_x + alien_horz > 1.5  ){
			move_rt = 0;
			speed += .008;
			alien_vert += .003;
			}
			
		}
		else{
			alien_horz = -speed;
			if (min_al_x + alien_horz < -1.5 ){
			move_rt = 1;
			speed += .008;
			alien_vert += .003;
			}
		}
		if (max_al_z > .38){
			//exit(0);
			curr_life--;
			died = true;
			if (curr_life == 0){
				game_over = true;
			}
		}
	}
}
void drawCannon(){
	if (cannon.active){
		glPushMatrix();
		glmUnitize(cannon_model);
		glTranslated(cannon.x, 0, cannon.z);
		glmScale(cannon_model, 0.15);
		render(cannon_model);
		glPopMatrix();
	}
}
void initBarrier(){
	
	//first
	BARRIER br1;
	br1.x = -.85;
	br1.z = .55;
	barrier_list.push_back(br1);
	BARRIER br2;
	br2.x = -.8;
	br2.z = .5;
	barrier_list.push_back(br2);
	BARRIER br3;
	br3.x = -.75;
	br3.z = .45;
	barrier_list.push_back(br3);
	/*barrier br4;
	br4.x = -.70;
	br4.z = .45;
	barrier_list.push_back(br4);*/
	BARRIER br5;
	br5.x = -.70;
	br5.z = .5;
	barrier_list.push_back(br5);
	BARRIER br6;
	br6.x = -.65;
	br6.z = .55;
	barrier_list.push_back(br6);
	//second
	BARRIER br7;
	br7.x = -.45;
	br7.z = .55;
	barrier_list.push_back(br7);
	BARRIER br8;
	br8.x = -.40;
	br8.z = .5;
	barrier_list.push_back(br8);
	BARRIER br9;
	br9.x = -.35;
	br9.z = .45;
	barrier_list.push_back(br9);
	BARRIER br10;
	br10.x = -.30;
	br10.z = .45;
	barrier_list.push_back(br10);
	BARRIER br11;
	br11.x = -.25;
	br11.z = .5;
	barrier_list.push_back(br11);
	BARRIER br12;
	br12.x = -.20;
	br12.z = .55;
	barrier_list.push_back(br12);
	//third
	BARRIER br13;
	br13.x = .20;
	br13.z = .55;
	barrier_list.push_back(br13);
	BARRIER br14;
	br14.x = .25;
	br14.z = .5;
	barrier_list.push_back(br14);
	BARRIER br15;
	br15.x = .30;
	br15.z = .45;
	barrier_list.push_back(br15);
	BARRIER br16;
	br16.x = .35;
	br16.z = .45;
	barrier_list.push_back(br16);
	BARRIER br17;
	br17.x = .40;
	br17.z = .5;
	barrier_list.push_back(br17);
	BARRIER br18;
	br18.x = .45;
	br18.z = .55;
	barrier_list.push_back(br18);
	//fouth
	BARRIER br19;
	br19.x = .65;
	br19.z = .55;
	barrier_list.push_back(br19);
	BARRIER br20;
	br20.x = .70;
	br20.z = .5;
	barrier_list.push_back(br20);
	BARRIER br21;
	br21.x = .75;
	br21.z = .45;
	barrier_list.push_back(br21);
	/*barrier br22;
	br22.x = .50;
	br22.z = .45;
	barrier_list.push_back(br22);*/
	BARRIER br23;
	br23.x = .80;
	br23.z = .5;
	barrier_list.push_back(br23);
	BARRIER br24;
	br24.x = .85;
	br24.z = .55;
	barrier_list.push_back(br24);
}
void drawBarrier(){
	int i = 0;
	for (i = 0; i < barrier_list.size(); i++){
		BARRIER br = barrier_list[i];
		if (br.active){
			setMaterial(.99, .85, 0, .2, .2, .2, .2, .5, .5, .5);
			glPushMatrix();
			glTranslated(br.x, br.y, br.z);
			glutSolidCube(.05);
			glPopMatrix();
		}
	}

}	
void cannonShotdetectCollision(){
	
		int i;

		//checking cannon bullet and barrier collision
		if (cannon_shot_list.size() != 0){
			std::vector< BULLETSHOT> new_cann_list1;
			for (i = 0; i < cannon_shot_list.size(); i++){
				BULLETSHOT shot = cannon_shot_list[i];
				int j;
				if (shot.active){
					std::vector< BARRIER> new_br_list;
					for (j = 0; j < barrier_list.size(); j++){
						BARRIER br = barrier_list[j];
						if (br.active && shot.active){
							if ((br.x - .03 < shot.x) && (shot.x < br.x + 0.03)){
								if ((br.z - .03 < shot.z) && (shot.z < br.z + .03)){
									br.active = false;
									shot.active = false;
									score -= 2;
									PlaySound(L"inputs/sounds/explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
								}
							}
						}
						if (br.active){
							new_br_list.push_back(br);
						}
					}
					barrier_list = new_br_list;
				}
				if (shot.active){
					new_cann_list1.push_back(shot);
				}
			}
			cannon_shot_list = new_cann_list1;
		}
		//checking cannon bullet and alien collision
		if (cannon_shot_list.size() != 0){
			std::vector< BULLETSHOT> new_cann_list2;
			for (i = 0; i < cannon_shot_list.size(); i++){
				BULLETSHOT shot = cannon_shot_list[i];
				int j;
				if (shot.active){
					std::vector< ALIEN> new_al_list;
					for (j = 0; j < alien_list.size(); j++){
						ALIEN al = alien_list[j];
						if (al.active && shot.active){
							if ((al.x - .15 < shot.x) && (shot.x < al.x + 0.15)){
								if ((al.z - .15 < shot.z) && (shot.z < al.z + .15)){
									al.active = false;
									shot.active = false;
									score += 10;
									PlaySound(L"inputs/sounds/explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
								}
							}
						}
						if (al.active){
							new_al_list.push_back(al);
						}
					}
					alien_list = new_al_list;
					
				}
				if (shot.active){
					new_cann_list2.push_back(shot);
				}
			}
			
			if (alien_list.size() == 0){
				//game_over = true;
				if (curr_level == 2){
					game_won = true;
					level_2_complete = true;
					PlaySound(L"inputs/sounds/applause.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
				}
				else{
					level_1_complete = true;
					curr_level++;
					PlaySound(L"inputs/sounds/cross.wav", NULL, SND_ASYNC | SND_FILENAME );
				}
			}
			else{
				cannon_shot_list = new_cann_list2;
			}
			
		}

		//checking cannon bullet and UFO collision
		if (ufo_show){
			if (cannon_shot_list.size() != 0){
				std::vector< BULLETSHOT> new_cann_list;
				for (i = 0; i < cannon_shot_list.size(); i++){
					BULLETSHOT shot = cannon_shot_list[i];
					if (shot.active){
						if ((ufo.z - .1 < shot.z) && (shot.z < ufo.z + 0.1)){
							if ((ufo.x - .1 < shot.x) && (shot.x < ufo.x + .1)){
								ufo.active = false;
								ufo_show = false;
								shot.active = false;
								score += 200;
								PlaySound(L"inputs/sounds/bonus.wav", NULL, SND_ASYNC | SND_FILENAME);
							}
						}
					}
					if (shot.active){
						new_cann_list.push_back(shot);
					}
				}
				cannon_shot_list = new_cann_list;
			}
		}

}
void alienShotdetectCollision(){
	int i;
	//checking alien bullet and barrier collision
	if (alien_shot_list.size() != 0){
		std::vector< BULLETSHOT> new_alien_shot_list;
		for (i = 0; i < alien_shot_list.size(); i++){
			BULLETSHOT shot = alien_shot_list[i];
			int j;
			if (shot.active){
				std::vector< BARRIER> new_br_list;
				for (j = 0; j < barrier_list.size(); j++){
					BARRIER br = barrier_list[j];
					if (br.active && shot.active){
						if ((br.x - .03 < shot.x) && (shot.x < br.x + 0.03)){
							if ((br.z - .03 < shot.z) && (shot.z < br.z + .03)){
								br.active = false;
								shot.active = false;
								PlaySound(L"inputs/sounds/explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
							}
						}
					}
					if (br.active){
						new_br_list.push_back(br);
					}
				}
				barrier_list = new_br_list;
			}
			if (shot.active){
				if ((cannon.x - .07 < shot.x) && (shot.x < cannon.x + 0.07)){
					if ((cannon.z - .1 < shot.z) && (shot.z < cannon.z + .1)){
						cannon.active = false;
						shot.active = false;
						PlaySound(L"inputs/sounds/explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
						curr_life--;
						died = true;
						if (curr_life == 0){
							game_over = true;
						}
					}
				}
				if (shot.active){
					new_alien_shot_list.push_back(shot);
				}
			}
		}
		alien_shot_list = new_alien_shot_list;
	}
}
void drawAlienShot(){

	if (alien_list.size()!=0){
		
		//glmUnitize(cannon_model);
		int num_alien = alien_list.size();
		int r = (rand() % (num_alien));
		ALIEN al = alien_list[r];
		float x = al.x;
		float y = al.y;
		float z = al.z;
		BULLETSHOT shot;
		shot.x = x;
		shot.y = y;
		shot.z = z;
		shot.active = true;
		//PlaySound(L"inputs/sounds/shoot.wav", NULL, SND_ASYNC | SND_FILENAME);
		alien_shot_list.push_back(shot);
	}
}
void drawAllAlienShot(){
	alienShotdetectCollision();
	if (alien_shot_list.size() != 0){
		int i = 0;
		std::vector< BULLETSHOT> new_cann_list;
		for (i = 0; i < alien_shot_list.size(); i++){
			BULLETSHOT shot = alien_shot_list[i];
			if (shot.active){
				setMaterial(0.0, 0.9, 0.0, 0.004895, 0.904895, 0.004895, 0.0 ,0.9 ,0.0, .5);
				glPushMatrix();
				glTranslated(shot.x, 0, shot.z);
				glutSolidSphere(.02, 50, 50);
				glPopMatrix();
				if (barrier_list.size() != 0){
					if (shot.z>.4){
						shot.z = shot.z + 0.05;
					}
					else{
						shot.z = shot.z + 0.2;
					}
				}
				else{
					shot.z = shot.z + 0.2;
				}
				if (shot.z < 1.0 ){
					new_cann_list.push_back(shot);
				}
			}
		}
		alien_shot_list = new_cann_list;
	}
	
}
void timeAlienShot(){
	
	time_t now = time(NULL);
	if ((now - alien_time) == 1 ){
		alien_time = now;
		drawAlienShot();
	}
	drawAllAlienShot();
}
void drawCannonShot(){
	float x = cannon.x;
	float y = 0;
	float z = .75;
	BULLETSHOT shot;
	shot.x = x;
	shot.y = y;
	shot.z = z;
	shot.active = true;
	PlaySound(L"inputs/sounds/shoot.wav", NULL, SND_ASYNC | SND_FILENAME);
	cannon_shot_list.push_back(shot);
}
void drawAllCannonShot(){
	
	cannonShotdetectCollision();
	if (cannon_shot_list.size() != 0){
		int i = 0;
		std::vector< BULLETSHOT> new_cann_list;
		for (i = 0; i < cannon_shot_list.size(); i++){
			BULLETSHOT shot = cannon_shot_list[i];
			if (shot.active){
				setMaterial(0.1137, 0.1137, 0.5137, 0.1137, 0.1137, 0.5137, 0.3600, 0.3600, 0.3600, .5);
				glPushMatrix();
				glTranslated(shot.x, 0, shot.z);
				glutSolidSphere(.02, 50, 50);
				glPopMatrix();
				if (barrier_list.size() != 0){
					if (shot.z<.4){
						shot.z = shot.z - 0.2;
					}
					else{
						shot.z = shot.z - 0.05;
					}
				}
				else{
					shot.z = shot.z - 0.2;
				}
				if (shot.z > -3.5){
					new_cann_list.push_back(shot);
				}
			}
		}
		cannon_shot_list = new_cann_list;
	}
	
	
}
void drawUFO(){
	setMaterial(.99, 0, 0, .2, .2, .2, .2, .5, .5, .5);
	glPushMatrix();
	glTranslated(ufo.x, ufo.y, ufo.z);
	glutSolidSphere(.1, 50, 50);
	glPopMatrix();
	ufo.x += ufo_move;
}
void timeUFO(){
	if (ufo.active){
		if ((time(NULL) - game_start_time) == 10){
			ufo_show = true;
			game_start_time = time(NULL);
		}
	}
	if (ufo_show){
		if (ufo.x > 2.5){
			ufo.active = false;
			ufo_show = false;
		}
		else
			drawUFO();
	}
}
void outputText(float x, float y, float z, float r, float g, float b, char *string)
{
	setMaterial(.99, 0, 0, .2, .2, .2, .2, .5, .5, .5);
	glRasterPos3f(x, y,z);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	char buf[100];
	char obuf[100];
	char wbuf[100];
	int rem_life = curr_life - 1;		
	if (game_won){
		sprintf(buf, "SCORE : %d", score);
		outputText(-.1, .8, 0, .99, 0.0, 0.0, buf);
		sprintf(wbuf, "YOU WON, CONGRATS!!!");
		outputText(-.45, 0, -.5, .99, 0.0, 0.0, wbuf);
	}
	else if(game_over){
		sprintf(buf, "SCORE : %d", score);
		outputText(-.1, .8, 0, .99, 0.0, 0.0, buf);
		sprintf(obuf, "GAME OVER!!!");
		outputText(-.3, 0, -.5, .99, 0.0, 0.0, obuf);
	}
	else{
		sprintf(buf, "SCORE : %d  LIVES : %d Level : %d", score, rem_life,curr_level);
		outputText(-.4, .8, 0, .99, 0.0, 0.0, buf);
	}
	//initBackgroundTexture();
	
	if (curr_level == 2){	
		drawAliensSecLevel();
	}
	else{
		drawBarrier();
		drawAliens();
	}
	drawCannon();
	timeAlienShot();
	timeUFO();
	drawAllCannonShot();
		
	glBegin(GL_LINES);
	glVertex3f(-1.0f,0, -3.0f); // origin of the line
	glVertex3f(1.0f,0, -3.0f); // ending point of the line
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0, -3.0f); // origin of the line
	glVertex3f(-1.0f, 0, .4f); // ending point of the line
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(1.0f, 0, -3.0f); // origin of the line
	glVertex3f(1.0f, 0, .4f); // ending point of the line
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0, .40f); // origin of the line
	glVertex3f(1.0f, 0, .40f); // ending point of the line
	glEnd();
	
	glFlush();
	glutSwapBuffers();
}
void redraw(int n){
	glutPostRedisplay();
	if (!game_over){
		glutTimerFunc(1, redraw, 0);
		if (level_1_complete || died){
			initGame();
		}
	}
	
}
void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.00, (GLfloat)w / (GLfloat)h, 1.0, 400.0);
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, viewup.x, viewup.y, viewup.z);

}
void keyboard(unsigned char key, int x, int y)
{
	if (!game_over && !game_won){
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 32:
			drawCannonShot();
			break;
		default:
			break;
		}
	}
	else{
 		switch (key)
		{
		case 27:
			exit(0);
			break;
		default:
			break;
		}
	}
}
void processSpecialKeys(int key, int x, int y) {
	
	if (!game_over && !game_won){
		switch (key)
		{

		case GLUT_KEY_LEFT:
			//Translating in -x axis
			if (cannon.x > -.7){
				cannon.x -= 0.05;
			}
			break;
		case GLUT_KEY_RIGHT:
			//Translating in +x axis
			if (cannon.x < .7){
				cannon.x += 0.05;
			}
			break;
		default:
			break;
		}
	}
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	setWindow();
	glutInitWindowPosition(50, 50);
	glutCreateWindow("~~ 3D SPACE INVADER ~~");
	//Calling the method to set all the light source
	setLightSource();
	//Iterating to set the gl_lights
	for (int itr = 0; itr < light_source_list.size(); itr++){
		lightSRC src = light_source_list[itr];
		float  lt_amb[] = { src.ambient.x, src.ambient.y, src.ambient.z, 1 };	// Ambient light property
		float  lt_dif[] = { src.diffuse.x, src.diffuse.y, src.diffuse.z, 1 };	// diffuse light property
		float  lt_spc[] = { src.specular.x, src.specular.y, src.specular.z, 1 }; //specular light property
		float  lt_pos[] = { src.pos.x, src.pos.y, src.pos.z, 0 };
		switch (itr)
		{
		case 0:
			//setting first light
			glLightfv(GL_LIGHT0, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT0);
			break;
		case 1:
			//setting second light
			glLightfv(GL_LIGHT1, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT1, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT1, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT1);
			break;
		case 2:
			//setting third light
			glLightfv(GL_LIGHT2, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT2, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT2, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT2);
			break;
		case 3:
			//setting fouth light
			glLightfv(GL_LIGHT3, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT3, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT3, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT3, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT3);
			break;
		case 4:
			//settiing fifth light
			glLightfv(GL_LIGHT4, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT4, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT4, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT4, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT4);
			break;
		case 5:
			//setting sixth light
			glLightfv(GL_LIGHT5, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT5, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT5, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT5, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT5);
			break;
		case 6:
			//setting the seventh light
			glLightfv(GL_LIGHT6, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT6, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT6, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT6, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT6);
			break;
		case 7:
			//setting the eigth light
			//maximum 8 lights can be set
			glLightfv(GL_LIGHT7, GL_POSITION, lt_pos);
			glLightfv(GL_LIGHT7, GL_AMBIENT, lt_amb);
			glLightfv(GL_LIGHT7, GL_DIFFUSE, lt_dif);
			glLightfv(GL_LIGHT7, GL_SPECULAR, lt_spc);
			glEnable(GL_LIGHT7);
			break;

		default:
			break;
		}

	}

	//Enable GL lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, redraw, 0);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	return 0;
}

/*Parsing the .obj and .mtl files and saving in gloral vaiables*/
int parseObject(){

	cannon_model = glmReadOBJ("inputs/Bomber.obj");
	alien_model = glmReadOBJ("inputs/alien.obj");
	//ufo_model = glmReadOBJ("inputs/Brake_Disc.obj");
	
	return 0;
}
/* Parsing the window.txt to get the window width and height*/
int setWindow(){

	FILE *file = fopen("inputs/window.txt", "r");
	if (file == NULL){
		printf("\nCannot open windows.txt");
		//Setting the default window size
		window.width = 512;
		window.height = 512;
		glutInitWindowSize(512, 512);
		return -1;
	}

	printf("\nCould open windows.txt");

	char lineHeader[128];

	while (1){

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			fclose(file);
			break; // EOF = End Of File. Quit the loop.
		}
		//reading the window width
		else if (strcmp(lineHeader, "width") == 0){

			fscanf(file, "%d\n", &window.width);

		}
		//reading the window height
		else if (strcmp(lineHeader, "height") == 0){

			fscanf(file, "%d\n", &window.height);

		}
	}

	glutInitWindowSize(window.width, window.height);
	return 0;
}
/* Parsing the lights.txt to get all the light sources*/
int setLightSource(){
	int num_light = 0;
	FILE *file = fopen("inputs/lights.txt", "r");
	if (file == NULL){
		printf("\nCannot open lights.txt");
		lightSRC src;
		//setting the default light
		src.ambient = glm::vec3(1, 1, 1);
		src.diffuse = glm::vec3(1, 1, 1);
		src.specular = glm::vec3(1, 1, 1);
		src.pos = glm::vec3(0, .8, 2);
		char *name = "default";
		src.light_name = name;
		light_source_list.push_back(src);
		return -1;
	}

	else{
		printf("\nCould open lights.txt");

		char lineHeader[128];
		std::vector<std::string> light_name_list;
		std::vector<glm::vec3> light_pos_list;
		std::vector<glm::vec3> light_amb_list;
		std::vector<glm::vec3> light_diff_list;
		std::vector<glm::vec3> light_spec_list;
		while (1){

			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
			{
				fclose(file);
				break; // EOF = End Of File. Quit the loop.
			}
			//reading the light name
			else if (strcmp(lineHeader, "light") == 0){
				char abc[128];
				std::string light_name;
				fscanf(file, "%s\n", &abc);
				light_name = abc;
				light_name_list.push_back(light_name);

			}
			//reading the light position
			else if (strcmp(lineHeader, "pos") == 0){
				glm::vec3 pos;
				fscanf(file, "%f %f %f\n", &pos.x, &pos.y, &pos.z);
				light_pos_list.push_back(pos);

			}
			//reading the ambient settings
			else if (strcmp(lineHeader, "ambient") == 0){
				glm::vec3 col;
				fscanf(file, "%f %f %f\n", &col.x, &col.y, &col.z);
				light_amb_list.push_back(col);

			}
			//reading the diffuse setting
			else if (strcmp(lineHeader, "diffuse") == 0){
				glm::vec3 col;
				fscanf(file, "%f %f %f\n", &col.x, &col.y, &col.z);
				light_diff_list.push_back(col);

			}
			//reading the specular setting
			else if (strcmp(lineHeader, "specular") == 0){
				glm::vec3 col;
				fscanf(file, "%f %f %f\n", &col.x, &col.y, &col.z);
				light_spec_list.push_back(col);

			}
		}

		if (light_name_list.size() != 0){
			int k = 0;
			//Creating a list of light source
			while (k < light_name_list.size() && k < 8)
			{
				lightSRC src;
				src.ambient = light_amb_list.at(k);
				src.diffuse = light_diff_list.at(k);
				src.specular = light_spec_list.at(k);
				src.pos = light_pos_list.at(k);
				src.light_name = light_name_list.at(k);
				light_source_list.push_back(src);
				k++;

			}
		}


	}


	return 0;
}
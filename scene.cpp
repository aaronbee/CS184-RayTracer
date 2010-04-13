
#include "raytracer.h"
#include "shape.h"
#include <stdio.h>

scene::scene(char* path)
{
  readScene(path);
}

void scene::readScene(char* path)
{
  FILE *inputfile = fopen(path, "r");
  if (!inputfile) {
	cerr << "Bad input file path" << endl;
	exit(1);
  }
  initialparse(inputfile);
  parsefile(inputfile);
  fclose(inputfile);
}

bool scene::getSample(vec2 *pixel)
{
  if (y == height)
	return false;
  if (x == width) {
	x = 0;
	y ++;
  }

  *pixel = vec2(x, y);
  x ++;

  return true;
}

void scene::render()
{
  vec2 pixel;
  ray r;
  color c;
  while (getSample(&pixel)) {
	r = cam.generateRay(pixel);
	c = raytracer.trace(r);
	f.put(pixel, c);
  }
}


void scene::initialparse (FILE *fp) {
  char line[1000], command[1000] ; // Very bad to prefix array size :-)

  while (1) {
    fgets(line,sizeof(line),fp) ;
    if (feof(fp)) { fprintf(stderr, "Nothing in file\n") ; exit(1); }  ;
    if (line[0] == '#') continue ; // Comment lines    
    int num = sscanf(line, "%s", command) ;
    if (num != 1) continue ; // Blank line etc.
    else break ;
  }
  
    /*****   SET IMAGE SIZE ******/

    // The first line should be the size command setting the image size
    assert(!strcmp(command, "size")) ;
    int num = sscanf(line, "%s %d %d", command, &width, &height) ;
    assert(num == 3) ;
    assert(!strcmp(command, "size")) ;
}

void scene::parsefile (FILE *fp) {
  char line[1000], command[1000] ; // Very bad to prefix array size :-)
  int sizeset = 0 ;

  //  initdefaults() ;
  
  while (!feof(fp)) {
    fgets(line,sizeof(line),fp) ;
    if (feof(fp)) break ;
    if (line[0] == '#') continue ; // Comment lines
    
    int num = sscanf(line, "%s", command) ;
    if (num != 1) continue ; // Blank line etc.

    // Now, we simply parse the file by looking at the first line for the 
    // various commands
    
    /**************        CAMERA LOCATION **********/
  
    if (!strcmp(command, "camera")) {
      double lookfrom[3], lookat[3], up[3], _fov ;
      int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
					   command, lookfrom, lookfrom+1, lookfrom+2, 
					   lookat, lookat+1, lookat+2, up, up+1, up+2, &_fov) ;
      if (num != 11) {
		fprintf(stderr, "camera from[3] at[3] up[3] fov\n") ;
		exit(1) ;
      }

      assert(!strcmp(command,"camera")) ;

	  cameraPos = vec3(lookfrom[0], lookfrom[1], lookfrom[2]);
	  cameraUp = vec3(up[0], up[1], up[2]);
	  cameraLookAt = vec3(lookat[0], lookat[1], lookat[2]);
	  fov = _fov;
    }

    /****************************************/

    /***********  GEOMETRY *******************/
	
    else if (!strcmp(command, "sphere")) {
	  double radius ; // Syntax is sphere x y z radius 
	  double pos[3] ;
	  int num = sscanf(line, "%s %lf %lf %lf %lf", command, pos, pos+1, pos+2, &radius) ;
	  if (num != 5) {
		fprintf(stderr, "sphere x y z radius\n") ;
		exit(1) ;
	  }

	  objects.push_back(new Sphere(vec3(pos[0], pos[1], pos[2]), radius));

	}
	/*
	else if (!strcmp(command, "maxverts")) {
	  int num = sscanf(line, "%s %d", command, &maxverts) ;
	  assert(num == 2) ; assert(maxverts > 0) ;
	  assert(!strcmp(command,"maxverts")) ;
	  assert(vert = new Vertex[maxverts]) ;
	}

	else if (!strcmp(command, "maxvertnorms")) {
	  int num = sscanf(line, "%s %d", command, &maxvertnorms) ;
	  assert(num == 2) ; assert(maxvertnorms > 0) ;
	  assert(!strcmp(command,"maxvertnorms")) ;
	  assert(vertnorm = new VertexNormal[maxvertnorms]) ;
	}

	else if (!strcmp(command, "vertex")) {  // Add a vertex to the stack
	  assert(maxverts) ; assert(curvert < maxverts) ;
	  Vertex v ;
	  int num = sscanf(line, "%s %lf %lf %lf", command, v.pos, v.pos+1, v.pos+2) ;
	  assert(num == 4) ; assert(!strcmp(command,"vertex")) ;
	  vert[curvert] = v ;
	  ++curvert ;
	}

	else if (!strcmp(command, "vertexnormal")) {  
	  // Add a vertex to the stack with a normal
	  assert(maxvertnorms) ; assert(curvertnorm < maxvertnorms) ;
	  VertexNormal vn ;

	  int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf", 
		  command, vn.pos, vn.pos+1, vn.pos+2, 
		  vn.normal, vn.normal+1, vn.normal+2) ;

	  assert(num == 7) ; assert(!strcmp(command,"vertexnormal")) ;
	  vertnorm[curvertnorm] = vn ;
	  ++curvertnorm ;
	}

        else if (!strcmp(command, "tri")) { // Triangle from 3 vertices
	 int pts[3] ; 
	 int num = sscanf(line, "%s %d %d %d", command, pts, pts+1, pts+2) ;
	 assert(num == 4) ; assert(!strcmp(command,"tri")) ;
	 int i,j ;
	 for (i = 0 ; i < 3 ; i++) {
	   assert(pts[i] >= 0 && pts[i] < maxverts) ;
	 }
	  double vertex[3][3] ;
	  double normal[3] ;
	  for (i = 0 ; i < 3 ; i++) 
	    for (j = 0 ; j < 3 ; j++)
	      vertex[i][j] = vert[pts[i]].pos[j] ;
	  
	  
	  // Calculate the face normal 
	  double vec1[3], vec2[3], vec3[3] ;
	  for (i = 0 ; i < 3 ; i++) {
		  vec1[i] = vertex[1][i] - vertex[0][i] ;
		  vec2[i] = vertex[2][i] - vertex[0][i] ;
	  }
	  vec3[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1] ;
	  vec3[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2] ;
	  vec3[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0] ;

	  double norm = 0 ;
	  for (i = 0 ; i < 3 ; i++) norm += vec3[i] * vec3[i] ;
	  norm = sqrt(norm) ;
      if (norm == 0) {normal[0] = 0 ; normal[1] = 0 ; normal[2] = 1.0 ; }
	  else {
		for (i = 0 ; i < 3 ; i++) normal[i] = vec3[i] / norm ;
	  }
	
	  glBegin(GL_TRIANGLES) ;
		glNormal3f(normal[0],normal[1],normal[2]) ;
		for (i = 0 ; i < 3 ; i++)
			glVertex3f(vertex[i][0],vertex[i][1],vertex[i][2]) ;
	  glEnd() ;

       }

        else if (!strcmp(command, "trinormal")) {
	  int pts[3] ;
	  int num = sscanf(line, "%s %d %d %d", command, pts, pts+1, pts+2) ;
	  assert(num == 4) ; assert(!strcmp(command,"trinormal")) ;
	  int i,j ;
	  for (i = 0 ; i < 3 ; i++) {
	    assert(pts[i] >= 0 && pts[i] < maxvertnorms) ;
	  }

	  glBegin(GL_TRIANGLES) ;
	  for (i = 0 ; i < 3 ; i++) {
	    VertexNormal &vn = vertnorm[pts[i]] ;
	    glNormal3f(vn.normal[0], vn.normal[1], vn.normal[2]) ;
	    glVertex3f(vn.pos[0], vn.pos[1], vn.pos[2]) ;
	  }
	  glEnd() ;

	}
	*/
    /******************************************/

    /**************** TRANSFORMATIONS *********/

	/*
	else if (!strcmp(command, "translate")) {
	  double x,y,z ; // Translate by x y z as in standard OpenGL

	  int num = sscanf(line, "%s %lf %lf %lf",command, &x, &y, &z) ;
	  if (num != 4) {
		fprintf(stderr, "translate x y z\n") ;
		exit(1) ;
	  }
	  glMatrixMode(GL_MODELVIEW) ;
	  glTranslatef(x,y,z) ;
	}

	else if (!strcmp(command, "rotate")) {
	  double ang, x,y,z ; // Rotate by an angle about axis x y z as in standard OpenGL

	  int num = sscanf(line, "%s %lf %lf %lf %lf",command, &x, &y, &z, &ang) ;
	  if (num != 5) {
		fprintf(stderr, "rotate angle x y z\n") ;
		exit(1) ;
	  }
	  glMatrixMode(GL_MODELVIEW) ;
	  glRotatef(ang,x,y,z) ;
	}

	else if (!strcmp(command, "scale")) {
	  double x,y,z ; // Scale by x y z as in standard OpenGL

	  int num = sscanf(line, "%s %lf %lf %lf",command, &x, &y, &z) ;
	  if (num != 4) {
		fprintf(stderr, "scale x y z\n") ;
		exit(1) ;
	  }
	  glMatrixMode(GL_MODELVIEW) ;
	  glScalef(x,y,z) ;
	}

	else if (!strcmp(command, "pushTransform")) {
	  // Push the current matrix on the stack as in OpenGL
	  glMatrixMode(GL_MODELVIEW) ;
	  glPushMatrix() ;
	}

	else if (!strcmp(command, "popTransform")) {
	  // Pop the current matrix as in OpenGL
	  glMatrixMode(GL_MODELVIEW) ;
      glPopMatrix() ;
	}
	*/
    /************************************************************/

    /********* MISCELLANEOUS IGNORED FOR OPENGL *******************/
	/*
        else if (!strcmp(command, "maxdepth")) {
	  int num = sscanf(line, "%s %d", command, &maxdepth) ;
	  assert(num == 2) ;
	  assert(!strcmp(command, "maxdepth")) ;
	  fprintf(stderr, "Maxdepth set to %d but irrelevant for OpenGL\n", 
		  maxdepth) ;
	}

       else if (!strcmp(command, "output")) {
	   char out[300] ;
	   int num = sscanf(line, "%s %s", command, out) ;
	   assert(num == 2) ;
	   assert(!strcmp(command, "output")) ;
	   fprintf(stderr, "Output image file set to: %s but ignored for OpenGL\n",out) ;
       }
	*/
    /*************************************************/

    /***************  LIGHTS *******************/
	/*
       else if (!strcmp(command, "directional")) {
	 float direction[4], color[4] ; color[3] = 1.0 ; direction[3] = 0.0 ;
	 int num = sscanf(line, "%s %f %f %f %f %f %f", command, direction, direction+1, direction+2, color, color+1, color+2) ;
	 assert(num == 7) ;
	 assert(lightnum >= 0 && lightnum < GL_MAX_LIGHTS) ;

	 int mylight = GL_LIGHT0 + lightnum ;
	 glLightfv(mylight, GL_DIFFUSE, color) ;
	 glLightfv(mylight, GL_SPECULAR, color) ;
	 glLightfv(mylight, GL_POSITION, direction) ;
	 glEnable(mylight) ;
	 ++lightnum ;
       }

      else if (!strcmp(command, "point")) {
	 float direction[4], color[4] ; color[3] = 1.0 ; direction[3] = 1.0 ;
	 int num = sscanf(line, "%s %f %f %f %f %f %f", command, direction, direction+1, direction+2, color, color+1, color+2) ;
	 assert(num == 7) ;
	 assert(lightnum >= 0 && lightnum < GL_MAX_LIGHTS) ;

	 int mylight = GL_LIGHT0 + lightnum ;
	 glLightfv(mylight, GL_DIFFUSE, color) ;
	 glLightfv(mylight, GL_SPECULAR, color) ;
	 glLightfv(mylight, GL_POSITION, direction) ;
	 glLightf(mylight, GL_CONSTANT_ATTENUATION, attenuation[0]) ;
	 glLightf(mylight, GL_LINEAR_ATTENUATION, attenuation[1]) ;
	 glLightf(mylight, GL_QUADRATIC_ATTENUATION, attenuation[2]) ;
	 glEnable(mylight) ;
	 ++lightnum ;	 
       }

     else if (!strcmp(command, "attenuation")) {
       int num = sscanf(line, "%s %lf %lf %lf", command, attenuation, attenuation + 1, attenuation +2) ;
       assert(num == 4) ;
       assert(!strcmp(command, "attenuation")) ;
     }

     else if (!strcmp(command, "ambient")) {
       float ambient[4] ; ambient[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, ambient, ambient+1, ambient+2) ;
       assert(num == 4) ;
       assert(!strcmp(command, "ambient")) ;
       glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient) ; 
     }
	*/
    /*******************************************************/

    /****************** MATERIALS ************************/
	/*
     else if (!strcmp(command, "diffuse")) {
       float diffuse[4] ; diffuse[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, diffuse, diffuse+1, diffuse+2) ;
       assert(num == 4) ; assert (!strcmp(command, "diffuse")) ;
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse) ;
       glColor3f(diffuse[0],diffuse[1],diffuse[2]) ;
     }

     else if (!strcmp(command, "specular")) {
       float specular[4] ; specular[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, specular, specular+1, specular+2) ;
       assert(num == 4) ; assert (!strcmp(command, "specular")) ;
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular) ;
     }

     else if (!strcmp(command, "shininess")) {
       float shininess ;
       int num = sscanf(line, "%s %f", command, &shininess) ;
       assert(num == 2) ; assert (!strcmp(command, "shininess")) ;
       glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess) ;
     }

     else if (!strcmp(command, "emission")) {
       float emission[4] ; emission[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, emission, emission+1, emission+2) ;
       assert(num == 4) ; assert (!strcmp(command, "emission")) ;
       glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission) ;
     }
	*/
    /*****************************************************/

    else {
      fprintf(stderr, "Unimplemented command: %s\n", command) ;
      exit(1) ;
    }
  }
}

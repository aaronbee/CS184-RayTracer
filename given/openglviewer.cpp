#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glut.h>

int sizex, sizey ;

struct Vertex {
  double pos[3] ;
} ;

struct VertexNormal {
  double pos[3] ;
  double normal[3] ;
} ;

Vertex * vert = 0;
VertexNormal * vertnorm = 0;
int maxverts = 0, maxvertnorms = 0;
int curvert = 0, curvertnorm = 0 ;

int maxdepth = 5 ;
int lightnum = 0 ;

double attenuation[3] = {1.0, 0.0, 0.0} ;

FILE *inputfile ;

int parsed = 0 ;

void display(void) {
   glFinish() ;
}


void initwindow () 
{
   glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB);
   glutInitWindowSize (sizex, sizey) ;
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("OpenGL viewer");
   glutDisplayFunc(display) ;
}   

void initdefaults (void) {
  

   // select clearing color 	
   glClearColor (0.0, 0.0, 0.0, 1.0);

   // initialize viewing values 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity() ;
   gluPerspective(45,(double) sizex / (double) sizey,.1,99.0) ;

   // initialize camera parameters
   glMatrixMode(GL_MODELVIEW) ;
   glLoadIdentity() ;
   gluLookAt(0,0,5.0,0,0,0,0,1,0) ;

   glEnable(GL_LIGHTING) ; 
   glDisable(GL_LIGHT0) ;
   float white[4] = {1,1,1,1.0} ;
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white) ;

   glEnable(GL_DEPTH_TEST) ;
   glDepthFunc(GL_LESS) ;

   //   glShadeModel(GL_FLAT) ; Can be useful for debugging.

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

}



// This is the main function that defines the scene.  
// For the most part, just read a line and do the appropriate thing.
// The raytracer just needs to replace this with the appropriate function 
// calls for the purpose of parsing.

void initialparse (FILE *fp) {
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
    int num = sscanf(line, "%s %d %d", command, &sizex, &sizey) ;
    assert(num == 3) ;
    assert(!strcmp(command, "size")) ;
    initwindow() ;
}

void parsefile (FILE *fp) {
  char line[1000], command[1000] ; // Very bad to prefix array size :-)
  int sizeset = 0 ;

  initdefaults() ;
  
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
      double lookfrom[3], lookat[3], up[3], fov ;
      int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
		       command, lookfrom, lookfrom+1, lookfrom+2, 
		       lookat, lookat+1, lookat+2, up, up+1, up+2, &fov) ;
      if (num != 11) {
	fprintf(stderr, "camera from[3] at[3] up[3] fov\n") ;
	exit(1) ;
      }

      assert(!strcmp(command,"camera")) ;

      // Set up modelview and projection matrices per camera parameters

      glMatrixMode(GL_MODELVIEW) ;
      glLoadIdentity() ;
      gluLookAt(lookfrom[0], lookfrom[1], lookfrom[2], 
		lookat[0], lookat[1], lookat[2], 
		up[0], up[1], up[2] ) ;
      glMatrixMode(GL_PROJECTION) ;
	  glLoadIdentity() ;
      gluPerspective(fov, (double) sizex / (double) sizey, 1.0, 10.0) ;
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

	  glMatrixMode(GL_MODELVIEW) ;
	  glPushMatrix() ;
	  glTranslatef(pos[0], pos[1], pos[2]) ;

	  GLUquadricObj * qobj ;
	  qobj = gluNewQuadric() ;
      assert(qobj) ;
      gluQuadricOrientation(qobj, GLU_OUTSIDE) ;
//	  glColor3f(1.0,1.0,1.0) ;
	  gluSphere(qobj,radius,40,40) ; // Draw sphere with stacks in lat-long
	  glMatrixMode(GL_MODELVIEW) ;
	  glPopMatrix() ;
    }

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

    /******************************************/

    /**************** TRANSFORMATIONS *********/

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

    /************************************************************/

    /********* MISCELLANEOUS IGNORED FOR OPENGL *******************/
     
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

    /*************************************************/

    /***************  LIGHTS *******************/

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

    /*******************************************************/

    /****************** MATERIALS ************************/

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

    /*****************************************************/

    else {
      fprintf(stderr, "Unimplemented command: %s\n", command) ;
      exit(1) ;
    }
    GLenum g = glGetError() ;
    if (g != GL_NO_ERROR) {
      fprintf(stderr, "Error: %d\n",g) ;
      exit(1) ;
    }

  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // Escape to quit
    exit(0) ;
    break ;
  }
}

void idle (void) {
  glutIdleFunc(NULL) ;
  parsefile(inputfile) ;
  parsed = 1 ;
  glutPostRedisplay() ;
}

int main(int argc,  char ** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: openglviewer filename\n") ;
    exit(1) ;
  }
  FILE *fp ; 
  assert (fp = fopen(argv[1], "rt")) ;
  --argc ; ++argv ; 
  glutInit(&argc, argv) ;
  inputfile = fp ; 
  initialparse(fp) ;
  //  assert(glGetError() == GL_NO_ERROR) ;
  glutKeyboardFunc(keyboard) ;
  glutIdleFunc(idle) ;
  glutMainLoop() ;
  return 0 ; // Required to stop warnings
}


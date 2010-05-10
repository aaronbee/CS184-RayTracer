
#include "raytracer.h"
#include "shape.h"
#include "light.h"
#include <stdio.h>

Scene::Scene(char* path)
{
  shapes = new vector<Shape *>();
  lights = new vector<Light *>();
  
  outputPath = string("test.png");
  maxDepth = 5;
  attenuation[0] = 1.0;
  attenuation[1] = 0.0;
  attenuation[2] = 0.0;
  transformations.push(identity3D());
  ambient = Color(0.2, 0.2, 0.2);
  numSamples = 1;
  readScene(path);
  totalPix = width * height;
}

void Scene::init()
{
  cam = new Camera();
  rt = new RayTracer();
  film = new Film();
  tree = createBVHTree(*shapes, 0);
}

void Scene::readScene(char* path)
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

bool Scene::getSample(vec2 *pixel)
{
  static int count = 0;
  if (y == height)
	return false;

  *pixel = vec2(x, y);
  count ++;

  if (count == scene->getNumSamples()) {
	count = 0;
	x ++;
  }

  if (x == width) {
	x = 0;
	y ++;
  }

  return true;
}

void Scene::printProgressHeader() {
  int i = 0;
  cout << "0%";
  i += 2;
  for (; i < 24; i ++) {
	cout << " ";
  }
  cout << "50%";
  i += 3;
  for (; i < 46; i++) {
    cout << " ";
  }
  cout << "100%" << endl;
}

void Scene::printProgress(int count) {
  static int printed = 0;
  int shouldPrint = 50.0 * ((double) count / (double) totalPix) / scene->getNumSamples();
  for (; printed < shouldPrint; printed ++) {
	cout << "X" << flush;
  }
}

void Scene::render()
{
  vec2 pixel;
  Ray r;
  Color c;
  printProgressHeader();
  int count = 0;

  while (getSample(&pixel)) {
	r = cam->generateRay(pixel);
	c = rt->trace(r, scene->getMaxDepth());
	film->put(pixel, c);
	count++;
	printProgress(count);
  }
  
  cout << endl;
  film->writeToFile(outputPath);
}


void Scene::initialparse (FILE *fp) {
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

void Scene::parsefile (FILE *fp) {
  char line[1000], command[1000] ; // Very bad to prefix array size :-)
  vec3** verts = NULL;
  int curvert = 0;
  int maxverts;

  vertnorm* vertnorms = NULL;
  int curvertnorm = 0;
  int maxvertnorms;

  Color curDiffuse = Color(vec3(0, 0, 0));
  Color curSpecular = Color(vec3(0, 0, 0));
  Color curEmission = Color(vec3(0, 0, 0));
  double curShininess = 0.0;

  //  int sizeset = 0 ;

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

	  Scene::cameraPos = vec3(lookfrom[0], lookfrom[1], lookfrom[2]);
	  Scene::cameraUp = vec3(up[0], up[1], up[2]);
	  Scene::cameraLookAt = vec3(lookat[0], lookat[1], lookat[2]);
	  Scene::fov = _fov;
    }

    /****************************************/

    /***********  GEOMETRY *******************/
	
    else if (!strcmp(command, "sphere")) {
	  double radius ; // Syntax is sphere x y z radius 
	  double pos[3] ;
	  double index;
	  int num = sscanf(line, "%s %lf %lf %lf %lf %lf", command, pos, pos+1, pos+2, &radius, &index) ;
	  if (num != 6) {
		fprintf(stderr, "sphere x y z radius\n") ;
		exit(1) ;
	  }

	  shapes->push_back(new Sphere(vec3(pos[0], pos[1], pos[2]), radius,
								   curDiffuse, curSpecular, curEmission,
								   curShininess, index, transformations.top()));

	}
	
	else if (!strcmp(command, "maxverts")) {
	  int num = sscanf(line, "%s %d", command, &maxverts) ;
	  assert(num == 2) ; assert(maxverts > 0) ;
	  assert(!strcmp(command,"maxverts")) ;
	  assert(verts = new vec3 *[maxverts]) ;
	}
  
	else if (!strcmp(command, "maxvertnorms")) {
	  int num = sscanf(line, "%s %d", command, &maxvertnorms) ;
	  assert(num == 2) ; assert(maxvertnorms > 0) ;
	  assert(!strcmp(command,"maxvertnorms")) ;
	  assert(vertnorms = new vertnorm [maxvertnorms]) ;
	}
  
  
	else if (!strcmp(command, "vertex")) {  // Add a vertex to the stack
	  assert(maxverts) ; assert(curvert < maxverts) ;
    double x,y,z;
	  int num = sscanf(line, "%s %lf %lf %lf", command, &x, &y, &z) ;
	  assert(num == 4) ; assert(!strcmp(command,"vertex")) ;
	  verts[curvert] = new vec3(x,y,z) ;
	  ++curvert ;
	}
  
  
	else if (!strcmp(command, "vertexnormal")) {  
	  // Add a vertex to the stack with a normal
	  assert(maxvertnorms) ; assert(curvertnorm < maxvertnorms) ;
	  vertnorm vn ;
          double x,y,z,nx,ny,nz;

	  int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf", 
		  command, &x, &y, &z, &nx, &ny, &nz); 

	  assert(num == 7) ; assert(!strcmp(command,"vertexnormal")) ;
          vn.vert = new vec3(x,y,z);
          vn.norm = new vec3(nx,ny,nz);
	  vertnorms[curvertnorm] = vn ;
	  ++curvertnorm ;
	}
	
	
	else if (!strcmp(command, "tri")) { // Triangle from 3 vertices
	 int pts[3] ; 
	 int num = sscanf(line, "%s %d %d %d", command, pts, pts+1, pts+2) ;
	 assert(num == 4) ; assert(!strcmp(command,"tri")) ;
	 int i ;
	 for (i = 0 ; i < 3 ; i++) {
	   assert(pts[i] >= 0 && pts[i] < maxverts) ;
	 }
	 vec4 temp0, temp1, temp2;
	 temp0 = vec4(*verts[pts[0]]);
	 temp1 = vec4(*verts[pts[1]]);
	 temp2 = vec4(*verts[pts[2]]);
	 vec3 vert0 = vec3(transformations.top() * temp0);
	 vec3 vert1 = vec3(transformations.top() * temp1);
	 vec3 vert2 = vec3(transformations.top() * temp2);

	 shapes->push_back(new Triangle(vert0, vert1, vert2, curDiffuse,
									curSpecular, curEmission, curShininess));

  }
  
        else if (!strcmp(command, "trinormal")) {
	  int pts[3] ;
	  int num = sscanf(line, "%s %d %d %d", command, pts, pts+1, pts+2) ;
	  assert(num == 4) ; assert(!strcmp(command,"trinormal")) ;
	  int i;
	  for (i = 0 ; i < 3 ; i++) {
	    assert(pts[i] >= 0 && pts[i] < maxvertnorms) ;
	  }

	 vec4 temp0, temp1, temp2;
	 vertnorm v0 = vertnorms[pts[0]];
	 vertnorm v1 = vertnorms[pts[1]];
	 vertnorm v2 = vertnorms[pts[2]];
	 temp0 = vec4(*(v0.vert));
	 temp1 = vec4(*(v1.vert));
	 temp2 = vec4(*(v2.vert));
	 vec3 vert0 = vec3(transformations.top() * temp0);
	 vec3 vert1 = vec3(transformations.top() * temp1);
	 vec3 vert2 = vec3(transformations.top() * temp2);
	 
	shapes->push_back(new TriNormal(vert0,vert1,vert2, *(v0.norm), *(v1.norm), *(v2.norm), curDiffuse, curSpecular, curEmission, curShininess)); 
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
	  vec3 t = vec3(x, y, z);
	  mat4 m = transformations.top() * translation3D(t);
	  transformations.pop();
	  transformations.push(m);
	}
	
	else if (!strcmp(command, "rotate")) {
	  double ang, x,y,z ; // Rotate by an angle about axis x y z as in standard OpenGL

	  int num = sscanf(line, "%s %lf %lf %lf %lf",command, &x, &y, &z, &ang) ;
	  if (num != 5) {
		fprintf(stderr, "rotate angle x y z\n") ;
		exit(1) ;
	  }
	  vec3 r = vec3(x, y, z);
	  mat4 m = transformations.top() * rotation3D(r, ang);
	  transformations.pop();
	  transformations.push(m);
	}
	
	else if (!strcmp(command, "scale")) {
	  double x,y,z ; // Scale by x y z as in standard OpenGL

	  int num = sscanf(line, "%s %lf %lf %lf",command, &x, &y, &z) ;
	  if (num != 4) {
		fprintf(stderr, "scale x y z\n") ;
		exit(1) ;
	  }
	  vec3 s = vec3(x, y, z);
	  mat4 m = transformations.top() * scaling3D(s);
	  transformations.pop();
	  transformations.push(m);
	}

	else if (!strcmp(command, "pushTransform")) {
	  // Push the current matrix on the stack as in OpenGL
	  transformations.push(transformations.top());
	}

	else if (!strcmp(command, "popTransform")) {
	  // Pop the current matrix as in OpenGL
	  if (transformations.size() == 1) {
		cerr << "Cannot pop anymore." << endl;
	  } else {
		transformations.pop();
	  }
	}

    /************************************************************/

    /********* MISCELLANEOUS IGNORED FOR OPENGL *******************/
	
	else if (!strcmp(command, "maxdepth")) {
	  int maxdepth;
	  int num = sscanf(line, "%s %d", command, &maxdepth) ;
	  assert(num == 2) ;
	  assert(!strcmp(command, "maxdepth")) ;
	
	  maxDepth = maxdepth;
	}
	
	else if (!strcmp(command, "output")) {
	   char out[300] ;
	   int num = sscanf(line, "%s %s", command, out) ;
	   assert(num == 2) ;
	   assert(!strcmp(command, "output")) ;

	   outputPath = string(out);

	}

	else if (!strcmp(command, "samples")) {
	  int samples;
	  int num = sscanf(line, "%s %d", command, &samples) ;
	  assert(num == 2) ;
	  assert(!strcmp(command, "samples")) ;
	
	  numSamples = samples;
	}


    /*************************************************/

    /***************  LIGHTS *******************/
       else if (!strcmp(command, "directional")) {
	 double x,y,z,r,g,b;
	 int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf", command, &x, &y, &z, &r, &g, &b) ;
	 assert(num == 7) ;

	 vec3 dir = vec3(x,y,z);
	 dir = vec3(transformations.top() * vec4(dir, 0), 3);

	 lights->push_back(new DirectionalLight(dir, Color(r,g,b)));

       }

      else if (!strcmp(command, "point")) {
	 double x,y,z,r,g,b;
	 int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf", command, &x, &y, &z, &r, &g, &b) ;
	 assert(num == 7) ;

	 vec3 pos = vec3(x,y,z);
	 pos = vec3(transformations.top() * vec4(pos, 1));

	 lights->push_back(new PointLight(pos, Color(r,g,b), attenuation));

       }

      else if (!strcmp(command, "area")) {
        double x,y,z,i,j,r,g,b;
        int num = sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf", command, &x, &y, &z, &i, &j, &r, &g, &b) ;
	 assert(num == 9) ;

	 vec3 pos = vec3(x,y,z);
	 pos = vec3(transformations.top() * vec4(pos, 1));

	 lights->push_back(new AreaLight(pos, i, j, Color(r,g,b), attenuation));

       }

     else if (!strcmp(command, "attenuation")) {
       double c, l, q;
       int num = sscanf(line, "%s %lf %lf %lf", command, &c, &l, &q );
       assert(num == 4) ;
       assert(!strcmp(command, "attenuation")) ;
       attenuation[0] = c;
	   attenuation[1] = l;
	   attenuation[2] = q;
     }

     else if (!strcmp(command, "ambient")) {
       double r,g,b; 
       int num = sscanf(line, "%s %lf %lf %lf", command, &r,&g,&b) ;
       assert(num == 4) ;
       assert(!strcmp(command, "ambient")) ;
       ambient = Color(r,g,b);
     }
    /*******************************************************/

    /****************** MATERIALS ************************/

     else if (!strcmp(command, "diffuse")) {
       float diffuse[4] ; diffuse[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, diffuse, diffuse+1, diffuse+2) ;
       assert(num == 4) ; assert (!strcmp(command, "diffuse")) ;

	   curDiffuse = Color(vec3(diffuse[0], diffuse[1], diffuse[2]));
     }

     else if (!strcmp(command, "specular")) {
       float specular[4] ; specular[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, specular, specular+1, specular+2) ;
       assert(num == 4) ; assert (!strcmp(command, "specular")) ;
      
	   curSpecular = Color(vec3(specular[0], specular[1], specular[2]));
     }

     else if (!strcmp(command, "shininess")) {
       float shininess ;
       int num = sscanf(line, "%s %f", command, &shininess) ;
       assert(num == 2) ; assert (!strcmp(command, "shininess")) ;

	   curShininess = shininess;
     }

     else if (!strcmp(command, "emission")) {
       float emission[4] ; emission[3] = 1.0 ;
       int num = sscanf(line, "%s %f %f %f", command, emission, emission+1, emission+2) ;
       assert(num == 4) ; assert (!strcmp(command, "emission")) ;

	   curEmission = Color(vec3(emission[0], emission[1], emission[2]));
     }

    /*****************************************************/

    else {
      fprintf(stderr, "Unimplemented command: %s\n", command) ;
      exit(1) ;
    }
  }
}

/*
 * Ce code est une modification des tutoriaux OpenGL
 * Nehe [http://nehe.gamedev.net/]
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>
#include <sys/time.h>
#include "tiny_gc.h"
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"

/* La "surface SDL", c'est à dire la fenêtre où l'on dessine
 * obligatoirement une variable globale, car les "callbacks"
 * gérant par exemple les touches clavier en ont besoin et
 * ne la reçoive (malheureusement) pas en argument.
 */
SDL_Surface *surface;

/* fonction pour quitter */
void Quit( int returnCode )
{
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

/* fonction pour rediensionner la fenêtre */
int resizeWindow( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;
 
    /* Protect against a divide by zero */
   if ( height == 0 )
	height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return( 1 );
}

unsigned int Smooth = 0;


/* initialisation d'OpenGL */
int initGL( GLvoid )
{

    /* Enable smooth shading */
    glShadeModel( Smooth?GL_SMOOTH:GL_FLAT );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );


    glEnable( GL_LIGHTING );
    /* Enable Light One */
    glEnable( GL_LIGHT1 );

    /* Ambient Light Values */
    GLfloat LightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv( GL_LIGHT1, GL_AMBIENT, LightAmbient );
    /* Diffuse Light Values */
    GLfloat LightDiffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv( GL_LIGHT1, GL_DIFFUSE, LightDiffuse );
    /* Light Position (*/
    GLfloat LightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv( GL_LIGHT1, GL_POSITION, LightPosition );
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_CULL_FACE);

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    return( 1 );
}


/* variable globale de la scène */

half_edge tetra;
gl_object *gl_tetra;

int N=100;

void initGLScene() {  

  /* Construction d'un tétraèdre (non régulier) */

  gl_vertex *v1 = GLvertex3f(1.0,0.0,0.0);
  gl_vertex *v2 = GLvertex3f(0.0,1.0,0.0);
  gl_vertex *v3 = GLvertex3f(0.0,0.0,1.0);
  gl_vertex *v4 = GLvertex3f(-1.0,-1.0,-1.0);

  tetra = tetraedron(v1,v2,v3,v4);
  if (Smooth) {
    set_average_normal(tetra);
    gl_tetra = triangulation_to_gl_object(tetra);
  } else {
    gl_tetra = triangulation_poly_to_gl_object(tetra);
  }

}

void toggleSmooth() {
  if (Smooth) {
      Smooth = 0;
      glShadeModel( GL_FLAT );
    }
  else {
      Smooth = 1;
      glShadeModel( GL_SMOOTH );
   }
  initGLScene();
}

static unsigned int frames = 0;
static struct timeval lasttime;
 
void count_frames() {
  struct timeval curtime;

  frames++;
  gettimeofday(&curtime,NULL);
  if (curtime.tv_sec - lasttime.tv_sec >= 5) {
    printf("%.1f frames by second\n",
	   (double) frames / 
	   ((double) curtime.tv_sec - (double) lasttime.tv_sec 
	    + (double) (curtime.tv_usec -lasttime.tv_usec) * 1e-6));
    lasttime = curtime;
    frames = 0;
  }
}

void init_count_frames() {
  frames = 0;
  gettimeofday(&lasttime,NULL);
}

/* La fonction qui dessine la scène */
int drawGLScene( GLvoid )
{
    /* rotational vars for the triangle and quad, respectively */
    static GLfloat rtri;

    count_frames();

    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* dessine la sphere GLU */
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -6.0f );
    glRotatef( rtri, 0.0f, 1.0f, 1.0f );

    glColor3f(   0.7f,  0.2f,  0.2f ); 

    glDrawObject(gl_tetra);

    /* Draw it to the screen */
    SDL_GL_SwapBuffers( );

    /* Increase The Rotation Variable For The Triangle */
    rtri  += 0.2f;

    return( 1 );
}


/* fonstion de gestion du clavier */
void handleKeyPress( SDL_keysym *keysym )
{
    switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
	    /* ESC key was pressed */
	    Quit( 0 );
	    break;
	case SDLK_F1:
	    /* F1 key was pressed
	     * this toggles fullscreen mode
	     */
	    SDL_WM_ToggleFullScreen( surface );
	    break;
	case SDLK_s:
	  toggleSmooth();
	  break;
	case SDLK_p:
	case SDLK_KP_PLUS:
	case SDLK_PLUS:
	  N += 10;
	  initGLScene();
	  break;
	case SDLK_m:
	case SDLK_KP_MINUS:
	case SDLK_MINUS:
	  if (N > 10) N -= 10;
	  initGLScene();
	  break;
	default:
	    break;
	}

    return;
}

int main( int argc, char **argv )
{
  GC_init(&argc,0x1000000,0x1000);
  init_count_frames();

  /* Flags to pass to SDL_SetVideoMode */
  int videoFlags;
  /* used to collect events */
  SDL_Event event;
  /* this holds some info about our display */
  const SDL_VideoInfo *videoInfo;
  
  /* initialize SDL */
  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
	    fprintf( stderr, "Video initialization failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
	{
	    fprintf( stderr, "Video query failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    surface = SDL_SetVideoMode( 848, 480, 32,
				videoFlags );

    /* Verify there is a surface */
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	    Quit( 1 );
	}

    /* initialize OpenGL */
    initGL( );
    initGLScene();

    /* resize the initial window */
    resizeWindow( 848, 480 );
  

    /* wait for events */ 
    while ( 1 )
      {
	/* handle the events in the queue */
	
	while ( SDL_PollEvent( &event ) )
	  {
	    switch( event.type )
	      {
	      case SDL_VIDEORESIZE:
		/* handle resize event */
		surface = SDL_SetVideoMode( event.resize.w,
					    event.resize.h,
					    32, videoFlags );
		if ( !surface )
		  {
		    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
		    Quit( 1 );
		  } 
		resizeWindow( event.resize.w, event.resize.h );
		break;
	      case SDL_KEYDOWN:
		/* handle key presses */
		handleKeyPress( &event.key.keysym );
		break;
	      case SDL_QUIT:
		/* handle quit requests */
		Quit(0);
		break;
	      default:
		break;
	      }
	  }
	
	drawGLScene( );
      }

    /* Should never get here */
    return( 0 );
}

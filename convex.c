#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"
#include "convex.h"
#include "tiny_gc.h"

/* Fonction calculant le déterminant de quatre sommets A,B,C,D,
   C'est à dire le double du volume du tétraèdre, avec un signe
   indiquant l'orientation.
*/


double gl_det(gl_vertex* v1, gl_vertex* v2, gl_vertex* v3, gl_vertex* v4) {
  if (v1->vertex_type == GL_DOUBLE) {
    vecteur3d_cell V1, V2, V3;
    vec3d(&V1,v1->coord.a3d,v2->coord.a3d);
    vec3d(&V2,v1->coord.a3d,v3->coord.a3d);
    vec3d(&V3,v1->coord.a3d,v4->coord.a3d);
    
    return det3d(&V1,&V2,&V3);
  } else {
    vecteur3f_cell V1, V2, V3;
    vec3f(&V1,v1->coord.a3f,v2->coord.a3f);
    vec3f(&V2,v1->coord.a3f,v3->coord.a3f);
    vec3f(&V3,v1->coord.a3f,v4->coord.a3f);
    
    return (double) det3f(&V1,&V2,&V3);
  }
}

/* Supression des triangles visibles depuis le noeud pts.
   renvoie NULL si aucun triangle n'est supprimé, une
   arrête au bord du trou ainsi crée sinon. 

   Cette arrête vérifie donc e->next = 0 et e->prev != 0
*/
half_edge remove_visible(half_edge e, gl_vertex *pts) {
  edge_list sides = NULL;
  
  void fn(half_edge f, gl_vertex* v1, gl_vertex* v2, gl_vertex* v3) {
    /* QUESTION 2 : fonction itéré sur tous les triangles
       et qui teste si on doit garder le triangle.
       Elle conserve aussi les 3 arrêtes dans la liste sides */
		if(gl_det(v1, v2, v3, pts) > 0){
			remove_triangle(f);
		}
		
    /* INSERER ICI */
  }

  iter_triangles(e, fn);

  /* recherche dans la liste d'une arrête au bord du trou */
  /* INSERER ICI */

  return NULL;
}

/* Petite fonction pour aller chercher l'arrête suivante sur
   à partir d'une arrête au bord */
half_edge next_on_side(half_edge e) {
  e = e->opp;
  while(e->next) e = e->next;
  return e;
}

/* Fonction qui rempli un trou (l'arrête e fourni étant au bord de 
   ce trou. En créant des triangles ayant tous pour sommet le nouveau
   point pts */
void fill_hole(half_edge e, gl_vertex *pts) {

  /* Question 2 */
	
  /* INSERER ICI */
}


/* Fonction calculant l'enveloppe convexe du tableau de sommets
   fournis. nb_pts est la taille du tableau */

half_edge convex_hull(gl_vertex *pts, size_t nb_pts) {

  if (nb_pts < 4) {
    fprintf(stderr, "enveloppe for at least four points\n");
  }
 
  /* On inverse deux points si le tétraèdre initial à un volume positif */
  /* Question 3 : améliorer le code ci-dessous, pour choisir un tétraèdre
     de plus grand volume  */
  if (gl_det(&(pts[0]), &(pts[1]), &(pts[2]), &(pts[3])) > 0.0) {
    gl_vertex tmp = pts[0];
    pts[0] = pts[1];
    pts[1] = tmp;
  }
    
  /* On construit le tétraèdre initial */
  half_edge e = tetraedron(&(pts[0]), &(pts[1]), &(pts[2]), &(pts[3]));

  size_t i;

  /* On ajoute tous les autres points un par un avec les deux fonctions
     remove_visible et fill_hole */
  for(i=4;i<nb_pts;i++) {
    /* Question 2 */
    /* INSERER ICI */
  }

  return(e);    
}

double randd(double min, double max)
{
  double r=(double)rand()/(double)RAND_MAX;
  return r*(max-min)+min;
}

half_edge test_random(size_t n) {
  gl_vertex *v = GC_malloc(n * sizeof(gl_vertex));
  size_t i;

  for(i=0;i<n;i++) {
    v[i] = *(GLvertex3d(randd(-1.0,1.0),randd(-1.0,1.0),randd(-1.0,1.0)));
  }

  global_pts = v;
  nb_global_pts = n;
  return convex_hull(v,n);
}

half_edge test_simple() {
  size_t n = 5;
  gl_vertex *v = GC_malloc(n * sizeof(gl_vertex));

  v[0] = *(GLvertex3d(0.0,0.0,0.0));
  v[1] = *(GLvertex3d(1.0,0.0,0.0));
  v[2] = *(GLvertex3d(0.0,1.0,0.0));
  v[3] = *(GLvertex3d(0.0,0.0,1.0));
  v[4] = *(GLvertex3d(1.0,1.0,1.0));

  global_pts = v;
  nb_global_pts = 5;
  return convex_hull(v,n);
}

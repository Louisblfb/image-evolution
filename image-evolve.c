#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <unistd.h>
#include <string.h>

#include <cairo/cairo.h>

#define shape_cnt   100
#define point_cnt   6

/* global variables */
cairo_surface_t *goal_img;
cairo_t *goal_con;
cairo_surface_t *evol_img;
cairo_t *evol_con;
cairo_surface_t *png_get;

int width = 600;
int height = 600;

/* data structures */
typedef struct {
    float r, g, b, a;
    float pointX[point_cnt];
    float pointY[point_cnt];
    float x, y;
} poly;

poly dna_evol[shape_cnt];
poly dna_best[shape_cnt];

/* function definintions */
void copy_surface();
void main_loop();

void seed_poly();
void draw_poly();

void mutate_color();
void mutate_points();
void mutate_XY();

int 
main(int argc, char *argv[]) {

    if(argc == 1) {
        printf("error: missing argument\n");
        return 0;
    }
   
    /* load png and get w h */
    png_get = cairo_image_surface_create_from_png(argv[1]);
    width = cairo_image_surface_get_width(png_get);
    height = cairo_image_surface_get_height(png_get);

    /* create evol surface */
    evol_img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    evol_con = cairo_create(evol_img);
    cairo_scale(evol_con, width, height);

    /* create and copy png to goal surface */
    goal_img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    goal_con = cairo_create(goal_img);
    cairo_scale(goal_con, width, height);
    copy_surface(png_get, goal_con);

    main_loop();

    /* write png and cleanup */
    //cairo_surface_write_to_png(evol_img, "output.png");
    cairo_destroy(goal_con);
    cairo_destroy(evol_con);
    cairo_surface_destroy(png_get);
    cairo_surface_destroy(goal_img);
    cairo_surface_destroy(evol_img);
    return 0;
}

void 
copy_surface(cairo_surface_t *surf, cairo_t *contxt) {

    cairo_set_source_rgb(contxt, 1,1,1);
    cairo_rectangle(contxt, 0,0, width, height);
    cairo_fill(contxt);
    cairo_set_source_surface(contxt, surf, 0,0);
    cairo_paint(contxt);
}

void
main_loop() {

    char path[24];
    cairo_set_source_rgb(evol_con, 1,1,1);
    cairo_rectangle(evol_con, 0,0, width, height);
    cairo_fill(evol_con);

    seed_poly(dna_evol);
    draw_poly(dna_evol, evol_con, evol_img);
    cairo_surface_write_to_png(evol_img, "output/output.png");

    for (int i=0; i<shape_cnt; i++) {
    sprintf(path, "output/%d-output1.png", i);
    mutate_color(dna_evol, i);
    draw_poly(dna_evol, evol_con, evol_img);
    cairo_surface_write_to_png(evol_img, path);

    sprintf(path, "output/%d-output2.png", i);
    mutate_points(dna_evol, i);
    draw_poly(dna_evol, evol_con, evol_img);
    cairo_surface_write_to_png(evol_img, path);

    sprintf(path, "output/%d-output3.png", i);
    mutate_XY(dna_evol, i);
    draw_poly(dna_evol, evol_con, evol_img);
    cairo_surface_write_to_png(evol_img, path);}
}

void
seed_poly(poly dna[]) {

    /* initial seed of all dna attributes */
    srand(time(NULL));
    for (int i=0; i<shape_cnt; i++) {
        for (int j=0; j<point_cnt; j++) {
            dna[i].pointX[j] = (float)rand() / (float)RAND_MAX;
            dna[i].pointY[j] = (float)rand() / (float)RAND_MAX;
            printf("point %d: %f ", j, dna[i].pointX[j]);
        }
        dna[i].x = (float)rand() / (float)RAND_MAX; 
        dna[i].y = (float)rand() / (float)RAND_MAX; 
        dna[i].r = (float)rand() / (float)RAND_MAX; 
        dna[i].g = (float)rand() / (float)RAND_MAX; 
        dna[i].b = (float)rand() / (float)RAND_MAX; 
        dna[i].a = (float)rand() / (float)RAND_MAX; 
        printf("X: %f Y: %f R: %f G: %f B: %f A: %f\n", 
                dna[i].x, dna[i].y,dna[i].g,dna[i].b,dna[i].a,dna[i].a);
    }
}

void 
draw_poly(poly dna[], cairo_t *contxt, cairo_surface_t *surf) {

    /* draw dna to surface */
    cairo_set_line_width(contxt, 0);
    for (int i=0; i<shape_cnt; i++) {
        cairo_set_source_rgba(contxt, dna[i].r,dna[i].g,dna[i].b,dna[i].a);
        cairo_move_to(contxt, dna[i].x, dna[i].y);
        for (int j=0; j<point_cnt; j++) {
            cairo_line_to(contxt, dna[i].pointX[j], dna[i].pointY[j]);
        }
        cairo_fill(contxt);
    }
}

void
mutate_color(poly dna[], int i) {

    dna[i].r = (float)rand() / (float)RAND_MAX; 
    dna[i].g = (float)rand() / (float)RAND_MAX; 
    dna[i].b = (float)rand() / (float)RAND_MAX; 
    dna[i].a = (float)rand() / (float)RAND_MAX; 
  
}

void
mutate_points(poly dna[], int i) {

    for (int j=0; j<point_cnt; j++) {
        dna[i].pointX[j] = (float)rand() / (float)RAND_MAX;
        dna[i].pointY[j] = (float)rand() / (float)RAND_MAX;
    }
}

void 
mutate_XY(poly dna[], int i) {

    dna[i].x = (float)rand() / (float)RAND_MAX; 
    dna[i].y = (float)rand() / (float)RAND_MAX; 
}










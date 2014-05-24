#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <cairo/cairo.h>

#define shape_cnt   100
#define point_cnt   6
#define ABS(val) ((val) < 0 ? -(val) : (val))

/* global variables */
cairo_surface_t *goal_img;
cairo_t *goal_con;
cairo_surface_t *evol_img;
cairo_t *evol_con;
cairo_surface_t *png_get;

int width = 600;
int height = 600;
int pert;
float diff = 0.0;

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
float get_fitness();
void copy_dna();

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

    main_loop(atoi(argv[2]));

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
main_loop(int argc) {
    int cycle =0, check = 0;
    float best_fit;
    float evol_fit;
    cairo_set_source_rgb(evol_con, 1,1,1);
    cairo_rectangle(evol_con, 0,0, width, height);
    cairo_fill(evol_con);

    seed_poly(dna_evol);
    copy_dna(dna_evol, dna_best);
    draw_poly(dna_best, evol_con, evol_img);
    best_fit = get_fitness(goal_img, evol_img);
    pert = get_fitness(goal_img, goal_img);
    cairo_surface_write_to_png(evol_img, "initial.png");

    while(cycle < argc) {
        int dec_poly = (int)((rand() / (double)RAND_MAX) * (shape_cnt));
        int dec_op = (int)((rand() / (double)RAND_MAX) * (4));
        //printf("poly: %d - operation: %d - ", dec_poly, dec_op);
       
        if (dec_op == 0) 
            mutate_color(dna_evol, dec_poly);
        else if (dec_op == 1)
            mutate_points(dna_evol, dec_poly);
        else if (dec_op == 2)
            mutate_XY(dna_evol, dec_poly);
        else if (dec_op == 3) {
            int ran_poly = (int)((rand() / (double)RAND_MAX) * (shape_cnt));
            poly temp = dna_evol[dec_poly];
            dna_evol[dec_poly] = dna_evol[ran_poly];
            dna_evol[ran_poly] = temp;
            //printf("poly: %d to poly: %d\n", dec_poly, ran_poly);
        }
        
        draw_poly(dna_evol, evol_con, evol_img); 
        evol_fit = get_fitness(goal_img, evol_img);

        if (evol_fit < best_fit) {
            printf("cycle: %d/%d fitness: %0.6f%% poly: %d\n", cycle, argc, diff, dec_poly);
            copy_dna(dna_evol, dna_best);
            best_fit = evol_fit;
        }
    
        else if (check == 100) {
            printf("cycle: %d\n", cycle);
            check = 0;
        }        

        else 
            copy_dna(dna_best, dna_evol);
       
        if (check == 100)
            check = 0; 
        cycle++;
        check++;
    }

    cairo_surface_write_to_png(evol_img, "final.png");
}

void
seed_poly(poly dna[]) {

    /* initial seed of all dna attributes */
    srand(getpid() + time(NULL));
    for (int i=0; i<shape_cnt; i++) {
        for (int j=0; j<point_cnt; j++) {
            dna[i].pointX[j] = (float)rand() / (float)RAND_MAX;
            dna[i].pointY[j] = (float)rand() / (float)RAND_MAX;
            /*printf("point %d: %f ", j, dna[i].pointX[j]);*/
        }
        dna[i].x = (float)rand() / (float)RAND_MAX; 
        dna[i].y = (float)rand() / (float)RAND_MAX; 
        dna[i].r = (float)rand() / (float)RAND_MAX; 
        dna[i].g = (float)rand() / (float)RAND_MAX; 
        dna[i].b = (float)rand() / (float)RAND_MAX; 
        dna[i].a = (float)rand() / (float)RAND_MAX; 
        /*printf("X: %f Y: %f R: %f G: %f B: %f A: %f\n", 
                dna[i].x, dna[i].y,dna[i].g,dna[i].b,dna[i].a,dna[i].a);*/
    }
}

void 
draw_poly(poly dna[], cairo_t *contxt, cairo_surface_t *surf) {

    /* draw dna to surface */
    //cairo_set_source_rgb(contxt, 1,1,1);
    //cairo_rectangle(contxt, 0,0, width,height);
    //cairo_fill(contxt);

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

    //srand(time(NULL));
    dna[i].r = (float)rand() / (float)RAND_MAX; 
    dna[i].g = (float)rand() / (float)RAND_MAX; 
    dna[i].b = (float)rand() / (float)RAND_MAX; 
    dna[i].a = (float)rand() / (float)RAND_MAX; 
  
}

void
mutate_points(poly dna[], int i) {

    //srand(time(NULL));
    for (int j=0; j<point_cnt; j++) {
        dna[i].pointX[j] = (float)rand() / (float)RAND_MAX;
        dna[i].pointY[j] = (float)rand() / (float)RAND_MAX;
    }
}

void 
mutate_XY(poly dna[], int i) {

    //srand(time(NULL));
    dna[i].x = (float)rand() / (float)RAND_MAX; 
    dna[i].y = (float)rand() / (float)RAND_MAX; 
}

float 
get_fitness(cairo_surface_t *surface_a, cairo_surface_t *surface_b) {

    unsigned char *goal_surf = cairo_image_surface_get_data(surface_a);
    unsigned char *test_surf = cairo_image_surface_get_data(surface_b);

    float max_fitness;
    int fitness;

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {                        
            int pixel = y*width*4 + x*4;
            
            unsigned char goal_a = goal_surf[pixel];
            unsigned char goal_r = goal_surf[pixel+1];
            unsigned char goal_g = goal_surf[pixel+2];
            unsigned char goal_b = goal_surf[pixel+3];

            unsigned char test_a = test_surf[pixel];
            unsigned char test_r = test_surf[pixel+1];
            unsigned char test_g = test_surf[pixel+2];
            unsigned char test_b = test_surf[pixel+3];

            max_fitness += goal_a + goal_r + goal_g + goal_b;
            
            fitness += ABS(test_a - goal_a);
            fitness += ABS(test_r - goal_r);
            fitness += ABS(test_g - goal_g);
            fitness += ABS(test_b - goal_b);

            }
    }
     
    diff = ((max_fitness-fitness) / (float)max_fitness) * 100;
    //printf("fitness: %0.6f%%\n", ((max_fitness-fitness)/(float)max_fitness) * 100 );

    return fitness;
}

void
copy_dna(poly dna_a[], poly dna_b[]) {

    for (int i=0; i<shape_cnt; i++) {
        for (int j=0; j<point_cnt; j++) {
            dna_b[i].pointX[j] = dna_a[i].pointX[j];
            dna_b[i].pointY[j] = dna_a[i].pointY[j];
        }
        dna_b[i].x = dna_a[i].x;
        dna_b[i].y = dna_a[i].y;
        dna_b[i].r = dna_a[i].r;
        dna_b[i].g = dna_a[i].g;
        dna_b[i].b = dna_a[i].b;
        dna_b[i].a = dna_a[i].a;

    } 
} 






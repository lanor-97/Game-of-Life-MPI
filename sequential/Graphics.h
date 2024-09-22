#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "iostream"
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "Matrix.h"
using namespace std;

class Graphics  {
public:
	~Graphics();
	static const int 	SCREEN = 600;
	static const int 	BITDIM = 30;
	static const int 	dim = SCREEN / BITDIM;
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* dead = NULL;
	ALLEGRO_BITMAP* alive = NULL;
	ALLEGRO_EVENT_QUEUE* events = NULL;

	ALLEGRO_DISPLAY* getDisplay() const  { return display; }
	bool initializeGraphics();
	bool inputPhase(Matrix&);
	void print(Matrix&);
};

bool Graphics::initializeGraphics()  {
	if(!al_init())
		return false;

	if(!al_init_image_addon())
		return false;

	if(!al_install_mouse())
		return false;

	if(!al_install_keyboard())
		return false;

	display = al_create_display(SCREEN, SCREEN);
	if(!display)
		return false;

	events = al_create_event_queue();
	if(!events)  {
		al_destroy_display(display);
		return false;
	}

	dead = al_load_bitmap("dead.png");
	alive = al_load_bitmap("alive.png");
	if(!dead && !alive)  {
		al_destroy_display(display);
		al_destroy_event_queue(events);
		return false;
	}

	al_register_event_source(events, al_get_display_event_source(display));
	al_register_event_source(events, al_get_mouse_event_source());
	al_register_event_source(events, al_get_keyboard_event_source());

	return true;
}

bool Graphics::inputPhase(Matrix& m)  {
	print(m);

	while(true)  {
	    ALLEGRO_EVENT ev;
	    al_wait_for_event(events, &ev);

	    if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)  {
	    	if(ev.mouse.button == 1)  {
		    	m.data[ev.mouse.x / BITDIM][ev.mouse.y / BITDIM] = true;
		    }
		    else  {
		    	m.data[ev.mouse.x / BITDIM][ev.mouse.y / BITDIM] = false;
		    }
	    	print(m);
	    }
	    else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
	    	return false;
	    }
	    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
	    	if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
	    		return true;
	    	if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
	    		return false;
	   	}
	}
}

void Graphics::print(Matrix& m) {
    for(int i = 0; i < dim; i++)  {
        for(int j = 0; j < dim; j++)  {
            if(m.data[i][j])
                al_draw_bitmap(alive, BITDIM*i, BITDIM*j, 0);
            else
                al_draw_bitmap(dead, BITDIM*i, BITDIM*j, 0);
        }
        al_flip_display();
    }
}

Graphics::~Graphics()  {
	al_destroy_display(display);
    al_destroy_event_queue(events);
    al_destroy_bitmap(dead);
    al_destroy_bitmap(alive);
}

#endif
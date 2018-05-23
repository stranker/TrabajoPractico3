#include <stdio.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "Player.h"
#include "Enemy.h"

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;

bool rect_overlaps(const float x1, const float y1, const float w1, const float h1, const float x2, const float y2, const float w2, const float h2) {

	return ((x1 < x2 + w2) && (x2 < x1 + w1) && (y1 < y2 + h2) && (y2 < y1 + h1));

}

int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	Player* mario;
	Enemy* goomba;
	bool redraw = true;
	bool gameOver = false;

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_init_image_addon()) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to initialize the Keyboard!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	mario = new Player(0, 0);
	goomba = new Enemy(SCREEN_W, 300);

	al_set_target_bitmap(mario->GetSprite());
	al_set_target_bitmap(goomba->GetSprite());

	al_set_target_bitmap(al_get_backbuffer(display));

	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(mario->GetSprite());
		al_destroy_bitmap(goomba->GetSprite());
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(255, 255, 255));

	al_flip_display();

	al_start_timer(timer);

	while (!gameOver)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_bitmap(mario->GetSprite(), mario->GetPosX(), mario->GetPosY(), 0);
			al_draw_bitmap(goomba->GetSprite(), goomba->GetPosX(), goomba->GetPosY(), 0);

			al_flip_display();
		}

		mario->Update(ev);
		goomba->Update(SCREEN_W,SCREEN_H);

		if (rect_overlaps(mario->GetPosX(),mario->GetPosY(),mario->CollisionW(),mario->CollisionH(), goomba->GetPosX(), goomba->GetPosY(),goomba->CollisionW(),goomba->CollisionH()))
		{
			gameOver = true;
		}
	}

	al_destroy_bitmap(mario->GetSprite());
	al_destroy_bitmap(goomba->GetSprite());
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	delete mario;
	delete goomba;

	return 0;
}
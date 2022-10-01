#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 720

#define P_WIDTH 50
#define P_HEIGHT 50

struct player {
  double x1, x2, y1, y2;
  double vel, acel;
};
typedef struct player player_t;

int main() {
  if (!al_init()) {
    printf("erro ao inicializar allegro\n");
    return 1;
  }

  if (!al_install_keyboard()) {
    printf("erro ao inicializar teclado\n");
    return 1;
  }

  if (!al_init_image_addon()) {
    printf("erro ao inicializar addon de imagem\n");
    return 1;
  }

  if (!al_init_primitives_addon()) {
    printf("erro ao inicializar addon de primitivas\n");
    return 1;
  }

  if (!al_init_font_addon()) {
    printf("erro ao inicializar addon de fonte\n");
    return 1;
  }

  ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
  if (!timer) {
    printf("erro ao inicializar timer\n");
    return 1;
  }

  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  if (!queue) {
    printf("erro ao inicializar queue\n");
    return 1;
  }

  ALLEGRO_FONT *font = al_create_builtin_font();
  if (!font) {
    printf("erro ao inicializar font\n");
    return 1;
  }

  ALLEGRO_DISPLAY *disp = al_create_display(WIDTH, HEIGHT);
  if (!disp) {
    printf("erro ao inicializar disp\n");
    return 1;
  }

  ALLEGRO_KEYBOARD_STATE kbdstate;

  player_t player;
  player.x1 = WIDTH / 2.0;
  player.y1 = HEIGHT / 2.0;
  player.x2 = player.x1 + P_WIDTH;
  player.y2 = player.y1 + P_HEIGHT;
  player.vel = 8;
  player.acel = 0.1;

  // al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  ALLEGRO_EVENT event;

  int render = 0;
  int fim = 0;
  double t = 0;
  int vel = player.vel;

  al_start_timer(timer);
  while (true) {
    al_wait_for_event(queue, &event);
    al_get_keyboard_state(&kbdstate);

    if (event.type == ALLEGRO_EVENT_TIMER)
      render = 1;

    if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE))
      fim = 1;
    else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) {
      if (player.y1 <= 0)
        player.y1 = 0;
      else
        player.y1 -= 20;
      t = 0;
      vel = player.vel;
    }
    else
    {
      if (player.y1 + P_HEIGHT >= HEIGHT)
      {
        player.y1 = HEIGHT - P_HEIGHT;
      }
      else
      {
        vel += player.acel * t;
        player.y1 += vel;
        t += 0.5;
      }
    }
    
    player.x2 = player.x1 + P_WIDTH;
    player.y2 = player.y1 + P_HEIGHT;

    if (fim)
      break;

    if (render && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      al_draw_filled_rectangle(player.x1, player.y1, player.x2, player.y2,
                               al_map_rgb_f(1, 1, 1));

      al_flip_display();

      render = 0;
    }
  }

  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_event_queue(queue);
  al_uninstall_keyboard();

  return 0;
}

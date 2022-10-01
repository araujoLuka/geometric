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
#define MAX_JUMP 150

#define GROUND 100

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
  player.vel = 0;
  player.acel = 0.1; // aceleração da gravidade

  al_register_event_source(queue, al_get_timer_event_source(timer));
  ALLEGRO_EVENT event;

  bool render = false;
  bool jump = false;
  bool up = false;
  int fim = 0;
  double t = 0;
  int vel = player.vel;

  al_start_timer(timer);
  while (true) {
    al_wait_for_event(queue, &event);
    al_get_keyboard_state(&kbdstate);

    if (event.type == ALLEGRO_EVENT_TIMER)
      render = true;

    player.vel += player.acel * t;
    t += 0.5;

    player.y1 += player.vel;
    player.y2 = player.y1 + P_HEIGHT;

    if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE))
      fim = 1;
    else if (al_key_down(&kbdstate, ALLEGRO_KEY_SPACE) && (!jump)) {
      printf("pula\n");
      jump = true;
      player.vel = -10;
    } else if (player.y1 + P_HEIGHT >= HEIGHT - GROUND) { // ground collision
      printf("chao\n");
      jump = false;
      t = 0;
      player.vel = 0;
      player.y1 = HEIGHT - GROUND - P_HEIGHT;
      player.y2 = player.y1 + P_HEIGHT;
    }

    if (fim)
      break;

    if (render && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      al_draw_filled_rectangle(player.x1, player.y1, player.x2, player.y2,
                               al_map_rgb_f(1, 1, 1));

      al_draw_filled_rectangle(0, HEIGHT - GROUND, WIDTH, HEIGHT,
                               al_map_rgb_f(255, 0, 0));

      al_flip_display();

      render = true;
    }
  }

  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_event_queue(queue);
  al_uninstall_keyboard();

  return 0;
}

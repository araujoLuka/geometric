#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1280
#define HEIGHT 720

#define P_WIDTH 50
#define P_HEIGHT 50
#define OB_SIZE 50
#define MAX_JUMP 150

#define GROUND 620

enum OBSTACLES {RECT, SPIKE};

struct vector
{
  double x, y;
};
typedef struct vector vec2;

struct player {
  double x1, x2, y1, y2;
  double vel, acel;
};
typedef struct player player_t;

struct obst
{
  vec2 p1, p2, p3;
  int type;
  bool sobe;
};
typedef struct obst obst_t;

void make_rect(obst_t *ob, int i)
{
  ob[i].p1.x = WIDTH + 500 * i;
  ob[i].p1.y = GROUND - OB_SIZE;
  ob[i].p2.x = ob[i].p1.x + OB_SIZE;
  ob[i].p2.y = ob[i].p1.y + OB_SIZE;
  ob[i].type = RECT;
  ob[i].sobe = true;
}

int main() {
  if (!al_init()) {
    printf("erro ao inicializar allegro\n");
    return 1;
  }

  if (!al_install_keyboard()) {
    printf("erro ao inicializar teclado\n");
    return 1;
  }

  al_install_mouse();

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
  ALLEGRO_MOUSE_STATE mse;

  player_t player;
  player.x1 = WIDTH / 2.5;
  player.y1 = HEIGHT / 2.0;
  player.x2 = player.x1 + P_WIDTH;
  player.y2 = player.y1 + P_HEIGHT;
  player.vel = 0;
  player.acel = 0.3; // aceleração da gravidade


  obst_t ob[10];
  int n=0;

  for (n=0; n < 5; n++)
    make_rect(ob, n);

  al_register_event_source(queue, al_get_timer_event_source(timer));
  ALLEGRO_EVENT event;

  bool render = false;
  bool jump = true;
  int fim = 0;
  double t = 0;
  bool over = false;

  srand(time(NULL));

  al_start_timer(timer);
  while (true) {
    al_wait_for_event(queue, &event);
    al_get_keyboard_state(&kbdstate);
    al_get_mouse_state(&mse);

    if (event.type == ALLEGRO_EVENT_TIMER)
      render = true;

    player.vel += player.acel * t;
    t += 0.5;

    player.y1 += player.vel;
    player.y2 = player.y1 + P_HEIGHT;

    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
    {
      player.y1 = event.mouse.y;
      player.y2 = player.y1 + P_HEIGHT;
    }

    for (int i=0; i < n; i++)
    {
      ob[i].p1.x -= 13;
      if (ob[i].p1.x + OB_SIZE < 0)
        ob[i].p1.x = WIDTH + rand() % 1000 + 500;

      ob[i].p2.x = ob[i].p1.x + OB_SIZE;

      if (player.y2 > ob[i].p1.y && player.vel > 0)
      {
        if (player.x1 < ob[i].p1.x && player.x2 < ob[i].p1.x)
          ob[i].sobe = false;
      }

      if (player.y2 > ob[i].p1.y)
      {
        if (player.x2 > ob[i].p1.x && player.x1 < ob[i].p2.x)
        {
          if (player.vel > 0 && ob[i].sobe )
          {
            jump = false;
            t = 0;
            player.vel = 0;
            player.y1 = ob[i].p1.y - P_HEIGHT;
            player.y2 = player.y1 + P_HEIGHT;
          }
          else
          {
            over = true;
          }
        }
      }
    }

    if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE))
      fim = 1;
    else if (al_key_down(&kbdstate, ALLEGRO_KEY_SPACE) && (!jump) && player.vel == 0) {
      for (int i=0; i < n; i++)
        ob[i].sobe = true;
      jump = true;
      player.vel = -13;
    } else if (player.y1 + P_HEIGHT >= GROUND) { // ground collision
      jump = false;
      t = 0;
      player.vel = 0;
      player.y1 = GROUND - P_HEIGHT;
      player.y2 = player.y1 + P_HEIGHT;
    }

    if (fim)
      break;

    if (render && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      al_draw_filled_rectangle(player.x1, player.y1, player.x2, player.y2,
                               al_map_rgb(255, 255, 255));

      for (int i=0; i < n; i++)
        al_draw_filled_rectangle(ob[i].p1.x, ob[i].p1.y, ob[i].p2.x, ob[i].p2.y,
                                al_map_rgb(255, 200, 0));

      al_draw_filled_rectangle(0, GROUND, WIDTH, HEIGHT,
                               al_map_rgb(255, 0, 0));

      al_flip_display();

      render = true;
    }

    if (over)
    {
      for (int i=0; i < n; i++)
      {
        ob[i].p1.x = WIDTH * 2 + 500 * i;
        ob[i].p2.x = ob[i].p1.x + OB_SIZE;
      }
      while (1)
      {
        al_get_keyboard_state(&kbdstate);
        if (al_key_down(&kbdstate, ALLEGRO_KEY_F9))
          break;
      }
      
      over = false;
    }
  }

  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_event_queue(queue);
  al_uninstall_keyboard();

  return 0;
}

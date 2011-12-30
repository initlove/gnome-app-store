#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>

#define RECT_WIDTH      400
#define RECT_HEIGHT     300
#define N_RECTS         7

static const gchar *rect_color[N_RECTS] = {
  "#edd400",
  "#f57900",
  "#c17d11",
  "#73d216",
  "#3465a4",
  "#75507b",
  "#cc0000"
};

static ClutterActor *rectangle[N_RECTS]; 
static ClutterActor *viewport = NULL;

static void
on_drag_end (ClutterDragAction   *action,
             ClutterActor        *actor,
             gfloat               event_x,
             gfloat               event_y,
             ClutterModifierType  modifiers)
{
  gfloat viewport_x = clutter_actor_get_x (viewport);
  gfloat offset_x;
  gint child_visible;

  /* check if we're at the viewport edges */
  if (viewport_x > 0)
    {
      clutter_actor_animate (viewport, CLUTTER_EASE_OUT_BOUNCE, 250,
                             "x", 0.0,
                             NULL);
      return;
    }

  if (viewport_x < (-1.0f * (RECT_WIDTH * (N_RECTS - 1))))
    {
      clutter_actor_animate (viewport, CLUTTER_EASE_OUT_BOUNCE, 250,
                             "x", (-1.0f * (RECT_WIDTH * (N_RECTS - 1))),
                             NULL);
      return;
    }

  /* animate the viewport to fully show the child once we pass
   * a certain threshold with the dragging action
   */
  offset_x = fabsf (viewport_x) / RECT_WIDTH + 0.5f;
  if (offset_x > (RECT_WIDTH * 0.33))
    child_visible = (int) offset_x + 1;
  else
    child_visible = (int) offset_x;

  /* sanity check on the children number */
  child_visible = CLAMP (child_visible, 0, N_RECTS);

return ; //dliang
  clutter_actor_animate (viewport, CLUTTER_EASE_OUT_QUAD, 250,
                         "x", (-1.0f * RECT_WIDTH * child_visible),
                         NULL);
}

int
main (int argc, gchar **argv)
{
  ClutterActor *stage;
  ClutterActor *scroll;
  ClutterAction *action;
  gint i;

  if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    return 1;

  stage = clutter_stage_new ();
  clutter_stage_set_title (CLUTTER_STAGE (stage), "Scrolling");
  clutter_actor_set_size (stage, 800, 600);
  g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

  ClutterActor *text;
  gchar *str = NULL;
  gint len;

  g_file_get_contents ("/tmp/text", &str, &len, NULL);
  printf ("str is %s\n", str);
  text = clutter_text_new ();
  clutter_actor_set_width (text, 400);
  clutter_text_set_line_wrap (text, TRUE);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), text);
  clutter_text_set_text (text, str);

  clutter_actor_show (stage);

  clutter_main ();

  return EXIT_SUCCESS;
}

/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2023 Beaver - Gum Balls Gimp Plugin

Graph here to test plugin without installing paste this syntax inside Gimp's GEGL Graph plugin.
(Requres my plugins Bokeh, thresholdalpha2, and bevel. All which ship with this plugin.

Start of Syntax

id=contain
src aux=[ ref=contain
lb:bokeh
opacity value=3.5
color-overlay value=#e569ff
id=1
gimp:layer-mode layer-mode=hsv-value composite-mode=auto  opacity=0.30 aux=[ ref=1 lb:bevel radius1=10 azimuth=32  ]
lb:threshold-alpha

median-blur radius=0
]


--end of Syntax--
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

#define fixgraph \
" opacity value=3 median-blur radius=0 gaussian-blur std-dev-x=0.5 std-dev-y=0.5  median-blur radius=0  "\

#define smoothballs \
"  gimp:threshold-alpha value=0.5500 lb:edgesmooth gaus=1.2 value=4 alpha-percentile2=30 median-blur radius=0 opacity value=2 median-blur radius=0 lb:threshold-alpha id=1 src-in aux=[ ref=1 median-blur radius=11 ]  "\

enum_start (confetti_balls)
  enum_value (balls, "balls",
              N_("Balls"))
  enum_value (confetti, "confetti",
              N_("Confetti"))
enum_end (confettiballs)



property_enum (type, _("Content to rain everywhere"),
    confettiballs, confetti_balls,
     balls)
  description (_("Choose between balls or confetti"))


property_color (color, _("Color"), "#ffef00")

property_seed    (seed, _("Random seed"), rand1)
    description  (_("The random seed for balls or confetti"))

property_int (ballsize, _("Size"), 15)
    description (_("Make the balls or confetti bigger"))
  value_range   (1, 50)
  ui_range   (1, 50)
ui_meta ("visible", "!type {confetti}" )

property_double (confetti, _("Confetti meter"), 70.0)
    description (_("Cubism for a confetti effect"))
    value_range (5.0, 90.0)
    ui_meta     ("unit", "pixel-distance")
ui_meta ("visible", "!type {balls}" )

property_double (confettisize, _("Confetti size"), 0.5)
    description (_("Cubism tile saturation controls confetti size"))
    value_range (0.4, 1.0)
    ui_meta     ("unit", "pixel-distance")
ui_meta ("visible", "!type {balls}" )

property_seed    (confettiseed, _("Random seed for confetti"), rand2)
    description  (_("The random seed for confetti"))
ui_meta ("visible", "!type {balls}" )


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     balls
#define GEGL_OP_C_SOURCE balls.c

#include "gegl-op.h"

typedef struct
{
  GeglNode *input;
  GeglNode *b1;
  GeglNode *b2;
  GeglNode *c1;
  GeglNode *replace;
  GeglNode *nop1;
  GeglNode *fix2;
  GeglNode *fix1;
  GeglNode *o1;
  GeglNode *o2;
  GeglNode *ta1;
  GeglNode *value1;
  GeglNode *bev1;
  GeglNode *nopbev1;
  GeglNode *sb1;
  GeglNode *cubism;
  GeglNode *nothing;
  GeglNode *output;
}State;

static void attach (GeglOperation *operation)
{
}

static void update_graph (GeglOperation *operation)
{

  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);

  State *state = o->user_data = g_malloc0 (sizeof (State));

  state->input    = gegl_node_get_input_proxy (gegl, "input");
  state->output   = gegl_node_get_output_proxy (gegl, "output");

  state->b1 = gegl_node_new_child (gegl,
                                  "operation", "lb:bokeh", "amount", 0.050, "size", 1, "seed", 0,
                                  NULL);

  state->b2 = gegl_node_new_child (gegl,
                                  "operation", "lb:bokeh",
                                  NULL);


  state->c1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  state->replace = gegl_node_new_child (gegl,
                                  "operation", "gegl:src",
                                  NULL);

  state->sb1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", smoothballs,
                                  NULL);

  state->fix1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 0,   "abyss-policy", 0,
                                  NULL);

  state->fix2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 0,  "abyss-policy", 0,
                                  NULL);

  state->nop1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);


  state->bev1 = gegl_node_new_child (gegl,
                                  "operation", "lb:bevel", "radius1", 15.0, "bevel1", 0.0,
                                  NULL);

  state->nopbev1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

  state->o1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 5.5,
                                  NULL);

  state->o2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 3.5,
                                  NULL);


  state->ta1 = gegl_node_new_child (gegl,
                                  "operation", "lb:threshold-alpha",  "alphadegree", 0.5,
                                  NULL);

state->value1 = gegl_node_new_child (gegl,
                              "operation", "gimp:layer-mode", "layer-mode", 40, "composite-mode", 0, "opacity", 0.35, NULL);

  state->cubism = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

  state->nothing = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

  gegl_operation_meta_redirect (operation, "seed", state->b1, "seed");
  gegl_operation_meta_redirect (operation, "seed", state->b2, "seed");
  gegl_operation_meta_redirect (operation, "color", state->c1, "value");
  gegl_operation_meta_redirect (operation, "ballsize", state->b1, "size");
  gegl_operation_meta_redirect (operation, "ballsize", state->b2, "size");
  gegl_operation_meta_redirect (operation, "confetti", state->cubism, "tile-size");
  gegl_operation_meta_redirect (operation, "confettisize", state->cubism, "tile-saturation");
  gegl_operation_meta_redirect (operation, "confettiseed", state->cubism, "seed");


switch (o->type) {
        break;
    case balls:
  gegl_node_link_many (state->input,  state->nop1, state->replace, state->fix2, state->output, NULL);
  gegl_node_link_many (state->nop1, state->b1,  state->o1, state->c1, state->nopbev1, state->value1,  state->fix1,  state->sb1, NULL);
  gegl_node_connect (state->value1, "aux", state->bev1, "output");
  gegl_node_link_many (state->nopbev1, state->bev1,  NULL);
  gegl_node_connect (state->replace, "aux", state->sb1, "output");
        break;
    case confetti:
  gegl_node_link_many (state->input,   state->nop1, state->replace, state->fix2,  state->output, NULL);
  gegl_node_link_many (state->nop1, state->b2, state->cubism,  state->o2, state->c1, state->nopbev1, state->value1,  state->fix1, NULL);
  gegl_node_connect (state->value1, "aux", state->bev1, "output");
  gegl_node_link_many (state->nopbev1, state->bev1,  NULL);
  gegl_node_connect (state->replace, "aux", state->fix1, "output");
    }
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_meta_class->update = update_graph;
operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:gumballs",
    "title",       _("Gum Balls and Confetti"),
    "reference-hash", "confejkaefkftdtetti",
    "description", _("Render gum balls and confetti on canvas"),
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Gum balls and confetti..."),
    NULL);
}

#endif

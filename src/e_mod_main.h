#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>

E_API extern E_Module_Api e_modapi;

E_API void *e_modapi_init     (E_Module *m);
E_API int   e_modapi_shutdown (E_Module *m);
E_API int   e_modapi_save     (E_Module *m);

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

static E_Module *_module = NULL; // a handle to this module given to us
struct _Config
{
  Eina_List *items;

  E_Module *module;
  E_Config_Dialog *config_dialog;
};

struct _Config_Item
{
  const char *id;
  Eina_Bool slide;
  int switch_time;
  int dir_value;
  int enable_switch;
  char *current_dir;
};

void e_int_config_wp_slideshow_module(Evas_Object *parent, Config_Item *ci);

extern Config *wp_slideshow_config;


/**
 * @addtogroup Optional_Gadgets
 * @{
 *
 * @defgroup Module_Wallpaper_Slideshow Wallpaper_Slideshow
 *
 * Slide Wallpapers with Timer
 *
 * @}
 */

#endif

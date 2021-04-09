#include "e_mod_main.h"

////////////////////////////////////////////////////////////////////////////
// module "global tracking" stuff for this module
// static E_Module *_module = NULL; // a handle to this module given to us


////////////////////////////////////////////////////////////////////////////

// per gadget instance data
typedef struct {
   E_Gadcon_Client *gcc;
   E_Gadcon_Popup  *popup;
   Evas_Object     *popup_label;
   Ecore_Timer     *wp_timer;
   Config_Item     *cfg;
} Instance;

// store private data for all the instances of the gadget - may be more than 1
static Eina_List *_instances = NULL;

////////////////////////////////////////////////////////////////////////////
// gadget controller class - delcare
static E_Gadcon_Client *_gc_init     (E_Gadcon *gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client *gcc);
static void             _gc_orient   (E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static const char      *_gc_label    (const E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon     (const E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new   (const E_Gadcon_Client_Class *client_class);

Config *wp_slideshow_config = NULL;


static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static E_Action *act = NULL;

static void
_popup_free(Instance *inst)
{
   E_FREE_FUNC(inst->popup, e_object_del);
}

static void
_cb_popup_del(void *obj)
{
   _popup_free(e_object_data_get(obj));
}

static void
_cb_popup_comp_del(void *data, Evas_Object *obj)
{
   _popup_free(data);
}

static Eina_Bool
_cb_key_down(void *data, Ecore_Event_Key *ev)
{
   if (!strcmp(ev->key, "Escape")) _popup_free(data);
   return ECORE_CALLBACK_PASS_ON;
}


static Eina_Bool
wp_switch(void *data)
{
   Eina_List *files_new = NULL, *files = NULL, *l;

   const char *file_dir = "";
   const char *file_in_dir;
   char bg_file[PATH_MAX];
   const char *file_set = "";
   int x = 0;
   int y = 0;

   Eina_Strbuf *tmp;
   tmp = eina_strbuf_new();

   file_dir = data;

//TODO rotated image if needed - EXIF data
   files = ecore_file_ls(file_dir);

   if (eina_list_count(files) > 0)
     {
        EINA_LIST_FOREACH(files, l, file_in_dir)
        {
          if ((!eina_str_has_extension(file_in_dir, ".edj")) &&
             (!eina_str_has_extension(file_in_dir, ".gif")) &&
             (!eina_str_has_extension(file_in_dir, ".png")) &&
             (!eina_str_has_extension(file_in_dir, ".jpg")) &&
             (!eina_str_has_extension(file_in_dir, ".jpeg")) &&
             (!eina_str_has_extension(file_in_dir, ".mp4")) &&
             (!eina_str_has_extension(file_in_dir, ".m4v")) &&
             (!eina_str_has_extension(file_in_dir, ".mp2")) &&
             (!eina_str_has_extension(file_in_dir, ".mpg")) &&
             (!eina_str_has_extension(file_in_dir, ".mpeg")) &&
             (!eina_str_has_extension(file_in_dir, ".avi")) &&
             (!eina_str_has_extension(file_in_dir, ".mov")) &&
             (!eina_str_has_extension(file_in_dir, ".ogg")) &&
             (!eina_str_has_extension(file_in_dir, ".ogv")) &&
             (!eina_str_has_extension(file_in_dir, ".ts")) &&
             (!eina_str_has_extension(file_in_dir, ".mov")))
            {
               l = eina_list_remove(l, file_in_dir);
               continue;
            }
         files_new = eina_list_append(files_new, file_in_dir);
       }
       EINA_LIST_FOREACH(files_new, l, file_in_dir)
       {
         snprintf(bg_file, sizeof(bg_file), "%s/%s", file_dir, file_in_dir);

         if (e_config->desktop_default_background)
           if (!strcmp(bg_file, e_config->desktop_default_background))
             x = y + 1;

         y++;
       }

       if (x >= (int)eina_list_count(files_new)) x = 0;

       file_set = eina_list_nth(files_new, x);
     
       snprintf(bg_file, sizeof(bg_file), "%s/%s", file_dir, file_set);

       e_bg_default_set(bg_file);
       e_bg_update();
     }

     // TODO Free data/lists
// EINA_LIST_FREE(files, file_in_dir)
// EINA_LIST_FREE(files_new, file_in_dir)
   eina_strbuf_free(tmp);
   return ECORE_CALLBACK_RENEW;
}
static void
disable_timer(Instance *inst)
{
  if (inst->wp_timer)
    {
      ecore_timer_del(inst->wp_timer);
      inst->wp_timer = NULL;
    }
}

E_API void
e_bg_auto_transition_set(const char *file_dir, Instance *inst)
{
   const char *file_dir_new = eina_stringshare_add(file_dir);
   disable_timer(inst);

     ERR("WALLPAPER\n");
   if (file_dir_new != NULL)
     {
        inst->wp_timer = ecore_timer_add(10 * 60, wp_switch, (void*)file_dir_new);
        wp_switch((void*)file_dir_new);
     }
}

static void
_wp_slideshow_menu_cb_cfg(void *data, E_Menu *menu EINA_UNUSED, E_Menu_Item *mi EINA_UNUSED)
{
   Instance *inst = data;

   E_FREE_FUNC(inst->popup, e_object_del);
   e_int_config_wp_slideshow_module(NULL, inst->cfg);
}

static void
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event)
{
   Instance *inst = data;
   Evas_Event_Mouse_Down *ev = event;
   char buf[64];
   E_Menu *m;
   E_Menu_Item *mi;
   int cx, cy;
   if (ev->button == 1)
     {
      if (ecore_timer_freeze_get(inst->wp_timer) == EINA_TRUE)
      {
         ecore_timer_thaw(inst->wp_timer);
         edje_object_signal_emit(obj, "show_play", "show_play");
         // script play button starten
      }
      else
      {
         ecore_timer_freeze(inst->wp_timer);
         edje_object_signal_emit(obj, "show_pause", "show_pause");
         // pause button einblenden
      }

     }
   else if (ev->button == 3)
     {
        m = e_menu_new();

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, "Settings");
        e_util_menu_item_theme_icon_set(mi, "configure");
        e_menu_item_callback_set(mi, e_int_config_wp_slideshow_module, inst);

        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);



   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy,
                                     NULL, NULL);
   e_menu_activate_mouse(m, e_zone_current_get(),
                         cx + ev->output.x, cy + ev->output.y, 1, 1,
                         E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
   evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                            EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event)
{
   Instance *inst = data;
   Evas_Event_Mouse_Wheel *ev = event;
//    char buf[64];
   E_Menu *m;
   E_Menu_Item *mi;
   int cx, cy;
//    if (ev->z == 1)
      e_bg_auto_transition_set("/home/simon/.e/e/backgrounds/Linux/", inst);
//    else if (ev->z == -1)
      // TODO Rückwärts in der Liste
}



static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "Wallpaper-Slideshow",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};


static Config_Item *
_conf_item_get(const char *id)
{
   Config_Item *ci;

   GADCON_CLIENT_CONFIG_GET(Config_Item, wp_slideshow_config->items, _gc_class, id);

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->switch_time = 10;
   ci->enable_switch = EINA_TRUE;

   wp_slideshow_config->items = eina_list_append(wp_slideshow_config->items, ci);
   e_config_save_queue();

   return ci;
}

static void
_e_mod_action(const char *params)
{
   Eina_List *l;
   Instance *inst;

   if (!params) return;
   
   _cb_mouse_wheel(inst, NULL, NULL, NULL);
}

static void
_e_mod_action_cb_edge(E_Object *obj EINA_UNUSED, const char *params, E_Event_Zone_Edge *ev EINA_UNUSED)
{
   _e_mod_action(params);
}

static void
_e_mod_action_cb(E_Object *obj EINA_UNUSED, const char *params)
{
   _e_mod_action(params);
}

static void
_e_mod_action_cb_key(E_Object *obj EINA_UNUSED, const char *params, Ecore_Event_Key *ev EINA_UNUSED)
{
   _e_mod_action(params);
}

static Eina_Bool
_e_mod_action_cb_mouse(E_Object *obj EINA_UNUSED, const char *params, E_Binding_Event_Mouse_Button *ev EINA_UNUSED)
{
   _e_mod_action(params);
   return EINA_TRUE;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{  // gadcon is requesting a new instance of your gadgets
   Instance *inst;
   E_Gadcon_Client *gcc;
   Evas_Object *o;

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   inst->cfg = _conf_item_get(id);
   

   // create a new gadget object - edje here. you could just create an evas
   // rectangle or any other kind of object here...
   o = edje_object_add(gc->evas);
   // try find a theme overridden group first so theme gets first say
   if (!e_theme_edje_object_set(o, "base/theme/modules/Wallpaper-Slideshow", "modules/Wallpaper-Slideshow/main"))
     {  // fallback - going to cheat for now and use the icon object as the
        // gadge but normsally you'd provide another edj file with a fallback
        // design
        char buf[4096];
        snprintf(buf, sizeof(buf), "%s/e-module-Wallpaper-Slideshow.edj", e_module_dir_get(_module));
        edje_object_file_set(o, buf, "icon");
     }
   // create the new client
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   _instances = eina_list_append(_instances, inst);

   // attach standard right mouse menu
   e_gadcon_client_util_menu_attach(gcc);
   // to demo/test - listen for mouse presses
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL,
                                  _cb_mouse_wheel, inst);
   
//    e_bg_auto_transition_set("/home/simon/.e/e/backgrounds/Neue Hintergründe/", inst);
   e_bg_auto_transition_set("/home/simon/.e/e/backgrounds/Linux/", inst);
   
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{  // shutdown the gadget - free out instance data then
   Instance *inst = gcc->data;
   disable_timer(inst);
   _popup_free(inst);
   _instances = eina_list_remove(_instances, inst);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{  // take input orientation given for the gadget and set an aspect and size
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static const char *
_gc_label(const E_Gadcon_Client_Class *client_class)
{  // return a label to be used in config dialogs selecting gadgets
   return "Wallpaper-Slideshow";
}

static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class, Evas *evas)
{  // return an icon for config dialogs selecting gadgets
   Evas_Object *o;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-Wallpaper-Slideshow.edj", e_module_dir_get(_module));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(const E_Gadcon_Client_Class *client_class)
{ // return a string "id" for this gadget to use in config like shelf to
   // identify this gadget - make it unique for this gadget
//    return _gc_class.name;
   Config_Item *ci = NULL;

   ci = _conf_item_get(NULL);
   return ci->id;
}

////////////////////////////////////////////////////////////////////////////
// core api for all modules if they have gadgets or not - e calls these
// api's or uses the structures to find out core module info. rememebr
// a module can extend e in any way. a module can provide 1 0 or more gadgets
// too...
E_API E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION, "Wallpaper-Slideshow" // we must have a name for this module
};

E_API void *
e_modapi_init(E_Module *m)
{  // called when e loads this module

   conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, switch_time, FLOAT);
   E_CONFIG_VAL(D, T, enable_switch, INT);
   conf_edd = E_CONFIG_DD_NEW("Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   wp_slideshow_config = e_config_domain_load("module.wallpaper-slideshow", conf_edd);

   
   if (!wp_slideshow_config)
     wp_slideshow_config = E_NEW(Config, 1);

   act = e_action_add("Wallpaper-Slideshow");
   if (act)
     {
        act->func.go = _e_mod_action_cb;

        e_action_predef_name_set("Wallpaper-Slideshow", "Next Wallpaper", "Wallpaper-Slideshow", "next_wallpaper", NULL, 0);
     }

   wp_slideshow_config->module = m;
   _module = m; // store module handle that e passes in to use later
   e_gadcon_provider_register(&_gc_class); // register our gadcon class
   return m;
}

E_API int
e_modapi_shutdown(E_Module *m)
{  // called when e unloads this modue - clean up everything we created
   e_gadcon_provider_unregister(&_gc_class); // unregister class
   return 1;
}

E_API int
e_modapi_save(E_Module *m)
{  // called when e wants this module to save its config - no config to save
   e_config_domain_save("module.wallpaper-slideshow", conf_edd, wp_slideshow_config);
   return 1;
}

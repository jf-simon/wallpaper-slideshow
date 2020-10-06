#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   Config_Item cfg;
};

/* Protos */
static void        *_create_data(E_Config_Dialog *cfd);
static void         _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void
e_int_config_wp_slideshow_module(Evas_Object *parent EINA_UNUSED, Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   if (e_config_dialog_find("E", "utils/wallpaper-slideshow")) return;
   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   snprintf(buf, sizeof(buf), "%s/e-module-wallpaper-slideshow.edj",
            e_module_dir_get(_module));
   cfd = e_config_dialog_new(NULL, "Wallpaper Slideshow Settings",
                             "E", "utils/wallpaper-slideshow", buf, 0, v, ci);
   wp_slideshow_config->config_dialog = cfd;
}

static void *
_create_data(E_Config_Dialog *cfd EINA_UNUSED)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   ci = cfd->data;

   memcpy(&(cfdata->cfg), ci, sizeof(Config_Item));

   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd  EINA_UNUSED,
           E_Config_Dialog_Data *cfdata)
{
   wp_slideshow_config->config_dialog = NULL;
   free(cfdata);
}

static void
_file_chosen(void            *data,
             Evas_Object *obj,
             void            *event_info)
{
   E_Config_Dialog_Data *cfdata;
   cfdata = data;
   Eina_Strbuf *tmp;
   tmp = eina_strbuf_new();
   Evas_Object *entry = obj;
   const char *file = event_info;
   if (file)
     {
        elm_object_text_set(entry, file);
        elm_fileselector_path_set(entry, file);
        eina_strbuf_append(tmp, file);
        eina_strbuf_append(tmp, "/");
//         eina_stringshare_replace(&cfdata->transition_custom_dir, eina_strbuf_string_get(tmp));
     }
     eina_strbuf_free(tmp);
//    e_widget_radio_toggle_set(dir, 1);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd,
                      Evas *evas,
                      E_Config_Dialog_Data *cfdata)
{

   Evas_Object *box;
   
  
   Evas_Object *lb, *slider, *dir, *rdg, *sep, *custom_dir, *parent, *o, *sl_refresh;
      Evas_Object *ob, *tab, *of;
   E_Radio_Group *rg;
   

   parent = e_win_evas_win_get(evas);
   
   
   o = e_widget_table_add(parent, 0);
   
//    lb = e_widget_label_add(o, "Time between wallpaper switch");
//    e_widget_table_object_append(o, lb, 0, 0, 1, 1, 1, 1, 0, 1); //1, 0, 1, 1, 1, 1, 0, 1);//2, 0, 1, 1, 1, 1, 0, 1);

   slider = e_widget_slider_add(o, 1, 0, "%1.0f min", 1, 120, 1, &cfdata->cfg.switch_time,
                            NULL, NULL, 100);
   

   e_widget_table_object_append(o, slider, 0, 1, 1, 1, 1, 1, 0, 1); //1, 0, 1, 1, 1, 1, 0, 1);//2, 0, 1, 1, 1, 1, 0, 1);
   
//    custom_dir = e_widget_entry_add(parent, &cfdata->cfg.current_dir, NULL, NULL, NULL);
   

//    e_widget_table_object_append(o, custom_dir, 0, 2, 1, 1, 1, 1, 0, 1); //1, 0, 1, 1, 1, 1, 0, 1);//2, 0, 1, 1, 1, 1, 0, 1);
   
   return o;
   
}

static int
_basic_apply_data(E_Config_Dialog *cfd  EINA_UNUSED,
                  E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   memcpy(ci, &(cfdata->cfg), sizeof(Config_Item));
//    ci->slide = EINA_TRUE;
   ci->switch_time = cfdata->cfg.switch_time;
//    ci->dir = 1;
//    ci->enable_switch = 1;
   ci->current_dir = "test";
   e_config_save_queue();
   return 1;
}


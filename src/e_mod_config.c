#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   Config_Item cfg;
   Evas_Object *l_system;
   Evas_Object *l_user;
   Evas_Object *l_custom;
   Evas_Object *fs_en;
   int              enable_switch;
   int              switch_time;
   int              fmdir;
   const char       *custom_dir;
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
            e_module_dir_get(wp_slideshow_config->module));
   cfd = e_config_dialog_new(NULL, "Wallpaper Slideshow Settings",
                             "E", "utils/wallpaper-slideshow", buf, 0, v, ci);
   wp_slideshow_config->config_dialog = cfd;
   
}


static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->switch_time = ci->switch_time;
   cfdata->enable_switch = ci->enable_switch;
}

static void *
_create_data(E_Config_Dialog *cfd EINA_UNUSED)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);

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

/*
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
}*/


static void
_cb_dir(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   E_Config_Dialog_Data *cfdata;
   char path[PATH_MAX];

//    e_widget_disabled_set(cfdata->l_system, 1);
//    e_widget_disabled_set(cfdata->l_user, 1);
}


static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd,
                      Evas *evas,
                      E_Config_Dialog_Data *cfdata)
{
   Evas_Object *box;
   
  
   Evas_Object *lb, *slider, *dir, *rdg, *sep, *parent, *o, *sl_refresh;
   Evas_Object *ob, *tab, *of, *l, *l_system, *l_user, *l_custom;
   E_Radio_Group *rg;
   char buf[4096];


   o = e_widget_list_add(evas, 0, 0);
   
   lb = e_widget_label_add(evas, "Enable Switch");
   e_widget_list_object_append(o, lb, 0, 0, 0);

   lb = e_widget_check_add(evas, "On", &(cfdata->enable_switch));
   e_widget_list_object_append(o, lb, 0, 0, 0);
   
   lb = e_widget_label_add(evas, "Time between wallpaper switch");
   e_widget_list_object_append(o, lb, 0, 0, 0);

   slider = e_widget_slider_add(evas, 1, 0, "%1.0f min", 1, 120, 1, 0, &(cfdata->switch_time), NULL, 100);
   
   e_widget_list_object_append(o, slider, 0, 0, 0);

   lb = e_widget_label_add(evas, "Choose Image Directory to use");
   e_widget_list_object_append(o, lb, 0, 0, 0);

   rg = e_widget_radio_group_new(&(cfdata->fmdir));

   /* create dir radios */
   
   snprintf(buf, sizeof(buf), "System Directory [%s]", efreet_data_home_get());
   l = e_widget_radio_add(evas, buf, 0, rg);
   cfdata->l_system = l;
   e_widget_list_object_append(o, l, 0, 0, 0);

   
   snprintf(buf, sizeof(buf), "User Directory [%s]", efreet_data_home_get());
   l = e_widget_radio_add(evas, buf, 1, rg);
   cfdata->l_user = l;
   e_widget_list_object_append(o, l, 0, 0, 0);
   l = e_widget_radio_add(evas, "Custom Directory", 2, rg);
   cfdata->l_custom = l;
   e_widget_list_object_append(o, l, 0, 0, 0);
   
   ///
   cfdata->fs_en = elm_fileselector_entry_add(evas);
   elm_fileselector_folder_only_set(cfdata->fs_en, EINA_TRUE);
   elm_fileselector_path_set(cfdata->fs_en, cfdata->custom_dir);
   elm_object_text_set(cfdata->fs_en, "Select a file");
   evas_object_size_hint_weight_set(cfdata->fs_en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(cfdata->fs_en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   
   e_widget_list_object_append(o, cfdata->fs_en, 0, 0, 0);
///
   
   return o;
   
}

static int
_basic_apply_data(E_Config_Dialog *cfd  EINA_UNUSED,
                  E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   
   printf("SWITCHTIME; %f\n", cfdata->switch_time);
   printf("ENABLE; %i\n", cfdata->enable_switch);
   memcpy(ci, &(cfdata->cfg), sizeof(Config_Item));
   
   ci->switch_time = cfdata->switch_time;
   ci->enable_switch = cfdata->enable_switch;
   cfdata->custom_dir = elm_fileselector_path_get(cfdata->fs_en);
   ci->custom_dir = cfdata->custom_dir;
//    ci->current_dir = "test";
   e_config_save_queue();
   return 1;
}

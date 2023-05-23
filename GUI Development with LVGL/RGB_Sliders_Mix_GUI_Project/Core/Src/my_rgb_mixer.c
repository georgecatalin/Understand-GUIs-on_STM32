#include "my_rgb_mixer.h"


enum slider_type
{
    SLIDER_RED = 0,
    SLIDER_BLUE,
    SLIDER_GREEN
};

typedef struct
{
    uint8_t slider_type;
    lv_obj_t *label;
} rgb_mixer_t;

rgb_mixer_t label_r, label_b, label_g;
lv_obj_t* base_object_rectangle;

static void my_callback_function(lv_event_t *the_event)
{
    static uint8_t red_value, blue_value, green_value;

    lv_obj_t *target_intended_slider = lv_event_get_target(the_event);

    rgb_mixer_t *user_data = lv_event_get_user_data(the_event);
    int32_t my_value = lv_slider_get_value(target_intended_slider);
    lv_label_set_text_fmt(user_data->label, "%ld", my_value);

    if (user_data->slider_type == SLIDER_RED)
    {
        red_value = my_value;
    }

    if(user_data->slider_type == SLIDER_BLUE)
    {
        blue_value = my_value;
    }

    if(user_data->slider_type == SLIDER_GREEN)
    {
        green_value = my_value;
    }

    lv_obj_set_style_bg_color(base_object_rectangle, lv_color_make(red_value, blue_value, green_value), LV_PART_MAIN);
}


void rgb_mixer_create_UI(void)
{
    label_r.slider_type = SLIDER_RED;
    label_b.slider_type = SLIDER_BLUE;
    label_g.slider_type = SLIDER_GREEN;

    //TODO DISPLAY AND SET PROPERTIES OF A BASE OBJECT
    lv_obj_t *active_screen_object = lv_scr_act(); // get the screen object that is active and was created automatically when registering the display driver with the lvgl
    base_object_rectangle = lv_obj_create(active_screen_object);

    lv_obj_set_width(base_object_rectangle, 300);
    lv_obj_set_height(base_object_rectangle, 60);
    lv_obj_align(base_object_rectangle, LV_ALIGN_TOP_MID, 0, 30);

    lv_obj_set_style_border_color(base_object_rectangle, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(base_object_rectangle, 3, LV_PART_MAIN);

    //TODO DISPLAY A HEADING LABEL
    lv_obj_t* heading = lv_label_create(lv_scr_act());
    lv_label_set_text(heading, "RGB Color Switcher");
    lv_obj_align(heading, LV_ALIGN_TOP_MID, 0, 5);


    //TODO DISPLAY 3 SLIDERS
    lv_obj_t *slider_r = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider_r, 300);
    lv_obj_align_to(slider_r, base_object_rectangle,LV_ALIGN_TOP_MID, 0, 60);

    lv_obj_set_style_bg_color(slider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_KNOB);


    //TODO DISPLAY A LABEL ASSOCIATED TO SLIDER A
    label_r.label = lv_label_create(lv_scr_act());
    lv_label_set_text(label_r.label, "0");
    lv_obj_align_to(label_r.label, slider_r, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t *slider_b = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider_b, 300);
    lv_obj_align_to(slider_b, slider_r , LV_ALIGN_TOP_MID, 0, 50);


    lv_obj_set_style_bg_color(slider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB);

    //TODO DISPLAY A LABEL ASSOCIATED TO SLIDER B
    label_b.label = lv_label_create(lv_scr_act());
    lv_label_set_text(label_b.label, "0");
    lv_obj_align_to(label_b.label, slider_b, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t* slider_g = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider_g, 300);
    lv_obj_align_to(slider_g, slider_b, LV_ALIGN_TOP_MID, 0, 50);


    lv_obj_set_style_bg_color(slider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PALETTE_GREEN);
    lv_obj_set_style_bg_color(slider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PART_KNOB);

    //TODO DISPLAY A LABEL ASSOCIATED TO SLIDER C
    label_g.label = lv_label_create(lv_scr_act());
    lv_label_set_text(label_g.label, "0");
    lv_obj_align_to(label_g.label, slider_g, LV_ALIGN_TOP_MID, 0, 0);


    /*Set value range for sliders*/
    lv_slider_set_range(slider_r, 0, 255);
    lv_slider_set_range(slider_g, 0, 255);
    lv_slider_set_range(slider_b, 0, 255);

    //TODO SET THE HANDLING OF EVENT
    lv_obj_add_event_cb(slider_r, my_callback_function, LV_EVENT_VALUE_CHANGED, &label_r); // label_r user data will be passed to the callback
    lv_obj_add_event_cb(slider_b, my_callback_function, LV_EVENT_VALUE_CHANGED, &label_b);  // label_b user data will be passed to the callback
    lv_obj_add_event_cb(slider_g, my_callback_function, LV_EVENT_VALUE_CHANGED, &label_g);  // label_g user data will be passed to the callback


}

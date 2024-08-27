
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "lv_drv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#if USE_SDL
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lv_drivers/sdl/sdl.h"
#elif USE_X11
#include "lv_drivers/x11/x11.h"
#endif
#include <time.h>
#include "esp_ui.hpp"
/* These are built-in app examples in `esp-ui` library */
#include "app_examples/phone/simple_conf/src/phone_app_simple_conf.hpp"
#include "app_examples/phone/complex_conf/src/phone_app_complex_conf.hpp"
#include "app_examples/phone/squareline/src/phone_app_squareline.hpp"

/*********************
 *      DEFINES
 *********************/
#define EXAMPLE_USE_EXTERNAL_STYLESHEET   (1)
#if EXAMPLE_USE_EXTERNAL_STYLESHEET
  #if (DISP_HOR_RES == 1024) && (DISP_VER_RES == 600)
    #include "esp-ui-phone_1024_600_stylesheet/src/esp_ui_phone_1024_600_stylesheet.h"
    #define EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET()   ESP_UI_PHONE_1024_600_DARK_STYLESHEET()
  #elif (DISP_HOR_RES == 800) && (DISP_VER_RES == 480)
    #include "esp-ui-phone_800_480_stylesheet/src/esp_ui_phone_800_480_stylesheet.h"
    #define EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET()   ESP_UI_PHONE_800_480_DARK_STYLESHEET()
  #elif (DISP_HOR_RES == 480) && (DISP_VER_RES == 480)
    #include "esp-ui-phone_480_480_stylesheet/src/esp_ui_phone_480_480_stylesheet.h"
    #define EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET()   ESP_UI_PHONE_480_480_DARK_STYLESHEET()
  #elif (DISP_HOR_RES == 320) && (DISP_VER_RES == 240)
    #include "esp-ui-phone_320_240_stylesheet/src/esp_ui_phone_320_240_stylesheet.h"
    #define EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET()   ESP_UI_PHONE_320_240_DARK_STYLESHEET()
  #endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static void hal_deinit(void);
static void* tick_thread(void *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static pthread_t thr_tick;    /* thread */
static bool end_tick = false; /* flag to terminate thread */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_disp_t *disp = NULL;
static lv_indev_t *mouse_indev = NULL;
static lv_indev_t *kb_indev = NULL;
static lv_indev_t *enc_indev = NULL;
static void on_clock_update_timer_cb(struct _lv_timer_t *t);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
    (void)argc; /*Unused*/
    (void)argv; /*Unused*/

    /* Initialize LVGL */
    lv_init();

    /* Initialize the HAL (display, input devices, tick) for LVGL */
    hal_init();

    /* Create a phone object */
    ESP_UI_Phone *phone = new ESP_UI_Phone(disp);
    ESP_UI_CHECK_NULL_RETURN(phone, 1, "Create phone failed");

#ifdef EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET
    /* Add external stylesheet and activate it */
    ESP_UI_PhoneStylesheet_t *phone_stylesheet = new ESP_UI_PhoneStylesheet_t EXAMPLE_ESP_UI_PHONE_DARK_STYLESHEET();
    ESP_UI_CHECK_NULL_RETURN(phone_stylesheet, 1, "Create phone stylesheet failed");
    ESP_UI_CHECK_FALSE_RETURN(phone->addStylesheet(phone_stylesheet), 1, "Add phone stylesheet failed");
    ESP_UI_CHECK_FALSE_RETURN(phone->activateStylesheet(phone_stylesheet), 1, "Activate phone stylesheet failed");
    delete phone_stylesheet;
#endif

    /* Configure and begin the phone */
    ESP_UI_CHECK_FALSE_RETURN(phone->setTouchDevice(mouse_indev), 1, "Set touch device failed");
    ESP_UI_CHECK_FALSE_RETURN(phone->begin(), 1, "Begin failed");
    // ESP_UI_CHECK_FALSE_RETURN(phone->getCoreHome().showContainerBorder(), 1, "Show container border failed");

    /* Install apps */
    PhoneAppSimpleConf *phone_app_simple_conf = new PhoneAppSimpleConf(true, true);
    ESP_UI_CHECK_NULL_RETURN(phone_app_simple_conf, 1, "Create phone app simple conf failed");
    ESP_UI_CHECK_FALSE_RETURN((phone->installApp(phone_app_simple_conf) >= 0), 1, "Install phone app simple conf failed");
    PhoneAppComplexConf *phone_app_complex_conf = new PhoneAppComplexConf(true, true);
    ESP_UI_CHECK_NULL_RETURN(phone_app_complex_conf, 1, "Create phone app complex conf failed");
    ESP_UI_CHECK_FALSE_RETURN((phone->installApp(phone_app_complex_conf) >= 0), 1, "Install phone app complex conf failed");
    PhoneAppSquareline *phone_app_squareline = new PhoneAppSquareline(true, true);
    ESP_UI_CHECK_NULL_RETURN(phone_app_squareline, 1, "Create phone app squareline failed");
    ESP_UI_CHECK_FALSE_RETURN((phone->installApp(phone_app_squareline) >= 0), 1, "Install phone app squareline failed");

    /* Create a timer to update the clock */
    ESP_UI_CHECK_NULL_RETURN(lv_timer_create(on_clock_update_timer_cb, 1000, phone), 1, "Create clock update timer failed");

    while(1) {
        /* Periodically call the lv_task handler.
        * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        usleep(5 * 1000);
    }

    // hal_deinit();
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void on_clock_update_timer_cb(struct _lv_timer_t *t)
{
    time_t now;
    struct tm timeinfo;
    bool is_time_pm = false;
    ESP_UI_Phone *phone = (ESP_UI_Phone *)t->user_data;

    time(&now);
    localtime_r(&now, &timeinfo);
    is_time_pm = (timeinfo.tm_hour >= 12);
    ESP_UI_CHECK_FALSE_EXIT(phone->getHome().getStatusBar()->setClock(timeinfo.tm_hour, timeinfo.tm_min, is_time_pm),
                            "Refresh status bar failed");

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t free_kb = mon.free_size / 1024;
    uint32_t total_kb = mon.total_size / 1024;
    ESP_UI_CHECK_FALSE_EXIT(phone->getHome().getRecentsScreen()->setMemoryLabel(free_kb, total_kb, 0, 0),
                            "Refresh memory label failed");
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* mouse input device */
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1);
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /* keyboard input device */
  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2);
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

  /* mouse scroll wheel input device */
  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3);
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

#if USE_SDL
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  sdl_init();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
  static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  // mouse_init();
  indev_drv_1.read_cb = sdl_mouse_read;

  // keyboard_init();
  indev_drv_2.read_cb = sdl_keyboard_read;

  // mousewheel_init();
  indev_drv_3.read_cb = sdl_mousewheel_read;

#elif USE_X11
  lv_x11_init("LVGL Simulator Demo", DISP_HOR_RES, DISP_VER_RES);

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[DISP_HOR_RES * 100];
  static lv_color_t buf1_2[DISP_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = lv_x11_flush;
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  indev_drv_1.read_cb = lv_x11_get_pointer;
  indev_drv_2.read_cb = lv_x11_get_keyboard;
  indev_drv_3.read_cb = lv_x11_get_mousewheel;
#endif
  /* Set diplay theme */
  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  /* Tick init */
  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);

  /* register input devices */
  mouse_indev = lv_indev_drv_register(&indev_drv_1);
  kb_indev = lv_indev_drv_register(&indev_drv_2);
  enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(kb_indev, g);
  lv_indev_set_group(enc_indev, g);

  /* Set a cursor for the mouse */
  LV_IMG_DECLARE(mouse_cursor_icon);                   /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor*/
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);      /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);        /*Connect the image  object to the driver*/
}

/**
 * Releases the Hardware Abstraction Layer (HAL) for the LVGL graphics library
 */
static void hal_deinit(void)
{
  end_tick = true;
  pthread_join(thr_tick, NULL);

#if USE_SDL
  // nop
#elif USE_X11
  lv_x11_deinit();
#endif
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static void* tick_thread(void *data) {
  (void)data;

  while(!end_tick) {
    usleep(5000);
    lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
  }

  return NULL;
}

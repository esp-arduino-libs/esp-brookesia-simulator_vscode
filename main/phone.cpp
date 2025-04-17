/*********************
 *      INCLUDES
 *********************/
#include "esp_brookesia.hpp"
// #include "systems/phone/stylesheets/480_480/dark/stylesheet.h"
#include "app_examples/phone/simple_conf/src/phone_app_simple_conf.hpp"
#include "app_examples/phone/complex_conf/src/phone_app_complex_conf.hpp"

/*********************
 *      DEFINES
 *********************/
/**
 * Use the stylesheet corresponding to the resolution; otherwise, another built-in stylesheet will be used.
 */
// #if (DISP_HOR_RES == 320) && (DISP_VER_RES == 240)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_320_240_DARK_STYLESHEET()
// #elif (DISP_HOR_RES == 320) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_320_480_DARK_STYLESHEET()
// #elif (DISP_HOR_RES == 480) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_480_480_DARK_STYLESHEET()
//   #define SETTINGS_UI_STYLESHEET()                        SETTINGS_UI_480_480_STYLESHEET_DARK()
// #elif (DISP_HOR_RES == 720) && (DISP_VER_RES == 1280)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_720_1280_DARK_STYLESHEET()
//   #define SETTINGS_UI_STYLESHEET()                        SETTINGS_UI_720_1280_STYLESHEET_DARK()
//   #define DOUBAO_STYLESHEET                               doubao::STYLESHEET_720_1280_DARK
// #elif (DISP_HOR_RES == 800) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_800_480_DARK_STYLESHEET()
// #elif (DISP_HOR_RES == 800) && (DISP_VER_RES == 1280)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_800_1280_DARK_STYLESHEET()
// #elif (DISP_HOR_RES == 1024) && (DISP_VER_RES == 600)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_1024_600_DARK_STYLESHEET()
//   #define SETTINGS_UI_STYLESHEET()                        SETTINGS_UI_1024_600_STYLESHEET_DARK()
//   #define DOUBAO_STYLESHEET                               doubao::STYLESHEET_1024_600_DARK
// #elif (DISP_HOR_RES == 1280) && (DISP_VER_RES == 800)
//   #define EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET()   ESP_BROOKESIA_PHONE_1280_800_DARK_STYLESHEET()
// #endif

#define LVGL_TIMER_HANDLER_PERIOD_US  (5 * 1000)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void get_local_time(struct tm *timeinfo, time_t *rawtime) {
#ifdef _WIN32
    // For Windows systems
    localtime_s(timeinfo, rawtime);
#else
    // For POSIX systems
    localtime_r(rawtime, timeinfo);
#endif
}
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
static void on_clock_update_timer_cb(struct _lv_timer_t *t);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int phone_main(void)
{
    ESP_BROOKESIA_LOGI("Using display resolution: %dx%d", DISP_HOR_RES, DISP_VER_RES);

    esp_brookesia_squareline_ui_comp_init();

    /* Create a phone object */
    ESP_Brookesia_Phone *phone = new ESP_Brookesia_Phone();
    ESP_BROOKESIA_CHECK_NULL_RETURN(phone, 1, "Create phone failed");

#ifdef EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET
    /* Add external stylesheet and activate it */
    ESP_Brookesia_PhoneStylesheet_t *stylesheet = new ESP_Brookesia_PhoneStylesheet_t EXAMPLE_ESP_BROOKESIA_PHONE_DARK_STYLESHEET();
    ESP_BROOKESIA_CHECK_NULL_RETURN(stylesheet, 1, "Create phone stylesheet failed");

    ESP_BROOKESIA_LOGI("Using stylesheet (%s)", stylesheet->core.name);
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone->addStylesheet(stylesheet), 1, "Add phone stylesheet failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone->activateStylesheet(stylesheet), 1, "Activate phone stylesheet failed");
    delete stylesheet;
#endif

    /* Configure and begin the phone */
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone->begin(), 1, "Begin failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone->getCoreHome().showContainerBorder(), 1, "Show container border failed");

    /* Install apps */
    PhoneAppSimpleConf *app_simple_conf = new PhoneAppSimpleConf();
    ESP_BROOKESIA_CHECK_NULL_RETURN(app_simple_conf, 1, "Create app simple conf failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN((phone->installApp(app_simple_conf) >= 0), 1, "Install app simple conf failed");
    PhoneAppComplexConf *app_complex_conf = new PhoneAppComplexConf();
    ESP_BROOKESIA_CHECK_NULL_RETURN(app_complex_conf, 1, "Create app complex conf failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN((phone->installApp(app_complex_conf) >= 0), 1, "Install app complex conf failed");
    // PhoneAppSquareline *app_squareline = new PhoneAppSquareline();
    // ESP_BROOKESIA_CHECK_NULL_RETURN(app_squareline, 1, "Create app squareline failed");
    // ESP_BROOKESIA_CHECK_FALSE_RETURN((phone->installApp(app_squareline) >= 0), 1, "Install app squareline failed");

    /* Create a timer to update the clock */
    ESP_BROOKESIA_CHECK_NULL_RETURN(lv_timer_create(on_clock_update_timer_cb, 1000, phone), 1, "Create clock update timer failed");

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
    ESP_Brookesia_Phone *phone = (ESP_Brookesia_Phone *)t->user_data;

    time(&now);
    get_local_time(&timeinfo, &now);
    is_time_pm = (timeinfo.tm_hour >= 12);
    ESP_BROOKESIA_CHECK_FALSE_EXIT(
      phone->getHome().getStatusBar()->setClock(timeinfo.tm_hour, timeinfo.tm_min, is_time_pm),
      "Refresh status bar failed"
    );

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t free_kb = mon.free_size / 1024;
    uint32_t total_kb = mon.total_size / 1024;
    ESP_BROOKESIA_CHECK_FALSE_EXIT(
      phone->getHome().getRecentsScreen()->setMemoryLabel(free_kb, total_kb, 0, 0),
      "Refresh memory label failed"
    );
}

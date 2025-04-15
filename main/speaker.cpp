/*********************
 *      INCLUDES
 *********************/
#include "esp_brookesia.hpp"
#include "app_examples/speaker/simple_conf/src/speaker_app_simple_conf.hpp"
#include "app_examples/speaker/complex_conf/src/speaker_app_complex_conf.hpp"
#include "app_examples/speaker/squareline/src/speaker_app_squareline.hpp"

using namespace esp_brookesia::systems::speaker;

/*********************
 *      DEFINES
 *********************/
/**
 * Use the stylesheet corresponding to the resolution; otherwise, another built-in stylesheet will be used.
 */
// #if (DISP_HOR_RES == 320) && (DISP_VER_RES == 240)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_320_240_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 320) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_320_480_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 480) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_480_480_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 720) && (DISP_VER_RES == 1280)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_720_1280_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 800) && (DISP_VER_RES == 480)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_800_480_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 800) && (DISP_VER_RES == 1280)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_800_1280_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 1024) && (DISP_VER_RES == 600)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_1024_600_DARK_STYLESHEET
// #elif (DISP_HOR_RES == 1280) && (DISP_VER_RES == 800)
//   #define EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET   ESP_BROOKESIA_SPEAKER_1280_800_DARK_STYLESHEET
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

int speaker_main(void)
{
    ESP_BROOKESIA_LOGI("Using display resolution: %dx%d", DISP_HOR_RES, DISP_VER_RES);

    esp_brookesia_squareline_ui_comp_init();

    /* Create a speaker object */
    ESP_Brookesia_Speaker *speaker = new ESP_Brookesia_Speaker();
    ESP_BROOKESIA_CHECK_NULL_RETURN(speaker, 1, "Create speaker failed");

#ifdef EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET
    /* Add external stylesheet and activate it */
    ESP_Brookesia_SpeakerStylesheet_t *stylesheet = new ESP_Brookesia_SpeakerStylesheet_t EXAMPLE_ESP_BROOKESIA_SPEAKER_DARK_STYLESHEET;
    ESP_BROOKESIA_CHECK_NULL_RETURN(stylesheet, 1, "Create speaker stylesheet failed");

    ESP_BROOKESIA_LOGI("Using stylesheet (%s)", stylesheet->core.name);
    ESP_BROOKESIA_CHECK_FALSE_RETURN(speaker->addStylesheet(stylesheet), 1, "Add speaker stylesheet failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(speaker->activateStylesheet(stylesheet), 1, "Activate speaker stylesheet failed");
    delete stylesheet;
#endif

    /* Configure and begin the speaker */
    ESP_BROOKESIA_CHECK_FALSE_RETURN(speaker->begin(), 1, "Begin failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(speaker->getCoreHome().showContainerBorder(), 1, "Show container border failed");

    /* Install apps */
    SpeakerAppSimpleConf *app_simple_conf = new SpeakerAppSimpleConf();
    ESP_BROOKESIA_CHECK_NULL_RETURN(app_simple_conf, 1, "Create app simple conf failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN((speaker->installApp(app_simple_conf) >= 0), 1, "Install app simple conf failed");
    SpeakerAppComplexConf *app_complex_conf = new SpeakerAppComplexConf();
    ESP_BROOKESIA_CHECK_NULL_RETURN(app_complex_conf, 1, "Create app complex conf failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN((speaker->installApp(app_complex_conf) >= 0), 1, "Install app complex conf failed");
    SpeakerAppSquareline *app_squareline = new SpeakerAppSquareline();
    ESP_BROOKESIA_CHECK_NULL_RETURN(app_squareline, 1, "Create app squareline failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN((speaker->installApp(app_squareline) >= 0), 1, "Install app squareline failed");

    /* Create a timer to update the clock */
    ESP_BROOKESIA_CHECK_NULL_RETURN(lv_timer_create(on_clock_update_timer_cb, 1000, speaker), 1, "Create clock update timer failed");

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
    ESP_Brookesia_Speaker *speaker = (ESP_Brookesia_Speaker *)t->user_data;

    time(&now);
    get_local_time(&timeinfo, &now);
    is_time_pm = (timeinfo.tm_hour >= 12);
    ESP_BROOKESIA_CHECK_FALSE_EXIT(
      speaker->getHome().getStatusBar()->setClock(timeinfo.tm_hour, timeinfo.tm_min, is_time_pm),
      "Refresh status bar failed"
    );

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t free_kb = mon.free_size / 1024;
    uint32_t total_kb = mon.total_size / 1024;
    ESP_BROOKESIA_CHECK_FALSE_EXIT(
      speaker->getHome().getRecentsScreen()->setMemoryLabel(free_kb, total_kb, 0, 0),
      "Refresh memory label failed"
    );
}

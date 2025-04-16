
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
#include <mutex>
#include <thread>
#include "lv_drv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lv_drivers/sdl/sdl.h"
#include <time.h>
#include "esp_brookesia.hpp"
#include "ai_framework/hmi/anim_face/robot_face.h"

#define USE_BROOKESIA   1

/*********************
 *      DEFINES
 *********************/
#define LVGL_TIMER_HANDLER_PERIOD_US  (5 * 1000)

#ifdef RUN_TEST
#define RUN_TEST_TIMEOUT_S  (5)
#endif

/**********************
 *      TYPEDEFS
 **********************/
// Define expression switching parameters
#define EXPRESSION_DURATION    100   // Duration for each expression (in frames) - reduced from 300

// Add FPS calculation defines and variables
#define FPS_UPDATE_INTERVAL_MS 1000  // Update FPS display every second
#define MOVING_AVERAGE_SAMPLES  10   // Number of samples for moving average

// Segmented rendering configuration
#define SEGMENT_HEIGHT         40    // Height of each segment (can be adjusted based on memory availability)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static void hal_deinit(void);
static void* tick_thread(void *data);
static float calculate_fps(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static pthread_t thr_tick;    /* thread */
static bool end_tick = false; /* flag to terminate thread */

static robot_face_t *g_face = NULL;
static lv_obj_t * canvas = NULL;
static lv_obj_t *obj_img_run_particles = NULL;
static std::mutex face_mtx;
static graphic_buffer_t *face_buffer = NULL;

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
static std::mutex lv_mtx;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern int phone_main(void);
extern int speaker_main(void);

int main(int argc, char **argv)
{
    (void)argc; /*Unused*/
    (void)argv; /*Unused*/

    /* Initialize LVGL */
    lv_init();

    /* Initialize the HAL (display, input devices, tick) for LVGL */
    hal_init();

#if USE_BROOKESIA
    ESP_BROOKESIA_LOGI("Using display resolution: %dx%d", DISP_HOR_RES, DISP_VER_RES);

    esp_brookesia_squareline_ui_comp_init();

    // phone_main();
    speaker_main();

#else

    canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(canvas, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(canvas, 0, 0);

    obj_img_run_particles = lv_img_create(canvas);
    lv_obj_set_align(obj_img_run_particles, LV_ALIGN_CENTER);

    // Initialize graphics library
    graphic_flush_cfg_t flush_cfg = {
        .flush = [](void *buffer, int x_start, int y_start, int x_end, int y_end, void *user_data) {
            // std::lock_guard<std::mutex> lock(lv_mtx);
            lv_canvas_set_buffer(canvas, buffer, DISP_HOR_RES, DISP_VER_RES, LV_IMG_CF_TRUE_COLOR);
            return true;
        },
        .check_flush_ready = [](int timeout_ms, void *user_data) {
            // usleep(timeout_ms * 1000);
            return true;
        },
        .user_data = NULL
    };
    graphic_init(&flush_cfg);

    // Graphics buffer and memory allocation
    uint16_t *draw_buf = NULL;
    size_t buffer_size = 0;

    // Using full-screen buffer - higher memory usage but better performance
    printf("Creating full-screen graphics buffer...\n");
    buffer_size = DISP_HOR_RES * DISP_VER_RES * sizeof(uint16_t);
    draw_buf = (uint16_t *)calloc(buffer_size, 1);
    if (!draw_buf) {
        printf("Failed to allocate memory for full-screen buffer\n");
        return 1;
    }

    // Create standard full-screen buffer
    face_buffer = graphic_create_buffer(
        DISP_HOR_RES,          // Width
        DISP_VER_RES,          // Height
        draw_buf                // Buffer
    );

    if (!face_buffer) {
        printf("Failed to create graphics buffer");
        free(draw_buf);
        return 1;
    }

    // Print memory usage information
    printf("Allocated %d bytes for graphics buffer (%.2f KB)\n",
             (int)buffer_size, buffer_size / 1024.0f);

    // Initialize robot face
    robot_face_t *face = NULL;
    robot_face_init(&face, FACE_HAPPY);  // Start with happy expression

    // Save to global variable for rendering callback
    g_face = face;

    // Initialize FPS timer
    uint32_t last_fps_tick = SDL_GetTicks();

    // Create robot face rendering task
    lv_timer_create([] (lv_timer_t *timer) {
          std::lock_guard<std::mutex> face_lock(face_mtx);

          // Update animation state
          robot_face_update(g_face);

          // Using full-screen buffer approach - clear, render, flush
          graphic_clear(face_buffer, COLOR_BLACK);
          robot_face_render(g_face, face_buffer, DISP_HOR_RES / 2, DISP_VER_RES / 2);
          graphic_flush(face_buffer, 0, 0);

          // Calculate FPS
          calculate_fps();
      }, 30, NULL);

      std::thread([&]() {
          // Expression cycling variables
          uint32_t expression_index = 0;

          // Complete array of expressions to cycle through
          robot_face_type_t expressions[10] = {
              FACE_HAPPY,
              FACE_ANGRY,
              FACE_LISTENING,
              FACE_SURPRISED,
              FACE_SLEEPY,
              FACE_THINKING,
              FACE_CUTE,
              FACE_ALERT,
              FACE_WORRIED,
              FACE_SERIOUS
          };

          // Animation state variables
          uint32_t frame_count = 0;
          std::unique_lock<std::mutex> lock(face_mtx);
          lock.unlock();

          // Animation loop
          while (1) {
            // Cycle to next expression
            expression_index = (expression_index + 1) % 10;
            ESP_LOGI(TAG, "Switching to expression: %d\n", expressions[expression_index]);

            // Set new expression
            lock.lock();
            robot_face_set_type(g_face, expressions[expression_index]);
            lock.unlock();

            sleep(5);
          }
      }).detach();

#endif

    std::unique_lock<std::mutex> lock(lv_mtx);
    while(1) {
        /* Periodically call the lv_task handler.
        * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        lock.unlock();
        usleep(LVGL_TIMER_HANDLER_PERIOD_US);
        lock.lock();

#ifdef RUN_TEST
        static uint32_t loop_cnt = 0;
        if (++loop_cnt >= RUN_TEST_TIMEOUT_S * 1000 * 1000 / LVGL_TIMER_HANDLER_PERIOD_US) {
            ESP_BROOKESIA_LOGW("Run test timeout");
            break;
        }
#endif
    }

    // hal_deinit();
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
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

/**
 * @brief Calculate and return current FPS (frames per second)
 *
 * @return float Current FPS value as moving average
 */
static float calculate_fps(void) {
    static uint32_t frame_count_fps = 0;
    static uint32_t last_fps_tick = 0;
    static float fps_history[MOVING_AVERAGE_SAMPLES] = {0};
    static int fps_history_index = 0;
    static float current_fps = 0;

    frame_count_fps++;

    uint32_t current_tick = SDL_GetTicks(); // Use SDL_GetTicks() to get milliseconds on Linux
    uint32_t elapsed = current_tick - last_fps_tick;

    // Update FPS calculation once per second
    if (elapsed >= FPS_UPDATE_INTERVAL_MS) {
        // Calculate FPS
        float new_fps = (1000.0f * frame_count_fps) / elapsed;

        // Add to moving average
        fps_history[fps_history_index] = new_fps;
        fps_history_index = (fps_history_index + 1) % MOVING_AVERAGE_SAMPLES;

        // Calculate average FPS
        float total = 0;
        for (int i = 0; i < MOVING_AVERAGE_SAMPLES; i++) {
            total += fps_history[i];
        }
        current_fps = total / MOVING_AVERAGE_SAMPLES;

        // Reset counters
        frame_count_fps = 0;
        last_fps_tick = current_tick;

        // Log FPS
        printf("Current FPS: %.2f\n", current_fps);
    }

    return current_fps;
}

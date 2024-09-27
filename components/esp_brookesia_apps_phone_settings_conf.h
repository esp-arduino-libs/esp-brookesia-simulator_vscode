/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Debug /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Enable debug logs for modules */
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG              (1)
#if ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_MANAGER      (1)
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_UI           (1)

// Widgets
#if ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_UI
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_WIDGETS_CELL_CONTAINER   (0)
#endif
// Screens
#if ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_UI
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_SCREEN_BASE              (0)
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_SCREEN_SETTINGS          (0)
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_SCREEN_WLAN              (1)
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_SCREEN_WLAN_VERIFICATION (1)
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_LOG_ENABLE_DEBUG_SCREEN_ABOUT             (0)
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// File Version ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Do not change the following versions, they are used to check if the configurations in this file are compatible with
 * the current version of `esp_brookesia_apps_phone_settings_conf.h` in the library. The detailed rules are as follows:
 *
 *   1. If the major version is not consistent, then the configurations in this file are incompatible with the library
 *      and must be replaced with the file from the library.
 *   2. If the minor version is not consistent, this file might be missing some new configurations, which will be set to
 *      default values. It is recommended to replace it with the file from the library.
 *   3. Even if the patch version is not consistent, it will not affect normal functionality.
 *
 */
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_CONF_FILE_VER_MAJOR  0
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_CONF_FILE_VER_MINOR  1
#define ESP_BROOKESIA_APPS_PHONE_SETTINGS_CONF_FILE_VER_PATCH  0

// LICENSE_CODE ZON
#ifndef LUM_SDK_H
#define LUM_SDK_H

#define LUM_SDK_CHOICE_NONE 0
#define LUM_SDK_CHOICE_PEER 1
#define LUM_SDK_CHOICE_NOT_PEER 2

typedef enum peer_txt_t {
    PEER_TXT_NO_ADS = 0,
    PEER_TXT_REMOVE_ADS = 0, // "Remove Ads"
    PEER_TXT_PREMIUM = 1,
    PEER_TXT_PREMIUM_VER = 1, // "Premium version"
    PEER_TXT_FREE = 2,
    PEER_TXT_FREE_APP = 2, // "Get the app for free"
    //PEER_TXT_DONATE = 3, depracated
    PEER_TXT_I_AGREE = 4, // "I Agree"
} peer_txt_t;

typedef enum not_peer_txt_t {
    NOT_PEER_TXT_ADS = 0, // "I prefer to see ads"
    NOT_PEER_TXT_LIMITED = 1, // "I prefer limited use"
    NOT_PEER_TXT_PREMIUM = 2, // deprecated (don't use)
    NOT_PEER_TXT_NO_DONATE = 3, // deprecated (don't use)
    NOT_PEER_TXT_NOT_AGREE = 4, // deprecated (don't use)
    NOT_PEER_TXT_I_DISAGREE = 5, // deprecated (don't use)
    NOT_PEER_TXT_SUBSCRIPTION = 6, // "I prefer to subscribe"
    NOT_PEER_TXT_BUY = 7, // deprecated (don't use)
    NOT_PEER_TXT_PAY = 8, // "I prefer to pay"
    NOT_PEER_TXT_NO_THANK_YOU = 9, // "No, Thank You"
} not_peer_txt_t;

// depracated (don't use)
typedef enum dlg_type_t {
    DLG_TYPE_DEFAULT = 0, // classic
    DLG_TYPE_FIXED300X250 = 1,
    DLG_TYPE_NEW2019 = 2,
} dlg_type_t;

// depracated (don't use)
typedef enum txt_type_t {
    TXT_TYPE_DEFAULT = 0, // classic
    TXT_TYPE_BANDWIDTH = 1,
} txt_type_t;

typedef enum dlg_pos_type_t {
    DLG_POS_TYPE_CENTER_OWNER = 0, // default
    DLG_POS_TYPE_CENTER_SCREEN = 1,
    DLG_POS_TYPE_MANUAL = 2,
} dlg_pos_type_t;

#define WINAPI __stdcall
typedef void (WINAPI *lum_sdk_choice_change_t)(void);

#ifndef LUM_SDK_INTERNAL

#define DLLIMPORT _declspec(dllimport)
#define DEPRECATED __declspec(deprecated)

// C-style imports
#ifdef __cplusplus
extern "C" {
#endif
DLLIMPORT int WINAPI lum_sdk_is_supported_c(void);
DLLIMPORT int WINAPI lum_sdk_is_supported2_c(int *reason, char **msg);
DLLIMPORT void WINAPI lum_sdk_init_ui_c(char *appid);
DLLIMPORT void WINAPI lum_sdk_init_monitor_c(char *appid);
DLLIMPORT void WINAPI lum_sdk_uninit_c(void);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_dlg_size_c(int dlg_size);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_dlg_type_c(dlg_type_t dlg_type);
DLLIMPORT void WINAPI lum_sdk_set_dlg_pos_type_c(dlg_pos_type_t dlg_pos_type);
DLLIMPORT void WINAPI lum_sdk_set_dlg_pos_c(double top, double left);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_txt_type_c(txt_type_t txt_type);
DLLIMPORT void WINAPI lum_sdk_set_txt_culture_c(char *txt_culture);
DLLIMPORT void WINAPI lum_sdk_set_bg_color_c(char *bg_color);
DLLIMPORT void WINAPI lum_sdk_set_btn_color_c(char *btn_color);
DLLIMPORT void WINAPI lum_sdk_set_txt_color_c(char *txt_color);
DLLIMPORT void WINAPI lum_sdk_set_app_name_c(char *app_name);
DLLIMPORT void WINAPI lum_sdk_set_tos_link_c(char *tos_link);
DLLIMPORT void WINAPI lum_sdk_set_logo_link_c(char *logo_link);
DLLIMPORT void WINAPI lum_sdk_set_benefit_txt_c(char *benefit_txt);
DLLIMPORT void WINAPI lum_sdk_set_peer_txt_c(peer_txt_t peer_txt);
DLLIMPORT void WINAPI lum_sdk_set_not_peer_txt_c(not_peer_txt_t not_peer_txt);
DLLIMPORT int WINAPI lum_sdk_get_choice_c(void);
DLLIMPORT void WINAPI lum_sdk_clear_choice_c(void);
DLLIMPORT void WINAPI lum_sdk_set_choice_change_cb_c(
    lum_sdk_choice_change_t cb);
DLLIMPORT char * WINAPI lum_sdk_get_bw_c(int raw_bw);

// internal and testing use only
DLLIMPORT void WINAPI lum_sdk_init_c(char *appid);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_init_autorun_c(char *appid,
    int autorun);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_init_wait_c(char *appid);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_run_c(void);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_run_wait_c(void);
#ifdef __cplusplus
}
#endif

// C++ decorated imports
DLLIMPORT int WINAPI lum_sdk_is_supported(void);
DLLIMPORT int WINAPI lum_sdk_is_supported2(int *reason);
DLLIMPORT void WINAPI lum_sdk_init_ui(char *appid);
DLLIMPORT void WINAPI lum_sdk_init_monitor(char *appid);
DLLIMPORT void WINAPI lum_sdk_uninit(void);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_dlg_size(int dlg_size);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_dlg_type(dlg_type_t dlg_type);
DLLIMPORT void WINAPI lum_sdk_set_dlg_pos_type(dlg_pos_type_t dlg_pos_type);
DLLIMPORT void WINAPI lum_sdk_set_dlg_pos(double top, double left);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_set_txt_type(txt_type_t txt_type);
DLLIMPORT void WINAPI lum_sdk_set_txt_culture(char *txt_culture);
DLLIMPORT void WINAPI lum_sdk_set_bg_color(char *bg_color);
DLLIMPORT void WINAPI lum_sdk_set_btn_color(char *btn_color);
DLLIMPORT void WINAPI lum_sdk_set_txt_color(char *txt_color);
DLLIMPORT void WINAPI lum_sdk_set_app_name(char *app_name);
DLLIMPORT void WINAPI lum_sdk_set_tos_link(char *tos_link);
DLLIMPORT void WINAPI lum_sdk_set_logo_link(char *logo_link);
DLLIMPORT void WINAPI lum_sdk_set_benefit_txt(char *benefit_txt);
DLLIMPORT void WINAPI lum_sdk_set_peer_txt(peer_txt_t peer_txt);
DLLIMPORT void WINAPI lum_sdk_set_not_peer_txt(not_peer_txt_t not_peer_txt);
DLLIMPORT int WINAPI lum_sdk_get_choice(void);
DLLIMPORT void WINAPI lum_sdk_clear_choice(void);
DLLIMPORT void WINAPI lum_sdk_set_choice_change_cb(lum_sdk_choice_change_t cb);
DLLIMPORT char * WINAPI lum_sdk_get_bw(int raw_bw);

// internal and testing use only
DLLIMPORT void WINAPI lum_sdk_init(char *appid);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_init_autorun(char *appid, int autorun);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_init_wait(char *appid);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_run(void);
DEPRECATED DLLIMPORT void WINAPI lum_sdk_run_wait(void);
#endif

#endif

#ifndef DISPLAYCONF_DEF
#define DISPLAYCONF_DEF

#define NOTICE_TIMEOUT 20
#define SLEEP_TIMEOUT 255

#define PAGE_STATUS 0
#define PAGE_NOTICE 1
#define PAGE_MENU 2
#define PAGE_LOADING 3
#define PAGE_BUSY 4
#define PAGE_SPLASH 5

#define FNAME_SIZE 15 //13 chars + [] for dir.

#if ENABLE_GUI && ENABLE_LCD1602
#define MENU_ITEMS 2
#else
#define MENU_ITEMS 6
#endif

#endif
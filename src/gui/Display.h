#ifndef DISPLAY_H
#define DISPLAY_H

#include "DisplayConf.h"
#include <stdint.h>

class Display
{
  protected:
    struct __attribute__((__packed__)) uiFlags
    {
        unsigned int page : 6;
        unsigned int driveSel : 2;
    } f;
    uint8_t idle_timer;
    uint8_t sleep_timer;
    uint8_t notice_timer;
    const char *notice_header;
    const char *notice_message;
    virtual void sleepOn() {};
    virtual void sleepOff() {};
    virtual void drawMenu() {};
    virtual void drawPage();
    virtual void loadingScreen() {};
    virtual void busyScreen() { };
    virtual void noticeScreen() {};
    virtual void splashScreen() {};
    virtual void statusScreen() {};

  private:
  public:
    int16_t idx_sel; // where were we in the menu index ?
    int8_t menu_sel; // menu index
    int8_t menu_max;
    char menuFileNames[MENU_ITEMS][FNAME_SIZE];
    Display();
    ~Display();
    virtual void init(){};
    void setPage(uint8_t);
    uint8_t getPage()
    {
        return f.page;
    }
    void showNoticeP(const char *, const char *);
    void showNotice(const char *, char *);
    void update();
    void showDriveBusy(uint8_t r_drive);
    void showDriveIdle();
    void showDriveLoading();
    void selectDrive(uint8_t r_drive)
    {
        f.driveSel = r_drive;
    }
    uint8_t getSelectedDrive()
    {
        return f.driveSel;
    }
    bool isDriveA()
    {
        return f.driveSel & (1 << 0) ? true : false;
    }
    bool isDriveB()
    {
        return f.driveSel & (1 << 1) ? true : false;
    }
    void loadMenuFiles();
    void buttonAction(int8_t button);
};

#endif

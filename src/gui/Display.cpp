
#include "Display.h"
#include "ADCButton.h"
#include "DiskFile.h"
#include "DisplayConf.h"
#include "FloppyDrive.h"
#include "fddEMU.h"
#include "serial/SerialUI.h"
#include <string.h>

Display::Display()
{
    notice_timer = NOTICE_TIMEOUT;
    sleep_timer = SLEEP_TIMEOUT;
    menu_sel = 0;
    setPage(PAGE_SPLASH);
}

Display::~Display()
{
}

void Display::showNoticeP(const char *header, const char *message)
{
    notice_header = header;
    notice_message = message;
    setPage(PAGE_NOTICE);
    notice_timer = NOTICE_TIMEOUT;
    idle_timer = 0; // show ASAP
    update();
}

void Display::showDriveIdle()
{
    Serial.print(F("showDriveIdle\n"));
    selectDrive(0);
    setPage(PAGE_STATUS);
    idle_timer = 0; // update screen ASAP
    update();
}

void Display::showDriveBusy(uint8_t r_drive)
{
    selectDrive(r_drive);
    setPage(PAGE_BUSY);
    idle_timer = 0; // update screen ASAP
    update();
}

void Display::showDriveLoading()
{
    setPage(PAGE_LOADING);
    idle_timer = 0; // update screen ASAP
    update();
}

void Display::setPage(uint8_t r_page)
{
    f.page = r_page; // set requested page
    if (sleep_timer == 0)
    {
        sleepOff();
#if DEBUG
        Serial.print(F("Screen wakeup\n"));
#endif // DEBUG
       // showDriveIdle();
    }
    sleep_timer = SLEEP_TIMEOUT; // reset sleep timer
}

void Display::drawPage()
{
    switch (f.page)
    {
    case PAGE_STATUS:
        statusScreen();
        break;
    case PAGE_SPLASH:
        splashScreen();
        break;
    case PAGE_BUSY:
        busyScreen();
        break;
    case PAGE_LOADING:
        loadingScreen();
        break;
    case PAGE_NOTICE:
        noticeScreen();
        break;
    case PAGE_MENU:
        drawMenu();
        break;
    default:
        statusScreen();
    }
}

void Display::loadMenuFiles()
{
    menu_max = MENU_ITEMS;
    if (sdfile.nFiles + 1 < menu_max)
        menu_max = sdfile.nFiles + 1;

    // Limit menu selection
    if (menu_sel < 0)
    {
        menu_sel = 0;
        idx_sel--;
    }
    else if (menu_sel >= menu_max)
    {
        menu_sel = menu_max - 1;
        idx_sel++;
    }
    // Limit index selection (+1)add one more entry for "nodisk" or "virtual disk"
    if (idx_sel > (sdfile.nFiles - MENU_ITEMS + 1))
        idx_sel = sdfile.nFiles - MENU_ITEMS + 1;
    else if (idx_sel < 0)
        idx_sel = 0;
    sdfile.openDir((char *)s_RootDir); // open directory
    for (int16_t i = 0; i < idx_sel; i++)
        sdfile.getNextFile(); // skip some files
    for (int8_t i = 0; i < menu_max; i++)
    {
        if (sdfile.getNextFile())
            memcpy(menuFileNames[i], sdfile.getFileName(), 13);
        else
        {
#if ENABLE_VFFS
            strcpy_P(menuFileNames[i], str_label);
#else
            strcpy_P(menuFileNames[i], str_nodisk);
#endif // ENABLE_VFFS
        }
    }
}

void Display::update()
{
    if (idle_timer == 0)
    {

        if (sleep_timer)
        {
            if (notice_timer)
            {
                notice_timer--;
                sleep_timer = SLEEP_TIMEOUT; // reset sleep timer
                if (notice_timer == 0)
                    setPage(PAGE_STATUS);
            }

            // update page
            drawPage();

            sleep_timer--;
            if (sleep_timer == 0)
            {
                sleepOn();
#if DEBUG
                Serial.print(F("Screen sleep\n"));
#endif // DEBUG
            }
        }
    }
    idle_timer++;
}

void Display::buttonAction(int8_t button)
{
    if (button <= 0) // do nothing
        return;

    if (sleep_timer == 0)     // if screen asleep
        setPage(PAGE_STATUS); // wake up screen @status

    sleep_timer = SLEEP_TIMEOUT; // reset sleep timer

    switch (button)
    {
    case BTN_EXTRA:                   // load virtual disk to selected drive
        if (getPage() == PAGE_STATUS) // behave as drive select
        {
            if (getSelectedDrive())
            {
                drive[getSelectedDrive() - 1].loadVirtualDisk();
                showDriveIdle();
            }
        }
        break;
    case BTN_DOWN:                    // Next file
        if (getPage() == PAGE_STATUS) // behave as drive select+
        {
            if (getSelectedDrive() == 0)
                selectDrive(DRIVE0);
#if ENABLE_DRIVE_B
            else if (isDriveA())
                selectDrive(DRIVE1);
#endif // ENABLE_DRIVE_B
        }
        else if (getPage() == PAGE_MENU) // behave as file select+
        {
            menu_sel++;
            loadMenuFiles();
        }
        break;
    case BTN_UP:                      // Previous file
        if (getPage() == PAGE_STATUS) // behave as drive select-
        {
            if (isDriveB())
                selectDrive(DRIVE0);
        }
        else if (getPage() == PAGE_MENU) // behave as file select-
        {
            menu_sel--;
            loadMenuFiles();
        }
        break;
    case BTN_OK:                      // load disk
        if (getPage() == PAGE_STATUS) // if we are in STATUS page
        {
            if (getSelectedDrive()) // if a drive is selected open file selection menu
            {
                menu_sel = 0;
                idx_sel = 0;
                setPage(PAGE_MENU);
                loadMenuFiles();
            }
        }
        else if (getPage() == PAGE_MENU) // if we are in file selection menu load selected file
        {
            bool load_res = true;
            ;

            showDriveLoading();
            if (strcmp_P(menuFileNames[menu_sel], str_label) == 0)
                drive[getSelectedDrive() - 1].loadVirtualDisk();
            else
                load_res = drive[getSelectedDrive() - 1].load(menuFileNames[menu_sel]);
            if (load_res)
                setPage(PAGE_STATUS); // return to status else show error message
        }
        break;
    case BTN_CANCEL:                // eject disk
        if (getPage() == PAGE_MENU) // behave as cancel
        {
            setPage(PAGE_STATUS); // return to status
        }
        else if (getPage() == PAGE_STATUS)
        {
            if (getSelectedDrive()) // if a drive is selected behave as eject
                drive[getSelectedDrive() - 1].eject();
        }
        break;
    }
}

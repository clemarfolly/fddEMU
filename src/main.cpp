// -----------------------------------------------------------------------------
// fddEMU "Floppy Disk Drive Emulator"
// Copyright (C) 2021 Acemi Elektronikci
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------------------


#include "FloppyDrive.h"
#include "simpleUART.h"
#include <avr/io.h>
#if WDT_ENABLED
  #include <avr/wdt.h>
#endif  //WDT_ENABLED  
#include "pff.h"
#include "FDisplay.h"
#include "constStrings.h"
#include "DiskFile.h"
#include "ADCButton.h"

int16_t idx_sel = 0; //where were we in the menu index ?

void loadMenuFiles()
{      
  disp.menu_max = MENU_ITEMS;
  if (sdfile.nFiles < disp.menu_max) disp.menu_max = sdfile.nFiles;
  //Limit menu selection
  if (disp.menu_sel < 0) 
	  {
	  disp.menu_sel = 0;
	  idx_sel--;
	  }
  else if (disp.menu_sel >= disp.menu_max) 
	{
	  disp.menu_sel = disp.menu_max - 1;
	  idx_sel++;
	}
  //Limit index selection  
  if (idx_sel >= (sdfile.nFiles - MENU_ITEMS)) idx_sel = sdfile.nFiles - MENU_ITEMS - 1;
  else if (idx_sel < 0) idx_sel = 0;  
  sdfile.openDir((char *)s_RootDir);  //open directory
  for (int16_t i=0; i < idx_sel; i++) sdfile.getNextFile(); //skip some files
  for (int8_t i=0; i < disp.menu_max && sdfile.getNextFile(); i++)
  {
    memcpy(disp.menuFileNames[i], sdfile.getFileName(), 13);    
  }
}

void buttonAction(int button)
{
  if (button <= 0 ) //do nothing
    return;

  switch(button)
  {
    case  5:  //select drive     
      Serial.print_P(str_selected);
      Serial.print_P(str_drive);

      switch (disp.getSelectedDrive())
		  {
		    case 0:
          disp.selectDrive(1 << BIT_DRIVE0);	
          disp.setPage(PAGE_STATUS);		    
			    Serial.write('A');
			    break;
      #if ENABLE_DRIVE_B
		    case 1:          
			    disp.selectDrive(1 << BIT_DRIVE1);          
          disp.setPage(PAGE_STATUS);
			    Serial.write('B');
			    break;
      #endif //ENABLE_DRIVE_B  
		    default:
			    disp.showDriveIdle();
			    Serial.print_P(str_none);          
		  }
      Serial.write('\n');
      break;
    case  4:  //Next file
      if ( disp.getSelectedDrive() ) //if a drive selected
      {
        if (disp.getPage() == PAGE_MENU)
        {
          disp.menu_sel++;
        }
        else
        {
			    disp.menu_sel = 0;
          idx_sel = 0;			
          disp.setPage(PAGE_MENU);
        }
        loadMenuFiles();
        //Output to serial  
        Serial.print_P(str_selected);
        Serial.print(disp.menuFileNames[disp.menu_sel]);
        Serial.write('\n');  
      }
    break;
  case  3:  //Previous file
      if ( disp.getSelectedDrive() ) //if a drive selected
      {
        if (disp.getPage() == PAGE_MENU)
        {
          disp.menu_sel--;
        }
        else
        {
			    disp.menu_sel = 0;
          idx_sel = 0;			
          disp.setPage(PAGE_MENU);
        }
        loadMenuFiles();
        //Output to serial  
        Serial.print_P(str_selected);
        Serial.print(disp.menuFileNames[disp.menu_sel]);
        Serial.write('\n');  
      }
		break;
  case  2:  //load disk    
    if (disp.getPage() != PAGE_MENU) break; //if we are not in menu disable load   
    disp.showDriveLoading();
		Serial.print_P(str_loading);
		Serial.print(disp.menuFileNames[disp.menu_sel]);
		Serial.write('\n');		
    drive[disp.getSelectedDrive() - 1].load(disp.menuFileNames[disp.menu_sel]); 
    disp.showDriveIdle();
		break;		
  case  1:  //eject disk
    if (disp.getPage() == PAGE_MENU)  //behave as cancel
    {
      disp.showDriveIdle();
      Serial.print_P(str_cancel);
    }
    else
    {      
      if (disp.getSelectedDrive()) //if a drive is selected behave as eject
      {
        Serial.print_P(str_eject);
        if (disp.isDrive0()) Serial.write('A');
        else if (disp.isDrive1()) Serial.write('B');;
        Serial.write('\n');
        drive[disp.getSelectedDrive() - 1].eject();
      }
    }
    disp.setPage(PAGE_STATUS);
    break;
  }  
}

void readRx()
{
char ch = Serial.read();

switch(ch)
	{
    case 'S':
    case 's':
      buttonAction(5);
      break;
	  case 'p':
	  case 'P':
      buttonAction(3);
		  break;
	  case 'n':
	  case 'N':
      buttonAction(4);
		  break;      
	  case 'l':	//OK
	  case 'L':
      buttonAction(2);
		  break;		
	  case 'e':	//Cancel
	  case 'E':
      buttonAction(1);
		  break;
  #if DEBUG    
	  case 'x':
	  case 'X':
		  disp.showNoticeP(errHDR, err_test);
  #endif //DEBUG    
	}
}

void serialIntro()
{
  Serial.write('\n');
  Serial.write('\n');
  Serial.print_P(str_fddEMU);
  Serial.write(' ');
  Serial.print_P(str_2021);
  Serial.write(' ');
  Serial.print_P(str_acemi);
  Serial.write(' ');
  Serial.print_P(str_elektron);
  Serial.write('\n');
  Serial.write('\n');
  Serial.print_P(str_usage);    
  Serial.write('\n');
  Serial.write('\n');
}

int main(void)
{ 
#if WDT_ENABLED  
  wdt_enable(WDTO_8S);  
#endif  //WDT_ENABLED
  Serial.init(115200);
  serialIntro();  
  drive[0].load((char *)s_bootfile);   //if there is "BOOT.IMG" on SD load it
   
  for(;;)
  {   
  #if WDT_ENABLED  
    wdt_reset();
  #endif //WDT_ENABLED  
    if (GET_DRVSEL() )
    {
       disp.showDriveBusy(GET_DRVSEL());
      Serial.print_P(str_busy); //very short message, take too long and get drive read errors 
      drive[GET_DRVSEL() - 1].run();      
      Serial.print_P(str_drive);
      if (disp.isDrive0()) Serial.write('A');
      else if (disp.isDrive1()) Serial.write('B');
      Serial.print_P(str_idle);
      disp.showDriveIdle();
      Serial.write('\n');
    }    
    if (adcReady) buttonAction(abtn.read());    
	  if (rxReady) readRx();
	  disp.update();
  }
}
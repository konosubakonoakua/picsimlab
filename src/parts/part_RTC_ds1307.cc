/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"../picsimlab5.h"
#include"part_RTC_ds1307.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC
};

const char pin_names[8][10] = {"X1", "X2", "Vbat", "GND", "SDA", "SCL", "SQW", "VCC"};
const char pin_values[8][10] = {
 "OSC1",
 "OSC2",
 "BAT",
 "GND",
 {0},
 {1},
 {2},
 "+5V"
};

cpart_RTC_ds1307::cpart_RTC_ds1307(unsigned x, unsigned y)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image;

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());


 Bitmap = new lxBitmap (image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 rtc2_init (&rtc2);
 rtc2_rst (&rtc2);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
};

cpart_RTC_ds1307::~cpart_RTC_ds1307(void)
{
 rtc2_end (&rtc2);
 delete Bitmap;
}

void
cpart_RTC_ds1307::Draw(void)
{

 int i;
 board *pboard = Window1.GetBoard ();

 canvas.Init ();

 lxFont font (8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_IC:
     canvas.SetColor (0, 0, 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     canvas.RotatedText ("DS1307", output[i].x1, output[i].y2 - 15, 0.0);
     break;
    default:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

     canvas.SetFgColor (255, 255, 255);
     canvas.RotatedText (pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

     int pinv =pin_values[output[i].id - O_P1][0]; 
     if (pinv > 10)
      {
       canvas.SetFgColor (155, 155, 155);
       canvas.RotatedText (pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
      }
     else
      {
       if (input_pins[pinv] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y2 - 30, 90.0);
       else
        canvas.RotatedText (pboard->MGetPinName (input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
      }
     break;
    }


  }


 canvas.End ();

 rtc2_update (&rtc2);
}

unsigned short
cpart_RTC_ds1307::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_RTC_ds1307::get_out_id(char * name)
{

 if (strcmp (name, "P1") == 0)return O_P1;
 if (strcmp (name, "P2") == 0)return O_P2;
 if (strcmp (name, "P3") == 0)return O_P3;
 if (strcmp (name, "P4") == 0)return O_P4;
 if (strcmp (name, "P5") == 0)return O_P5;
 if (strcmp (name, "P6") == 0)return O_P6;
 if (strcmp (name, "P7") == 0)return O_P7;
 if (strcmp (name, "P8") == 0)return O_P8;


 if (strcmp (name, "IC") == 0)return O_IC;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

String
cpart_RTC_ds1307::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2]);

 return prefs;
}

void
cpart_RTC_ds1307::ReadPreferences(String value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2]);
 Reset ();
}

CPWindow * WProp_RTC_ds1307;

void
cpart_RTC_ds1307::ConfigurePropertiesWindow(CPWindow * wprop)
{
 String Items = "0  NC,";
 String spin;
 WProp_RTC_ds1307 = wprop;
 board *pboard = Window1.GetBoard ();

 for (int i = 1; i <= pboard->MGetPinCount (); i++)
  {
   spin = pboard->MGetPinName (i);

   if (spin.Cmp (lxT ("error")))
    {
     Items = Items + itoa (i) + "  " + spin + ",";
    }
  }

 ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo5"))->SetItems (Items);
 if (input_pins[0] == 0)
  ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = pboard->MGetPinName (input_pins[0]);
   ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo5"))->SetText (itoa (input_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo6"))->SetItems (Items);
 if (input_pins[1] == 0)
  ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {
   spin = pboard->MGetPinName (input_pins[1]);
   ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo6"))->SetText (itoa (input_pins[1]) + "  " + spin);
  }

 ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo7"))->SetItems (Items);
 if (input_pins[2] == 0)
  ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo7"))->SetText ("0  NC");
 else
  {
   spin = pboard->MGetPinName (input_pins[2]);
   ((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo7"))->SetText (itoa (input_pins[2]) + "  " + spin);
  }

 ((CButton*) WProp_RTC_ds1307->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp_RTC_ds1307->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp_RTC_ds1307->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_RTC_ds1307::ReadPropertiesWindow(void)
{
 input_pins[0] = atoi (((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo5"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo6"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp_RTC_ds1307->GetChildByName ("combo7"))->GetText ());
}

void
cpart_RTC_ds1307::Process(void)
{
 const picpin * ppins = Window1.GetBoard ()->MGetPinsValues ();
 board *pboard = Window1.GetBoard ();

 Window5.Set_i2c_bus (input_pins[0] - 1, rtc2_io (&rtc2, ppins[input_pins[1] - 1].value, ppins[input_pins[0] - 1].value));
 pboard->MSetPin (input_pins[0], Window5.Get_i2c_bus (input_pins[0] - 1));

}
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# generated by wxGlade 0.6.3 on Sat Mar  7 17:08:15 2009

import wx
from MiniPloterSerialInterface import MiniPloterSerialInterface

# begin wxGlade: extracode
# end wxGlade



class mini_ploter_01(wx.Frame):
    def __init__(self, *args, **kwds):
        self.mini_ploter = MiniPloterSerialInterface(0, 9600, 1)
        self.couter_of_plotere_max_steps = 100
        self.couter_x_steps = 0
        self.couter_y_steps = 0

        # begin wxGlade: mini_ploter_01.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.notebook_1 = wx.Notebook(self, -1, style=wx.NB_RIGHT)
        self.notebook_1_pane_3 = wx.Panel(self.notebook_1, -1)
        self.notebook_1_pane_2 = wx.Panel(self.notebook_1, -1)
        self.notebook_1_pane_1 = wx.Panel(self.notebook_1, -1)
        self.window_1 = wx.SplitterWindow(self, -1, style=wx.SP_3D|wx.SP_BORDER)
        self.slider_x = wx.Slider(self.window_1, -1, 0, 0, self.couter_of_plotere_max_steps, style=wx.SL_HORIZONTAL|wx.SL_AUTOTICKS|wx.SL_LABELS|wx.SL_SELRANGE)
        self.slider_y = wx.Slider(self.window_1, -1, 0, 0, self.couter_of_plotere_max_steps, style=wx.SL_VERTICAL|wx.SL_AUTOTICKS|wx.SL_LABELS|wx.SL_INVERSE)
        self.button_home = wx.Button(self.notebook_1_pane_1, -1, "HOME")
        self.button_restart = wx.Button(self.notebook_1_pane_1, -1, "RESTART")
        self.button_stop = wx.Button(self.notebook_1_pane_1, -1, "STOP")
        self.button_1_fix = wx.Button(self.notebook_1_pane_2, -1, "")
        self.button_x_up = wx.Button(self.notebook_1_pane_2, -1, "X - UP [ j ]")
        self.button_3_fix = wx.Button(self.notebook_1_pane_2, -1, "")
        self.button_x_up_speed = wx.Button(self.notebook_1_pane_2, -1, "Xx5 [ f ]")
        self.button_y_up = wx.Button(self.notebook_1_pane_2, -1, "Y - UP [ i ]")
        self.button_y_down = wx.Button(self.notebook_1_pane_2, -1, "Y-DOWN [ k ]")
        self.button_y_up_speed = wx.Button(self.notebook_1_pane_2, -1, "Yx5 [ t ]")
        self.button_y_down_speed = wx.Button(self.notebook_1_pane_2, -1, "Yx5 [ g ]")
        self.button_2_fix = wx.Button(self.notebook_1_pane_2, -1, "")
        self.button_x_down = wx.Button(self.notebook_1_pane_2, -1, "X-DOWN [ l ]")
        self.button_4_fix = wx.Button(self.notebook_1_pane_2, -1, "")
        self.button_x_down_speed = wx.Button(self.notebook_1_pane_2, -1, "Xx5 [ h ]")
        self.button_1_test = wx.Button(self.notebook_1_pane_2, -1, "test [ 1 ]")
        self.button_2_test = wx.Button(self.notebook_1_pane_2, -1, "test [ 2 ]")
        self.button_3_test = wx.Button(self.notebook_1_pane_2, -1, "test [ 3 ]")
        self.button_4_test = wx.Button(self.notebook_1_pane_2, -1, "test [ 4 ]")
        self.list_box_set_motor_speed = wx.ListBox(self.notebook_1_pane_3, -1, choices=["5", "7", "10", "20", "50"], style=wx.LB_SINGLE)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_COMMAND_SCROLL, self.slider_x_scrool_event, self.slider_x)
        self.Bind(wx.EVT_COMMAND_SCROLL, self.slider_y_scrool_event, self.slider_y)
        self.Bind(wx.EVT_BUTTON, self.button_home_click_event, self.button_home)
        self.Bind(wx.EVT_BUTTON, self.button_restart_click_event, self.button_restart)
        self.Bind(wx.EVT_BUTTON, self.button_stop_click_event, self.button_stop)
        self.Bind(wx.EVT_BUTTON, self.button_x_up_evt, self.button_x_up)
        self.Bind(wx.EVT_BUTTON, self.button_x_up_speed_evt, self.button_x_up_speed)
        self.Bind(wx.EVT_BUTTON, self.button_y_up_evt, self.button_y_up)
        self.Bind(wx.EVT_BUTTON, self.button_y_down_evt, self.button_y_down)
        self.Bind(wx.EVT_BUTTON, self.button_y_up_speed_evt, self.button_y_up_speed)
        self.Bind(wx.EVT_BUTTON, self.button_y_down_speed_evt, self.button_y_down_speed)
        self.Bind(wx.EVT_BUTTON, self.button_x_down_evt, self.button_x_down)
        self.Bind(wx.EVT_BUTTON, self.button_x_down_speed_evt, self.button_x_down_speed)
        self.Bind(wx.EVT_BUTTON, self.button_1_test_evt, self.button_1_test)
        self.Bind(wx.EVT_BUTTON, self.button_2_test_evt, self.button_2_test)
        self.Bind(wx.EVT_BUTTON, self.button_3_test_evt, self.button_3_test)
        self.Bind(wx.EVT_BUTTON, self.button_4_test_evt, self.button_4_test)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.set_motor_speed_dclick_evt, self.list_box_set_motor_speed)
        self.Bind(wx.EVT_LISTBOX, self.set_motor_speed_evt, self.list_box_set_motor_speed)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: mini_ploter_01.__set_properties
        self.SetTitle("frame_1")
        self.SetSize((400, 300))
        self.slider_x.SetMinSize((200, 145))
        self.slider_y.SetMinSize((195, 145))
        self.button_1_fix.Enable(False)
        self.button_3_fix.Enable(False)
        self.button_2_fix.Enable(False)
        self.button_4_fix.Enable(False)
        self.list_box_set_motor_speed.SetSelection(2)
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: mini_ploter_01.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.VERTICAL)
        sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_6 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_10 = wx.BoxSizer(wx.VERTICAL)
        sizer_9 = wx.BoxSizer(wx.VERTICAL)
        sizer_8 = wx.BoxSizer(wx.VERTICAL)
        sizer_7 = wx.BoxSizer(wx.VERTICAL)
        sizer_3 = wx.BoxSizer(wx.HORIZONTAL)
        self.window_1.SplitVertically(self.slider_x, self.slider_y)
        sizer_2.Add(self.window_1, 1, wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_3.Add(self.button_home, 0, 0, 0)
        sizer_3.Add(self.button_restart, 0, 0, 0)
        sizer_3.Add(self.button_stop, 0, 0, 0)
        self.notebook_1_pane_1.SetSizer(sizer_3)
        sizer_7.Add(self.button_1_fix, 0, 0, 0)
        sizer_7.Add(self.button_x_up, 0, 0, 0)
        sizer_7.Add(self.button_3_fix, 0, 0, 0)
        sizer_7.Add(self.button_x_up_speed, 0, 0, 0)
        sizer_6.Add(sizer_7, 1, wx.EXPAND, 0)
        sizer_8.Add(self.button_y_up, 0, 0, 0)
        sizer_8.Add(self.button_y_down, 0, 0, 0)
        sizer_8.Add(self.button_y_up_speed, 0, 0, 0)
        sizer_8.Add(self.button_y_down_speed, 0, 0, 0)
        sizer_6.Add(sizer_8, 1, wx.EXPAND, 0)
        sizer_9.Add(self.button_2_fix, 0, 0, 0)
        sizer_9.Add(self.button_x_down, 0, 0, 0)
        sizer_9.Add(self.button_4_fix, 0, 0, 0)
        sizer_9.Add(self.button_x_down_speed, 0, 0, 0)
        sizer_6.Add(sizer_9, 1, wx.EXPAND, 0)
        sizer_10.Add(self.button_1_test, 0, 0, 0)
        sizer_10.Add(self.button_2_test, 0, 0, 0)
        sizer_10.Add(self.button_3_test, 0, 0, 0)
        sizer_10.Add(self.button_4_test, 0, 0, 0)
        sizer_6.Add(sizer_10, 1, wx.EXPAND, 0)
        self.notebook_1_pane_2.SetSizer(sizer_6)
        sizer_4.Add(self.list_box_set_motor_speed, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
        self.notebook_1_pane_3.SetSizer(sizer_4)
        self.notebook_1.AddPage(self.notebook_1_pane_1, "tab1")
        self.notebook_1.AddPage(self.notebook_1_pane_2, "tab2")
        self.notebook_1.AddPage(self.notebook_1_pane_3, "tab3")
        sizer_2.Add(self.notebook_1, 1, wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_1.Add(sizer_2, 1, wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetSizer(sizer_1)
        self.Layout()
        # end wxGlade

    def slider_x_scrool_event(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Event handler `slider_x_scrool_event' not implemented!"
        event.Skip()

    def slider_y_scrool_event(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Event handler `slider_y_scrool_event' not implemented!"
        event.Skip()

    def button_home_click_event(self, event): # wxGlade: mini_ploter_01.<event_handler>
        dial = wx.MessageDialog(None,\
                 'Are you setting manually X and Y axises to HOME position ?',\
                 'Question', wx.YES_NO | wx.NO_DEFAULT | wx.ICON_QUESTION)
        ret = dial.ShowModal()

        print "Please set manual HOME for X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_HOME_X)
        print "Please set manual HOME for Y axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_HOME_Y)
        event.Skip()

    def button_restart_click_event(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Event handler `button_restart_click_event' not implemented!"
        event.Skip()

    def button_stop_click_event(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Event handler `button_stop_click_event' not implemented!"
        event.Skip()

    def button_x_up_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Stepup X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_UP_X)
        self.couter_x_steps +=1
        print "self.couter_x_steps: ", self.couter_x_steps
        self.slider_x.SetValue(self.couter_x_steps)
        event.Skip()

    def button_x_up_speed_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Stepup speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_UP_X_SPEED)
        event.Skip()

    def button_y_up_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Stepup Y axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_UP_Y)
        self.couter_y_steps +=1
        print "self.couter_y_steps: ", self.couter_y_steps
        self.slider_y.SetValue(self.couter_y_steps)
        event.Skip()

    def button_y_down_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down Y axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_DOWN_Y)
        self.couter_y_steps -=1
        print "self.couter_y_steps: ", self.couter_y_steps
        self.slider_y.SetValue(self.couter_y_steps)
        event.Skip()

    def button_y_up_speed_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Stepup speed Y axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_UP_Y_SPEED)
        event.Skip()

    def button_y_down_speed_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed Y axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_DOWN_Y_SPEED)
        event.Skip()

    def button_x_down_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_DOWN_X)
        self.couter_x_steps -=1
        print "self.couter_x_steps: ", self.couter_x_steps
        self.slider_x.SetValue(self.couter_x_steps)
        event.Skip()

    def button_x_down_speed_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_SET_DOWN_X_SPEED)
        event.Skip()

    def button_1_test_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_TEST_1)
        event.Skip()

    def button_2_test_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_TEST_2)
        event.Skip()

    def button_3_test_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_TEST_3)
        event.Skip()

    def button_4_test_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        print "Down speed X axis"
        self.mini_ploter.send_cmd(MiniPloterSerialInterface.CMD_TEST_4)
        event.Skip()

    def set_motor_speed_dclick_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        selection_index = self.list_box_set_motor_speed.GetSelection()
        self.set_motor_speed_cmd(selection_index)

        event.Skip()

    def set_motor_speed_evt(self, event): # wxGlade: mini_ploter_01.<event_handler>
        selection_index = self.list_box_set_motor_speed.GetSelection()
        self.set_motor_speed_cmd(selection_index)

        event.Skip()

    def set_motor_speed_cmd(self, selection_index):
        set_cmd = None
        selection_value = self.list_box_set_motor_speed.GetStringSelection()

        print "selection value = ", selection_value
        print "selection index = ", selection_index

        selection_speed_ref = str(selection_index + 5) 

        if ( MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_5 == selection_speed_ref ):
            set_cmd = MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_5
        elif ( MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_7 == selection_speed_ref ):
            set_cmd = MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_7
        elif ( MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_10 == selection_speed_ref ):
            set_cmd = MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_10
        elif ( MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_20 == selection_speed_ref ):
            set_cmd = MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_20
        elif ( MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_50 == selection_speed_ref ):
            set_cmd = MiniPloterSerialInterface.CMD_SET_MOTOR_SPEED_50
        else:
            set_cmd = None
            print "Unknown command !"

        if ( set_cmd <> None ):
            self.mini_ploter.send_cmd(set_cmd)

# end of class mini_ploter_01


class MiniPloterApp(wx.App):
    def OnInit(self):
        wx.InitAllImageHandlers()
        frame_1 = mini_ploter_01(None, -1, "")
        self.SetTopWindow(frame_1)
        frame_1.Show()
        return 1

# end of class MiniPloterApp

if __name__ == "__main__":
    mini_ploter_01 = MiniPloterApp(0)
    mini_ploter_01.MainLoop()

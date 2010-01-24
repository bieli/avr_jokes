#!/usr/bin/python

# slider.py

import wx

class MyFrame(wx.Frame):
    def __init__(self, parent, id, title):

        wx.Frame.__init__(self, parent, id, title, wx.DefaultPosition, (300, 150))
        panel = wx.Panel(self, -1)

        vbox = wx.BoxSizer(wx.VERTICAL)
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.sld = wx.Slider(panel, -1, 200, 150, 500, wx.DefaultPosition, (250, -1),
                              wx.SL_AUTOTICKS | wx.SL_HORIZONTAL | wx.SL_LABELS)
        btn1 = wx.Button(panel, 8, 'Adjust')
        btn2 = wx.Button(panel, 9, 'Close')

        wx.EVT_BUTTON(self, 8, self.OnAdjust)
        wx.EVT_BUTTON(self, 9, self.OnClose)
        vbox.Add(self.sld, 1, wx.ALIGN_CENTRE)
        hbox.Add(btn1, 1, wx.RIGHT, 10)
        hbox.Add(btn2, 1)
        vbox.Add(hbox, 0, wx.ALIGN_CENTRE | wx.ALL, 20)
        panel.SetSizer(vbox)

    def OnAdjust(self, event):
        val = self.sld.GetValue()

        self.SetSize((val*2, val))
    def OnClose(self, event):
        self.Close()

class MyApp(wx.App):
    def OnInit(self):
        frame = MyFrame(None, -1, 'slider.py')
        frame.Show(True)
        frame.Centre()
        return True

app = MyApp(0)
app.MainLoop()

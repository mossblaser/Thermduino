#!/usr/bin/python

import wx

from ui.ui import MainWindow as MainWindowBase


class MainWindow(MainWindowBase):
	
	def __init__(self, *args, **kwargs):
		MainWindowBase.__init__(self, *args, **kwargs)



class App(wx.App):
	
	def __init__(self, *args, **kwargs):
		wx.App.__init__(self, *args, **kwargs)
	
	
	def OnInit(self):
		self.main_window = MainWindow(None, -1, "")
		self.SetTopWindow(self.main_window)
		self.main_window.Show()
		
		return True



if __name__=="__main__":
	app = App(0)
	app.MainLoop()


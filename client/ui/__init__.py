#!/usr/bin/python

import wx
from ui import MainWindow as MainWindowBase

class MainWindow(MainWindowBase):
	
	def __init__(self, *args, **kwargs):
		MainWindowBase.__init__(self, *args, **kwargs)
		
		self.Bind(wx.EVT_CLOSE, self.on_close, self)
		
		self.set_connected(False)
		
		timer_id = 100
		self.update_timer = wx.Timer(self, timer_id)
		wx.EVT_TIMER(self, timer_id, self.on_timer)
		self.update_timer.Start(1000)
	
	
	def on_close(self, event):
		pass
	
	
	def on_timer(self, event):
		pass
	
	
	@property
	def calibrating(self):
		return self.edit_calibration.GetValue()
	
	
	def set_connected(self, connected):
		"Setup the form for whether we're connected or not."
		
		self.status.SetLabel("Not Connected" if not connected else "Checking...")
		self.temperature.SetLabel("-"        if not connected else "Checking...")
		self.default_temperature.Enable(connected)
		self.default_temperature_tip.Show(self.default_temperature_tip.IsShown() and connected)
		
		self.programme_editor.SetLabel("Not Connected..." if not connected else "Loading...")
		self.programme_editor.Enable(connected)
		self.reset_changes_btn.Enable(connected)
		self.load_programmes_btn.Enable(connected)
		
		self.device_time_date.SetLabel("-" if not connected else "Checking...")
		self.device_date_code.SetLabel("-" if not connected else "Checking...")
		self.refresh_time_btn.Enable(connected)
		self.sync_time_btn.Enable(connected)
		self.min_temp.Enable(self.calibrating and connected)
		self.min_angle.Enable(self.calibrating and connected)
		self.max_temp.Enable(self.calibrating and connected)
		self.max_angle.Enable(self.calibrating and connected)
		self.edit_calibration.Enable(connected)
		
		self.connect_btn.SetLabel("Reconnect" if connected else "Connect")

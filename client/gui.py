#!/usr/bin/python

import wx
from ui import MainWindow as MainWindowBase

from BackgroundArduino import BackgroundArduino
from Programme import Programme, ParseError


RECONNECT_FREQUENCY = 10


class MainWindow(MainWindowBase):
	
	def __init__(self, *args, **kwargs):
		MainWindowBase.__init__(self, *args, **kwargs)
		
		self.arduino = None
		
		# Load the serial port name
		try:
			serial_port = open("serial_port.conf", "r").read()
		except IOError:
			serial_port = ""
		self.serial_port.ChangeValue(serial_port)
		
		# Connect to the arduino
		try:
			self.connect()
		except:
			pass # Oh well...
		
		self._last_connected = 0
	
	
	def on_timer(self, event):
		if self.arduino is None:
			self._last_connected += 1
			
			# Retry connecting
			if self._last_connected%RECONNECT_FREQUENCY == 0:
				self.connect()
		
		if self.current_page == "Dashboard":
			self.refresh_dashboard()
		elif self.current_page == "Setup":
			self.refresh_setup()
	
	
	@property
	def current_page(self):
		return self.notebook.GetPageText(self.notebook.GetSelection())
	
	
	def connect(self):
		# Connect to the arduino
		self.disconnect()
		
		port = self.serial_port.GetValue()
		port = port if port != "" else "0"
		
		try:
			self.arduino = BackgroundArduino(port, 9600)
			try:
				self.on_connect(None)
			except Exception, e:
				print e
			self.set_connected(True)
		except:
			self.set_connected(False)
			if self.arduino is not None:
				self.arduino.kill()
			raise
	
	
	def disconnect(self, *args, **kwargs):
		if self.arduino is not None:
			self.arduino.kill(*args, **kwargs)
		self.set_connected(False)
		self.arduino = None
	
	
	def on_connect_click(self, event):
		# Save port selection
		open("serial_port.conf","w").write(self.serial_port.GetValue())
		self.connect()
		return True
	
	
	def on_connect(self, event):
		self.refresh_dashboard()
		self.refresh_setup()
		self._last_connected = 0
		self.reset_programmes()
		self.edit_calibration.SetValue(False)
		
		temp = self.arduino._get_default_temperature()
		if self.default_temperature.GetValue() != temp:
			self.default_temperature.SetValue(temp)
	
	
	def on_close(self, event):
		if event.CanVeto():
			event.Veto()
			self.Hide()
		else:
			self.Destroy()
			self.disconnect(async = True)
	
	
	def show_status(self, id):
		if   id == 0: status_label = "Syncing Clock..."
		elif id == 1: status_label = "OK (No Programmes Loaded)"
		elif id == 2: status_label = "OK (No Programmes Running)"
		elif id == 3: status_label = "OK (Programme Running)"
		elif id == 4: status_label = "OK (Programme Running)"
		else: status_label = "Communication Error"
		self.status.SetLabel(status_label)
		
		self.default_temperature_tip.Show(id >= 3)
		self.default_temperature_tip.Fit()
		self.notebook.Layout()
	
	
	def refresh_dashboard(self):
		if self.arduino is not None:
			try:
				self.show_status(self.arduino._get_status())
				
				_, min_temp, _, max_temp = self.arduino._get_calibration()
				self.default_temperature.SetRange(min_temp, max_temp)
				
				self.temperature.SetLabel(str(self.arduino._get_temperature()))
			
			except:
				self.disconnect()
				raise
	
	
	def show_time_and_date(self):
		self.device_time_date.SetLabel("Off by %d secs"%self.arduino._get_clock_drift())
		day_number, day_of_week, _, _ = self.arduino._get_time()
		self.device_date_code.SetLabel("(Date Code: +%d, %s)"%(
			day_number,
			"Mon Tue Wed Thu Fri Sat Sun".split()[day_of_week]
		))
	
	
	def refresh_setup(self):
		if self.arduino is not None:
			try:
				self.show_time_and_date()
				
				if not self.calibrating:
					min_angle, min_temp, max_angle, max_temp = self.arduino._get_calibration()
					self.min_angle.SetValue(min_angle)
					self.min_temp.SetValue(min_temp)
					self.max_angle.SetValue(max_angle)
					self.max_temp.SetValue(max_temp)
			
			except:
				self.disconnect()
				raise
	
	
	def on_refresh_time_click(self, event):
		if self.arduino is not None:
			try:
				self.show_time_and_date()
			except:
				self.disconnect()
				raise
	
	
	def on_sync_time_click(self, event):
		if self.arduino is not None:
			try:
				self.arduino._sync_clock()
				self.refresh_setup()
			except:
				self.disconnect()
				raise
	
	
	def on_notebook_change(self, event):
		if self.current_page == "Dashboard":
			self.refresh_dashboard()
			self.notebook.Layout()
		elif self.current_page == "Setup":
			self.refresh_setup()
	
	
	def on_default_temperature_change(self, event):
		if self.arduino is not None:
			try:
				self.arduino._set_default_temperature(self.default_temperature.GetValue())
				self.refresh_dashboard()
			except:
				self.disconnect()
				raise
	
	
	def reset_programmes(self):
		if self.arduino is not None:
			try:
				programmes = self.arduino._get_programmes()
				self.programme_editor.SetValue("\n".join(str(p) for p in programmes))
			except:
				self.disconnect()
				raise
	
	
	def on_load_programmes_click(self, event):
		if self.arduino is not None:
			# Attempt to parse input programmes first
			programmes = []
			for text in filter(None, self.programme_editor.GetValue().split("\n")):
				try:
					programmes.append(Programme(text.strip()))
				except ParseError, e:
					wx.MessageBox("Error Interpreting Programme:\n   '%s'\n(%s)"%(
					              text, str(e)), "Programme Error", style=wx.ICON_ERROR)
					raise
			
			try:
				self.arduino._set_programmes(programmes)
				self.reset_programmes()
			except TypeError, e: # XXX: Error returned by timeout...
				self.disconnect()
				raise
			except:
				wx.MessageBox("Could not load programme!\n"
				              + "Are the dates within 250 days of today and each other?",
				              "Programme Error", style=wx.ICON_ERROR)
	
	
	def calibrate_device(self, save = False):
		if self.arduino is not None:
			try:
				min_temp = self.min_temp.GetValue()
				max_temp = self.max_temp.GetValue()
				min_angle = self.min_angle.GetValue()
				max_angle = self.max_angle.GetValue()
				
				def_temp = self.default_temperature.GetValue()
				if def_temp < min_temp:
					def_temp = min_temp
				elif def_temp > max_temp:
					def_temp = max_temp
				self.default_temperature.SetValue(def_temp)
				self.on_default_temperature_change(None)
				
				self.arduino._set_calibration(min_angle, min_temp, max_angle, max_temp, save)
			except:
				self.disconnect()
				raise
	
	
	def on_min_temp_change(self, event):
		min_temp = self.min_temp.GetValue()
		max_temp = self.max_temp.GetValue()
		
		if min_temp >= max_temp:
			max_temp = min_temp + 1
			self.max_temp.SetValue(max_temp)
		self.calibrate_device()
	
	
	def on_max_temp_change(self, event):
		min_temp = self.min_temp.GetValue()
		max_temp = self.max_temp.GetValue()
		
		if max_temp < min_temp:
			min_temp = max_temp - 1
			self.min_temp.SetValue(min_temp)
		self.calibrate_device()
	
	
	def on_min_angle_change(self, event):
		min_angle = self.min_angle.GetValue()
		max_angle = self.max_angle.GetValue()
		
		if min_angle >= max_angle:
			max_angle = min_angle + 1
			self.max_angle.SetValue(max_angle)
		self.calibrate_device()
	
	
	def on_max_angle_change(self, event):
		min_angle = self.min_angle.GetValue()
		max_angle = self.max_angle.GetValue()
		
		if max_angle < min_angle:
			min_angle = max_angle - 1
			self.min_angle.SetValue(min_angle)
		self.calibrate_device()
	
	
	def on_edit_calibration_click(self, event):
		self.min_temp.Enable(self.calibrating)
		self.min_angle.Enable(self.calibrating)
		self.max_temp.Enable(self.calibrating)
		self.max_angle.Enable(self.calibrating)
		
		if not self.calibrating:
			self.calibrate_device(True)
		
		self.refresh_setup()
		
	
	
	def on_reset_programme_click(self, event):
		self.reset_programmes()
	
	
	def on_help_click(self, event):
		wx.MessageBox(
			"\n".join((
				"Examples:",
				"    Turn heating down to 10 at weekends.",
				"       10*c on Sat, Sun",
				"    Turn heating down to 10 at 6pm for the rest of friday.",
				"       10*c on Fri after 6pm",
				"    Turn heating down to 10 on 11/01/2011.",
				"       10*c on 11/01/2011",
				"    Turn heating down to 10 from 11/01/2011 to 20/01/2011",
				"       10*c from 11/01/2011 to 20/01/2011",
				"    Turn heating down to 10 from 11/01/2011 6:30pm to 20/01/2011 1:00am",
				"       10*c from 11/01/2011 6:30pm to 20/01/2011 1:00am",
				"",
				"When the programmes are uploaded they may be reworded slightly.",
				"Programmes lower down the list have higher priority.",
				"If a programme is running it overrides the default temperature.",
				"Start-dates are inclusive, end-dates are exclusive.",
				"Dates must be within 254 days of today and each other.",
				"Times must be a multiple of 15 minutes.",
			)),
			"Programme Help"
		)



class TrayIcon(wx.TaskBarIcon):
	
	def __init__(self):
		wx.TaskBarIcon.__init__(self)
		icon = wx.Icon("./ui/icon.ico", wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon, "Thermduino")



class App(wx.App):
	
	def __init__(self, *args, **kwargs):
		wx.App.__init__(self, *args, **kwargs)
	
	
	def OnInit(self):
		self.tray_icon = TrayIcon()
		self.main_window = MainWindow(None, -1, "")
		self.SetTopWindow(self.main_window)
		self.main_window.Show()
		
		wx.EVT_TASKBAR_LEFT_UP(self, self.show_window)
		wx.EVT_TASKBAR_RIGHT_UP(self, self.show_popup_menu)
		
		self.popup_menu = wx.Menu()
		self.quit_menu_item = self.popup_menu.Append(-1, "Quit")
		self.Bind(wx.EVT_MENU, self.quit_now, self.quit_menu_item)
		
		return True
	
	
	def show_popup_menu(self, event):
		self.tray_icon.PopupMenu(self.popup_menu)
		
	
	
	def show_window(self, event):
		self.main_window.Show()
	
	
	def quit_now(self, event):
		self.main_window.Close(True)
		self.tray_icon.Destroy()



if __name__=="__main__":
	app = App(0)
	app.MainLoop()


#!/usr/bin/python

import time

from threading import Thread, Lock
from Arduino import Arduino


DRIFT_THRESHOLD = 60
LOOP_PERIOD = 1
CLOCK_SYNC_FREQUENCY = 5


class BackgroundArduino(Arduino, Thread):
	
	def __init__(self, *args, **kwargs):
		Thread.__init__(self)
		Arduino.__init__(self, *args, **kwargs)
		
		self.lock = Lock()
		
		self._kill = False
		self.start()
	
	
	def run(self):
		"Background thread."
		
		# Background main-loop
		i = 0
		while not self._kill:
			i += 1
			with self.lock:
				# Sync Clock if needed
				if i%CLOCK_SYNC_FREQUENCY == 0:
					drift = self.get_clock_drift()
					if drift > DRIFT_THRESHOLD:
						self.sync_clock()
			
			time.sleep(LOOP_PERIOD)
	
	
	def kill(self, async=False):
		if self.is_alive():
			self._kill = True
			if not async:
				self.join()
		
		self.serial.close()
	
	
	def _get_status(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_status(self, *args,**kwargs)
	
	
	def _get_clock_drift(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_clock_drift(self, *args,**kwargs)
	
	
	def _sync_clock(self, *args,**kwargs):
		with self.lock:
			return Arduino.sync_clock(self, *args,**kwargs)
	
	
	def _get_default_temperature(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_default_temperature(self, *args,**kwargs)
	
	
	def _set_default_temperature(self, *args,**kwargs):
		with self.lock:
			return Arduino.set_default_temperature(self, *args,**kwargs)
	
	
	def _get_calibration(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_calibration(self, *args,**kwargs)
	
	
	def _get_temperature(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_temperature(self, *args,**kwargs)
	
	
	def _get_programmes(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_programmes(self, *args,**kwargs)
	
	
	def _set_programmes(self, *args,**kwargs):
		with self.lock:
			return Arduino.set_programmes(self, *args,**kwargs)
	
	
	def _get_time(self, *args,**kwargs):
		with self.lock:
			return Arduino.get_time(self, *args,**kwargs)
	
	
	def _set_calibration(self, *args,**kwargs):
		with self.lock:
			return Arduino.set_calibration(self, *args,**kwargs)

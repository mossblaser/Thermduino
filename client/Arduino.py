#!/usr/bin/python

from datetime import date, datetime, timedelta
import time
import serial

from Programme import Programme


class Arduino(object):
	def __init__(self, device, port):
		self.serial = serial.Serial(device, port, timeout = 1)
	
	
	def read(self):
		return self.serial.read()
	
	
	def readByte(self):
		return ord(self.read())
	
	
	def readULong(self):
		num = 0
		for i in range(4):
			num = (num<<8) | self.readByte()
		return num
	
	
	def write(self, value):
		self.serial.write(value)
	
	def writeByte(self, value):
		self.serial.write(chr(value & 0xFF))
	
	def writeULong(self, value):
		for i in range(3,-1,-1):
			self.writeByte((value >> (i*8)) & 0xFF)
	
	
	def get_status(self):
		self.write("s")
		return ord(self.read())
	
	
	def get_temperature(self):
		self.write("t")
		return ord(self.read())
	
	
	def get_default_temperature(self):
		self.write("d")
		return ord(self.read())
	
	
	def set_default_temperature(self, temperature):
		self.write("D%c"%(temperature))
		
		if (ord(self.read()) != 0x01):
			raise Exception("Error setting default temperature!")
	
	
	def get_calibration(self):
		self.write("c")
		return tuple(ord(self.read()) for x in range(4))
	
	
	def set_calibration(self, min_angle, min_temperature,
	                          max_angle, max_temperature, save = False):
		self.write("C%c%c%c%c%c"%(min_angle, min_temperature,
	                            max_angle, max_temperature,
	                            1 if save else 0))
		
		if (ord(self.read()) != 0x01):
			raise Exception("Error setting calibration!")
	
	
	def get_eeprom_writes(self):
		self.write("e")
		return self.readULong()
	
	
	def get_time(self):
		self.write("n")
		return tuple([self.readByte() for x in range(2)]
		             + [self.readULong(), self.readULong()])
	
	
	def set_time(self, day_number, day_of_week, secs_today, start_time):
		self.write("N%c%c"%(day_number, day_of_week))
		self.writeULong(secs_today)
		self.writeULong(start_time)
		
		if (ord(self.read()) != 0x01):
			raise Exception("Error setting day number!")
	
	
	def get_clock_drift(self):
		day_number, day_of_week, secs_today, start_time = self.get_time()
		
		start_time_day = date.fromtimestamp(start_time)
		start_time_s = datetime(start_time_day.year,
		                        start_time_day.month,
		                        start_time_day.day)
		
		# Get the time the device thinks it is
		now_on_device = start_time_s + timedelta(day_number, secs_today)
		device_timedelta = abs(now_on_device - datetime.today())
		
		if day_number == 255:
			# If the device has hit the max day_number then ignore the day number as
			# it is nolonger important.
			device_timedelta = timedelta(0, device_timedelta.seconds)
		
		day_of_week_drift = abs(day_of_week - date.today().weekday())
		
		total_drift = ((device_timedelta.days + day_of_week_drift
		                 )*24*60*60
		               + device_timedelta.seconds)
		
		return total_drift
	
	
	def sync_clock(self, today_day_number = None):
		day_number, day_of_week, secs_today, start_time = self.get_time()
		
		# Day number as specified
		if today_day_number is not None:
			day_number = today_day_number
		
		# Day of week is today's day
		day_of_week = date.today().weekday()
		
		# Start time is today - day number
		start_time = int(time.mktime(
		                  (date.today() - timedelta(day_number)).timetuple()))
		
		# Seconds today is diferrence between now and today's unix time
		secs_today = int(time.time() - time.mktime(date.today().timetuple()))
		
		timeinfo = (day_number, day_of_week, secs_today, start_time)
		self.set_time(*timeinfo)
		return timeinfo
	
	
	def get_zero_day_date(self):
		_, _, _, day_zero_secs = self.get_time()
		day_zero_tpl = time.localtime(day_zero_secs)
		return date(day_zero_tpl.tm_year,
		            day_zero_tpl.tm_mon,
		            day_zero_tpl.tm_mday)
	
	
	def get_programmes(self):
		day_zero = self.get_zero_day_date()
		
		self.write("p")
		num_programmes = self.readByte()
		
		programmes = []
		for i in range(num_programmes):
			programmes.append(Programme(day_zero, *(self.readByte()
			                                        for i in range(5))))
		
		return programmes
	
	
	def set_programmes(self, programmes):
		# Filter out expired programmes
		programmes = filter((lambda p: not p.has_expired(datetime.today())),
		                    programmes)
		
		# Find out what day zero should be
		one_off_programmes = filter((lambda p: not p.weekday), programmes)
		day_zero = min([p.start_date for p in one_off_programmes] + [date.today()])
		
		# What is today's day number given that day_zero
		today_day_number = (date.today() - day_zero).days
		
		# Sync the clock to use new day_zero number
		self.sync_clock(today_day_number)
		
		data = []
		for programme in programmes:
			data.extend(programme.get_data(day_zero))
		data = [chr(d) for d in data]
		
		self.write("P")
		self.writeByte(len(programmes))
		for byte in data:
			self.write(byte)
		
		if (self.readByte() != 0x01):
			raise Exception("Error loading programmes!")

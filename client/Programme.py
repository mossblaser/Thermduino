#!/usr/bin/python

import time, datetime


def to_unixtime(value):
	"Convert a datetime object into a unix timestamp"
	return time.mktime(value.timetuple())


def to_daytime(value):
	"""
	Convert a datetime object into a number of seconds into the day in which that
	datetime falls.
	"""
	return (((value.hour * 60) + value.minute) * 60) + value.second


def from_daytime(value):
	"""
	Convert from a number of seconds into a day into a datetime.time object.
	"""
	HOUR = (60*60)
	MINUTE = 60
	
	hours = value / HOUR
	minutes = (value - (hours * HOUR)) / MINUTE
	seconds = (value - (hours * HOUR)) - (minutes * MINUTE)
	
	return datetime.time(hours, minutes, seconds)


class Programme(object):
	
	# Number of seconds per unit in a start_time/end_time.
	TIME_CONVERSION_RATIO = 900
	
	
	def __init__(self):
		self._weekday      = None
		self._start_date   = None
		self._end_date     = None
		self._days_of_week = None
		self._start_time   = None
		self._end_time     = None
	
	
	def set_weekday(self, value):
		self._weekday = value
	def is_weekday(self):
		return self._weekday
	weekday = property(fget = is_weekday, fset = set_weekday)
	
	
	def set_start_date(self, value):
		if self.weekday: raise Exception("Weekday programme has no start date.")
		self._start_date = value
	def get_start_date(self):
		if self.weekday: return None
		return self._start_date
	start_date = property(fget = get_start_date, fset = set_start_date)
	
	
	def set_end_date(self, value):
		if self.weekday: raise Exception("Weekday programme has no end date.")
		self._end_date = value
	def get_end_date(self):
		if self.weekday: return None
		return self._end_date
	end_date = property(fget = get_end_date, fset = set_end_date)
	
	
	def set_days_of_week(self, value):
		if not self.weekday: raise Exception("One-off programme has no days of week.")
		self._days_of_week = value
	def get_days_of_week(self):
		if not self.weekday: return None
		return self._days_of_week
	days_of_week = property(fget = get_days_of_week, fset = set_days_of_week)
	
	
	def set_week_bits(self, value):
		days_of_week = [False]*7
		for day in range(7):
			days_of_week[day] = value & (1<<day) != 0
		self.days_of_week = tuple(days_of_week)
	def get_week_bits(self):
		if not self.weekday: return None
		return sum(int(en) << num for (num, en) in enumerate(self.days_of_week))
	week_bits = property(fget = get_week_bits, fset = set_week_bits)
	
	
	def set_start_time(self, value):
		self._start_time = value
	def get_start_time(self):
		return self._start_time
	start_time = property(fget = get_start_time, fset = set_start_time)
	
	
	def set_end_time(self, value):
		self._end_time = value
	def get_end_time(self):
		return self._end_time
	end_time = property(fget = get_end_time, fset = set_end_time)
	
	
	def set_from_data(self, day_zero, f0, f1, f2, f3, f4):
		"""
		Set this Programme object up with the data encoded in the 5 fields passed.
		Data is asssumed to be within the limits allowed by the format.
		"""
		
		self.weekday = f0 == 0xFF
		
		self.start_time = from_daytime(f1 * Programme.TIME_CONVERSION_RATIO)
		self.end_time = from_daytime(f3 * Programme.TIME_CONVERSION_RATIO)
		
		self.temperature = f4
		
		if self.weekday:
			self.week_bits = f2
		else:
			self.start_date = day_zero + datetime.timedelta(f0)
			self.end_date = day_zero + datetime.timedelta(f2)
	
	
	def get_data(self, day_zero):
		"""
		Return a tuple containing the five fields used to encode programmes for the
		device. Takes a datetime object which represents the refrence-day used by
		the arduino for timekeeping.
		"""
		
		# Field 0 is either 0xFF on weekdays or 0x00-0xFE for one-offs
		if self.weekday:
			f0 = 0xFF
		else:
			day_number = (self.start_date - day_zero).days
			if self.start_date < day_zero or day_number > 0xFE:
				raise Exception("Day number out of range!")
			f0 = day_number
		
		# Field 1 is the starting time
		f1 = to_daytime(self.start_time) / Programme.TIME_CONVERSION_RATIO
		
		# Field 2 is the weekday bitfield for weekdays or end-day for one-offs
		f2 = self.week_bits if self.weekday else (self.end_date - day_zero).days
		
		# Field 3 is the end time
		f3 = to_daytime(self.end_time) / Programme.TIME_CONVERSION_RATIO
		
		# Field 4 is the temperature
		if not 0 < self.temperature <= 0xFF:
			raise Exception("Temperature out of range!")
		f4 = self.temperature
		
		return (f0,f1,f2,f3,f4)

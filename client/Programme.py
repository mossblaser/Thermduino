#!/usr/bin/python

import re
import time, datetime


# Match a date
DATE_RX = r"(?:(\d{1,2})/(\d{1,2})/(\d{2,4}))"

# Match a time
TIME_RX = r"(?:(\d{1,2})(?::(\d{2}))?\s*(am|pm)?)"

# Day names
DAY_NAMES = [
	"mon monday mondays".split(),
	"tue tues tuesday tuesdays".split(),
	"wed weds wednesday wednesdays".split(),
	"thu thur thurs thursday thursdays".split(),
	"fri friday fridays".split(),
	"sat saturday saturdays".split(),
	"sun sunday sundays".split(),
]

# Recognises a week-day
DAY_RX = r"(?:%s)"%("|".join(sum(DAY_NAMES, [])))

# Valid Seperators for a list of weekdays
DAY_SEP_RX = r"(?:,?\s+)"


def parse_days(raw):
	regex = re.compile(DAY_SEP_RX)
	days_raw = regex.split(raw)
	
	days = [False]*7
	
	for day_raw in days_raw:
		for day_num, names in enumerate(DAY_NAMES):
			if day_raw in names:
				days[day_num] = True
				break
	
	return days


def parse_time(raw = None, hh=None, mm=None, ampm=None):
	"""
	Parse a time in the format accepted by the Programme parser. Also accepts
	pre-tokenized values.
	"""
	if raw is not None:
		regex = re.compile(TIME_RX)
		hh, mm, ampm = regex.match(raw).groups()
	
	# Parse the hour (produce 24-hr clock)
	hh = int(hh) if hh is not None else 0
	hh = ((hh%12) + 12 if ampm == "pm" else
	      (hh%12)      if ampm == "am" else hh)%24
	
	# Assume minute of 0 if not specified
	mm = int(mm) if mm is not None else 0
	
	return ((hh * 60) + mm) * 60


def parse_datetime(raw):
	"""
	Parse a date and a time in the format accepted by the Programme parser.
	Returns a tuple with a datetime.date and a daytime (int).
	"""
	
	regex = re.compile(r"%s(?:\s+%s)?$"%(DATE_RX, TIME_RX))
	dd,mm,yyyy, HH,MM,AMPM = regex.match(raw).groups()
	
	# Convert two-digit years into a full year
	yyyy = int(yyyy) + 2000 if int(yyyy) < 1970 else int(yyyy)
	
	# Generate the date
	date = datetime.date(yyyy, int(mm), int(dd))
	
	time = parse_time(hh=HH, mm=MM, ampm=AMPM)
	
	return date, time


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


def format_time(value):
	"Format a daytime (converting it into a datetime) to the standard format"
	t = from_daytime(value)
	return t.strftime("%I:%M%p").lower()


def format_date(value):
	"Format a datetime to the standard format"
	return value.strftime("%d/%m/%Y")


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
		self._start_time = (value / Programme.TIME_CONVERSION_RATIO) * Programme.TIME_CONVERSION_RATIO
	def get_start_time(self):
		return self._start_time
	start_time = property(fget = get_start_time, fset = set_start_time)
	
	
	def set_end_time(self, value):
		self._end_time = (value / Programme.TIME_CONVERSION_RATIO) * Programme.TIME_CONVERSION_RATIO
	def get_end_time(self):
		return self._end_time
	end_time = property(fget = get_end_time, fset = set_end_time)
	
	
	def set_from_data(self, day_zero, f0, f1, f2, f3, f4):
		"""
		Set this Programme object up with the data encoded in the 5 fields passed.
		Data is asssumed to be within the limits allowed by the format.
		"""
		
		self.weekday = f0 == 0xFF
		
		self.start_time = f1 * Programme.TIME_CONVERSION_RATIO
		self.end_time = f3 * Programme.TIME_CONVERSION_RATIO
		
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
		f1 = self.start_time / Programme.TIME_CONVERSION_RATIO
		
		# Field 2 is the weekday bitfield for weekdays or end-day for one-offs
		f2 = self.week_bits if self.weekday else (self.end_date - day_zero).days
		
		# Field 3 is the end time
		f3 = self.end_time / Programme.TIME_CONVERSION_RATIO
		
		# Field 4 is the temperature
		if not 0 < self.temperature <= 0xFF:
			raise Exception("Temperature out of range!")
		f4 = self.temperature
		
		return (f0,f1,f2,f3,f4)
	
	
	def has_expired(self, now):
		"""
		Test to see if this programme has expired (and therefore can be ignored and
		not uploaded to the device).
		"""
		
		if self.weekday:
			# Weekday programmes never expire (but they can be useless and not apply
			# to any days).
			return self.week_bits != 0
		else:
			# Has the end-date passed?
			return (self.end_date < now.date()
			        or (self.end_date == now.date()
			            and self.end_time < to_daytime(now.time())))
	
	
	def __str__(self):
		"""
		Return a string accepted by the method "parse" containing the data in this
		programme.
		"""
		output = ""
		
		# Temperature
		output += "%dc "%(self.temperature)
		
		if self.weekday:
			days = (day for (_, day) in
			        filter((lambda (num, _): self.days_of_week[num]),
			               enumerate("Mon Tue Wed Thu Fri Sat Sun".split())))
			output += "on %s "%(", ".join(days))
			
			if self.start_time != 0 and self.end_time != 60*60*24:
				output += "between %s and %s"%(format_time(self.start_time),
				                               format_time(self.end_time))
			elif self.start_time == 0 and self.end_time != 60*60*24:
				output += "until %s"%format_time(self.end_time)
			elif self.start_time != 0 and self.end_time == 60*60*24:
				output += "after %s"%format_time(self.start_time)
		else:
			if self.start_time == 0:
				frm = format_date(self.start_date)
			else:
				frm = "%s %s"%(format_date(self.start_date),
				               format_time(self.start_time))
			
			if self.end_time == 60*60*24:
				# Show next day's date as we're effectively stopping on that day
				to = format_date(self.end_date + datetime.timedelta(1))
			elif self.end_time == 0:
				to = format_date(self.end_date)
			else:
				to = "%s %s"%(format_date(self.end_date),
				              format_time(self.end_time))
			
			output += "from %s to %s"%(frm, to)
		
		return output.strip()
	
	
	def parse(self, raw):
		"""
		Parse a string in the format produced by __str__ and set this Programme to
		the values it contains.
		"""
		
		# The lower-case change is just to make am/pm and weekdays easier (which
		# aren't checked usign regexes) and the added space is a hack which means
		# that if a list of days doesn't have a time on it still ends with
		# whitespace (satisfying the list of days portion of the regex) which is
		# otherwise ignored.
		raw = raw.lower() + " "
		
		# Match a valid input string (woah... that's a big ass-regex...)
		regex = re.compile((
			r"(?P<temperature>\d{1,2})(?:\s*(?:[*]c|c))?\s+" + # Temperature
			r"(?:" +
				# One-off programme
					r"from\s+" +
					r"(?P<start>%(date)s?(?:\s+%(time)s)?)" + # Start date/time
					r"\s+to\s+"+
					r"(?P<end>%(date)s?(?:\s+%(time)s)?)" + # End date/time
				
				r"|" +
				
				# Weekday programme
					r"on\s+" +
					r"(?P<weekdays>(?:%(day)s%(day_sep)s)+)" + # Days of the week
					r"(?:"+
						# Range
						r"between\s+" +
						r"(?P<start_time>%(time)s)" +
						r"\s+and\s+"
						r"(?P<end_time>%(time)s)" +
					r"|"+
						# Until
						r"until\s+" +
						r"(?P<until_time>%(time)s)" +
					r"|"+
						# After
						r"after\s+" +
						r"(?P<after_time>%(time)s)" +
					r"|"+
						# No times
					r")" +
			r")\s*$"
		)%{"date":DATE_RX, "time":TIME_RX, "day":DAY_RX, "day_sep":DAY_SEP_RX})
		
		# Match the user's input
		match = regex.match(raw)
		if match is None:
			raise Exception("Could not parse input!")
		
		# Extract fields
		temperature_m = match.group("temperature")
		
		start_m = match.group("start")
		end_m = match.group("end")
		
		weekdays_m = match.group("weekdays")
		start_time_m = match.group("start_time")
		end_time_m = match.group("end_time")
		until_time_m = match.group("until_time")
		after_time_m = match.group("after_time")
		
		self.temperature = int(temperature_m)
		
		# Is it a weekday programme?
		self.weekday = weekdays_m is not None
		
		if not self.weekday:
			# Got a one-off programme
			self.start_date, self.start_time = parse_datetime(start_m)
			self.end_date, self.end_time = parse_datetime(end_m)
			
		else:
			# Got a weekday programme
			self.days_of_week = parse_days(weekdays_m)
			
			if after_time_m is not None:
				self.start_time = parse_time(after_time_m)
				self.end_time = 24*60*60
			elif until_time_m is not None:
				self.start_time = 0
				self.end_time = parse_time(until_time_m)
			elif start_time_m is not None and end_time_m is not None:
				self.start_time = parse_time(start_time_m)
				self.end_time = parse_time(end_time_m)
			else:
				self.start_time = 0
				self.end_time = 24*60*60


import math

def membertrap(data, x):

	val= 0.0
	if (x <= data[0]):
		val= 0.0
	elif (x >= data[3]):
			val= 0.0
	elif ((data[0] <= x) & (x < data[1])):
		val= (x-data[0])/(data[1]-data[0])
	elif  ((data[1] <= x) & (x < data[2])):
		val= 1.0
	elif  ((data[2] <= x) & (x < data[3])):
		val= 1.0-(x-data[2])/(data[3]-data[2])
	return val*data[4]


def memberbell(data, x):
    if (data[1] != 0.0 ):
        return math.exp(-0.5*((x-data[0])/(data[1]))*((x-data[0])/(data[1])))*data[2]
    elif (x == data[0]):
        return data[2] 
    else:
        return 0.0


def membersigmoid(data, x):
	if (data[0] > data[1]):
		if (x > data[0]):
			return data[2]
		elif (x < data[1]):
			return 0.0
		return (0.5 - 0.5*math.cos((x-data[1])*3.1415/(data[0]-data[1])))*data[2]
	if (data[0] < data[1]):
		if (x < data[0]):
			return data[2]
		elif (x > data[1]):
			return 0.0
		return (0.5 + 0.5*math.cos((x-data[0])*3.1415/(data[1]-data[0])))*data[2]
	return data[2]/2.0


def membertri(data, x):

	val = 0.0
	if (x <= data[0]):
		val= 0.0
	else: 
		if (x >= data[2]):
			val= 0.0
		else:
			if ((data[0] <= x) & (x < data[1])):
				val= (x-data[0])/(data[1]-data[0])
			else:
				if  ((data[1] <= x) & (x < data[2])):
					val= 1.0-(x-data[1])/(data[2]-data[1])
			
		
	
	return (val)*data[3]

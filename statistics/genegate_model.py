'''
Created on 01 июля 2014 г.

@author: dubinets
'''
f = open("Fe_cubic" + ".coo", "w")

f.write("//file created by convertor from ace CarIne Fe_cubic.coo" + "\n")
f.write("51,7008400   51,7852500   35,7405600   90,0000000   90,0000000   90,0000000   5" + "\n")

i=0

for z in range(0, 36, 1):
	for y in range(0, 51, 2):
		for x in range(0, 51, 2):
			i=i+1
			a = ("%.9f" % (x/51.7008400)).replace('.',',')
			b = ("%.9f" % (y/51.7852500)).replace('.',',')
			c = ("%.9f" % (z/35.7405600)).replace('.',',')
			f.write("   " + a + "   " + b + "   " + c + "   " + "26   0,047   0,000   1,0000" + "\n")
f.close()


print (i)
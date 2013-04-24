import shapefile
import random

w = shapefile.Writer(shapefile.POINT)
w.field("ID")

size = 100000
for i in xrange(size):
    x = float(random.randrange(0,size))
    y = float(random.randrange(0,size))
    

    yn = [True, False]
    if (random.choice(yn)):
        w.point(x,y)
        w.record(i)
        

w.save('points')
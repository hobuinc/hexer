import shapefile
import random

import pdb;pdb.set_trace()
w = shapefile.Writer('points',shapefile.POINT)
w.field("ID")

size = 100000
for i in range(size):
    x = float(random.randrange(0,size))
    y = float(random.randrange(0,size))


    yn = [True, False]
    if (random.choice(yn)):
        w.point(x,y)
        w.record(i)


w.close()

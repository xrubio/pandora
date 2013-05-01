#!/usr/bin/python

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Point2DInt

testPoint = Point2DInt(4,6)

print 'point , x: ' +str(testPoint._x) + ' y: ' + str(testPoint._y)
testPoint._x += 1
testPoint._y /= 2
print 'modified point, x: ' +str(testPoint._x) + ' y: ' + str(testPoint._y)


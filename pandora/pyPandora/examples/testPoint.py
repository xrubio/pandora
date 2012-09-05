#!/usr/bin/python

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Point2DInt

testPoint = Point2DInt(4,5)

print 'point , x: ' +str(testPoint._x) + ' y: ' + str(testPoint._y)
testPoint._x = 6
testPoint._y = 8
print '2 - point, x: ' +str(testPoint._x) + ' y: ' + str(testPoint._y)


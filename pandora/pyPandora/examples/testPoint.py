#!/usr/bin/python3.3

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Point2DInt

testPoint = Point2DInt(4,6)

print('point: ',testPoint)
testPoint._x += 1
testPoint._y = testPoint._y//2
print('modified point: ',testPoint)


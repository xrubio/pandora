#!/usr/bin/python

import sys
sys.path.append('../')
sys.path.append('../../')
from pyPandora import Point2DInt, StaticRaster, Raster

testStaticRaster = StaticRaster()
print 'Static Raster - size: ' + str(testStaticRaster.getSize()._x) + ' - ' + str(testStaticRaster.getSize()._y)
size = Point2DInt(5,5)
testStaticRaster.resize(size)
print 'Static Raster - size changed: ' + str(testStaticRaster.getSize()._x) + ' - ' + str(testStaticRaster.getSize()._y)

testRaster = Raster()
testRaster.resize(size)
testRaster.setInitValues(0, 1000, 0)

for i in range(0,size._x):
	for j in range(0,size._y):
		index = Point2DInt(i,j)
		testRaster.setValue(index, i*size._x+j)

for i in range(0,size._x):
	line = ''
	for j in range(0,size._y):
		index = Point2DInt(i,j)
		line = line + '[' + str(i) + '][' + str(j) + ']=' + str(testRaster.getValue(index)) + ' '
	print line


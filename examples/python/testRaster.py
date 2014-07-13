#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Point2DInt, StaticRaster, DynamicRaster, SizeInt

testStaticRaster = StaticRaster()
print('Static Raster - size: ',testStaticRaster.getSize())
size = SizeInt(5,5)
testStaticRaster.resize(size)
print('Static Raster - size changed: ',testStaticRaster.getSize())

testDynamicRaster = DynamicRaster()
testDynamicRaster.resize(size)
testDynamicRaster.setInitValues(0, 1000, 0)

print('Dynamic Raster - size: ',testDynamicRaster.getSize())
for i in range(0,size._width):
	for j in range(0,size._height):
		index = Point2DInt(i,j)
		testDynamicRaster.setValue(index, i*size._width+j)

for i in range(0,size._width):
	line = ''
	for j in range(0,size._height):
		index = Point2DInt(i,j)
		line = line + '[' + str(i) + '][' + str(j) + ']=' + str(testDynamicRaster.getValue(index)) + ' '
	print(line)


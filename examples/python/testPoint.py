#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Point2DInt

testPoint = Point2DInt(4,6)

print('point: ',testPoint)
testPoint._x += 1
testPoint._y = testPoint._y//2
print('modified point: ',testPoint)


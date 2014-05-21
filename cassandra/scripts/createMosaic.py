#!/usr/bin/python3

from subprocess import call
import math

shotsDir = '../shots/'
outDir = './mosaic/'
timeSteps = 100
numRows = 8

# for each time step create a mosaic
for i in range(0,timeSteps):
    newImages = []
    for j in range(1, 1+min(1+i,numRows*numRows)):
        newImages.append(shotsDir+'shots_'+str(j)+'/step_'+'%08d'%i+'.png')
    args = []
    args.append('montage')
    args.append('-tile')
    rowsOfImage = math.ceil(math.sqrt(len(newImages)))
    args.append(str(rowsOfImage)+'x'+str(rowsOfImage))
    args.append('-geometry')
    args.append(str(math.ceil(1000/rowsOfImage)-4)+'x'+str(math.ceil(1000/rowsOfImage)-4)+'+2+2')
    for tile in newImages:
        args.append(tile)
    args.append(outDir+'mosaic_'+'%08d'%i+'.png')
    call(args)

"""
# call convert to create the animated gif
convertArgs = []
convertArgs.append('convert')
convertArgs.append('-delay')
convertArgs.append('25')
convertArgs.append('-loop')
convertArgs.append('1')
#convertArgs.append('-morph')
#convertArgs.append('5')
#convertArgs.append('-quality')
#convertArgs.append('100')
for i in range(0, timeSteps):
    convertArgs.append(outDir+'mosaic_'+'%08d'%i+'.png')
convertArgs.append(outDir+'/mosaic.mpg')
call(convertArgs)
"""


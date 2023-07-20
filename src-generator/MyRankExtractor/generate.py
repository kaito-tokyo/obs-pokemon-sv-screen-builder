import cv2
from glob import glob
from jinja2 import Environment, FileSystemLoader, Template
import numpy as np
from os import path
import sys

data = []
cols = []
for file in glob('./assets/screenshots/MyRankExtractor/*.png'):
    name = path.splitext(path.basename(file))[0]
    args = name.split(' ')
    x0, x1, y0, y1 = [int(x) for x in args[0:4]]
    srcImg = cv2.imread(file)
    destImg = srcImg[y0:y1, x0:x1]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, 200, 255, cv2.THRESH_BINARY)
    cv2.imwrite('./assets/MyRankExtractor/' + name + '.png', destImg)

    data.append('{' + ','.join(str(x) for x in np.ravel(destImg).tolist()) + '}')
    cols.append(str(destImg.shape[1]))

env = Environment(
    loader=FileSystemLoader('src-generator/MyRankExtractor')
)
template = env.get_template('MyRankExtractor.j2')
result = template.render(data=data, cols=cols)
with open('src/modules/MyRankExtractorGenerated.cpp', 'w') as fp:
    fp.write(result)

import cv2
from glob import glob
from jinja2 import Environment, FileSystemLoader, Template
import numpy as np
from os import path
import sys

data = ['{0}', '{0}']
cols = [1, 1]
parenDict = {}
for file in glob('./assets/screenshots/OpponentRankExtractor/*.png'):
    name = path.splitext(path.basename(file))[0]
    args = name.split(' ')
    x0, x1, y0, y1 = [int(x) for x in args[0:4]]
    language = args[4]
    symbolName = args[5]
    srcImg = cv2.imread(file)
    destImg = srcImg[y0:y1, x0:x1]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, 128, 255, cv2.THRESH_BINARY)
    cv2.imwrite('./assets/OpponentRankExtractor/' + name + '.png', destImg)

    parenDict[language] = parenDict.get(language, [0, 1])
    if symbolName == 'parenStart':
        parenDict[language][0] = len(data)
    elif symbolName == 'parenEnd':
        parenDict[language][1] = len(data)

    data.append('{' + ','.join(str(x) for x in np.ravel(destImg).tolist()) + '}')
    cols.append(str(destImg.shape[1]))

parenMap = ['{"' + language + '", {' + ','.join([str(s), str(e)]) + '}}' for language, (s, e) in parenDict.items()]

env = Environment(
    loader=FileSystemLoader('src-generator/OpponentRankExtractor')
)
template = env.get_template('OpponentRankExtractor.j2')
result = template.render(data=data, cols=cols, parenMap=parenMap)
with open('src/OpponentRankExtractorGenerated.cpp', 'w') as fp:
    fp.write(result)

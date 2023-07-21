import cv2
from glob import glob
import numpy as np
from os import path
from cbor2 import dump

descriptorSize = 16
height = 196
algorithm = cv2.AKAZE_create(cv2.AKAZE_DESCRIPTOR_MLDB, descriptorSize)

descriptorSizes = []
pokemonNames = []
data = []

for file in glob('./assets/PokemonRecognizer/*/*.png'):
    name = path.splitext(path.basename(file))[0]
    srcBGRA = cv2.imread(file, cv2.IMREAD_UNCHANGED)
    scaledBGRA = cv2.resize(srcBGRA, (srcBGRA.shape[1] * height // srcBGRA.shape[0], height))
    targetBGR = cv2.cvtColor(scaledBGRA, cv2.COLOR_BGRA2BGR)
    mask = scaledBGRA[:, :, 3]

    descriptors = algorithm.detectAndCompute(targetBGR, mask)[1]
    data.append(np.ravel(descriptors).tolist())
    descriptorSizes.append((descriptorSize + 7) // 8)
    _, pokemonName = name.split(' ')
    pokemonNames.append(pokemonName)

with open('data/config/PokemonRecognizer.cbor', 'wb') as fp:
    dump({
        "data": data,
        "descriptorSizes": descriptorSizes,
        "height": height,
        "pokemonNames": pokemonNames,
    }, fp)

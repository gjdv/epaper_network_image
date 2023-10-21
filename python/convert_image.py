import os

import numpy as np
from matplotlib import image as mpimg


def figure_to_rb_raw(filename, white_threshold=0.8):
    """
    Convert image to .br.raw format
    :param filename: filename of the input image
    :param white_threshold: threshold to define white
    Writes to the same filename but with different extension (.br.raw)
    Black is defined as 3 equal rgb values below white_threshold
    Red is defined as anything having different r and g values
    """
    if os.path.exists(filename):
        img = mpimg.imread(filename)
    else:
        raise Exception("file could not be found: %s" % filename)

    # remove the dimension containing opacity
    if img.shape[2] == 4:
        img = img[:, :, :3]

    white_black = ~((img[:, :, 0] == img[:, :, 1]) & (img[:, :, 1] == img[:, :, 2]) & (img[:, :, 0] < white_threshold))
    white_red = ~(img[:, :, 0] != img[:, :, 1])

    raw_black = np.packbits(white_black).tobytes()
    raw_red = np.packbits(white_red).tobytes()

    size = int.to_bytes(len(raw_black), 2, byteorder='big')

    raw_filename = os.path.splitext(filename)[0] + '.br.raw'
    with open(raw_filename, 'wb') as file:
        file.write(size)       # channel-size
        file.write(b'\xff')    # separator byte
        file.write(raw_black)  # white-black channel
        file.write(raw_red)    # white-red channel


def main():
    """
    Create example image, store in png and in .br.raw format.
    """
    filename = 'example.png'
    w = 800
    h = 480
    img = np.zeros((h, w, 3))
    w3 = int(w/3)
    img[:, w3:w3*2, :] = 1.0  # white
    img[:, w3*2:, 0] = 1.0  # red
    mpimg.imsave(filename, img)
    figure_to_rb_raw(filename)


if __name__ == '__main__':
    main()

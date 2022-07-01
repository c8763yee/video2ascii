import cv2
from PIL import Image
from time import sleep, time
import os
import sys


def resize(img: Image.fromarray, new_width):
    width, height = img.size
    ratio = height / width
    new_height = int(ratio * new_width)
    return img.resize((new_width, new_height))


def frame2ascii(frame):
    return ''.join([ASCII_CHARS[int(pixel / 255 * (len(ASCII_CHARS) - 1))
                                ] for pixel in frame.getdata()])


def print_ascii(frame, length=50):
    os.system('cls')
    data = frame2ascii(resize(frame, length))
    sys.stdout.write(
        "\n".join([data[pixel:pixel+length] for pixel in range(0, len(data), length)]))
    sys.stdout.flush()
    

ASCII_CHARS = ["@", "#", "$", "%", "?", "*", "+", ";", ":", ",", "."]

cap = cv2.VideoCapture(
    'C:/Users/rrrrr/Downloads/standing here i realize_Trim.mp4')
def __main__():
    start = time()
    while True:
        flag, frame = cap.read()
        if not flag:
            break

        print_ascii(Image.fromarray(frame).convert('L'), 90)
        cv2.imshow('nanomachine son', frame)
        cv2.waitKey(15)
    print(f'\n delay: {(time() - start)}')
        
if __name__ == '__main__':
    __main__()
    cap.release()
    cv2.destroyAllWindows()
    sys.exit(0)

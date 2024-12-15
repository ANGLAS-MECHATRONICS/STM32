from PIL import Image
import sys

W = 240
print("\r\n")
print("<width> " + str(W))

H = 135
print("<height> " + str(H))

img = Image.open("image3.png")
if img.width != int(W) or img.height != int(H):
    print("Error: Разрешение указано не верно ( должно совпадать с картинкой )!!!")
    sys.exit(2)

f = open("picFile.txt", "a")

f.write("// size " + str(W) + " x " + str(H) + " \n")
f.write("const unsigned short img[] = { \n ")

for y in range(0, img.height):
    s = ""
    for x in range(0, img.width):
        pixel = img.getpixel((x, y))
        if len(pixel) == 4:  # Si la imagen tiene un canal alfa
            r, g, b, a = pixel
        else:  # Si la imagen no tiene un canal alfa
            r, g, b = pixel
        color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        # for right endiness, so ST7735_DrawImage would work
        color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8)
        s += "0x{:04X},".format(color565)
    s += "  \n"
    f.write(s)

f.write("}; \r\n")
f.close()
print("\r\n Готово !!! OK \r\n")
from cmath import pi
from PIL import Image
import sys
import argparse


# decimated the 8 bits per color component to 4 bits that the Xosera HW supports
def convert_12bpp(img):
    out_img = img.copy()
    pixels = []
    for px in out_img.getdata():
        pixels.append( (px[0] & 0xF0, px[1] & 0xF0, px[2] & 0xF0) )
    out_img.putdata(pixels)
    return out_img

#(argv1, argv2, argv3)
def convert_image(image_file, out_file, num_colors, image_width, image_height):
    im = Image.open(image_file)

    sm_img = im.resize((image_width, image_height)).convert("RGB")

    # must decimate the colors first otherwise out conversion to a palette will pick the same 4 bit color over and over again.
    #  This isn't perfrect size the adaptive palette will interpolate colors to make a "best match", these interpolated colors
    #  will still use the fill 24bpp color depth, not the 12bpp color depth that the Xosera HW has
    sm_img_12bpp = convert_12bpp(sm_img)
    #sm_img_12bpp.show()

    num_colors=int(num_colors)
    sm_img_clut = sm_img_12bpp.convert("P", palette=Image.Palette.ADAPTIVE, colors=num_colors)

    #sm_img_clut.show()

    f = open(f"{out_file}.raw", "wb")

    if num_colors == 256:
        for px in sm_img_clut.getdata():
            f.write(px.to_bytes(1, 'big'))
    elif num_colors == 16:
        b = 0
        count = 0
        for px in sm_img_clut.getdata():
            b = (b << 4) & 0xFF
            b |= px & 0x0F
            if count % 2 == 1:
                f.write(b.to_bytes(1, 'big'))
            count += 1
    else:
        print(f"Number of colors not supported {num_colors}")

    f.close()

    format = sm_img_clut.palette.getdata()[0]
    size = len(sm_img_clut.palette.getdata()[1])
    #print(f"Palette format {format} size {size}")
    colors = [[], [], []]
    offset = 0
    for d in sm_img_clut.palette.getdata()[1]:
        colors[offset%3].append(d)
        offset+=1

    f = open(f"{out_file}.pal.raw", "wb")
    for i in range(num_colors):
        r = colors[0][i] 
        g = colors[1][i] 
        b = colors[2][i] 
        if num_colors == 16:
            c = (r >> 4) << 12 | (r >> 4) << 8 | (g >> 4) << 4 | (b >> 4)
        else:
            c = (r >> 4) << 8 | (g >> 4) << 4 | (b >> 4)
        print(f"Palette {i}: {r:02X}, {g:02X}, {b:02X} {c:04X}")
        f.write(c.to_bytes(2, 'big'))
    f.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("imagefile")
    parser.add_argument("outfile")
    parser.add_argument("-c", "--numcolors", help="the number of colors in the pallete", type=int, default=256)
    parser.add_argument("-x", "--width", help="the number of colors in the pallete", type=int, default=320)
    parser.add_argument("-y", "--height", help="the number of colors in the pallete", type=int, default=240)
    args = parser.parse_args()
    convert_image(args.imagefile, args.outfile, args.numcolors, args.width, args.height)


if __name__ == "__main__":
    main()
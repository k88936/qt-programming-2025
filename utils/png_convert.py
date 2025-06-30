import os
from PIL import Image
icons_config=open("ui/icons.txt","w")
def convert_non_transparent(image_path):
    # Open the image
    img = Image.open(image_path).convert("RGBA")
    datas = img.getdata()

    new_data = []
    new_data2=[]
    new_data3=[]
    for item in datas:
        # Change all non-transparent pixels to white
        if item[3] != 0:
            new_data.append((0,0,0,item[3]))
            new_data2.append((48,246,217,item[3]))
            new_data3.append((255,191,0,item[3]))
        else:
            new_data.append(item)
            new_data2.append(item)
            new_data3.append(item)

    width, height = img.size
    # Update image data
    dark = os.path.splitext(image_path)[0] + "_dark.png"
    highlight = os.path.splitext(image_path)[0] + "_highlight.png"
    img.putdata(new_data)
    img.save(dark, "PNG")
    img.putdata(new_data2)
    img.save(highlight, "PNG")
    img.putdata(new_data3)
    img.save(os.path.splitext(image_path)[0]+"_orange.png", "PNG")
    print("QPushButton{min-width: %dpx;min-height: %dpx;max-width:%dpx;max-height:%dpx;image: url(:/%s);}QPushButton:hover,QPushButton:pressed,QPushButton:disabled,QPushButton:checked{image: url(:/%s);} "%(width,height,width,height,highlight,dark),file=icons_config)
    # print("QTabBar::tab{min-width: %dpx;min-height: %dpx;max-width:%dpx;max-height:%dpx;image: url(:/%s);}QTabBar::tab:hover,QTabBar::tab:selected,QTabBar::tab:last:selected{image: url(:/%s);\n"%(width,height,width,height,highlight,dark),file=icons_config)

convert_non_transparent("ui/github.png")
convert_non_transparent("ui/info.png")
convert_non_transparent("ui/play.png")
convert_non_transparent("ui/settings.png")

convert_non_transparent("ui/attack.png")
convert_non_transparent("ui/defence.png")
convert_non_transparent("ui/delete.png")
convert_non_transparent("ui/explore.png")
convert_non_transparent("ui/march.png")
convert_non_transparent("ui/patrol.png")
convert_non_transparent("ui/repair.png")
convert_non_transparent("ui/stay.png")

convert_non_transparent("ui/production.png")
convert_non_transparent("ui/research.png")
convert_non_transparent("ui/upgrade.png")

convert_non_transparent("ui/ore.png")
convert_non_transparent("ui/all.png")
convert_non_transparent("ui/play-button.png")
convert_non_transparent("ui/pause-button.png")
convert_non_transparent("ui/speed.png")
convert_non_transparent("ui/group.png")
convert_non_transparent("ui/add-group.png")

convert_non_transparent("ui/select.png")
convert_non_transparent("ui/quit.png")
convert_non_transparent("ui/arrow.png")
convert_non_transparent("ui/help.png")

icons_config.close()
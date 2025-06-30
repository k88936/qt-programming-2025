import os
from PIL import Image


def make_white_transparent(image_path):
    # Open the image
    img = Image.open(image_path).convert("RGBA")
    datas = img.getdata()

    t = 16
    new_data = []
    for item in datas:
        # Change all white (also shades of whites)
        # pixels to transparent

        if item[0] >= t and item[1] >= t and item[2] >= t:
            new_data.append((255, 255, 255, 0))
        else:
            new_data.append(item)

    # Update image data
    img.putdata(new_data)
    # Save the image
    img.save(image_path, "PNG")


def soft_transparent(image_path):
    # Open the image
    img = Image.open(image_path).convert("RGBA")
    datas = img.getdata()

    t = 256
    new_data = []
    for item in datas:
        # Change all white (also shades of whites)
        # pixels to transparent

        if item[0] <= t and item[1] <= t and item[2] <= t:
            new_data.append((item[0], item[1], item[2],max( item[0] , item[1] , item[2])))
        else:
            new_data.append(item)

    # Update image data
    img.putdata(new_data)
    # Save the image
    img.save(image_path, "PNG")


def process_directory(directory_path):
    for filename in os.listdir(directory_path):
        if filename.endswith(".png"):
            file_path = os.path.join(directory_path, filename)
            make_white_transparent(file_path)


# Example usage
# process_directory("resources/ui")

soft_transparent("title.png")

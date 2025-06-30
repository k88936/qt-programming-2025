from PIL import Image

def cut_image(image_path, new_width, new_height):
    # Open the image
    img = Image.open(image_path).convert("RGBA")
    original_width, original_height = img.size

    # Create a new image with the specified size and transparent background
    new_img = Image.new("RGBA", (new_width, new_height), (255, 255, 255, 0))

    # Calculate the position to paste the original image onto the new image
    paste_x = (new_width - original_width) // 2
    paste_y = (new_height - original_height) // 2

    # Paste the original image onto the new image
    new_img.paste(img, (paste_x, paste_y))

    # Save the new image
    new_img.save(image_path, "PNG")

# Example usage
# cut_image("resources/ui/loading_background.png",1920,1080)
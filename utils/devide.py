import cv2
import numpy as np

def draw_grid_with_indices(image_path, output_path, unique_output_path):
    # Load the image
    image = cv2.imread(image_path)
    if image is None:
        print("Error: Could not load image.")
        return

    # Get image dimensions
    height, width, _ = image.shape

    # Define the size of the grid boxes
    box_size = 16

    # Create a copy of the image to draw on
    output_image = image.copy()

    # Set font for text
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 0.25
    color = (255, 0, 0)  # Blue color in BGR
    thickness = 1

    # Extract blocks and find unique ones
    blocks = []
    for y in range(0, height, box_size):
        for x in range(0, width, box_size):
            block = image[y:y + box_size, x:x + box_size]
            if block.shape[0] == box_size and block.shape[1] == box_size:
                blocks.append(block)

    # Remove duplicate blocks
    unique_blocks = []
    for block in blocks:
        if not any(np.array_equal(block, unique_block) for unique_block in unique_blocks):
            unique_blocks.append(block)

    # Calculate new dimensions for the unique blocks image
    num_unique_blocks = len(unique_blocks)
    new_width = box_size * min(num_unique_blocks, width // box_size)
    new_height = box_size * ((num_unique_blocks + (width // box_size) - 1) // (width // box_size))

    # Create an image to hold the unique blocks
    unique_image = np.zeros((new_height, new_width, 3), dtype=np.uint8)

    # Place unique blocks into the new image
    index = 0
    for y in range(0, new_height, box_size):
        for x in range(0, new_width, box_size):
            if index < num_unique_blocks:
                unique_image[y:y + box_size, x:x + box_size] = unique_blocks[index]
                index += 1

    # Save the unique blocks image
    cv2.imwrite(unique_output_path, unique_image)

    # Draw the grid and indices on the unique blocks image
    for y in range(0, new_height, box_size):
        for x in range(0, new_width, box_size):
            # Draw vertical and horizontal lines
            cv2.line(unique_image, (x, 0), (x, new_height), (0, 255, 0), 1)
            cv2.line(unique_image, (0, y), (new_width, y), (0, 255, 0), 1)

            # Calculate the position for the text
            text_position = (x + 2, y + 12)

            # Put the index text on the image
            cv2.putText(unique_image, str(index), text_position, font, font_scale, color, thickness)
            index += 1

    # Draw the last vertical and horizontal lines
    cv2.line(unique_image, (new_width - 1, 0), (new_width - 1, new_height), (0, 255, 0), 1)
    cv2.line(unique_image, (0, new_height - 1), (new_width, new_height - 1), (0, 255, 0), 1)

    # Save the final image with grid and indices
    cv2.imwrite(output_path, unique_image)

# Example usage
draw_grid_with_indices('input.png', 'output_with_indices.png', 'unique_blocks.png')
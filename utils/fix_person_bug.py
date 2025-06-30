import os


def process_ini_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    in_graphics_section = False
    new_lines = []
    image_value = None
    image_turret_value = None
    scale_images_to_value = None

    can_append = True
    for line in lines:
        stripped_line = line.strip()

        if stripped_line.startswith('[') and stripped_line.endswith(']'):
            in_graphics_section = stripped_line == '[graphics]'
            new_lines.append(line)
            continue

        if in_graphics_section:
            if (stripped_line.startswith('total_frames')):
                can_append = False
            if stripped_line.startswith('image:'):
                image_value = stripped_line.split(':', 1)[1].strip()
                if (image_value == 'NONE'):
                    can_append = False
                if can_append:
                    new_lines.append('image:NONE\n')
                    continue
            elif stripped_line.startswith('image_turret:'):
                if can_append:
                    image_turret_value = stripped_line.split(':', 1)[1].strip()
                    new_lines.append(f'image_turret:{image_value}\n')
                    continue
            elif stripped_line.startswith('scaleImagesTo:'):
                if can_append:
                    scale_images_to_value = stripped_line.split(':', 1)[1].strip()
                    new_lines.append(f'scaleTurretImagesTo:{scale_images_to_value}\n')
                    continue

        new_lines.append(line)

    # print(new_lines)
    with open(file_path, 'w', encoding='utf-8') as file:
        file.writelines(new_lines)


def walk_directory(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.ini'):
                file_path = os.path.join(root, file)
                process_ini_file(file_path)


if __name__ == "__main__":
    # walk_directory('resources/units/紧急服务部门/警员')
    walk_directory('resources/units/human')

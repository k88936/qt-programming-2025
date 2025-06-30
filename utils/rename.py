import os
import re

def parse_ini_file(file_path):
    config = {}
    current_section = None

    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            if not line or line.startswith(('#', ';')):
                continue
            if line.startswith('[') and line.endswith(']'):
                current_section = line[1:-1].strip()
                if current_section not in config:
                    config[current_section] = {}
            elif '=' in line:
                key, value = map(str.strip, line.split('=', 1))
                if current_section:
                    config[current_section][key] = value
                else:
                    raise ValueError(f"Key-value pair found outside of section: {line}")
            elif ':' in line:
                key, value = map(str.strip, line.split(':', 1))
                if current_section:
                    config[current_section][key] = value
                else:
                    raise ValueError(f"Key-value pair found outside of section: {line}")
            else:
                raise ValueError(f"Invalid line in INI file: {line}")

    return config

def process_ini_file(file_path, dir_name):
    config = parse_ini_file(file_path)

    for section in config:
        if section.startswith('effect_') or section.startswith('projectile_'):
            continue
        for key in config[section]:
            if config[section][key].endswith('.png'):
                old_image_name = config[section][key]
                new_image_name = f"{dir_name}{old_image_name}"
                config[section][key] = new_image_name

                old_image_path = os.path.join(os.path.dirname(file_path), old_image_name)
                new_image_path = os.path.join(os.path.dirname(file_path), new_image_name)

                if os.path.exists(old_image_path):
                    os.rename(old_image_path, new_image_path)

    with open(file_path, 'w', encoding='utf-8') as file:
        for section in config:
            file.write(f"[{section}]\n")
            for key, value in config[section].items():
                file.write(f"{key} : {value}\n")
            file.write("\n")

def walk_directory(root_dir):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        dir_name = os.path.basename(dirpath)
        for filename in filenames:
            if filename.endswith('.ini'):
                ini_file_path = os.path.join(dirpath, filename)
                process_ini_file(ini_file_path, dir_name)

if __name__ == "__main__":
    root_directory = "resources/units/彩蛋单位/平民系列"
    walk_directory(root_directory)
import os


def process_ini_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    in_attack_section = False
    new_lines = []

    for line in lines:
        stripped_line = line.strip()

        if stripped_line.startswith('[') and stripped_line.endswith(']'):
            in_attack_section = stripped_line == '[attack]'
            new_lines.append(line)
            continue
        if stripped_line.startswith('[') and stripped_line.endswith(']'):
            in_attack_section = stripped_line == '[projectile_1]'
            new_lines.append(line)
            continue
        if in_attack_section:
            if stripped_line.startswith('maxAttackRange'):
                value = stripped_line.split(':', 1)[1].strip()
                if int(value)<25:
                    new_lines.append('maxAttackRange:25\n')
                else:
                    new_lines.append(line)
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
    walk_directory('resources/units/zombie')

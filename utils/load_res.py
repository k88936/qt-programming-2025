import os

qrc=open("resources.qrc","w")
def list_files(directory):
        for root, _, files in os.walk(directory):
            for file in files:
                relative_path = os.path.relpath(os.path.join(root, file), './')
                print(f'<file>{relative_path}</file>',file=qrc)
def list_single_file(file):
    print(f'<file>{file}</file>',file=qrc)
# Example usage

print("<RCC><qresource prefix=\"/\">",file=qrc)
list_files("ui")
# list_files("M2A3")
list_single_file("t_vshader.glsl")
list_single_file("t_fshader.glsl")
list_single_file("s_vshader.glsl")
print("</qresource></RCC>",file=qrc)

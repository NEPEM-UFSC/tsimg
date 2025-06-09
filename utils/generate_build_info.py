# generate_build_info.py
import os
import re
from datetime import datetime

def read_version_from_cmake():
    """Lê a versão do arquivo CMakeLists.txt"""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    cmake_file = os.path.join(script_dir, '../CMakeLists.txt')
    cmake_file = os.path.abspath(cmake_file)
    
    if os.path.exists(cmake_file):
        with open(cmake_file, 'r') as file:
            content = file.read()
            match = re.search(r'project\(tsimg\s+VERSION\s+([\d.]+)\)', content)
            if match:
                return match.group(1)
    return "0.1"  # fallback version

print("GBI - EVOKED ")
print("Generating build info...")

build_info_file = '../build/build_info.txt' # Changed path
version_file = '../version.rc'
header_file_path = '../src/build_info.h'

# Corrigir caminhos relativos
script_dir = os.path.dirname(os.path.abspath(__file__))
build_info_file = os.path.join(script_dir, '../build/build_info.txt')
version_file = os.path.join(script_dir, '../version.rc')
header_file_path = os.path.join(script_dir, '../src/build_info.h')

build_info_file = os.path.abspath(build_info_file)
version_file = os.path.abspath(version_file)
header_file_path = os.path.abspath(header_file_path)

print(f"Looking for version.rc at: {version_file}")

os.makedirs(os.path.dirname(build_info_file), exist_ok=True)

# Ler versão do CMakeLists.txt
version = read_version_from_cmake()
print(f"Version from CMakeLists.txt: {version}")

if os.path.exists(build_info_file):
    with open(build_info_file, 'r') as file:
        content = file.read().strip()
        if content:
            try:
                _, patch, microversion, build_number = content.split(',')
                build_number = int(build_number) + 1
                patch = int(patch)
            except ValueError:
                print(f'Error: Invalid build info in {build_info_file}. Resetting to defaults.')
                patch, microversion, build_number = 7, "alpha", 272
        else:
            patch, microversion, build_number = 7, "alpha", 272
else:
    patch, microversion, build_number = 7, "alpha", 272

# Criar versão completa para display
full_version = f"{version}.{patch}"

build_info = (
    f'{version},{patch},{microversion},{build_number}'
)
print(f"Full version: {full_version}")
print(f"Build info: {build_info}")

try:
    with open(build_info_file, 'w') as file:
        file.write(build_info)
        print('Build info generated successfully')
except FileNotFoundError:
    print(f'Error: The directory for the build info file does not exist: {os.path.dirname(build_info_file)}')
    print('Failed to generate build info')
    exit(1)
except PermissionError:
    print(f'Error: Permission denied when writing to the build info file: {build_info_file}')
    print('Failed to generate build info')
    exit(1)
except Exception as e:
    print(f'Error: {e}')
    print('Failed to generate build info')
    exit(1)

os.makedirs(os.path.dirname(header_file_path), exist_ok=True)

header_content = f'#pragma once\n\n#define BUILD_INFO "Version: {full_version}\\nMicroversion: {microversion}\\nBuild Number: {build_number}\\nBuild Date: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}"\n'

try:
    with open(header_file_path, 'w') as file:
        file.write(header_content)
        print('Header file generated successfully')
except FileNotFoundError:
    print(f'Error: The directory for the header file does not exist: {os.path.dirname(header_file_path)}')
    print('Failed to generate header file')
    exit(1)
except PermissionError:
    print(f'Error: Permission denied when writing to the header file: {header_file_path}')
    print('Failed to generate header file')
    exit(1)
except Exception as e:
    print(f'Error: {e}')
    print('Failed to generate header file')
    exit(1)

# Atualizar version.rc se existir (opcional)
if os.path.exists(version_file):
    print(f"Found version.rc, updating...")
    with open(version_file, 'r') as file:
        lines = file.readlines()

    try:
        version_parts = version.split('.')
        major = version_parts[0] if len(version_parts) > 0 else "0"
        minor = version_parts[1] if len(version_parts) > 1 else "1"
        
        with open(version_file, 'w') as file:
            for line in lines:
                if line.startswith(' FILEVERSION'):
                    file.write(f' FILEVERSION {major},{minor},{patch},{build_number}\n')
                elif line.startswith(' PRODUCTVERSION'):
                    file.write(f' PRODUCTVERSION {major},{minor},{patch},0\n')
                elif line.strip().startswith('VALUE "FileVersion"'):
                    file.write(f'            VALUE "FileVersion", "{full_version}.{build_number}\\0"\n')
                elif line.strip().startswith('VALUE "ProductVersion"'):
                    file.write(f'            VALUE "ProductVersion", "{full_version}\\0"\n')
                else:
                    file.write(line)
        print('Version.rc updated successfully')
    except Exception as e:
        print(f'Warning: Failed to update version.rc: {e}')
        print('Build will continue without version.rc update')
else:
    print(f'Warning: version.rc not found at {version_file}')
    print('Skipping version.rc update (this is optional)')

print("GBI - COMPLETED ")

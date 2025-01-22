# generate_build_info.py
import os
from datetime import datetime

print("GBI - EVOKED ")
print("Generating build info...")

build_info_file = '../utils/build_info.txt'
version_file = '../version.rc'
header_file_path = '../src/build_info.h'

build_info_file = os.path.abspath(build_info_file)
version_file = os.path.abspath(version_file)
header_file_path = os.path.abspath(header_file_path)

os.makedirs(os.path.dirname(build_info_file), exist_ok=True)

if os.path.exists(build_info_file):
    with open(build_info_file, 'r') as file:
        content = file.read().strip()
        if content:
            try:
                version, microversion, build_number = content.split(',')
                build_number = int(build_number) + 1
            except ValueError:
                print(f'Error: Invalid build number in {build_info_file}. Resetting build number to 1.')
                version, microversion, build_number = "1.0", "0", 1
        else:
            version, microversion, build_number = "1.0", "0", 1
else:
    version, microversion, build_number = "1.0", "0", 1

build_info = (
    f'{version},{microversion},{build_number}'
)
print(build_info)

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

header_content = f'#pragma once\n\n#define BUILD_INFO "Version: {version}\\nMicroversion: {microversion}\\nBuild Number: {build_number}\\nBuild Date: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}"\n'

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

if os.path.exists(version_file):
    with open(version_file, 'r') as file:
        lines = file.readlines()

    try:
        with open(version_file, 'w') as file:
            for line in lines:
                if line.startswith(' FILEVERSION'):
                    file.write(f' FILEVERSION {version.split(".")[0]},{build_number},0,0\n')
                elif line.startswith(' PRODUCTVERSION'):
                    file.write(f' PRODUCTVERSION {version.split(".")[0]},0,0,0\n')
                elif line.strip().startswith('VALUE "FileVersion"'):
                    file.write(f'            VALUE "FileVersion", "{version}.{build_number}\\0"\n')
                elif line.strip().startswith('VALUE "ProductVersion"'):
                    file.write(f'            VALUE "ProductVersion", "{version}\\0"\n')
                else:
                    file.write(line)
        print('Version.rc updated successfully')
    except Exception as e:
        print(f'Error: {e}')
        print('Failed to update version.rc')
        exit(1)
else:
    raise FileNotFoundError(f'File {version_file} not found')
print("GBI - COMPLETED ")

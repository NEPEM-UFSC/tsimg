# generate_build_info.py
import os
import re
import sys
import argparse
from datetime import datetime

def read_version_from_cmake(cmake_path):
    """Lê a versão do arquivo CMakeLists.txt"""
    if os.path.exists(cmake_path):
        with open(cmake_path, 'r') as file:
            content = file.read()
            match = re.search(r'project\(tsimg\s+VERSION\s+([\d.]+)\)', content)
            if match:
                return match.group(1)
    return "0.2"  # fallback version

def main():
    parser = argparse.ArgumentParser(description='Generate build info for tsimg')
    parser.add_argument('--output', required=True, help='Output path for build_info.cpp')
    parser.add_argument('--cmake', help='Path to CMakeLists.txt (deprecated if --project-version is used)')
    parser.add_argument('--project-version', help='Project version string (e.g. 0.2)')
    parser.add_argument('--version-rc', help='Path to version.rc to update')
    parser.add_argument('--build-info-txt', help='Path to store persistent build number')
    
    args = parser.parse_args()

    print("GBI - EVOKED ")
    print("Generating build info...")

    # Setup paths
    output_file = os.path.abspath(args.output)
    
    # Version logic
    if args.project_version:
        version = args.project_version
        print(f"Version from arguments: {version}")
    elif args.cmake:
        cmake_file = os.path.abspath(args.cmake)
        version = read_version_from_cmake(cmake_file)
        print(f"Version from CMakeLists.txt: {version}")
    else:
        version = "0.2" # fallback
        print(f"Using fallback version: {version}")

    # Build number logic
    patch = 0
    microversion = "alpha"
    build_number = 0

    if args.build_info_txt:
        build_info_txt = os.path.abspath(args.build_info_txt)
        os.makedirs(os.path.dirname(build_info_txt), exist_ok=True)
        
        if os.path.exists(build_info_txt):
            try:
                with open(build_info_txt, 'r') as file:
                    content = file.read().strip()
                    if content:
                        parts = content.split(',')
                        if len(parts) >= 4:
                            _, patch_str, microversion, build_num_str = parts
                            patch = int(patch_str)
                            build_number = int(build_num_str) + 1
            except Exception as e:
                print(f"Error reading build info txt: {e}")
        
        # Save new build info
        try:
            with open(build_info_txt, 'w') as file:
                file.write(f'{version},{patch},{microversion},{build_number}')
        except Exception as e:
            print(f"Error writing build info txt: {e}")

    full_version = f"{version}.{patch}"
    build_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    print(f"Full version: {full_version}")
    print(f"Build number: {build_number}")

    # Generate C++ Source
    cpp_content = f'''#include "build_info.h"

namespace tsimg {{
    std::string getBuildInfo() {{
        return "Version: {full_version}\\n"
               "Microversion: {microversion}\\n"
               "Build Number: {build_number}\\n"
               "Build Date: {build_date}";
    }}

    std::string getVersion() {{
        return "{full_version}";
    }}

    std::string getBuildDate() {{
        return "{build_date}";
    }}
}}
'''

    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    
    # Only write if changed to avoid unnecessary rebuilds (though date always changes)
    # We could check if only date changed and skip if we wanted to be very aggressive with caching,
    # but for now let's just write it.
    try:
        with open(output_file, 'w') as file:
            file.write(cpp_content)
        print(f'Generated {output_file}')
    except Exception as e:
        print(f'Error writing output file: {e}')
        sys.exit(1)

    # Update version.rc if requested
    if args.version_rc and os.path.exists(args.version_rc):
        print(f"Updating version.rc at {args.version_rc}")
        try:
            with open(args.version_rc, 'r') as file:
                lines = file.readlines()

            version_parts = version.split('.')
            major = version_parts[0] if len(version_parts) > 0 else "0"
            minor = version_parts[1] if len(version_parts) > 1 else "1"
            
            with open(args.version_rc, 'w') as file:
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
        except Exception as e:
            print(f'Warning: Failed to update version.rc: {e}')

    print("GBI - COMPLETED ")

if __name__ == "__main__":
    main()


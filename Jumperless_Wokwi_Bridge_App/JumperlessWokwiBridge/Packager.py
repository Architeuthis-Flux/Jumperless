import PyInstaller.__main__

import pathlib
import os

import shutil


# PyInstaller.__main__.run([
#     'JumperlessWokwiBridge.py',
#     '--onefile',
#     '--windowed',
#     '--onedir',
#     '--icon=icon.icns',
#     #'--add-binary=arduino-cli:.',
#     '--console',
#     '--target-arch=arm64',
#     '--path=/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/JumperlessWokwiBridge/.venv/lib/python3.12/site-packages',
#     '--noconfirm',
#     #'--clean',

    
# ])

os.system(f"python -m PyInstaller --icon=\"/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/jumperlesswokwibridge/icon.icns\" \
-y \
--console \
--windowed \
--target-arch arm64 \
--path \"/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/JumperlessWokwiBridge/.venv/lib/python3.12/site-packages\" \
JumperlessWokwiBridge.py \
--name Jumperless ")

generated_app_path = pathlib.Path("dist/Jumperless.app/Contents/MacOS/Jumperless")
generated_app_path_renamed = generated_app_path.with_name("Jumperless_cli")
target_app_path = pathlib.Path("dist/Jumperless.app/Contents/MacOS/")
icon_path = pathlib.Path("icon.icns")
nonexec_launcher_path = pathlib.Path("Jumperless_cli_launcher.sh")
launcher_path = pathlib.Path("Jumperless")
app_path = pathlib.Path("Jumperless.app")
apple_silicon_folder = pathlib.Path("apple silicon/Jumperless.app")
intel_folder = pathlib.Path("intel mac/Jumperless.app")

# rename the app to JumperlessWokwiBridge_cli
print("chmodding launcher")
os.system(f"chmod 755 {nonexec_launcher_path}")
print("Changed permissions for " + str(nonexec_launcher_path)+ '\n')

print("Renaming launcher to Jumperless")
os.system(f'cp {nonexec_launcher_path} {launcher_path}')
print("Renamed " + str(nonexec_launcher_path) + " to " + str(launcher_path)+ '\n')


print("Renaming app main app to Jumperless_cli")
os.rename(generated_app_path, generated_app_path_renamed)
print("Renamed " + str(generated_app_path) + "(main app) to " + str(generated_app_path_renamed)+ '\n')


os.system(f"cp {launcher_path} {target_app_path}")
print("Copied " + str(launcher_path) + "(launcher) to " + str(target_app_path)+ '\n')


# os.system(f"cp {generated_app_path} {target_app_path}")
# print("Copied " + str(generated_app_path) + " to " + str(target_app_path))

App = "/Applications/"
print("Copying app to applications folder")
shutil.copytree("Jumperless.app", App + "Jumperless.app", dirs_exist_ok=True )
print("Copied " + str(app_path) + " to " + str(App)+ '\n')

print("Copying app to apple silicon folder")
shutil.copytree("Jumperless.app", apple_silicon_folder, dirs_exist_ok=True )

print("doing this all again for intel mac")


os.system(f"python -m PyInstaller --icon=\"/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/jumperlesswokwibridge/icon.icns\" \
-y \
--console \
--windowed \
--target-arch universal2 \
--path \"/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/JumperlessWokwiBridge/.venv/lib/python3.12/site-packages\" \
JumperlessWokwiBridge.py \
--name Jumperless ")




print("chmodding launcher")
os.system(f"chmod 755 {nonexec_launcher_path}")
print("Changed permissions for " + str(nonexec_launcher_path)+ '\n')

print("Renaming launcher to Jumperless")
os.system(f'cp {nonexec_launcher_path} {launcher_path}')
print("Renamed " + str(nonexec_launcher_path) + " to " + str(launcher_path)+ '\n')


print("Renaming app main app to Jumperless_cli")
os.rename(generated_app_path, generated_app_path_renamed)
print("Renamed " + str(generated_app_path) + "(main app) to " + str(generated_app_path_renamed)+ '\n')


os.system(f"cp {launcher_path} {target_app_path}")
print("Copied " + str(launcher_path) + "(launcher) to " + str(target_app_path)+ '\n')


# os.system(f"cp {generated_app_path} {target_app_path}")
# print("Copied " + str(generated_app_path) + " to " + str(target_app_path))

App = "/Applications/"
print("Copying app to applications folder")
shutil.copytree("Jumperless.app", App + "Jumperless.app", dirs_exist_ok=True )
print("Copied " + str(app_path) + " to " + str(App)+ '\n')

print("Copying app to intel mac folder")
shutil.copytree("Jumperless.app", intel_folder, dirs_exist_ok=True )









# os.system(f"cp {"JumperlessWokwiBridge.app"} {App}")

# os.system(f"cp {icon_path} {generated_app_path}")

print("Done")


# python -m PyInstaller \
# --icon="/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/jumperlesswokwibridge/icon.icns" \
# -y \
# --console \
# --windowed \
# --target-arch x86_64 \
# --path "/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/JumperlessWokwiBridge/.venv/lib/python3.12/site-packages" \
# --add-binary "arduino-cli:." \
# JumperlessWokwiBridge.py 
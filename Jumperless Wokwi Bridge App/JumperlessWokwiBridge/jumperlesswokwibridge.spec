# -*- mode: python ; coding: utf-8 -*-


block_cipher = None


a = Analysis(
    ['jumperlesswokwibridge.py'],
    pathex=[],
    binaries=[],
    datas=[],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)
pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

# you should change the path to the .icns file unless your name is the same as mine

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='jumperlesswokwibridge',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=x86_64,
    codesign_identity=None,
    entitlements_file=None,
    icon=['/Users/kevinsanto/JumperlessWokwiBridge/icon.icns'],
)

#I'm not sure this part does anything but some person on Github says it worked so whatever
## Make app bundle double-clickable
import plistlib
from pathlib import Path
app_path = Path(app.name)

# read Info.plist
with open(app_path / 'Contents/Info.plist', 'rb') as f:
    pl = plistlib.load(f)

# write Info.plist
with open(app_path / 'Contents/Info.plist', 'wb') as f:
    pl['CFBundleExecutable'] = 'wrapper'
    plistlib.dump(pl, f)

# write new wrapper script
shell_script = """#!/bin/bash
dir=$(dirname $0)
open -a Terminal file://${dir}/%s""" % app.appname
with open(app_path / 'Contents/MacOS/wrapper', 'w') as f:
    f.write(shell_script)

# make it executable
(app_path  / 'Contents/MacOS/wrapper').chmod(0o755)

# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['JumperlessWokwiBridge.py'],
    pathex=['/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/JumperlessWokwiBridge/.venv/lib/python3.12/site-packages'],
    binaries=[],
    datas=[],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='Jumperless_cli',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch='arm64',
    codesign_identity=None,
    entitlements_file=None,
    icon=['/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/jumperlesswokwibridge/icon.icns'],
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='Jumperless_cli',
)
app = BUNDLE(
    coll,
    name='Jumperless_cli.app',
    icon='/Users/kevinsanto/Documents/GitHub/Jumperless/Jumperless_Wokwi_Bridge_App/jumperlesswokwibridge/icon.icns',
    bundle_identifier=None,
)

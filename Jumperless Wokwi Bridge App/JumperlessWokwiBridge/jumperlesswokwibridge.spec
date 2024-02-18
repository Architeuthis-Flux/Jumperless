# -*- mode: python ; coding: utf-8 -*-


block_cipher = None


a = Analysis(
    ['JumperlessWokwiBridge.py'],
    pathex=[],
    binaries=[('arduino-cli', '.')],
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

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='JumperlessWokwiBridge',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch='x86_64',
    codesign_identity=None,
    entitlements_file=None,
    icon=['/Users/kevinsanto/JumperlessWokwiBridge/icon.icns'],
)
coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='JumperlessWokwiBridge',
)
app = BUNDLE(
    coll,
    name='JumperlessWokwiBridge.app',
    icon='/Users/kevinsanto/JumperlessWokwiBridge/icon.icns',
    bundle_identifier=None,
)

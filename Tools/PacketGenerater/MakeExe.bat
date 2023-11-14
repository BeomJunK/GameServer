pushd %~dp0
pyinstaller --onefile PacketGenerater.py

MOVE .\dist\PacketGenerater.exe .\GenPackets.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\PacketGenerater.spec
PAUSE
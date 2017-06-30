echo off

set SOLUTIONNAME=DDS
set PROJECTNAME=DDS

set PROJECTSPATH=C:\ZSL\electro\projects
set AVRDUDE="C:\ZSL\electro\bin\avrdude\avrdude.exe"
set HEX=%PROJECTSPATH%\%SOLUTIONNAME%\%PROJECTNAME%\Debug\%PROJECTNAME%.hex"

set CHIPMODEL=m88p
rem -F

%AVRDUDE% -p %CHIPMODEL% -c 2ftbb -B 9600 -P ft0 -U flash:w:%HEX%:a

rem �������� ����� � ����������
rem %AVRDUDE% -p %CHIPMODEL% -c 2ftbb -B 2400 -P ft0 -U hfuse:r:high.txt:s -U lfuse:r:low.txt:s

rem �������� ����� �� ����������
rem 0xff - ��� Mega8 
rem    - ���������� ��������� � ���������
rem %AVRDUDE% -p %CHIPMODEL% -c 2ftbb -B 2400 -P ft0 -U lfuse:w:0xff:m -U hfuse:w:0xdf:m

rem �������� ��������� ����� �� ����������
rem %AVRDUDE% -p %CHIPMODEL% -c 2ftbb -B 2400 -P ft0 -U lfuse:w:0xe1:m -U hfuse:w:0xd9:m 

pause
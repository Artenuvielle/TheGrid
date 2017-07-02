@echo off

if not exist ".\Source\ThirdParty\libprotobuf\" (
	cd .\libprotobuf
	python install.py
	cd ..
	)

for /d /r ".\proto\" %%a in (out\) do if exist "%%a" rmdir /s /q "%%a"
cd .\proto
mkdir .\out
for %%f in (.\*.proto) do (
	..\Source\ThirdParty\libprotobuf\bin\protoc.exe --cpp_out=./out %%f
	..\Source\ThirdParty\libprotobuf\bin\regenerateforue4.py .\out\%%~nf.pb.h
	..\Source\ThirdParty\libprotobuf\bin\regenerateforue4.py .\out\%%~nf.pb.cc
	move .\out\%%~nf.pb.h ..\Source\TheGrid\
	move .\out\%%~nf.pb.cc ..\Source\TheGrid\
	)
cd ..
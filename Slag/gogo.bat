@if "%1"=="rebuild" (
  @rmdir /s /q bin\windows
)

@if not exist bin\windows (
  @mkdir bin\windows
)

@if not exist build (
  @mkdir build
)

@if exist bin\windows\gogo.exe (
  @if exist bin\windows\slag.exe (
    @goto skip_vm_compile
  )
)

@echo -------------------------------------------------------------------------------
@echo Load Visual Studio 2010 and open platforms\windows\slag_windows.sln.
@echo Set the build mode to Release and then right-click..build "slag" and "gogo".
@set /p enterkey=Press [ENTER] once the builds have finished.
@set enterkey=

@copy platforms\windows\Release\slag.exe  bin\windows
@copy platforms\windows\Release\gogo.exe  bin\windows

:skip_vm_compile

@if exist bin\windows\slagc.exe (
  @goto skip_slagc_compile
)

@cmd /C bin\windows\slag libraries\slag\slagc\slagc.etc -src_path "libraries/slag/slagc;libraries/slag/standard" slagc.slag -cpp -dest_path build

@echo -------------------------------------------------------------------------------
@echo Load Visual Studio 2010 and open platforms\windows\slag_windows.sln.
@echo Set the build mode to Release and then right-click..build "slagc".
@set /p enterkey=Press [ENTER] once the build has finished.
@set enterkey=

@copy platforms\windows\Release\slagc.exe bin\windows

:skip_slagc_compile

@if exist bin\windows\libraries (
  @goto skip_library_copy
)

@cmd /C bin\windows\gogo.exe -bin bin\windows libs -src_path "libraries/slag;libraries/slag/standard"

@where /Q slag
@if errorlevel 1 goto done

:skip_library_copy

@where /Q slag
@if errorlevel 1 (
  @REM Display the path message
  @cmd /C bin\windows\gogo.exe -bin bin\windows -src_path "libraries/slag;libraries/slag/standard"
  @goto done
)

@if "%1"=="rebuild" (
  @goto done
)

@cmd /C bin\windows\gogo.exe %1 %2 %3 %4

:done


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
  @if exist bin\windows\bard.exe (
    @goto skip_vm_compile
  )
)

@echo -------------------------------------------------------------------------------
@echo Load Visual Studio 2010 and open platforms\windows\bard_windows.sln.
@echo Set the build mode to Release and then right-click..build "bard" and "gogo".
@set /p enterkey=Press [ENTER] once the builds have finished.
@set enterkey=

@copy platforms\windows\Release\bard.exe  bin\windows
@copy platforms\windows\Release\gogo.exe  bin\windows

:skip_vm_compile

@if exist bin\windows\bardic.exe (
  @goto skip_bardic_compile
)

@cmd /C bin\windows\bard libraries\bard\bardic\bardic.etc -src_path "libraries/bard/bardic;libraries/bard/standard" bardic.bard -cpp -dest_path build

@echo -------------------------------------------------------------------------------
@echo Load Visual Studio 2010 and open platforms\windows\bard_windows.sln.
@echo Set the build mode to Release and then right-click..build "bardic".
@set /p enterkey=Press [ENTER] once the build has finished.
@set enterkey=

@copy platforms\windows\Release\bardic.exe bin\windows

:skip_bardic_compile

@if exist bin\windows\libraries (
  @goto skip_library_copy
)

@cmd /C bin\windows\gogo.exe -bin bin\windows libs -src_path "libraries/bard;libraries/bard/standard"

@where /Q bard
@if errorlevel 1 goto done

:skip_library_copy

@where /Q bard
@if errorlevel 1 (
  @REM Display the path message
  @cmd /C bin\windows\gogo.exe -bin bin\windows -src_path "libraries/bard;libraries/bard/standard"
  @goto done
)

@if "%1"=="rebuild" (
  @goto done
)

@cmd /C bin\windows\gogo.exe %1 %2 %3 %4

:done


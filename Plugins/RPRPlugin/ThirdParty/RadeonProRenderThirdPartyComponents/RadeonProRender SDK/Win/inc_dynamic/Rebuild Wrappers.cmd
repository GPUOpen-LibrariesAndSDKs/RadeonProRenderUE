echo Preparing wrapper headers

pushd "%~dp0"

powershell -Command "(gc ..\inc\RadeonProRender.h) -replace '^extern\s+\w+R_API_ENTRY\s+(\w+)\s+(\w+)', 'typedef $1 (*P$2)' | Out-File -encoding ASCII .WrappedDefinitions.h"
powershell -Command "(gc ..\inc\RadeonProRender.h) -replace '^(?!extern\s+\w+R_API_ENTRY).+', '// $&' | Out-File -encoding ASCII .WrappedFunctions.h"
powershell -Command "(gc .WrappedFunctions.h) -replace '^extern\s+\w+R_API_ENTRY\s+\w+\s+(\w+).+', 'RPR_DECL($1);' | Out-File -encoding ASCII .WrappedFunctions.h"


powershell -Command "(gc ..\inc\RadeonProRender_GL.h) -replace '^extern\s+\w+R_API_ENTRY\s+(\w+)\s+(\w+)', 'typedef $1 (*P$2)' | Out-File -encoding ASCII .WrappedDefinitions_GL.h"
powershell -Command "(gc ..\inc\RadeonProRender_GL.h) -replace '^(?!extern\s+\w+R_API_ENTRY).+', '// $&' | Out-File -encoding ASCII .WrappedFunctions_GL.h"
powershell -Command "(gc .WrappedFunctions_GL.h) -replace '^extern\s+\w+R_API_ENTRY\s+\w+\s+(\w+).+', 'RPR_DECL($1);' | Out-File -encoding ASCII .WrappedFunctions_GL.h"
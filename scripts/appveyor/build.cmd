setlocal
cd "%~dp0.."

msbuild.cmd build.proj ^
    /p:Configuration=%BUILD_TYPE% /p:EnableTests=True ^
    /p:ServerBuildFlags="-DENABLE_STATIC_STDLIB=ON;-DENABLE_GIT_REV=ON"

@echo off
REM 开发用构建脚本：工具链镜像只需构建一次，源码通过 volume 挂载
REM 用法:
REM   docker-make.bat           -- 构建固件
REM   docker-make.bat clean     -- 清理
REM   docker-make.bat ENABLE_SPECTRUM=0  -- 带参数构建

docker image inspect uvk5-toolchain >nul 2>&1
if errorlevel 1 (
    echo [INFO] 首次使用，正在构建工具链镜像 uvk5-toolchain...
    docker build ^
        --build-arg http_proxy=http://host.docker.internal:7897 ^
        --build-arg https_proxy=http://host.docker.internal:7897 ^
        -f Dockerfile.toolchain -t uvk5-toolchain .
    if errorlevel 1 (
        echo [ERROR] 工具链镜像构建失败
        exit /b 1
    )
    echo [INFO] 工具链镜像构建完成
)

if not exist compiled-firmware mkdir compiled-firmware

docker run --rm ^
    -v "%CD%:/build" ^
    -w /build ^
    uvk5-toolchain ^
    bash -c "make %* && cp -f firmware* compiled-firmware/ 2>/dev/null; echo Done."

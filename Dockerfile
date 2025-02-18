# 使用带有编译工具的基础镜像，这里使用 Ubuntu
FROM --platform=linux/amd64 ubuntu:22.04

# 安装构建工具和依赖项
RUN apt-get update && apt-get install -y \
    python3 \
    libssl-dev \
    gcc \
    build-essential \
    openssl \
    g++ \
    cmake \
    git \
    libbz2-dev \
    zlib1g-dev \
    libhts-dev \
    # 添加其他依赖项
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录，这是您的应用程序的根目录
WORKDIR /app

# 将您的应用程序源代码复制到 Docker 容器中
COPY . /app

# 构建您的应用程序，这里假设您使用 CMake
RUN mkdir build && cd build && \
    cmake .. && \
    make -j4

# 设置构建目录为工作目录
WORKDIR /app/build


# 运行您的应用程序
CMD ["bash"]
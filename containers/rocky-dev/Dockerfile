# A podman image contains all e2sar-daos dependencies.

# Parameters can be configured with "--build-arg"
ARG ROCKY_VERSION=8.9

######################################################################################
# Stage 1: basic dev packages and installing grpc & boost, referring to
# https://github.com/JeffersonLab/E2SAR/blob/main/.github/workflows/builddeps.yml.
######################################################################################

FROM rockylinux:${ROCKY_VERSION} AS builder

# Match the e2sar dep versions.
ENV BOOST_VERSION=1.85.0
ENV GRPC_VERSION=1.54.1

# NOTE: pathes start with "/" indicate absolute pathes
ENV BOOST_INSTALL_DIR=/e2sar-dep/boost-install
ENV GRPC_INSTALL_DIR=/e2sar-dep/grpc-install

## Basic dev packages
RUN dnf -y groupinstall "Development Tools" && \
    dnf -y install epel-release && \
    dnf -y install \
        cmake \
        git \
        curl \
        openssl-devel \
        zlib-devel \
        gcc \
        gcc-c++ \
        make \
        kernel-devel \
        autoconf \
        automake \
        libtool \
        pkg-config \
        protobuf-compiler \
        glib2-devel \
        re2-devel \
        libquadmath-devel \
        tar && \
    dnf clean all && \
    rm -rf /var/cache/dnf /var/tmp/*

## Install grpc and boost to the desired repo

# Create installation directories
RUN mkdir -p ${BOOST_INSTALL_DIR} && mkdir -p ${GRPC_INSTALL_DIR}

# Install grpc via git clone
WORKDIR /tmp-deps
RUN git clone --recurse-submodules -b v${GRPC_VERSION} https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
        -DgRPC_BUILD_TESTS=OFF \
        -DCMAKE_INSTALL_PREFIX=${GRPC_INSTALL_DIR} \
        -DBUILD_SHARED_LIBS=ON \
        ../.. && \
    make -j 8 && \
    make install

# Install boost
# The below website's SSL certification is blocked on ejfat-6.
# Download the tar.gz file at: https://archives.boost.io/release/1.85.0/source/boost_1_85_0.tar.gz
#    $ ls -lah
#    xmei ITD 140M Apr 15  2024 boost_1_85_0.tar.gz
# Copy the file to the curent directory first
WORKDIR /tmp-deps
COPY boost_1_85_0.tar.gz /tmp-deps/
RUN tar -xzf boost_1_85_0.tar.gz && \
    cd boost_1_85_0 && \
    ./bootstrap.sh && \
    ./b2 install --prefix=${BOOST_INSTALL_DIR}

#######################################################################################
# Stage 2: The env for build, compile and install E2SAR.
#######################################################################################
FROM rockylinux:${ROCKY_VERSION} AS e2sar-env

# Copy the grpc & boost artifacts from stage 1.
# This can reduce the image size by ~ 4GB.
ENV BOOST_INSTALL_DIR=/e2sar-dep/boost-install
ENV GRPC_INSTALL_DIR=/e2sar-dep/grpc-install

COPY --from=builder ${BOOST_INSTALL_DIR} ${BOOST_INSTALL_DIR}
COPY --from=builder ${GRPC_INSTALL_DIR} ${GRPC_INSTALL_DIR}

# Setup grpc&boost system env varibles
ENV GRPC_ROOT="${GRPC_INSTALL_DIR}}" \
    BOOST_ROOT="${BOOST_INSTALL_DIR}" \
    PKG_CONFIG_PATH="${GRPC_INSTALL_DIR}/lib/pkgconfig:${GRPC_INSTALL_DIR}/lib64/pkgconfig" \
    PATH="${GRPC_INSTALL_DIR}/bin:$PATH" \
    LD_LIBRARY_PATH="${GRPC_INSTALL_DIR}/lib:${GRPC_INSTALL_DIR}/lib64:${BOOST_INSTALL_DIR}/lib"

# Install runtime dependencies including python
# Reference: https://github.com/JeffersonLab/E2SAR/blob/main/scripts/notebooks/EJFAT/post-boot/recver.sh
RUN dnf -y update && \
    dnf -yq install epel-release && \
    dnf -yq groupinstall "Development Tools" && \
    dnf -yq --enablerepo=devel install gcc gcc-c++ gdb make cmake git which && \
    dnf -yq --enablerepo=devel install autoconf libtool pkg-config ninja-build openssl openssl-devel \
                                       protobuf-compiler glib2-devel re2-devel libquadmath-devel \
                                       python3-pip python39-devel python39 \
    && dnf clean all && \
    rm -rf /var/cache/dnf /var/tmp/*

# Configure Python versions
RUN update-alternatives --set python /usr/bin/python3.9 && \
    update-alternatives --set python3 /usr/bin/python3.9

# Install meson
RUN pip3 install meson pybind11 scapy

#######################################################################################
# Stage 3: Install and configure daos-agent.
# Refer to https://github.com/JeffersonLab/daos_ansible_install/blob/main/docker/Dockerfile.
#######################################################################################
FROM e2sar-env AS daos-runtime

# NOTE: for IB access, install the Infiniband kernels and drivers on host Ubuntu OS (ejfat-6).
# sudo apt install infiniband-diags ibutils ibverbs-utils rdmacm-utils libibverbs-dev

# Infiniband requirements
RUN dnf update -y && \
    dnf install -yq rdma-core libibverbs \
                    iputils iproute net-tools perftest infiniband-diags \
                    ucx ucx-devel wget && \
    dnf clean all && \
    rm -rf /var/cache/dnf /var/tmp/*

# Install DAOS client packages and header files
# TODO: update DAOS_VERSION when needed.
ENV DAOS_VERSION=2.4
RUN wget -O /etc/yum.repos.d/daos-packages.repo \
    https://packages.daos.io/v${DAOS_VERSION}/EL8/packages/x86_64/daos_packages.repo && \
	rpm --import https://packages.daos.io/RPM-GPG-KEY && \
    dnf -y install daos-client daos-devel

RUN systemctl enable daos_agent

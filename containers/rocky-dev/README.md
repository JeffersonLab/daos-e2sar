
## The container for e2sar-daos development

---

This container is for developing [E2SAR](https://github.com/JeffersonLab/E2SAR) on **RHEL Rocky** systems. **E2SAR itself is not included in the container, but can be mounted to it.** The conainer provides all the building dependecies for E2SAR.

---

Build the container with different stages.

```bash
podman build --target builder -t <tag-base:builder> .  # the first stage, install grpc & boost
podman build --target e2sar-env -t <tag:meson> .  # the 2nd stage, dependecies needed for compiling and building E2SAR
podman build -t <tag:final> .  
```

### Steps to compile E2SAR with the container
1. Prepare the submodules (mainly the `udplbd` submodule) of E2SAR.

    ```bash
    cd E2SAR   # E2SAR root directory
    # List the current commits for the submodules
    git submodule status
    312acacc80a874a11a6b8af0f0819f32d513c546 docs (heads/main)
    5712d107ec5b2aa8fe32e57acbc3acdbe175925f udplbd (v0.3.2)  # the desired commit
    7a530a6de8396c170faa83135595325da8da0918 wiki (heads/master)
    ```
    Cloning the `udplbd` project with `git submodule update --init --recursive` may not be successful even we are granted to access the private GitHub repo. Just go to the subdir `udplbd` and clone with the HTTPS url and then checkout to the specific commit.

    ``` bash
    cd udplbd
    rm -rf .git
    git clone https://github.com/esnet/udplbd.git . # clone to current dir. You need to have access to the udplbd project.
    git checkout 5712d107ec5b2aa8fe32e57acbc3acdbe175925f # commit SHA copied from the above
    cd ..
    ```

2. Run this image with `podman`.

    ```bash
    # Native OS
    cd E2SAR  # the E2SAR root dir
    podman run -it -v ${PWD}:/app -w /app --network host <image_id>
    ```

3. Compile and install the `E2SAR` project.
    ```bash
    # Inside the container
    meson setup --wipe build
    # Replace all the "c++11" with "c++17" in file build/build.ninja using `sed` command or whatever.
    ninja -C build
    ```

    If compiling is successful, try to install the `e2sar` library.

    ```bash
    cd build
    meson test --list  # list the tests
    meson test --suite unit  # unit tests
    meson install  # install to /usr/local
    ```

# VM Experiment

Reference: https://mergeboard.com/blog/2-qemu-microvm-docker/

## TODO

- Get networking working with slirp!


## Requirements

```bash
sudo yum install -y devtoolset-11
sudo yum install -y elfutils-libelf-devel
sudo yum install -y ninja-build
sudo yum install -y glib2 glib2-devel
sudo yum install -y pixman-devel
sudo yum install -y glibc-static
sudo yum install -y libguestfs-tools
scl enable devtoolset-11 bash
```

```bash
sudo apt-get update -y
sudo apt-get install -y ninja-build
sudo apt-get install -y libpixman-1-dev
sudo apt-get install -y libslirp-dev
sudo apt-get install -y flex
sudo apt-get install -y libelf-dev
sudo apt-get install -y libguestfs-tools
```


## Qemu

```bash
wget https://download.qemu.org/qemu-8.0.3.tar.xz
tar xJf qemu-8.0.3.tar.xz
cd qemu-8.0.3/
./configure
make -j16 qemu-system-x86_64
make -j16 qemu-img
cd ..
```


## Linux

```bash
# wget https://mergeboard.com/files/blog/qemu-microvm/defconfig
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.40.tar.xz
tar -xf linux-6.1.40.tar.xz 
cd linux-6.1.40/
cp ../defconfig .config
make olddefconfig
make -j16
make modules
mkdir build
make modules_install INSTALL_MOD_PATH=build
cd ..
```


## init

```bash
gcc -Wall -o init -static init.c
```


## Image

Reference: https://askubuntu.com/questions/1046828/how-to-run-libguestfs-tools-tools-such-as-virt-make-fs-without-sudo

```bash
DOCKER_BUILDKIT=1 docker build -f python.Dockerfile --output "type=tar,dest=python.tar" .
SUPERMIN_KERNEL=linux-6.1.40/arch/x86/boot/bzImage SUPERMIN_MODULES=linux-6.1.40/build/lib/modules/6.1.40 virt-make-fs --format=qcow2 --size=+100M python.tar python-large.qcow2
./qemu-8.0.3/build/qemu-img convert python-large.qcow2 -O qcow2 python.qcow2
./qemu-8.0.3/build/qemu-img create -f qcow2 -b python.qcow2 -F qcow2 python-diff.qcow2
./qemu-8.0.3/build/qemu-system-x86_64 -M microvm,x-option-roms=off,isa-serial=off,rtc=off -machine acpi=off -enable-kvm -cpu host -nodefaults -no-user-config -nographic -no-reboot -device virtio-serial-device -chardev stdio,id=virtiocon0 -device virtconsole,chardev=virtiocon0 -drive id=root,file=python-diff.qcow2,format=qcow2,if=none -device virtio-blk-device,drive=root -kernel linux-6.1.40/arch/x86/boot/bzImage -append "console=hvc0 root=/dev/vda rw acpi=off reboot=t panic=-1 quiet" -m 512 -smp 2 -L ./qemu-8.0.3/pc-bios/
```

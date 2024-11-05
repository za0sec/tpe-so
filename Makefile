
all:  bootloader kernel userland image

buddy: bootloader kernel_buddy userland image_buddy

bootloader:
	cd Bootloader; make all

kernel_buddy:
	cd Kernel; make all BUDDY=YES

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

image_buddy: kernel_buddy bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean

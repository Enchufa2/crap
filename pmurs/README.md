# pmurs: Poor Man's UDP Reverse Shell

This program opens a UDP socket and keeps executing whatever is received from
a remote IP/PORT in a shell, returning the output back. At the same time, it
sends a dummy UDP packet, a *heartbeat*, every `n` seconds to that IP/PORT to
open any NAT in the middle. Linux establishes a timeout of 60 seconds (see
`/proc/net/nf_conntrack`), so the interval should be shorter than that.

I use it to send commands to nodes in my experiments with minimal network
impact. **I warn you: don't use this outside of a controlled environment**.

## Compilation

### For Linux

```bash
autoreconf --install
./configure
make
```

### For Android

Substitute `NDK` for the location of the downloaded Android NDK, and put the
standalone `ANDROID_TOOLCHAIN` wherever you want.

```bash
export ANDROID_TOOLCHAIN=/tmp/my-android-toolchain
$NDK/build/tools/make_standalone_toolchain.py \
    --arch arm --api 24 --install-dir $ANDROID_TOOLCHAIN
export PATH=$ANDROID_TOOLCHAIN/bin:$PATH
export AM_CFLAGS="-fPIE -pie"
autoreconf --install
./configure --host=arm-linux-androideabi TARGET_SHELL=/system/bin/sh
make
```

## Usage

In the *controlled* machine:

```bash
./pmurs <remote_IP> <remote_PORT> <local_PORT> <heartbeat_interval>
```

Then, in the *controlling* machine, you can, for instance, connect with `netcat`
and start sending commands.

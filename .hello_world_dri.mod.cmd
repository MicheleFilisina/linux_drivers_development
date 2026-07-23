savedcmd_hello_world_dri.mod := printf '%s\n'   hello_world_dri.o | awk '!x[$$0]++ { print("./"$$0) }' > hello_world_dri.mod

gcc main.c -o afterhours.exe \
  -g3 -O0 \
  -Wall -Werror \
  -Wno-unused-value \
  -I libs/include \
  libs/lib/linux/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11

./afterhours.exe
gcc main.c -o afterhours.exe \
  -g3 -O0 \
  -I libs/include \
  libs/lib/windows/libraylib.a \
  -lm -lpthread -ldl -lgdi32 -lwinmm

./afterhours.exe
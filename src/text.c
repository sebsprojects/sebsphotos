#include "text.h"

#include <stdlib.h>
#include <font_droidsans_64d8.h>


Text *createText(char *shaderDir)
{
  Text *t = malloc(sizeof(Text));
  initShader(&t->textShader, shaderDir, "text");
  return t;
}
